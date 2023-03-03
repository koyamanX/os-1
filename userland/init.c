#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main(void) {
	char *buf = "Hello,world from init!\n";

	write(1, buf, strlen(buf));
	open("/usr/sbin/hello.txt", O_CREAT, 0);
	open("/usr/sbin/hello2.txt", O_CREAT, 0);

	return 0;
}
