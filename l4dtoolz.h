#ifndef L4DTOOLZ_H
#define L4DTOOLZ_H

#include "iserverplugin.h"

class l4dtoolz : public IServerPluginCallbacks {
public:
    virtual bool Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory);
    virtual void Unload();
    virtual void Pause() { }
    virtual void UnPause() { }
    virtual const char *GetPluginDescription() { return "L4DToolZ v2.4.1, https://github.com/lakwsh/l4dtoolz"; }
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
};
extern l4dtoolz g_l4dtoolz;

#endif  // L4DTOOLZ_H
