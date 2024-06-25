#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include<arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>

void error(char *msg) {
    perror(msg);
    exit(0);
}

int read_data(int sock, int size, unsigned char *buf) {
   int bytes_read = 0, len = 0;
   while (bytes_read < size && 
         ((len = recv(sock, buf + bytes_read,size-bytes_read, 0)) > 0)) {
       bytes_read += len;
   }
   if (len == 0 || len < 0) 
   {
	printf("errno: %d\n", errno);
	error("recv err");
   }
   return bytes_read;
}

int
main(void)
{
	const int portno = 1234;
	struct hostent *tcp_server;
	struct sockaddr_in tcp_serveraddr;
	int tcp_sockfd;
	char *hostname = "amiro1";

	tcp_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_sockfd < 0)
		error("Opening TCP Socket");
	//tcp_server = gethostbyname(hostname);
	//if (tcp_server == NULL)
	//	error("Err no such host");
	bzero((char *) &tcp_serveraddr, sizeof(tcp_serveraddr));
	tcp_serveraddr.sin_addr.s_addr = inet_addr("129.70.148.139");
	tcp_serveraddr.sin_family = AF_INET;
	//bcopy((char *)tcp_server->h_addr,(char *)&tcp_serveraddr.sin_addr.s_addr, tcp_server->h_length);
	tcp_serveraddr.sin_port = htons(portno);
	printf("connecting\n");
	if (connect(tcp_sockfd,  (struct sockaddr *) &tcp_serveraddr, sizeof(tcp_serveraddr)) < 0)
		error("Err: connecting");
	printf("connected to tcp server!\n");

	int s;
	int nbytes;
	struct sockaddr_can addr;
	struct can_frame frame;
	struct ifreq ifr;

	const char *ifname = "vcan0";

	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) == -1) {
		perror("Error while opening socket");
		return -1;
	}

	strcpy(ifr.ifr_name, ifname);
	ioctl(s, SIOCGIFINDEX, &ifr);

	addr.can_family  = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	printf("%s at index %d\n", ifname, ifr.ifr_ifindex);

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		perror("Error in socket bind");
		return -2;
	}
	printf("connected to can dev\n");

	ssize_t bytes_read;
	const int can_size=20;
	unsigned char tcp_buf[can_size];
	while (1)
	{
		bytes_read = read_data(tcp_sockfd, can_size, &tcp_buf[0]);
		for (int i=0;i<can_size;i++) {
			printf("%3d ", tcp_buf[i]);
		}
		printf("\n");
		//bytes_read = recv(tcp_sockfd, &frame, can_size,MSG_WAITALL);
		if (bytes_read==-1)
			continue;
		if (bytes_read != can_size)
		{
			printf("Err read: %ld bytes expect %d Dropping!?\n", bytes_read, can_size);
		}
		frame.can_id = ((canid_t*)tcp_buf)[1];
		//printf("%u %u %u %u\n", tcp_buf[4], tcp_buf[5], tcp_buf[6], tcp_buf[7]);
		unsigned char extd         = 0b00000001&tcp_buf[0];
		unsigned char rtr          = 0b00000010&tcp_buf[0];
		unsigned char ss           = 0b00000100&tcp_buf[0];
		unsigned char self         = 0b00001000&tcp_buf[0];
		unsigned char dlc_non_comp = 0b00010000&tcp_buf[0];

		frame.can_id |= extd<<31 | rtr<<30;
		frame.can_dlc=tcp_buf[8];
		frame.data[0] =tcp_buf[9];
		frame.data[1] =tcp_buf[10];
		frame.data[2] =tcp_buf[11];
		frame.data[3] =tcp_buf[12];
		frame.data[4] =tcp_buf[13];
		frame.data[5] =tcp_buf[14];
		frame.data[6] =tcp_buf[15];
		frame.data[7] =tcp_buf[16];

		nbytes = write(s, &frame, sizeof(struct can_frame));
		printf("Wrote %d bytes\n", nbytes);
		//sleep(1);
	}
	return 0;
}
