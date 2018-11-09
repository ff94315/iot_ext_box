#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include <string.h>

#include "CP2130_API.h"
#include "debug.h"
#include "common.h"
#include "nrf24l01.h"


libusb_context* context = NULL;
libusb_device** deviceList = NULL;
libusb_device_handle* cp2130Handle = NULL;

int kernelAttached = 0;

libusb_device_handle* get_cp2130Handle(){
	return cp2130Handle;
}

int reset_CP2130(){
	int usbTimeout = 500;
	if (libusb_control_transfer(cp2130Handle, H2D_req, SPI_RESET_CMD, 0x0000, 0x0000, NULL, 0, usbTimeout)){
        debug(LOG_ERR,"reset err");
        return -1;
    }
	debug(LOG_INFO,"reset succeed");	
	return 0;
}

int init_CP2130(){
	debug(LOG_INFO,"start init CP2130 IC");
    ssize_t deviceCount = 0;
    struct libusb_device_descriptor deviceDescriptor;
    libusb_device* device = NULL;
    
	// Initialize libusb
    if (libusb_init(&context) != 0)
        return -1;
    // Search the connected devices to find and open a handle to the CP2130
    deviceCount = libusb_get_device_list(context, &deviceList);
    if (deviceCount <= 0)
        return -1;
	
    for (int i = 0; i < deviceCount; i++){
        if (libusb_get_device_descriptor(deviceList[i], &deviceDescriptor) == 0){
            if ((deviceDescriptor.idVendor == 0x10C4) &&
                (deviceDescriptor.idProduct == 0x87A0)){
                device = deviceList[i];
                break;
            }
        }
    }
    if (device == NULL){
		debug(LOG_INFO,"ERROR: Device not found");
        return -1;
    }
    // If a device is found, then open it
    if (libusb_open(device, &cp2130Handle) != 0){
		debug(LOG_INFO,"ERROR: Could not open device");
        return -1;
    }
    // See if a kernel driver is active already, if so detach it and store a
    // flag so we can reattach when we are done
    if (libusb_kernel_driver_active(cp2130Handle, 0) != 0){
        libusb_detach_kernel_driver(cp2130Handle, 0); 
		kernelAttached = 1;
    }
 
    // Finally, claim the interface
    if (libusb_claim_interface(cp2130Handle, 0) != 0){
		debug(LOG_INFO,"ERROR: Could not claim interface");
        return -1;
    }

	if(NULL == cp2130Handle){
		debug(LOG_INFO,"init cp2130 error\n");
		clear_cp2130Handle();
		return -1;
	}
	libusb_reset_device(cp2130Handle);
	//reset_CP2130();

	return 0;
}


int clear_cp2130Handle(){
    if (cp2130Handle)
        libusb_release_interface(cp2130Handle, 0);
    if (kernelAttached)
        libusb_attach_kernel_driver(cp2130Handle, 0);
    if (cp2130Handle)
        libusb_close(cp2130Handle);
    if (deviceList)
        libusb_free_device_list(deviceList, 1);
    if (context)
        libusb_exit(context);
	return 0;
}

int CP2130_set_channel(int cs,char control){
	unsigned char control_buf_in[2] = {0};
	control_buf_in[0] = (unsigned char)cs;
	control_buf_in[1] = control;
	int usbTimeout = 500;
	if (libusb_control_transfer(cp2130Handle, H2D_req, SET_SPI_WORLD_CMD, 0x0000, 0x0000, control_buf_in, sizeof(control_buf_in), usbTimeout) != sizeof(control_buf_in)){
        debug(LOG_ERR,"can't set channel %d [%02x]",cs,control);
        return -1;
    }
	debug(LOG_INFO,"set channel %d succeed [%02x]",cs,control);
	return 0;
}

int CP2130_get_channel(){
	unsigned char control_buf_out[11] = {0};

	int usbTimeout = 500;
	
	if (libusb_control_transfer(cp2130Handle, D2H_REQ, GET_SPI_WORLD_CMD, 0x0000, 0x0000, control_buf_out, sizeof(control_buf_out), usbTimeout) != sizeof(control_buf_out)){
        debug(LOG_ERR,"can't get channel");
        return -1;
    }
	int i;
	for(i = 0;i < 11;i++){
		printf("%02x ",control_buf_out[i]);
	}
	
	printf("\n");
	return 0;
}

int CP2130_set_chipSelect(int cs,char control){
	unsigned char control_buf_in[2] = {0};
	control_buf_in[0] = (unsigned char)cs;
	control_buf_in[1] = control;
	
	int usbTimeout = 500;
	if (libusb_control_transfer(cp2130Handle, H2D_req, SET_CHIPSELECT, 0x0000, 0x0000, control_buf_in, sizeof(control_buf_in), usbTimeout) != sizeof(control_buf_in)){
        debug(LOG_ERR,"can't set chipSelect %d [%02x]",cs,control);
        return -1;
    }
	
	debug(LOG_INFO,"set chipSelect %d succeed [%02x]",cs,control);
	return 0;
}

int CP2130_get_chipSelect(){
	unsigned char control_buf_out[4] = {0};

	int usbTimeout = 500;
	
	if (libusb_control_transfer(cp2130Handle, D2H_REQ, GET_CHIPSELECT, 0x0000, 0x0000, control_buf_out, sizeof(control_buf_out), usbTimeout) != sizeof(control_buf_out)){
        debug(LOG_ERR,"can't get chipSelect");
        return -1;
    }
	int i;
	for(i = 0;i < 4;i++){
		printf("%02x ",control_buf_out[i]);
	}
	
	printf("\n");
	return 0;
}

int CP2130_set_GPIO_Mode_And_Level(int gpio,unsigned char mode,unsigned char level){
	unsigned char control_buf_in[3] = {0};
	control_buf_in[0] = (unsigned char)gpio;
	control_buf_in[1] = mode;
	control_buf_in[2] = level;
	
	int usbTimeout = 500;
	if (libusb_control_transfer(cp2130Handle, H2D_req, SET_GPIO, 0x0000, 0x0000, control_buf_in, sizeof(control_buf_in), usbTimeout) != sizeof(control_buf_in)){
        debug(LOG_ERR,"can't set GPIO_Mode_And_Level %d [%02x][%02x]",gpio,mode,level);
        return -1;
    }
	
	debug(LOG_INFO,"set GPIO_Mode_And_Level %d succeed [%02x][%02x]",gpio,mode,level);
	return 0;
}

int CP2130_get_GPIO_Mode_And_Level(int gpio){
	unsigned char control_buf_out[2] = {0};

	int usbTimeout = 500;
	
	if (libusb_control_transfer(cp2130Handle, D2H_REQ,GET_GPIO, 0x0000, 0x0000, control_buf_out, sizeof(control_buf_out), usbTimeout) != sizeof(control_buf_out)){
        debug(LOG_ERR,"get GPIO_Mode_And_Level err");
        return -1;
    }
	int i;
	for(i = 0;i < 2;i++){
		printf("%02x ",control_buf_out[i]);
	}
	
	printf("\n");
	return 0;
}

int CP2130_set_GPIO_value(int gpio,int value){
	unsigned char control_buf_in[4] = {0};


	if(value){
		control_buf_in[0] = 0xFF;
	    control_buf_in[1] = 0xFF;
	}

		
	switch(gpio){
		case 0:
			control_buf_in[3] = 0x08;
			break;
		case 1:
			control_buf_in[3] = 0x10;
			break;
		case 2:
			control_buf_in[3] = 0x20;
			break;
		case 3:
			control_buf_in[3] = 0x40;
			break;
		case 4:
			control_buf_in[3] = 0x80;
			break;
		case 5:
			control_buf_in[2] = 0x01;
			break;
		case 6:
			control_buf_in[2] = 0x04;
			break;
		case 7:
			control_buf_in[2] = 0x08;
			break;
		case 8:
			control_buf_in[2] = 0x10;
			break;
		case 9:
			control_buf_in[2] = 0x20;
			break;
		case 10:
			control_buf_in[2] = 0x40;
			break;
	}
	int usbTimeout = 500;
	if (libusb_control_transfer(cp2130Handle, H2D_req, SET_GPIO_value, 0x0000, 0x0000, control_buf_in, sizeof(control_buf_in), usbTimeout) != sizeof(control_buf_in)){
        debug(LOG_ERR,"can't set GPIO value %d [%d]",gpio,value);
        return -1;
    }
	
	debug(LOG_INFO,"set GPIO value succeed %d [%d]",gpio,value);
	return 0;
}


int CP2130_get_GPIO_value(int gpio){
	unsigned char control_buf_out[2] = {0};

	int usbTimeout = 500;
	
	if (libusb_control_transfer(cp2130Handle, D2H_REQ,GET_GPIO_value, 0x0000, 0x0000, control_buf_out, sizeof(control_buf_out), usbTimeout) != sizeof(control_buf_out)){
        debug(LOG_ERR,"get GPIO value err");
        return -1;
    }

	//debug(LOG_INFO,"read gpio[%02x][%02x]",control_buf_out[0],control_buf_out[1]);

	switch(gpio){
		case 0:
			if(control_buf_out[1] & 0x08)
				return 1;
			break;
		case 1:
			if(control_buf_out[1] & 0x10)
				return 1;
			break;
		case 2:
			if(control_buf_out[1] & 0x20)
				return 1;
			break;
		case 3:
			if(control_buf_out[1] & 0x40)
				return 1;
			break;
		case 4:
			if(control_buf_out[1] & 0x80)
				return 1;
			break;
		case 5:
			if(control_buf_out[0] & 0x01)
				return 1;
			break;
		case 6:
			if(control_buf_out[0] & 0x04)
				return 1;
			break;
		case 7:
			if(control_buf_out[0] & 0x08)
				return 1;
			break;
		case 8:
			if(control_buf_out[0] & 0x10)
				return 1;
			break;
		case 9:
			if(control_buf_out[0] & 0x20)
				return 1;
			break;
		case 10:
			if(control_buf_out[0] & 0x40)
				return 1;
			break;
		default:
			return 0;
	}		

	return 0;
}

int CP2130_set_pin_conf(){
	unsigned char control_buf_in[14] = {0};

	control_buf_in[0] = 0x03;
	control_buf_in[1] = 0x03;
	control_buf_in[2] = 0x03;
	control_buf_in[3] = 0x03;
	control_buf_in[3] = 0x02;
	control_buf_in[4] = 0x02;
	control_buf_in[5] = 0x02;
	control_buf_in[6] = 0x02;
	control_buf_in[7] = 0x02;
	control_buf_in[8] = 0x02;
	control_buf_in[9] = 0x00;
	control_buf_in[10] = 0x00;

	int usbTimeout = 500;
	
	if (libusb_control_transfer(cp2130Handle, H2D_req,0x6d, 0xA5F1, 0x0000, control_buf_in, sizeof(control_buf_in), usbTimeout) != sizeof(control_buf_in)){
        debug(LOG_ERR,"get GPIO value err");
        return -1;
    }

	return 0;
}



int CP2130_SPI_write(const unsigned char *value,uint32_t size){
	unsigned char write_command_buf[8 + size] = {0};
	write_command_buf[2] = SPI_WRITE_CMD;
	uint32_t* size_LE =(uint32_t*)(write_command_buf + 4);
	
#if __BYTE_ORDER == __BIG_ENDIAN
	*size_LE = big_little_endian(size);
#else
	*size_LE =  size;
#endif
	char* data = (char *)(write_command_buf + 8);
	memcpy(data,value,size);
	
    int bytesWritten;
    int usbTimeout = 500;
    
    if (libusb_bulk_transfer(cp2130Handle, 0X01, write_command_buf, sizeof(write_command_buf), &bytesWritten, usbTimeout))
    {
        debug(LOG_ERR,"ERROR: Error in bulk transfer");
        return -1;
    }
    debug(LOG_INFO,"Successfully write to SPI MOSI, number of bytes written = %d",bytesWritten);
    return 0;
}

int CP2130_SPI_read(unsigned char *value,uint32_t size){
	unsigned char write_command_buf[8] = {0};
	write_command_buf[2] = SPI_READ_CMD;
	uint32_t* size_LE =(uint32_t*)(write_command_buf + 4);
	
#if __BYTE_ORDER == __BIG_ENDIAN
	*size_LE = big_little_endian(size);
#else
	*size_LE =  size;
#endif

	int bytesWritten, tmp;
	int bytesToRead;
	unsigned char read_input_buf[size];
	int bytesRead;
	int usbTimeout = 500;

	if ((tmp = libusb_bulk_transfer(cp2130Handle, 0x01, write_command_buf,sizeof(write_command_buf), &bytesWritten, usbTimeout)) != 0)
	{
		if (tmp < 0) {
			debug(LOG_ERR,"libusb_bulk_transfer ERROR[%d]",tmp);
		}
		debug(LOG_ERR,"ERROR: Error in bulk write transfer");
		return -1;
	}
	debug(LOG_INFO,"Successfully write to SPI MOSI , number of bytes written = %d",bytesWritten);

	if ((tmp = libusb_bulk_transfer(cp2130Handle, 0x82, read_input_buf, sizeof(read_input_buf), &bytesRead, usbTimeout)) != 0){
		if (tmp < 0){
			debug(LOG_ERR,"libusb_bulk_transfer ERROR[%d]");
		}

		debug(LOG_ERR,"ERROR: Error in bulk read transfer");
		return -1;
	}

	debug(LOG_INFO,"Successfully read from SPI MISO, number of bytes read = %d",bytesRead);

	memcpy(value,read_input_buf,size);
	return 0;
	
}

int CP2130_SPI_writeRead(unsigned char *value,uint32_t size){
	unsigned char write_command_buf[8 + size] = {0};
	write_command_buf[2] = SPI_WRITEREAD_CMD;
	uint32_t* size_LE =(uint32_t*)(write_command_buf + 4);
	
#if __BYTE_ORDER == __BIG_ENDIAN
	*size_LE = big_little_endian(size);
#else
	*size_LE =  size;
#endif
	char* data = (char *)(write_command_buf + 8);
	memcpy(data,value,size);	

	int bytesWritten, tmp;
	int bytesToRead;
	unsigned char read_input_buf[size];
	int bytesRead;
	int usbTimeout = 500;

	if ((tmp = libusb_bulk_transfer(cp2130Handle, 0x01, write_command_buf,sizeof(write_command_buf), &bytesWritten, usbTimeout)) != 0)
	{
		if (tmp < 0) {
			debug(LOG_ERR,"libusb_bulk_transfer ERROR[%d]",tmp);
		}
		debug(LOG_ERR,"ERROR: Error in bulk write transfer");
		return -1;
	}
	debug(LOG_INFO,"Successfully write to SPI MOSI , number of bytes written = %d",bytesWritten);

	if ((tmp = libusb_bulk_transfer(cp2130Handle, 0x82, read_input_buf, sizeof(read_input_buf), &bytesRead, usbTimeout)) != 0){
		if (tmp < 0){
			debug(LOG_ERR,"libusb_bulk_transfer ERROR[%d]",tmp);
		}

		debug(LOG_ERR,"ERROR: Error in bulk read transfer");
		return -1;
	}
	debug(LOG_INFO,"Successfully read from SPI MISO, number of bytes read = %d",bytesRead);

	memcpy(value,read_input_buf,size);
	return 0;
}



