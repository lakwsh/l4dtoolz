#ifndef SIG_LINUX_H
#define SIG_LINUX_H

#define FUNC_T __cdecl
#define HOOK_DEF(ret, func, ...) static ret FUNC_T func(void *, __VA_ARGS__)
#define CALL(ret, ptr, ...) ((ret (*)(void *, __VA_ARGS__))ptr)

#define info_idx    0x89  // rodata
#define rules_off   0x01  // text (mov)
#define sv_off      0x06  // text (mov)
#define slots_idx   0x5F  // bss (sv)
#define cookie_idx  0x5B  // bss (sv)
#define edict_idx   0x85  // bss (sv)
#define client_idx  0x07  // rodata (sv)
#define lobby_idx   0x3E  // rodata (sv)
#define check_idx   0x3A  // rodata (sv)
#define steam3_idx  0x41  // rodata (sv)
#define steam3_off  0x0F  // text (call)
#define authrsp_idx 0x25  // bss

#endif  // SIG_LINUX_H