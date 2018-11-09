#include "debug.h"
#include "common.h"
#include "conf.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>


#include "CP2130_API.h"
#include "nrf24l01.h"

extern int deamon;
extern char *conf_path;

void sigchld_handler(int s)
{
    int status;
    pid_t rc;
    debug(LOG_DEBUG, "Handler for SIGCHLD called. Trying to reap a child");
    rc = waitpid(-1, &status, WNOHANG);
    debug(LOG_DEBUG, "Handler for SIGCHLD reaped child PID %d", rc);
}

void termination_handler(int s)
{
    debug(LOG_NOTICE, "Exiting...");
	clear_cp2130Handle();
    exit(s == 0 ? 1 : 0);
}

static void init_signals(void)
{
    struct sigaction sa;

    debug(LOG_DEBUG, "Initializing signal handlers");

    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        debug(LOG_ERR, "sigaction(): %s", strerror(errno));
        exit(1);
    }

	/* Trap SIGPIPE */
    sa.sa_handler = SIG_IGN;
    if (sigaction(SIGPIPE, &sa, NULL) == -1) {
        debug(LOG_ERR, "sigaction(): %s", strerror(errno));
        exit(1);
    }

    sa.sa_handler = termination_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    /* Trap SIGTERM */
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        debug(LOG_ERR, "sigaction(): %s", strerror(errno));
        exit(1);
    }

    /* Trap SIGQUIT */
    if (sigaction(SIGQUIT, &sa, NULL) == -1) {
        debug(LOG_ERR, "sigaction(): %s", strerror(errno));
        exit(1);
    }

    /* Trap SIGINT */
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        debug(LOG_ERR, "sigaction(): %s", strerror(errno));
        exit(1);
    }
}

void parse_commandline(int argc, char **argv) 
{
     int c;
     while (-1 != (c = getopt(argc, argv, "fc:")))
	 {
		 switch(c) 
		 {
			 case 'f':
				deamon = 0;
				break;
			 case 'c':
			 	conf_path = optarg;
				break;
			 default:
				exit(1);
				break;
		 }
	 }

}



int main_loop(){
	debug(LOG_INFO,"start_server");
	
	if(-1 == init_CP2130()){
		debug(LOG_ERR,"init cp2130 ERROR");
		return 0;
	}
	//CP2130_set_pin_conf();
	NRF24L01_start();


	while(1){
		debug(LOG_INFO,"GPIO 9[%d],GPIO 10[%d]",CP2130_get_GPIO_value(9),CP2130_get_GPIO_value(10));

	}
	
	clear_cp2130Handle();
    return 0;

}

int main(int argc,char **argv){
	debug(LOG_INFO,"read commond line");
	parse_commandline(argc,argv);
	//debug(LOG_INFO,"config get");
	//config_init();	
	debug(LOG_INFO,"init signal");
	init_signals();
	if (deamon){
		switch(safe_fork()) {
			case 0: 
				setsid();
				main_loop();
				break;
			default: 
				exit(0);
				break;
		}
	}
	else
		main_loop();
	return 0;
}








