#ifndef _INCLUDE_L4D2_SIGNATURE_WIN32_
#define _INCLUDE_L4D2_SIGNATURE_WIN32_

const char *srv_dll[] = {"server.dll", 0};
const char *eng_dll[] = {"engine.dll", 0};
const char *mat_dll[] = {"matchmaking_ds.dll", "matchmaking.dll", 0};

const char *info_players = "\x07\xB8\x08\x00\x00\x00\x7F\x05";
unsigned char info_players_new[] = {0x06, 0xBA, 0xB8, 0x3C, 0x00, 0x00, 0x00, 0xC3};

const char *lobby_match = "\x06\xCC\xB8\x08\x00\x00\x00";
unsigned char lobby_match_new[] = {0x01, 0x02, 0x3C};

const char *cookie = "\x07\x3B\xBE\x70\x01\x00\x00\x75";
const int cookie_off = -31;
const int sv_off = 13;

const char *maxslots = "\x07\x3B\x86\x80\x01\x00\x00\x7E";
unsigned char maxslots_new[] = {0x06, 0x00, 0x83, 0xF8, 0x3C, 0x90, 0x90, 0x90};

const char *slots_check_new = "\x02\xE2\x90\x90";

const char *players_range = "\x09\x8B\x86\x28\x02\x00\x00\x8B\x4D\x08";
const char *players_range_new = "\x06\x00\xB8\x1F\x00\x00\x00\x90";

const char *players_running = "\x0A\x8B\xE5\x5D\xC3\x8B\x80\x0C\x04\x00\x00";
const char *players_running_new = "\x01\xF0\xEB";

const char *allow_cheats = "\x09\x68\x00\x40\x00\x00\x8B\xCF\xFF\xD2";
const char *allow_cheats_new = "\x01\x0B\xEB";
#endif //_INCLUDE_L4D2_SIGNATURE_WIN32_
