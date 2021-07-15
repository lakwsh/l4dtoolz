#ifndef _INCLUDE_L4D2_SIGNATURE_LINUX_
#define _INCLUDE_L4D2_SIGNATURE_LINUX_

const char *srv_dll[] = {"server_srv.so", 0};
const char *eng_dll[] = {"engine_srv.so", 0};
const char *mat_dll[] = {"matchmaking_ds_srv.so", 0};

const char *info_players = "\x08\xC9\x3C\x01\x19\xC0\x83\xE0\xFC";
unsigned char info_players_new[] = {0x06, 0xF5, 0xB8, 0x3C, 0x00, 0x00, 0x00, 0xC3};

const char *lobby_match = "\x09\x55\xB8\x08\x00\x00\x00\x89\xE5\x5D";
unsigned char lobby_match_new[] = {0x01, 0x02, 0x3C};

const char *cookie = "\x08\x31\xFF\x8B\x45\x0C\x8B\x75\x10";
const int cookie_off = -25;
const int sv_off = 9;

const char *maxslots = "\x08\x3B\x9F\x7C\x01\x00\x00\x0F\x8E";
unsigned char maxslots_new[] = {0x06, 0x00, 0x83, 0xFB, 0x3C, 0x90, 0x90, 0x90};

const char *slots_check = "\x07\xFF\x50\x70\x84\xC0\x0F\x84";
const char *slots_check_new = "\x06\x0B\xBE\x01\x00\x00\x00\x90";

const char *players_range = "\x0C\x8B\x83\xC3\xC3\x00\x00\x39\xD0\x7C\x0B\x8B\x83";
const char *players_range_new = "\x06\x00\xB8\x1F\x00\x00\x00\x90";

const char *players_running = "\x08\x7F\xC3\x8B\x80\xC3\xC3\x00\x00";
const char *players_running_new = "\x02\x00\x90\x90";

const char *allow_cheats = "\x07\xFF\x50\x0C\x84\xC0\x89\xC6";
const char *allow_cheats_new = "\x03\x00\x30\xC0\x90";
#endif //_INCLUDE_L4D2_SIGNATURE_LINUX_
