#ifndef SIG_WIN32_H
#define SIG_WIN32_H

#define FUNC_T __stdcall
#define HOOK_DEF(ret, func, ...) static ret FUNC_T func(__VA_ARGS__)
#define CALL(ret, ptr, ...) ((ret (__thiscall *)(void *, __VA_ARGS__))ptr)

#define info_idx    0x88
#define rules_off   0x02
#define sv_off      0x08
#define slots_idx   0x60
#define cookie_idx  0x5C
#define edict_idx   0x87
#define client_idx  0x06
#define lobby_idx   0x3D
#define check_idx   0x39
#define steam3_idx  0x40
#define steam3_off  0x0D
#define authrsp_idx 0x20

#endif  // SIG_WIN32_H
