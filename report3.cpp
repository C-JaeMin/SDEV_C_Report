#include <stdio.h>

int main() {
	char origin_str[512];
	char copy_str[512];
	int i, array_size;
	
	printf("Input String >> ");
	scanf("%s", origin_str);
	
	array_size = sizeof(origin_str) / sizeof(origin_str[0]);

	printf("Original String : %s\n", origin_str);

	for (i = 0; i < array_size; i++) {
		copy_str[i] = origin_str[i];
	}

	printf("Copy String : %s", copy_str);
	
	return 0;
}
