#ifdef WIN32
#include "sig_win32.h"
#else
#include "sig_linux.h"
#endif
#include "l4dtoolz.h"

#define BINMSIZE 0x1400000  // 20M
#define CHKPTR(P, V) ((P) && !((uintptr_t)(P) & (V)))
#define CMPPTR(P, V, C) (CHKPTR(P, V) && abs((int)(P) - (int)(C)) < BINMSIZE)
#define READCALL(P) (((P) + 5 - 1) + *(int *)(P))
#define CHKVAL                                 \
    int new_value = ((ConVar *)var)->GetInt(); \
    int old_value = atoi(pOldValue);           \
    if (new_value == old_value) return;

#pragma pack(push, 1)
typedef struct {
    uint64_t id;
    int32_t code;
    uint64_t owner;
} ValidateAuthTicketResponse_t;
#pragma pack(pop)

l4dtoolz g_l4dtoolz;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(l4dtoolz, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS, g_l4dtoolz);

static IVEngineServer *g_engine;
static ICvar *g_cvar;
static int g_tickrate = 30;

static edict_t **edict_ptr;
static int *slots_ptr;
static uint64 *cookie_ptr;
static int *maxcl_ptr;
static uintptr_t *gamerules_ptr;
static uintptr_t rules_max_ptr;
static mem_sig_t *rules_max_org;
static uintptr_t dsp_max_ptr;
static mem_sig_t *dsp_max_org;
static uintptr_t lobby_req_ptr;
static mem_sig_t *lobby_req_org;
static uintptr_t *steam3_ptr;
static uintptr_t authreq_ptr;
static mem_sig_t *authreq_org;
static uintptr_t *authrsp_ptr;
static uintptr_t authrsp_org;
static uintptr_t tickint_ptr;
static mem_sig_t *tickint_org;

void l4dtoolz::OnChangeMax(IConVar *var, const char *pOldValue, float flOldValue) {
    CHKVAL
    if (!slots_ptr) {
        var->SetValue(-1);
        Msg("[L4DToolZ] sv_maxplayers init error\n");
        return;
    }
    if (new_value < 0) {
        write_sig(rules_max_ptr, rules_max_org);
        write_sig(dsp_max_ptr, dsp_max_org);
        return;
    }
    *slots_ptr = new_value;
    max_player_new->sig[1] = (uint8_t)new_value;
    write_sig(rules_max_ptr, max_player_new);
    if (!dsp_max_ptr) Msg("[L4DToolZ] sv_maxplayers(dsp) init error\n");
    else write_sig(dsp_max_ptr, max_player_new);
}
ConVar sv_maxplayers("sv_maxplayers", "-1", 0, "Max human players", true, -1, true, 31, l4dtoolz::OnChangeMax);

ConVar sv_lobby_cookie("sv_lobby_cookie", "0", 0);
void l4dtoolz::Cookie_f(const CCommand &args) {
    if (!cookie_ptr) {
        Msg("[L4DToolZ] sv_cookie init error\n");
        return;
    }
    if (*cookie_ptr) {
        char buf[20];
        snprintf(buf, sizeof(buf), "%llu", *cookie_ptr);
        sv_lobby_cookie.SetValue(buf);
    }
    if (args.ArgC() != 2) {
        g_engine->ServerCommand("sv_lobby_cookie\n");
        return;
    }
    uint64 val = atoll(args[1]);
    g_cvar->FindVar("sv_hosting_lobby")->SetValue(val != 0);
    g_cvar->FindVar("sv_allow_lobby_connect_only")->SetValue(val != 0);
    *cookie_ptr = val;
}
ConCommand cookie("sv_cookie", l4dtoolz::Cookie_f, "Lobby reservation cookie");

void l4dtoolz::OnSetMaxCl(IConVar *var, const char *pOldValue, float flOldValue) {
    CHKVAL
    if (!maxcl_ptr) {
        Msg("[L4DToolZ] sv_setmax init error\n");
        return;
    }
    *maxcl_ptr = new_value;
    Msg("[L4DToolZ] maxplayers set to %d\n", new_value);
}
ConVar sv_setmax("sv_setmax", "18", 0, "Max clients", true, 18, true, 32, l4dtoolz::OnSetMaxCl);

void l4dtoolz::ServerActivate(edict_t *, int, int) {
    int slots = sv_maxplayers.GetInt();
    if ((slots >= 0) && slots_ptr) *slots_ptr = slots;
    if (rules_max_ptr) return;
    if (!gamerules_ptr || !CHKPTR(*gamerules_ptr, 0x7U)) {  // malloc
        Msg("[L4DToolZ] sv_maxplayers(rules) init error\n");
        return;
    }
    rules_max_ptr = ((uintptr_t **)*gamerules_ptr)[0][info_idx];
    read_sig(rules_max_ptr, max_player_new, rules_max_org);
    if (slots >= 0) write_sig(rules_max_ptr, max_player_new);
}

// Linux: static float GetTickInterval(void *);
static float GetTickInterval() {
    static float tickinv = 1.0f / g_tickrate;
    return tickinv;
}

#ifdef WIN32
static int PreAuth(const void *, int, uint64 steamID)
#else
static int PreAuth(void *, const void *, int, uint64 steamID)
#endif
{
    if (!steamID) {
        Msg("[L4DToolZ] invalid steamID.\n");
        return 1;
    }
    Msg("[L4DToolZ] %llu connected.\n", steamID);
    return 0;
}

void l4dtoolz::OnBypassAuth(IConVar *var, const char *pOldValue, float flOldValue) {
    CHKVAL
    if (!steam3_ptr) {
    err:
        var->SetValue(0);
        Msg("[L4DToolZ] sv_steam_bypass init error\n");
        return;
    }
    uint8_t authreq[6] = {0x04, 0x00};
    auto authreq_new = (mem_sig_t *)authreq;
    if (!authreq_ptr) {
        auto gsv = (uintptr_t **)steam3_ptr[1];
        if (!CHKPTR(gsv, 0xFU)) goto err;
        authreq_ptr = (uintptr_t)&gsv[0][authreq_idx];
        read_sig(authreq_ptr, authreq_new, authreq_org);
    }
    *(uintptr_t *)authreq_new->sig = (uintptr_t)&PreAuth;
    if (new_value) write_sig(authreq_ptr, authreq_new);
    else write_sig(authreq_ptr, authreq_org);
}
ConVar sv_steam_bypass("sv_steam_bypass", "0", 0, "Bypass steam validation", true, 0, true, 1, l4dtoolz::OnBypassAuth);

void l4dtoolz::ClientSettingsChanged(edict_t *pEdict)  // rate
{
    if (g_tickrate == 30) return;
    if (!edict_ptr || !CHKPTR(*edict_ptr, 0x3U)) {
        Msg("[L4DToolZ] datarate init error.\n");
        return;
    }
    auto net = (int *)g_engine->GetPlayerNetInfo((int)(pEdict - *edict_ptr));
    if (net) net[rate_idx] = g_tickrate * 1000;  // real
}

PLUGIN_RESULT l4dtoolz::ClientConnect(bool *bAllowConnect, edict_t *pEntity, const char *, const char *, char *, int) {
    if (sv_steam_bypass.GetInt() != 1) return PLUGIN_CONTINUE;
    ValidateAuthTicketResponse_t rsp = {(uint64)g_engine->GetPlayerUserId(pEntity)};
#ifdef WIN32
    ((void(__thiscall *)(uintptr_t *, void *))authrsp_org)(steam3_ptr, &rsp);
#else
    ((void (*)(uintptr_t *, void *))authrsp_org)(steam3_ptr, &rsp);
#endif
    if (g_engine->GetPlayerUserId(pEntity) == -1) {
        *bAllowConnect = false;
        return PLUGIN_STOP;
    }
    Msg("[L4DToolZ] %llu validated.\n", rsp.id);
    return PLUGIN_CONTINUE;
}

#ifdef WIN32
static void PostAuth(ValidateAuthTicketResponse_t *rsp)
#else
static void PostAuth(void *, ValidateAuthTicketResponse_t *rsp)
#endif
{
    if (!rsp->code && (rsp->id != rsp->owner)) {
        rsp->code = 2;
        Msg("[L4DToolZ] %llu using family sharing, owner: %llu.\n", rsp->id, rsp->owner);
    }
#ifdef WIN32
    __asm {
        leave
        mov ecx, steam3_ptr
        jmp authrsp_org
    }
#else
    ((void (*)(uintptr_t *, void *))authrsp_org)(steam3_ptr, rsp);
#endif
}

void l4dtoolz::OnAntiSharing(IConVar *var, const char *pOldValue, float flOldValue) {
    CHKVAL
    if (!authrsp_ptr) {
        var->SetValue(0);
        Msg("[L4DToolZ] sv_anti_sharing init error\n");
        return;
    }
    if (new_value) *authrsp_ptr = (uintptr_t)&PostAuth;
    else *authrsp_ptr = authrsp_org;
}
ConVar sv_anti_sharing("sv_anti_sharing", "0", 0, "No family sharing", true, 0, true, 1, l4dtoolz::OnAntiSharing);

// Linux: static void ReplyReservationRequest(void *, void *, void *);
static void ReplyReservationRequest(void *, void *) { }

void l4dtoolz::OnForceUnreserved(IConVar *var, const char *pOldValue, float flOldValue) {
    CHKVAL
    if (!lobby_req_ptr) {
        var->SetValue(0);
        Msg("[L4DToolZ] sv_force_unreserved init error\n");
        return;
    }
    if (new_value) {
        write_sig(lobby_req_ptr, lobby_req_new);
        g_cvar->FindVar("sv_allow_lobby_connect_only")->SetValue(0);
        return;
    }
    write_sig(lobby_req_ptr, lobby_req_org);
}
ConVar sv_force_unreserved("sv_force_unreserved", "0", 0, "Disallow lobby reservation", true, 0, true, 1, l4dtoolz::OnForceUnreserved);

bool l4dtoolz::Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory) {
    g_engine = (IVEngineServer *)interfaceFactory(INTERFACEVERSION_VENGINESERVER, NULL);
    g_cvar = (ICvar *)interfaceFactory(CVAR_INTERFACE_VERSION, NULL);
    g_tickrate = CommandLine()->ParmValue("-tickrate", 30);

    ConnectTier1Libraries(&interfaceFactory, 1);
    ConVar_Register(0);

    if (!gamerules_ptr) {
        auto client = (uintptr_t **)gameServerFactory("ServerGameClients003", NULL);
        auto gamerules = *(uintptr_t **)(client[0][18] + info_off);  // mov
        if (CMPPTR(gamerules, 0x3U, gameServerFactory)) gamerules_ptr = gamerules;
    }
    if (!edict_ptr) {
        auto sv = *(uintptr_t ***)(((uint **)g_engine)[0][0x80] + sv_off);
        if (!CMPPTR(sv, 0x7U, interfaceFactory)) return false;
        edict_ptr = (edict_t **)&sv[edict_idx];
        slots_ptr = (int *)&sv[slots_idx];
        cookie_ptr = (uint64 *)&sv[cookie_idx];
        maxcl_ptr = (int *)&sv[maxcl_idx];
        auto sfunc = (uintptr_t * (*)(void)) READCALL(sv[0][steam3_idx] + steam3_off);
        if (CMPPTR(sfunc, 0xFU, interfaceFactory)) {
            steam3_ptr = sfunc();  // conn
            authrsp_ptr = &steam3_ptr[authrsp_idx];
            authrsp_org = *authrsp_ptr;
        }
        lobby_req_ptr = (uintptr_t)&sv[0][lobbyreq_idx];
        read_sig(lobby_req_ptr, lobby_req_new, lobby_req_org);
        *(uintptr_t *)lobby_req_new->sig = (uintptr_t)&ReplyReservationRequest;
    }
    if (!dsp_max_ptr) {
        auto match = (uintptr_t **)interfaceFactory("MATCHFRAMEWORK_001", NULL);
        auto title = ((uintptr_t * *(*)(void)) match[0][8])();
        dsp_max_ptr = title[0][4];
        read_sig(dsp_max_ptr, max_player_new, dsp_max_org);
    }

    if ((g_tickrate != 30) && !tickint_ptr) {
        auto game = (uintptr_t **)gameServerFactory(INTERFACEVERSION_SERVERGAMEDLL, NULL);
        tickint_ptr = (uintptr_t)&game[0][tickint_idx];
        uint8_t tickint[6] = {0x04, 0x00};
		auto tickint_new = (mem_sig_t *)tickint;
        read_sig(tickint_ptr, tickint_new, tickint_org);
        *(uintptr_t *)tickint_new->sig = (uintptr_t)&GetTickInterval;
        write_sig(tickint_ptr, tickint_new);
        ((uint *)g_cvar->FindVar("net_splitpacket_maxrate"))[15] = 0;  // m_bHasMax
        Msg("[L4DToolZ] tickrate: %d\n", g_tickrate);
    }
    return true;
}

void l4dtoolz::Unload() {
    ConVar_Unregister();
    DisconnectTier1Libraries();

    if (authrsp_ptr) *authrsp_ptr = authrsp_org;
    free_sig(authreq_ptr, authreq_org);
    free_sig(rules_max_ptr, rules_max_org);
    free_sig(dsp_max_ptr, dsp_max_org);
    free_sig(lobby_req_ptr, lobby_req_org);
    free_sig(tickint_ptr, tickint_org);
}
