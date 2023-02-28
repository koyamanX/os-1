#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main(void) {
	char *buf = "Hello,world from init!\n";

	write(1, buf, strlen(buf));
	open("hello.txt", O_CREAT, 0);
	open("hello2.txt", O_CREAT, 0);

	return 0;
}
