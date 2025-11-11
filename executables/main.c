#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

int main(int arg_count, char *path[]) {
	for (int i = 1; i < arg_count; i++) {
		printf("Current directory: %s\n", path[i]);

		DIR *dir = opendir(path[i]);
		//error opening directory
		if (dir == NULL) {
			perror("Error opening directory");
			continue;
		}

		struct dirent *entry;
		struct stat file_stat;

		//reads directory until no more entries found
		while ((entry = readdir(dir)) != NULL) {
			char full_path[1024];
			snprintf(full_path, sizeof(full_path), "%s/%s", path[i], entry->d_name);

			//checks if file is executable using the stat function
			if (stat(full_path, &file_stat) == 0) {
				if (S_ISREG(file_stat.st_mode) && (file_stat.st_mode & S_IXUSR)) {
					printf("%s\n", entry->d_name);
				}
			}
		}

		closedir(dir);
	}
	return 0;
}