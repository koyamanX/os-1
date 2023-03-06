#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

int main(void) {
	char *buf = "Hello,world from init!\n";
	dev_t dev;
	int pid;

	dev.major = 0;
	dev.minor = 0;
	mknod("/console", S_IFCHR | S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH, dev);
	dup(0);
	dup(0);

	pid = fork();
	if(pid == 0) {
		write(STDOUT_FILENO, "child\n", 6);
		while(1)
			asm volatile("nop");
	} else {
		write(STDOUT_FILENO, "parent\n", 7);
	}
	write(STDOUT_FILENO, buf, strlen(buf));

	return 0;
}
