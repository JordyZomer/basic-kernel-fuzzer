#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/random.h>
#include <time.h>
#include <unistd.h>

enum {
	FUZZ_ATTEMPTS = 300,
};

void random_bytes(unsigned char *dest, size_t size)
{
	for (size_t i = 0; i < size; ++i)
		dest[i] = rand() % 94 + 33;
}

void fuzz(const char *path)
{
	int fd;
	unsigned char data[4092];
	ssize_t ret;

	fd = open(path, O_RDWR);
	if (fd == -1) {
		fprintf(stderr, "Could not open %s: %s\n", path, strerror(errno));
		return;
	}

	random_bytes(data, sizeof data);

	ret = write(fd, data, sizeof data);
	close(fd);
	if (ret == -1) return;
	printf("Success writing %zd bytes to %s\n", ret, path);

	return;
}

void get_files(void)
{
	FILE *fp;
	char path[2048];

	fp = popen("find /sys/ -type f", "r");
	if (fp == NULL) {
		fprintf(stderr, "fatal: Could not find files\n");
		exit(EXIT_FAILURE);
	}

	while (fgets(path, sizeof path, fp) != NULL) {
		strtok(path, "\n");
		printf("Fuzzing %s\n", path);
		for (int i = 0; i < FUZZ_ATTEMPTS; i++)
			fuzz(path);
	}

	pclose(fp);
}

int main(void)
{
	srand(time(NULL) + getpid());
	get_files();
	return 0;
}
