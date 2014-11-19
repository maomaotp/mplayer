/*
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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#ifdef __FreeBSD__
#include <sys/cdrio.h>
#endif

#include "stream.h"


/// We keep these 2 for the gui atm, but they will be removed.
char* cdrom_device=NULL;
int dvd_chapter=1;
int dvd_last_chapter=0;
char* dvd_device=NULL;
char *bluray_device=NULL;

// Open a new stream  (stdin/file/vcd/url)

stream_t* open_stream(const char* filename,char** options, int* file_format){
  int dummy = 0;
  if (!file_format) file_format = &dummy;
  // Check if playlist or unknown
  if (*file_format != (2<<16) ){
    *file_format=0;
  }

	if(!filename) {
	   return NULL;
	}

  return open_stream_full(filename,0,options,file_format);
}
