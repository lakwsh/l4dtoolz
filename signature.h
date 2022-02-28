#ifndef _INCLUDE_SIGNATURE_
#define _INCLUDE_SIGNATURE_

typedef unsigned int uint;
typedef struct{
	void *addr;
	uint len;
} mem_info;

void *find_signature(const char *mask, mem_info *base_addr, bool pure = false);
void *get_func(void *addr, const char *func);
void find_base_from_list(const char *name[], mem_info *base_addr);
void write_signature(void *addr, const void *signature);
void get_original_signature(void *offset, const void *new_sig, void *&org_sig);
void safe_free(void *addr, void *&signature);
#endif //_INCLUDE_SIGNATURE_
