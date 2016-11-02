#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lkl.h>
#include <lkl_host.h>

void * get_sym(void *lib, const char* sym) {
	void *addr = dlsym(lib, sym);
	char *error = dlerror();
	if (error != NULL) {
		fprintf(stderr, "get_sym: %s\n", error);
		exit(EXIT_FAILURE);
	}
	return addr;
}

void *liblkl[2] = {NULL, NULL};

void* start_lkl(void *arg) {
	int num = *((int*)arg);
	if (liblkl[num] == NULL) {
		liblkl[num] = dlmopen(LM_ID_NEWLM, "./liblkl.so", RTLD_NOW);
		if (!liblkl[num]) {
			fprintf(stderr, "dlmopen: %s\n", dlerror());
			exit(EXIT_FAILURE);
		}

		struct lkl_host_operations* const lkl_host_ops =
				(struct lkl_host_operations*)
				get_sym(liblkl[num], "lkl_host_ops");
		unsigned int key = 100;
		printf("num = %d\n", num);
		lkl_host_ops->tls_alloc(&key, NULL);
		printf("tls_alloc key = %u\n", key);
		unsigned long value = (unsigned long)lkl_host_ops->tls_get(key);
		printf("tls_get value = %lu\n", value);
		value = num + 1000;
		lkl_host_ops->tls_set(key, (void*)value);
		value = (unsigned long)lkl_host_ops->tls_get(key);
		printf("tls_get value = %lu\n", value);
	}
}

int main() {
	int num = 0;
	start_lkl(&num);
	num = 1;
	start_lkl(&num);
	return 0;
}

