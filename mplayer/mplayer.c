/*
 * This file is part of MPlayer.
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>

#include "stream/pvr.h"

//liuqiang  add
int socketfd, clientfd;
struct task
{
	int radioId;
	int programId;
	char playtime[20];
	char uri[100];
};

int enable_mouse_movements;

int osd_duration = 1000;

static off_t seek_to_byte;


typedef struct {
  double pos;
  int type;
} m_time_size_t;

static m_time_size_t end_at = { .type = 0, .pos = 0 };


// streaming:
int audio_id  = -1;
int video_id  = -1;
int dvdsub_id = -1;
// this dvdsub_id was selected via slang
// use this to allow dvdnav to follow -slang across stream resets,
// in particular the subtitle ID for a language changes
int dvdsub_lang_id;
int vobsub_id = -1;
char *audio_lang;
char *dvdsub_lang;
char *filename;
int file_filter = 1;

// cache2:
int stream_cache_size = -1;

float stream_cache_min_percent      = 20.0;
float stream_cache_seek_min_percent = 50.0;

// dump:
char *stream_dump_name = NULL;
int stream_dump_type;
uint64_t stream_dump_count;
unsigned stream_dump_start_time;
unsigned stream_dump_last_print_time;
int capture_dump;

static int softsleep;

// sub:
char *font_name;
char *sub_font_name;
float font_factor = 0.75;
char **sub_name;
char **sub_paths;
float sub_delay;
float sub_fps;
int sub_auto = 1;
char *vobsub_name;
int subcc_enabled;
int suboverlap_enabled = 1;

//static unsigned int initialized_flags;

static void exit_sighandler(int x)
{
}


static void stream_dump_progress(uint64_t len, stream_t *stream)
{
   // uint64_t start = stream->start_pos;
   // uint64_t end   = stream->end_pos;
   // uint64_t pos   = stream->pos;

    stream_dump_count += len;

	printf(">>>stream_dump_count=%u\n", stream_dump_count);
}

static int is_at_end(stream_t *mystream, m_time_size_t *end_at, double pts)
{
    switch (end_at->type) {
    case 1: return pts != (-1LL<<63) && end_at->pos <= pts;
    case 2: return end_at->pos <= stream_tell(mystream);
    }
    return 0;
}

void socket_init()
{
	struct sockaddr_in servaddr, clientaddr;

	if( (socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
		printf("error\n");
		exit(-1);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8888);
    servaddr.sin_addr.s_addr = inet_addr("192.168.1.120");

    if(bind(socketfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind");
        exit(1);
    }

    listen(socketfd, 5);
    printf(">>now listen and wait connected\n");

    int recv_len = sizeof(clientaddr);
    if((clientfd = accept(socketfd, (struct sockaddr *)&clientaddr, &recv_len)) < 0) {
        perror("accept");
        exit(1);
    }
	return;
}

int main(int argc, char *argv[])
{
	struct task programInfo;
	char filename[120];

	socket_init();
	
	//将pid发送给client端
	char str[20];
	snprintf(str, sizeof(str), "%d", getpid());
	send( clientfd, str, sizeof(str), 0 );

    recv(clientfd, (char *)&programInfo, sizeof(programInfo), 0);
	snprintf(filename, sizeof(filename), "%d_%d_%s.asf", programInfo.radioId, programInfo.programId, programInfo.playtime);

	printf("filename = %s\n", filename);

    signal(SIGTERM, exit_sighandler); // kill
    signal(SIGHUP, exit_sighandler); // kill -HUP  /  xterm closed
    signal(SIGINT, exit_sighandler); // Interrupt from keyboard
    signal(SIGQUIT, exit_sighandler); // Quit from keyboard

	stream_t *mystream;
	mystream = NULL;

	int file_format = 0;
	mystream = open_stream(programInfo.uri, 0, &file_format);

    mystream->start_pos += seek_to_byte;

	//开始流下载
    unsigned char buf[4096];
    int len;
    FILE *f;

    stream_reset(mystream);
    stream_seek(mystream, mystream->start_pos);
    f = fopen(filename, "wb");
    if (!f) {
		printf("open file failed\n");
		exit(-1);
    }
    if (dvd_chapter > 1) {
        int chapter = dvd_chapter - 1;
        stream_control(mystream, STREAM_CTRL_SEEK_TO_CHAPTER, &chapter);
    }

    while (!mystream->eof) {
        double pts;
        if (stream_control(mystream, STREAM_CTRL_GET_CURRENT_TIME, &pts) != STREAM_OK)
            pts = MP_NOPTS_VALUE;
        if (is_at_end(mystream, &end_at, pts))
            break;
        len = stream_read(mystream, buf, 4096);
        if (len > 0) {
            if (fwrite(buf, len, 1, f) != 1) {
            }
        }
        stream_dump_progress(len, mystream);
        if (dvd_last_chapter > 0) {
            int chapter = -1;
            if ( stream_control(mystream, STREAM_CTRL_GET_CURRENT_CHAPTER, &chapter) == STREAM_OK && chapter + 1 > dvd_last_chapter )
                break;
        }
    }
    fclose(f);
	printf(">>>filename:%s,dumpsize=%d\n", filename, stream_dump_count);

    return 1;
}

