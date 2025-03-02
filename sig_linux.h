#ifndef SIG_LINUX_H
#define SIG_LINUX_H

#define FUNC_T __cdecl
#define HOOK_DEF(ret, func, ...) static ret FUNC_T func(void *, __VA_ARGS__)
#define CALL(ret, ptr, ...) ((ret (*)(void *, __VA_ARGS__))ptr)

#define info_idx    0x89  // rodata
#define rules_off   0x01
#define sv_off      0x06  // bss
#define slots_idx   0x5F
#define cookie_idx  0x5B
#define edict_idx   0x85
#define client_idx  0x07
#define lobby_idx   0x3E  // rodata
#define check_idx   0x3A  // rodata
#define steam3_idx  0x41
#define steam3_off  0x0D
#define authrsp_idx 0x24  // bss

#endif  // SIG_LINUX_H
