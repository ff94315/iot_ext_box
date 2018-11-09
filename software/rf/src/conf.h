#ifndef CONF_H
#define CONF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <stdint.h>

#define DEFAULT_CONF_PATH "/etc/hikapi.conf";

typedef struct {
	char ttyname[32];
	int ttyspeed;

	int port;
}SYS_CONF;

void config_init();
SYS_CONF *config_get();
void config_read(const char *filename,char *key,char *value);
void config_write(const char * configfile,char * key,const char * string);


#endif
