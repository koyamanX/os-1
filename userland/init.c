#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(void) {
	char *buf = "Hello,world from init!\n";
	dev_t dev;

	dev.major = 0;
	dev.minor = 0;
	write(1, buf, strlen(buf));
	mkdir("/dev", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	mknod("/dev/console", S_IFCHR | S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH, dev);
	open("/usr/sbin/hello2.txt", O_CREAT, 0);

	return 0;
}
