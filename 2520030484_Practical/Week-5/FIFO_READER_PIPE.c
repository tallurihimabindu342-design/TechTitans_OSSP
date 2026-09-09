#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>

int main() {
     char bfr[100];
     printf("receiving message from writer\n");
     int fd = open("/tmp/myfifo", O_RDONLY);
     read(fd,bfr,sizeof(bfr));
     printf("received message: %s", bfr);
     close(fd);
     return 0;
}
