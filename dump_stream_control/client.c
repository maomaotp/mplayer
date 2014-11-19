#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <signal.h>

#include "mysql/mysql.h"
#include "client.h"

int socketfd;

void init_socket()
{
    struct sockaddr_in servaddr;
    
    if((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8888);
    servaddr.sin_addr.s_addr = inet_addr("192.168.1.120");
   
    if(connect(socketfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect");
        exit(1);
    }
}

int main()
{
	init_socket();
	char mplayer_pid[20];
	struct task send_task;
	pid_t pid;

	//获取流下载的pid
	recv(socketfd, mplayer_pid, sizeof(mplayer_pid), 0);
	pid = atoi(mplayer_pid);
	printf("pid==%d\n", pid);

	struct playtime *time_list;
	time_list = (struct playtime *)calloc(200, sizeof(struct playtime));
	if(!time_list){
		printf("calloc error\n");
		return -1;
	}

	send_task.radioId = 19;

	queryInfo(&send_task, time_list);

	int i=0;
	while(time_list[i].programId != 0){
		send_task.programId = time_list[i].programId;
		strcpy( send_task.playtime, time_list[i].ptime );

		sleep(10);
	    send(socketfd, (char *)&send_task, sizeof(struct task), 0);
		printf("%d  %d  %s  %s\n", send_task.radioId, send_task.programId, send_task.playtime, send_task.uri);
		//kill(pid, SIGINT);

		i++;
	}

    return 0;
}
