#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>


static char **dynamicArray(char **array, int *counter, const char *word) {
	char **newArray = realloc(array, (*counter + 1) * sizeof(char *));
	newArray[*counter] = strdup(word);
	(*counter)++;
	return newArray;
}

static int sortNames(const void *a, const void *b) {
	const char *const *left = a;
	const char *const *right = b;
	return strcmp(*left, *right);
}


int main(int arg_count, char *path[]) {
	for (int i = 1; i < arg_count; i++) {
		printf("Current directory: %s\n", path[i]);

		DIR *dir = opendir(path[i]);

		struct dirent *entry;
		struct stat file_stat;
		char **names = NULL;
		int counter = 0;

		while ((entry = readdir(dir)) != NULL) {
			char full_path[1024];
			strcpy(full_path, path[i]);
			strcat(full_path, "/");
			strcat(full_path, entry->d_name);

			if (stat(full_path, &file_stat) == 0) {
				if (S_ISREG(file_stat.st_mode) && (file_stat.st_mode & S_IXUSR)) {
					names = dynamicArray(names, &counter, entry->d_name);
				}
			}
		}

		if (counter > 1) {
			qsort(names, counter, sizeof(char *), sortNames);
		}

		for (int j = 0; j < counter; j++) {
			printf("%s\n", names[j]);
			free(names[j]);
		}
		free(names);

		closedir(dir);
	}
	return 0;
}

