#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(void) {
	char *buf = "Hello,world from init!\n";
	dev_t dev;
	int fd;

	dev.major = 0;
	dev.minor = 0;
	fd = mknod("/console", S_IFCHR | S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH, dev);
	write(fd, buf, strlen(buf));
	open("/usr/sbin/hello2.txt", O_CREAT, 0);

	return 0;
}
