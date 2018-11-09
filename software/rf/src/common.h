#ifndef _COMMON_H_
#define _COMMON_H_
#include<string.h>
#include<stdio.h>
#include<errno.h>
#include<stdarg.h>
#include<time.h>
#include<unistd.h>
#include<stdlib.h>    
#include<arpa/inet.h>
#include<sys/socket.h>  
#include<netinet/in.h>  
#include<arpa/inet.h>  
#include<netdb.h>  
#include<syslog.h>
#include<net/if.h>  
#include<sys/ioctl.h>  
#include<arpa/inet.h> 
#include <stdint.h>


#define safe_free(space) if(NULL != space)\
{\
	free(space);\
	space = NULL;\
}

uint16_t *get_flag();
pthread_t *get_uart_thread();
pthread_mutex_t *get_thread_lock();


struct in_addr * wd_gethostbyname(const char *name);
char * safe_strdup(const char *s);
void * safe_malloc (int size);
pid_t safe_fork(void);
void parse_commandline(int argc, char **argv); 
int big_little_endian(int x);

#endif

