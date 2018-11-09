#include "common.h"
#include "conf.h"
#include "debug.h"
#include <stdint.h>

uint16_t flag= 0;
uint16_t *get_flag(){
	return &flag;
}

pthread_t uart_thread = 0;
pthread_t *get_uart_thread(){
	return &uart_thread;
}

pthread_mutex_t lock;
pthread_mutex_t *get_thread_lock(){
	return &lock;
}


int big_little_endian(int x)
{
	int tmp;  
	tmp = (((x)&0xff)<<24) + (((x>>8)&0xff)<<16) + (((x>>16)&0xff)<<8) + (((x>>24)&0xff));  
	return tmp;  
}  

char * safe_strdup(const char *s) {
	char * retval = NULL;
	if (!s) {		
		exit(1);
	}
	retval = strdup(s);
	if (!retval) {		
		exit(1);
	}
	return(retval);
}


void * safe_malloc (int size) {
	void * retval = NULL;
	retval = malloc(size);
	if (!retval) {
		exit(1);
	}	
	memset(retval,0,size);
	return (retval);
}

pid_t safe_fork(void) {
	pid_t result;
	result = fork();
	if (result == -1) {
		debug(LOG_CRIT, "Failed to fork");
		exit (1);
	}
	else if (result == 0){
		/* I'm the child*/
		debug(LOG_DEBUG, "Failed to fork");
	}

	return result;
}



struct in_addr * wd_gethostbyname(const char *name)
{
	struct hostent *phe = NULL;
	struct hostent	he;
	struct in_addr *h_addr;
	int errnum = 0; 
	char buf[8192] ={0}; 
	if(NULL == name)
	{
		debug(LOG_ERR, "invalid host name");
		return NULL;
	}
		
	if(0 != gethostbyname_r(name,&he,buf,8192,&phe,&errnum))
	{//hostname error
		debug(LOG_ERR, "dns parse hostname(%s) failed", name);		
		return NULL;
	}	
	if(NULL == phe)
	{//parse ip error 
		debug(LOG_ERR, "dns parse ip(%s) failed", name);		
		return NULL;
	}
	
	h_addr = (struct in_addr *)phe->h_addr_list[0];
	
	return h_addr;
}



