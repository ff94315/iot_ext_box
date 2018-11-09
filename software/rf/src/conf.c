#include "common.h"
#include "debug.h"
#include "conf.h"



SYS_CONF s_config;

char *conf_path = "./api";

int getmac(const char *devname, char *mac)
{
    struct ifreq ifreq;
    int sock = 0;

    sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock < 0)
    {
        perror("error sock");
        return -1;
    }

    strcpy(ifreq.ifr_name,devname);
    if(ioctl(sock,SIOCGIFHWADDR,&ifreq) < 0)
    {
        perror("error ioctl");
        return -1;
    }

    int i = 0;
    for(i = 0; i < 6; i++){
        sprintf(mac+2*i, "%02X", (unsigned char)ifreq.ifr_hwaddr.sa_data[i]);
    }
    mac[strlen(mac)] = 0;
                
    return 0;
}

void config_init(){
	bzero(config_get(),sizeof(SYS_CONF));

	config_read(conf_path,"ttyname", config_get()->ttyname);
	if(0 == strlen(config_get()->ttyname)){
		debug(LOG_ERR, "Could not read ttyname exiting...");
		exit(1);
	}

	char ttyspeed[8] = {0};
	config_read(conf_path,"ttyspeed", ttyspeed);
	if(0 == strlen(ttyspeed)){
		debug(LOG_ERR, "Could not read ttyspeed exiting...");
		exit(1);
	}else
		config_get()->ttyspeed = atoi(ttyspeed);
	
	char port[8] = {0};
	config_read(conf_path,"port",port);
	if(0 == strlen(port)){
		debug(LOG_WARNING, "Could not read port use default 8000");
		//使用默认端口8000
		config_get()->port = 8000;
	}else
		config_get()->port = atoi(port);


	debug(LOG_INFO,"ttyname[%s],ttyspeed[%ld],port[%d]",
	config_get()->ttyname,config_get()->ttyspeed,config_get()->port);

}


SYS_CONF *config_get(){
	return &s_config;
}


void config_read(const char *filename,char *key,char *value)
{
	FILE *fd;
	char line[1024], *s, *p1, *p2;
	int len;
	
	if (!(fd = fopen(filename, "r"))) {
		debug(LOG_ERR, "Could not open config exiting...");
		exit(1);
	}

	while (!feof(fd) && fgets(line, 1024, fd)) {
		s = line;
		if (s[strlen(s) - 1] == '\n')
			s[strlen(s) - 1] = '\0';
		if ((p1 = strchr(s, ' '))) {
			p1[0] = '\0';
		} else if ((p1 = strchr(s, '\t'))) {
			p1[0] = '\0';
		}

		if (p1) {
			p1++;
			// Trim leading spaces
			len = strlen(p1);
			while (*p1 && len) {
				if (*p1 == ' ')
					p1++;
				else
					break;
				len = strlen(p1);
			}
			
			if ((p2 = strchr(p1, ' '))) {
				p2[0] = '\0';
			} else if ((p2 = strchr(p1, '\r'))) {
				p2[0] = '\0';
			} else if ((p2 = strchr(p1, '\n'))) {
				p2[0] = '\0';
			}
		}

		if (p1 && p1[0] != '\0') {
			/* Strip trailing spaces */
			if ((strncmp(s, "#", 1)) != 0) {
				if(strcasecmp(s,key) == 0){
					debug(LOG_DEBUG, "Parsing token: (%s), value: (%s)", s, p1);
					strcpy(value,p1);
				}
			}
		}
	}

	fclose(fd);
}

