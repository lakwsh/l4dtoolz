#include "l4dtoolz.h"
#ifdef WIN32
#include "signature_win32.h"
#else
#include "signature_linux.h"
#endif

l4dtoolz g_l4dtoolz;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(l4dtoolz, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS, g_l4dtoolz);

IVEngineServer *engine = NULL;
ICvar *icvar = NULL;
uint tickrate = 30;

void *l4dtoolz::sv_ptr = NULL;
uint *l4dtoolz::slots_ptr = NULL;
uint64 *l4dtoolz::cookie_ptr = NULL;
uint *l4dtoolz::maxcl_ptr = NULL;
uint *l4dtoolz::gamerules_ptr = NULL;
void *l4dtoolz::rules_max_ptr = NULL;
void *l4dtoolz::rules_max_org = NULL;
void *l4dtoolz::dsp_max_ptr = NULL;
void *l4dtoolz::dsp_max_org = NULL;
void *l4dtoolz::lobby_req_ptr = NULL;
void *l4dtoolz::lobby_req_org = NULL;
uint *l4dtoolz::steam3_ptr = NULL;
void *l4dtoolz::authreq_ptr = NULL;
void *l4dtoolz::authreq_org = NULL;
uint *l4dtoolz::authrsp_ptr = NULL;
uint l4dtoolz::authrsp_org = 0;
uint *l4dtoolz::tickint_ptr = NULL;
void *l4dtoolz::tickint_org = NULL;
void *l4dtoolz::set_rate_ptr = NULL;
void *l4dtoolz::set_rate_org = NULL;
void *l4dtoolz::vomit_fix_buf = NULL;
void *l4dtoolz::vomit_fix_ptr1 = NULL;
void *l4dtoolz::vomit_fix_org1 = NULL;
#ifdef WIN32
void *l4dtoolz::vomit_fix_ptr2 = NULL;
void *l4dtoolz::vomit_fix_org2 = NULL;
#endif

void l4dtoolz::OnChangeMax(IConVar *var, const char *pOldValue, float flOldValue){
	if(!slots_ptr){
	err_rules:
		var->SetValue(-1);
		Msg("[L4DToolZ] sv_maxplayers init error\n");
		return;
	}
	if(!rules_max_ptr){
		if(!gamerules_ptr || !CHKPTR(*gamerules_ptr, 0x7)) goto err_rules; // malloc
		rules_max_ptr = ((uint ****)gamerules_ptr)[0][0][info_idx];
		read_signature(rules_max_ptr, max_player_new, rules_max_org);
	}
	int new_value = ((ConVar *)var)->GetInt();
	int old_value = atoi(pOldValue);
	if(new_value==old_value) return;
	if(new_value<0){
		write_signature(rules_max_ptr, rules_max_org);
		write_signature(dsp_max_ptr, dsp_max_org);
		return;
	}
	*slots_ptr = new_value;
	max_player_new[3] = (unsigned char)new_value;
	write_signature(rules_max_ptr, max_player_new);
	if(!dsp_max_ptr) Msg("[L4DToolZ] sv_maxplayers(dsp) init error\n");
	else write_signature(dsp_max_ptr, max_player_new);
}
ConVar sv_maxplayers("sv_maxplayers", "-1", 0, "Max human players", true, -1, true, 31, l4dtoolz::OnChangeMax);

ConVar sv_lobby_cookie("sv_lobby_cookie", "0", 0);
void l4dtoolz::Cookie_f(const CCommand &args){
	if(!cookie_ptr){
		Msg("[L4DToolZ] sv_cookie init error\n");
		return;
	}
	if(*cookie_ptr){
		char buf[20];
		snprintf(buf, sizeof(buf), "%llu", *cookie_ptr);
		sv_lobby_cookie.SetValue(buf);
	}
	if(args.ArgC()!=2){
		engine->ServerCommand("sv_lobby_cookie\n");
		return;
	}
	uint64 val = atoll(args[1]);
	icvar->FindVar("sv_hosting_lobby")->SetValue(val!=0);
	icvar->FindVar("sv_allow_lobby_connect_only")->SetValue(val!=0);
	*cookie_ptr = val;
}
ConCommand cookie("sv_cookie", l4dtoolz::Cookie_f, "Lobby reservation cookie");

void l4dtoolz::OnSetMaxCl(IConVar *var, const char *pOldValue, float flOldValue){
	if(!maxcl_ptr){
		Msg("[L4DToolZ] sv_setmax init error\n");
		return;
	}
	int new_value = ((ConVar *)var)->GetInt();
	int old_value = atoi(pOldValue);
	if(new_value==old_value) return;
	*maxcl_ptr = new_value;
	Msg("[L4DToolZ] maxplayers set to %d\n", new_value);
}
ConVar sv_setmax("sv_setmax", "18", 0, "Max clients", true, 18, true, 32, l4dtoolz::OnSetMaxCl);

void l4dtoolz::LevelInit(char const *){
	if(!slots_ptr) return;
	int slots = sv_maxplayers.GetInt();
	if(slots>=0) *slots_ptr = slots;
}

// Linux: float GetTickInterval(void *);
float GetTickInterval(){
	static float tickinv = 1.0/tickrate;
	return tickinv;
}

#ifdef WIN32
int PreAuth(const void *, int, uint64 steamID){
#else
int PreAuth(void *, const void *, int, uint64 steamID){
#endif
	if(!steamID){
		Msg("[L4DToolZ] invalid steamID.\n");
		return 1;
	}
	Msg("[L4DToolZ] %llu connected.\n", steamID);
	return 0;
}

void l4dtoolz::OnBypassAuth(IConVar *var, const char *pOldValue, float flOldValue){
	if(!steam3_ptr){
	err_bypass:
		var->SetValue(0);
		Msg("[L4DToolZ] sv_steam_bypass init error\n");
		return;
	}
	int new_value = ((ConVar *)var)->GetInt();
	int old_value = atoi(pOldValue);
	if(new_value==old_value) return;
	unsigned char authreq_new[6] = {0x04, 0x00};
	if(!authreq_ptr){
		auto gsv = (uint **)steam3_ptr[1];
		if(!CHKPTR(gsv, 0xf)) goto err_bypass;
		authreq_ptr = &gsv[0][authreq_idx];
		read_signature(authreq_ptr, authreq_new, authreq_org);
	}
	*(uint *)&authreq_new[2] = (uint)&PreAuth;
	if(new_value) write_signature(authreq_ptr, authreq_new);
	else write_signature(authreq_ptr, authreq_org);
}
ConVar sv_steam_bypass("sv_steam_bypass", "0", 0, "Bypass steam validation", true, 0, true, 1, l4dtoolz::OnBypassAuth);

PLUGIN_RESULT l4dtoolz::ClientConnect(bool *bAllowConnect, edict_t *pEntity, const char *, const char *, char *, int){
	if(!set_rate_ptr && tickrate!=30){
		auto net = engine->GetPlayerNetInfo(1);
		if(net){
			set_rate_ptr = ((uint ***)net)[0][setrate_idx];
			read_signature(set_rate_ptr, set_rate_new, set_rate_org);
			*(uint *)&set_rate_new[3] = tickrate*1000;
			write_signature(set_rate_ptr, set_rate_new);
		}
	}
	if(sv_steam_bypass.GetInt()!=1) return PLUGIN_CONTINUE;
	const CSteamID *steamID = engine->GetClientSteamID(pEntity);
	if(!steamID){
		Msg("[L4DToolZ] invalid steamID.\n");
	reject:
		*bAllowConnect = false;
		return PLUGIN_STOP;
	}
	ValidateAuthTicketResponse_t rsp = {*(uint64 *)steamID};
#ifdef WIN32
	((void (__thiscall *)(void *, void *))authrsp_org)(steam3_ptr, &rsp);
#else
	((void (*)(void *, void *))authrsp_org)(steam3_ptr, &rsp);
#endif
	if(engine->GetPlayerUserId(pEntity)==-1) goto reject;
	Msg("[L4DToolZ] %llu validated.\n", rsp.id);
	return PLUGIN_CONTINUE;
}

#ifdef WIN32
void l4dtoolz::PostAuth(ValidateAuthTicketResponse_t *rsp){
#else
void l4dtoolz::PostAuth(void *, ValidateAuthTicketResponse_t *rsp){
#endif
	if(rsp->id!=rsp->owner){
		rsp->code = 2;
		Msg("[L4DToolZ] %llu using family sharing, owner: %llu.\n", rsp->id, rsp->owner);
	}
#ifdef WIN32
	__asm{
		leave
		mov ecx, steam3_ptr
		jmp authrsp_org
	}
#else
	((void (*)(void *, void *))authrsp_org)(steam3_ptr, rsp);
#endif
}

void l4dtoolz::OnAntiSharing(IConVar *var, const char *pOldValue, float flOldValue){
	if(!authrsp_ptr){
		var->SetValue(0);
		Msg("[L4DToolZ] sv_anti_sharing init error\n");
		return;
	}
	int new_value = ((ConVar *)var)->GetInt();
	int old_value = atoi(pOldValue);
	if(new_value==old_value) return;
	if(new_value) *authrsp_ptr = (uint)&PostAuth;
	else *authrsp_ptr = authrsp_org;
}
ConVar sv_anti_sharing("sv_anti_sharing", "0", 0, "No family sharing", true, 0, true, 1, l4dtoolz::OnAntiSharing);

// Linux: void ReplyReservationRequest(void *, void *, void *);
void ReplyReservationRequest(void *, void *){
	return;
}

void l4dtoolz::OnForceUnreserved(IConVar *var, const char *pOldValue, float flOldValue){
	if(!lobby_req_ptr){
		var->SetValue(0);
		Msg("[L4DToolZ] sv_force_unreserved init error\n");
		return;
	}
	int new_value = ((ConVar *)var)->GetInt();
	int old_value = atoi(pOldValue);
	if(new_value==old_value) return;
	if(new_value){
		write_signature(lobby_req_ptr, lobby_req_new);
		icvar->FindVar("sv_allow_lobby_connect_only")->SetValue(0);
		return;
	}
	write_signature(lobby_req_ptr, lobby_req_org);
}
ConVar sv_force_unreserved("sv_force_unreserved", "0", 0, "Disallow lobby reservation", true, 0, true, 1, l4dtoolz::OnForceUnreserved);

bool l4dtoolz::Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory){
	engine = (IVEngineServer *)interfaceFactory(INTERFACEVERSION_VENGINESERVER, NULL);
	icvar = (ICvar *)interfaceFactory(CVAR_INTERFACE_VERSION, NULL);
	tickrate = CommandLine()->ParmValue("-tickrate", 30);

	ConnectTier1Libraries(&interfaceFactory, 1);
	ConVar_Register(0);

	mem_info base = {NULL, 0};

	find_base_from_list(srv_dll, &base);
	if(!gamerules_ptr){
		auto client = (uint **)gameServerFactory("ServerGameClients003", NULL);
		auto gamerules = *(uint **)(client[0][18]+info_off);
		if(CMPPTR(gamerules, 0x3)) gamerules_ptr = gamerules;
	}
	if(tickrate!=30){
		vomit_fix_ptr1 = find_signature(vomit_fix, &base);
		if(!vomit_fix_ptr1 || !(vomit_fix_buf = malloc(0x14))){
			Msg("[L4DToolZ] vomit_fix init error\n");
			goto err_vomit;
		}
		((float *)vomit_fix_buf)[4] = 1.0/30; // 0x10
		*(uint *)&vomit_fix_new[3] = (uint)vomit_fix_buf;
		read_signature(vomit_fix_ptr1, vomit_fix_new, vomit_fix_org1);
		write_signature(vomit_fix_ptr1, vomit_fix_new);
	#ifdef WIN32
		base.len -= (uint)vomit_fix_ptr1+1-(uint)base.addr;
		base.addr = (void *)((uint)vomit_fix_ptr1+1);
		vomit_fix_ptr2 = find_signature(vomit_fix, &base);
		read_signature(vomit_fix_ptr2, vomit_fix_new, vomit_fix_org2);
		write_signature(vomit_fix_ptr2, vomit_fix_new);
	#endif
	}
err_vomit:

	//find_base_from_list(mat_dll, &base);
	if(!dsp_max_ptr){
		auto match = (uint **)interfaceFactory("MATCHFRAMEWORK_001", NULL);
		auto title = ((uint ***(*)())match[0][8])();
		dsp_max_ptr = title[0][4];
		read_signature(dsp_max_ptr, max_player_new, dsp_max_org);
	}

	find_base_from_list(eng_dll, &base);
	if(!sv_ptr){
		uint **sv = *(uint ***)(((uint **)engine)[0][0x80]+sv_off);
		if(!CMPPTR(sv, 0xf)) goto err_sv;
		sv_ptr = sv;
		slots_ptr = (uint *)&sv[slots_idx];
		cookie_ptr = (uint64 *)&sv[cookie_idx];
		maxcl_ptr = (uint *)&sv[maxcl_idx];
		auto sfunc = (uint *(*)(void))READCALL(sv[0][steam3_idx]+steam3_off);
		if(CMPPTR(sfunc, 0xf)){
			steam3_ptr = sfunc(); // conn
			authrsp_ptr = &steam3_ptr[authrsp_idx];
			authrsp_org = *authrsp_ptr;
		}
		lobby_req_ptr = &sv[0][lobbyreq_idx];
		read_signature(lobby_req_ptr, lobby_req_new, lobby_req_org);
		*(uint *)&lobby_req_new[2] = (uint)&ReplyReservationRequest;
	}
err_sv:

	if(tickrate==30) return true;
	if(!tickint_ptr){
		auto game = (uint **)gameServerFactory(INTERFACEVERSION_SERVERGAMEDLL, NULL);
		tickint_ptr = &game[0][tickint_idx];
		unsigned char tickint_new[6] = {0x04, 0x00};
		read_signature(tickint_ptr, tickint_new, tickint_org);
		*(uint *)&tickint_new[2] = (uint)&GetTickInterval;
		write_signature(tickint_ptr, tickint_new);
	}
	((uint *)icvar->FindVar("net_splitpacket_maxrate"))[15] = false; // m_bHasMax
	Msg("[L4DToolZ] tickrate: %d\n", tickrate);

	engine->ServerCommand("sv_setmax 31\n");
	return true;
}

void l4dtoolz::Unload(){
	ConVar_Unregister();
	DisconnectTier1Libraries();

	if(authrsp_ptr) *authrsp_ptr = authrsp_org;
	free_signature(authreq_ptr, authreq_org);
	free_signature(rules_max_ptr, rules_max_org);
	free_signature(dsp_max_ptr, dsp_max_org);
	free_signature(lobby_req_ptr, lobby_req_org);
	free_signature(tickint_ptr, tickint_org);
	free_signature(set_rate_ptr, set_rate_org);
	if(vomit_fix_buf){
		free_signature(vomit_fix_ptr1, vomit_fix_org1);
	#ifdef WIN32
		free_signature(vomit_fix_ptr2, vomit_fix_org2);
	#endif
		free(vomit_fix_buf);
		vomit_fix_buf = NULL;
	}
}
