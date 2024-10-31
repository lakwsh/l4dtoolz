#ifndef SIG_LINUX_H
#define SIG_LINUX_H

#include "sigtool.h"

#define FUNC_T __cdecl
#define HOOK_DEF(ret, func, ...) static ret FUNC_T func(void *, __VA_ARGS__)
#define CALL(ret, ptr, ...) ((ret (*)(__VA_ARGS__))ptr)

#define rules_off    0x01
#define info_idx     0x89  // rodata
#define sv_off       0x06  // bss
#define edict_idx    0x85
#define slots_idx    0x5F
#define cookie_idx   0x5B
#define steam3_idx   0x41
#define steam3_off   0x0D
#define authrsp_idx  0x24  // bss
#define lobbyreq_idx 0x3E  // rodata

#endif  // SIG_LINUX_H
