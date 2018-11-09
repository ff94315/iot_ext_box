/********************************************************************\
 * This program is free software; you can redistribute it and/or    *
 * modify it under the terms of the GNU General Public License as   *
 * published by the Free Software Foundation; either version 2 of   *
 * the License, or (at your option) any later version.              *
 *                                                                  *
 * This program is distributed in the hope that it will be useful,  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    *
 * GNU General Public License for more details.                     *
 *                                                                  *
 * You should have received a copy of the GNU General Public License*
 * along with this program; if not, contact:                        *
 *                                                                  *
 * Free Software Foundation           Voice:  +1-617-542-5942       *
 * 59 Temple Place - Suite 330        Fax:    +1-617-542-2652       *
 * Boston, MA  02111-1307,  USA       gnu@gnu.org                   *
 *                                                                  *
\********************************************************************/

/* $Id$ */
/** @file debug.c
    @brief Debug output routines
    @author Copyright (C) 2004 Philippe April <papril777@yahoo.com>
*/

#include "debug.h"
#include "common.h"
#include <pthread.h>

int deamon = 1; 
/** @internal
Do not use directly, use the debug macro */
void _debug(const char *filename, int line, int level, const char *format, ...)
{
	char buf[28];
	va_list vlist;
	time_t ts;
	time(&ts);
	if (!deamon){
	    fprintf(stdout, "[%d][%.24s][%d](%s:%d) ", level, ctime_r(&ts, buf), (int)pthread_self(),filename, line);
	    va_start(vlist, format);
	    vfprintf(stdout, format, vlist);
	    va_end(vlist);
	    fputc('\n', stdout);
	    fflush(stdout);
	}
}

