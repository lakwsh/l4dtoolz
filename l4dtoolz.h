#ifndef L4DTOOLZ_H
#define L4DTOOLZ_H

#include "eiface.h"
#include "interface.h"
#include "tier1/tier1.h"

class l4dtoolz : public IServerPluginCallbacks {
public:
    virtual bool Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory);
    virtual void Unload();
    virtual void Pause() { }
    virtual void UnPause() { }
    virtual const char *GetPluginDescription() { return "L4DToolZ(2155) v2.3.2, https://github.com/lakwsh/l4dtoolz"; }
    virtual void LevelInit(char const *pMapName) { }
    virtual void ServerActivate(edict_t *pEdictList, int edictCount, int clientMax);
    virtual void GameFrame(bool simulating) { }
    virtual void LevelShutdown() { }
    virtual void ClientActive(edict_t *pEntity) { }
    virtual void ClientDisconnect(edict_t *pEntity) { }
    virtual void ClientPutInServer(edict_t *pEntity, char const *playername) { }
    virtual void SetCommandClient(int index) { }
    virtual void ClientSettingsChanged(edict_t *pEdict);
    virtual PLUGIN_RESULT ClientConnect(bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen);
    virtual PLUGIN_RESULT ClientCommand(edict_t *pEntity, const CCommand &args) { return PLUGIN_CONTINUE; }
    virtual PLUGIN_RESULT NetworkIDValidated(const char *pszUserName, const char *pszNetworkID) { return PLUGIN_CONTINUE; }
    virtual void OnQueryCvarValueFinished(QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue) { }

    static void Cookie_f(const CCommand &args);
    static void OnChangeMax(IConVar *var, const char *pOldValue, float flOldValue);
    static void OnSetMaxCl(IConVar *var, const char *pOldValue, float flOldValue);
    static void OnBypassAuth(IConVar *var, const char *pOldValue, float flOldValue);
    static void OnAntiSharing(IConVar *var, const char *pOldValue, float flOldValue);
    static void OnForceUnreserved(IConVar *var, const char *pOldValue, float flOldValue);
};
extern l4dtoolz g_l4dtoolz;

#endif  // L4DTOOLZ_H
