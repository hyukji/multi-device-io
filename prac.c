#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#ifdef __WINDOWS__
	int OS = 1;
#elif __unix__
	int OS = 2;
#endif

int main(int argc, char* argv[])
{          
printf("OS %d", OS);
}
