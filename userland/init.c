#include <unistd.h>
#include <string.h>

int main(void) {
	char *buf = "Hello,world from init!\n";

	write(1, buf, strlen(buf));

	return 0;
}
