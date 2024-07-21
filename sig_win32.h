#ifndef SIG_WIN32_H
#define SIG_WIN32_H

#include "sigtool.h"

#define tickint_idx  0x09
#define rate_idx     0x2C
#define info_off     0x02
#define info_idx     0x88
#define sv_off       0x08
#define edict_idx    0x86
#define slots_idx    0x60
#define cookie_idx   0x5C
#define maxcl_idx    0x41
#define steam3_idx   0x40
#define steam3_off   0x0D
#define authreq_idx  0x1A
#define authrsp_idx  0x20
#define lobbyreq_idx 0x3D

uint8_t max_player[] = {0x06, 0x00, 0xB8, '*', 0x00, 0x00, 0x00, 0xC3};
auto max_player_new = (mem_sig_t *)max_player;
uint8_t lobby_req[] = {0x04, 0x00, '*', '*', '*', '*'};
auto lobby_req_new = (mem_sig_t *)lobby_req;

#endif  // SIG_WIN32_H
