#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int arg_count, char *path[]) {
	char command[1024];
	for (int i = 1; i < arg_count; i++) {
		strcpy(command, "ls -la ");
		strcat(command, path[i]);
		printf("Current directory: %s\n", path[i]);
		strcat(command, " | grep -E ^.{3}x");
		system(command);
	}

	return 0;
}
