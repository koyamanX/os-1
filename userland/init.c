#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(void) {
	char *buf = "Hello,world from init!\n";

	write(1, buf, strlen(buf));
	mkdir("/dev", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	open("/usr/sbin/hello2.txt", O_CREAT, 0);

	return 0;
}
