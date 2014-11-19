/*
 * based on previous Real RTSP support from Roberto Togni and xine team.
 *
 * Copyright (C) 2006 Benjamin Zores
 *
 * This file is part of MPlayer.
 *
 * MPlayer is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * MPlayer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with MPlayer; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <ctype.h>
#include "config.h"
#if !HAVE_WINSOCK2_H
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#include <errno.h>

#include "libmpdemux/demuxer.h"
#include "network.h"
#include "stream.h"
#include "tcp.h"
#include "librtsp/rtsp.h"
#include "librtsp/rtsp_session.h"

#define RTSP_DEFAULT_PORT 554

static int
rtsp_streaming_read (int fd, char *buffer,
                     int size, streaming_ctrl_t *stream_ctrl)
{
  return rtsp_session_read (stream_ctrl->data, buffer, size);
}

static int
rtsp_streaming_start (stream_t *stream)
{
  return 0;
}

static void
rtsp_streaming_close (struct stream *s)
{
}

static int
rtsp_streaming_open (stream_t *stream, int mode, void *opts, int *file_format)
{
  return STREAM_OK;
}

const stream_info_t stream_info_rtsp = {
  "RTSP streaming",
  "rtsp",
  "Benjamin Zores, Roberto Togni",
  "ported from xine",
  rtsp_streaming_open,
  {"rtsp", NULL},
  NULL,
  0 /* Urls are an option string */
};
