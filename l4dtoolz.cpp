#include "eiface.h"
#include "tier1/tier1.h"
#ifdef WIN32
#include "sig_win32.h"
#else
#include "sig_linux.h"
#endif
#include "sigtool.h"
#include "l4dtoolz.h"

#define BINMSIZE 0x1400000  // 20M
#define CHKPTR(P, V) ((P) && !((uintptr_t)(P) & (V)))
#define CMPPTR(P, V, C) (CHKPTR(P, V) && abs((int)(P) - (int)(C)) < BINMSIZE)
#define READCALL(P) (((P) + 5 - 1) + *(int *)(P))
#define CHK_RET_MSG(cond, name)                       \
    do {                                              \
        if (cond) {                                   \
            Msg("[L4DToolZ] " #name " init error\n"); \
            return;                                   \
        }                                             \
    } while (0);
#define CHK_RET_VAR_MSG(cond, name, val)              \
    do {                                              \
        if (cond) {                                   \
            var->SetValue(val);                       \
            Msg("[L4DToolZ] " #name " init error\n"); \
            return;                                   \
        }                                             \
    } while (0);

#pragma pack(push, 1)
class CSteamID {
public:
    uint64_t m_steamid;
};
typedef struct {
    CSteamID id;
    int32_t code;
    CSteamID owner;
} ValidateAuthTicketResponse_t;
#pragma pack(pop)

l4dtoolz g_l4dtoolz;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(l4dtoolz, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS, g_l4dtoolz);

static IVEngineServer *g_engine;
static ICvar *g_cvar;
static int g_tickrate = 30;

static uintptr_t *sv_ptr;
static uintptr_t ***gamerules_ptr;
static uintptr_t rules_max_ptr;
static uintptr_t rules_max_org;
static uintptr_t match_max_ptr;
static uintptr_t match_max_org;
static uintptr_t lobbyreq_ptr;
static uintptr_t lobbyreq_org;
static uintptr_t check_ptr;
static uintptr_t check_org;
static uintptr_t *steam3_ptr;
static uintptr_t *authrsp_ptr;
static uintptr_t authrsp_org;
static uintptr_t tickint_ptr;
static uintptr_t tickint_org;

static void write_dword(uintptr_t addr, uintptr_t val) {
    static uint8_t dword[6] = {0x04, 0x00};
    static auto patch = (mem_sig_t *)dword;
    if (!addr) return;
    *(uintptr_t *)patch->sig = val;
    write_sig(addr, patch);
}

// Linux: static int FUNC_T GetTotalNumPlayersSupported(void *);
static int FUNC_T GetTotalNumPlayersSupported() {
    return 32;  // max
}

// Linux: static int FUNC_T GetMaxHumanPlayers(void *);
static int FUNC_T GetMaxHumanPlayers() {
    return g_cvar->FindVar("sv_maxplayers")->GetInt();
}

void OnChangeMax(IConVar *var, const char *, float) {
    CHK_RET_VAR_MSG(!sv_ptr, "sv", -1);
    int new_value = ((ConVar *)var)->GetInt();
    if (new_value < 0) {
        write_dword(rules_max_ptr, rules_max_org);
        return;
    }
    ((int *)sv_ptr)[slots_idx] = new_value;
    write_dword(match_max_ptr, (uintptr_t)&GetTotalNumPlayersSupported);
    write_dword(rules_max_ptr, (uintptr_t)&GetMaxHumanPlayers);
}
ConVar sv_maxplayers("sv_maxplayers", "-1", 0, "Max human players", true, -1, true, 31, OnChangeMax);

ConVar sv_lobby_cookie("sv_lobby_cookie", "0", 0);
void Cookie_f(const CCommand &args) {
    CHK_RET_MSG(!sv_ptr, "sv");
    auto cookie = (uint64 *)&sv_ptr[cookie_idx];
    if (*cookie) {
        char buf[20];
        snprintf(buf, sizeof(buf), "%llu", *cookie);
        sv_lobby_cookie.SetValue(buf);
    }
    if (args.ArgC() != 2) {
        g_engine->ServerCommand("sv_lobby_cookie\n");
        return;
    }
    uint64 val = atoll(args[1]);
    g_cvar->FindVar("sv_hosting_lobby")->SetValue((val != 0) ? 1 : 0);
    if (!val) g_cvar->FindVar("sv_allow_lobby_connect_only")->SetValue(0);
    *cookie = val;
}
ConCommand cookie("sv_cookie", Cookie_f, "Lobby reservation cookie");

#define maxcl_idx 0x41
void OnSetMaxCl(IConVar *var, const char *, float) {
    CHK_RET_MSG(!sv_ptr, "sv");
    int new_value = ((ConVar *)var)->GetInt();
    ((int *)sv_ptr)[maxcl_idx] = new_value;
    Msg("[L4DToolZ] maxplayers set to %d\n", new_value);
}
ConVar sv_setmax("sv_setmax", "18", 0, "Max clients", true, 18, true, 32, OnSetMaxCl);

void l4dtoolz::ServerActivate(edict_t *, int, int) {
    int slots = sv_maxplayers.GetInt();
    if (slots >= 0) ((int *)sv_ptr)[slots_idx] = slots;
    if (rules_max_ptr) return;
    CHK_RET_MSG(!gamerules_ptr || !CHKPTR(*gamerules_ptr, 0x7U), "sv_maxplayers(rules)");  // malloc
    rules_max_ptr = (uintptr_t)&(*gamerules_ptr)[0][info_idx];
    rules_max_org = (*gamerules_ptr)[0][info_idx];
    if (slots >= 0) write_dword(rules_max_ptr, (uintptr_t)&GetMaxHumanPlayers);
}

#define steam_off 0x7D
HOOK_DEF(bool, CheckChallengeType, uintptr_t client, int, void *, int, void *key, int keylen) {
    if (keylen < 8) return false;
    memcpy((void *)(client + steam_off), key, sizeof(CSteamID));  // malloc
    return true;
}

void OnBypassAuth(IConVar *var, const char *, float) {
    CHK_RET_VAR_MSG(!steam3_ptr || !check_ptr, "sv_steam_bypass", 0);
    if (((ConVar *)var)->GetInt()) write_dword(check_ptr, (uintptr_t)&CheckChallengeType);
    else write_dword(check_ptr, check_org);
}
ConVar sv_steam_bypass("sv_steam_bypass", "0", 0, "Bypass steam validation", true, 0, true, 1, OnBypassAuth);

#define rate_idx     0x2C
#define snapshot_idx 0x88  // 2231
void l4dtoolz::ClientSettingsChanged(edict_t *pEdict)  // rate
{
    if (g_tickrate == 30) return;
    CHK_RET_MSG(!sv_ptr, "sv");
    auto edicts = ((edict_t **)sv_ptr)[edict_idx];
    CHK_RET_MSG(!CHKPTR(edicts, 0x3U), "edicts");
    auto idx = (int)(pEdict - edicts);
    auto net = (int *)g_engine->GetPlayerNetInfo(idx);
    if (net) net[rate_idx] = g_tickrate * 1000;  // real
    auto client = CALL(int *, ((uintptr_t **)sv_ptr)[0][client_idx], int)(sv_ptr, idx - 1);  // +4
    if (client) client[snapshot_idx - 1] = 1.0f / g_tickrate;
}

PLUGIN_RESULT l4dtoolz::ClientConnect(bool *bAllowConnect, edict_t *pEntity, const char *, const char *, char *, int) {
    if (sv_steam_bypass.GetInt() != 1) return PLUGIN_CONTINUE;
    ValidateAuthTicketResponse_t rsp = {g_engine->GetClientXUID(pEntity)};
    CALL(void, authrsp_org, void *)(steam3_ptr, &rsp);
    if (g_engine->GetPlayerUserId(pEntity) == -1) {
        *bAllowConnect = false;
        return PLUGIN_STOP;
    }
    Msg("[L4DToolZ] %llu validated.\n", rsp.id);
    return PLUGIN_CONTINUE;
}

HOOK_DEF(void, OnValidateAuthTicketResponse, ValidateAuthTicketResponse_t *rsp) {
    if (!rsp->code && (rsp->id.m_steamid != rsp->owner.m_steamid)) {
        rsp->code = 2;
        Msg("[L4DToolZ] %llu using family sharing, owner: %llu.\n", rsp->id.m_steamid, rsp->owner.m_steamid);
    }
    CALL(void, authrsp_org, void *)(steam3_ptr, rsp);
}

void OnAntiSharing(IConVar *var, const char *, float) {
    CHK_RET_VAR_MSG(!authrsp_ptr, "sv_anti_sharing", 0);
    if (((ConVar *)var)->GetInt()) *authrsp_ptr = (uintptr_t)&OnValidateAuthTicketResponse;
    else *authrsp_ptr = authrsp_org;
}
ConVar sv_anti_sharing("sv_anti_sharing", "0", 0, "No family sharing", true, 0, true, 1, OnAntiSharing);

HOOK_DEF(void, ReplyReservationRequest, void *, void *) { }

void OnForceUnreserved(IConVar *var, const char *, float) {
    CHK_RET_VAR_MSG(!lobbyreq_ptr, "sv_force_unreserved", 0);
    if (((ConVar *)var)->GetInt()) {
        write_dword(lobbyreq_ptr, (uintptr_t)&ReplyReservationRequest);
        g_cvar->FindVar("sv_allow_lobby_connect_only")->SetValue(0);
        return;
    }
    write_dword(lobbyreq_ptr, lobbyreq_org);
}
ConVar sv_force_unreserved("sv_force_unreserved", "0", 0, "Disallow lobby reservation", true, 0, true, 1, OnForceUnreserved);

// Linux: static float FUNC_T GetTickInterval(void *);
static float FUNC_T GetTickInterval() {
    static float interval = 1.0f / g_tickrate;
    return interval;
}

#define rules_idx   0x12  // rodata
#define sv_idx      0x80  // rodata
#define title_idx   0x8   // rodata
#define match_idx   0x4   // rodata
#define tickint_idx 0x09  // rodata
bool l4dtoolz::Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory) {
    g_engine = (IVEngineServer *)interfaceFactory(INTERFACEVERSION_VENGINESERVER, NULL);
    g_cvar = (ICvar *)interfaceFactory(CVAR_INTERFACE_VERSION, NULL);
    g_tickrate = CommandLine()->ParmValue("-tickrate", 30);

    ConnectTier1Libraries(&interfaceFactory, 1);
    ConVar_Register(0);

    if (!gamerules_ptr) {
        auto client = (uintptr_t **)gameServerFactory("ServerGameClients003", NULL);
        auto gamerules = *(uintptr_t ****)(client[0][rules_idx] + rules_off);
        if (CMPPTR(gamerules, 0x3U, gameServerFactory)) gamerules_ptr = gamerules;
    }
    if (!sv_ptr) {
        auto sv = *(uintptr_t ***)(((uint **)g_engine)[0][sv_idx] + sv_off);
        if (!CMPPTR(sv, 0x7U, interfaceFactory)) return false;
        sv_ptr = (uintptr_t *)sv;
        lobbyreq_ptr = (uintptr_t)&sv[0][lobby_idx];
        lobbyreq_org = sv[0][lobby_idx];
        check_ptr = (uintptr_t)&sv[0][check_idx];
        check_org = sv[0][check_idx];
        auto func = (uintptr_t *(*)(void))READCALL(sv[0][steam3_idx] + steam3_off);
        if (CMPPTR(func, 0xFU, interfaceFactory)) {
            steam3_ptr = func();  // conn
            authrsp_ptr = &steam3_ptr[authrsp_idx];
            authrsp_org = *authrsp_ptr;
        }
    }
    if (!match_max_ptr) {
        auto match = (uintptr_t **)interfaceFactory("MATCHFRAMEWORK_001", NULL);
        auto title = ((uintptr_t **(*)(void))match[0][title_idx])();
        match_max_ptr = (uintptr_t)&title[0][match_idx];
        match_max_org = title[0][match_idx];
    }

    if ((g_tickrate != 30) && !tickint_ptr) {
        auto game = (uintptr_t **)gameServerFactory(INTERFACEVERSION_SERVERGAMEDLL, NULL);
        tickint_ptr = (uintptr_t)&game[0][tickint_idx];
        tickint_org = game[0][tickint_idx];
        write_dword(tickint_ptr, (uintptr_t)&GetTickInterval);
        ((uint *)g_cvar->FindVar("net_splitpacket_maxrate"))[15] = 0;  // m_bHasMax
        Msg("[L4DToolZ] tickrate: %d\n", g_tickrate);
    }
    return true;
}

void l4dtoolz::Unload() {
    ConVar_Unregister();
    DisconnectTier1Libraries();

    write_dword(check_ptr, check_org);
    if (authrsp_ptr) *authrsp_ptr = authrsp_org;
    write_dword(rules_max_ptr, rules_max_org);
    write_dword(match_max_ptr, match_max_org);
    write_dword(lobbyreq_ptr, lobbyreq_org);
    write_dword(tickint_ptr, tickint_org);
}
