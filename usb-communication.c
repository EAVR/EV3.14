/**
 * Host-side USB direct command example : communication test case
 * 
 * Prerequisite : sudo apt-get install libusb-1.0-0-dev
 * Compilation  : gcc -o usbcmd usbcmd.c -lusb-1.0 -lrt
 * Execution    : sudo ./usbcmd
 * 
 * jacques.gangloff@unistra.fr, 25/9/13
 */
  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include <time.h>
#include <sys/time.h>
#include <sys/timerfd.h>

#include <errno.h>
#include "/home/maximilian/lms/libusb/libusb-1.0.18/libusb/libusb.h"

#include "ev3sources/lms2012/main/com.h"

#define NB_OF_EV3										2	//nb of EV3s connected

#define EV3_USB_TIMEOUT             2000    // Milliseconds

/* These informations can be obtained with lsusb -v */
#define EV3_INTERFACE_NUMBER        0
#define EV3_CONFIGURATION_NB        1
#define EV3_EP_OUT                  0x01
#define EV3_EP_IN                   0x81
#define EV3_PACKET_SIZE             0x400

/* Error codes */
#define EV3_OK                      10
#define EV3_USB_ERROR               11
#define EV3_NOT_PRESENT             12
#define EV3_CONFIGURATION_ERROR     13
#define EV3_IN_USE                  14
#define EV3_USB_WRITE_ERROR         15
#define EV3_USB_READ_ERROR          16
#define EV3_USB_PARTIAL_TRANS       17
#define EV3_USB_OVERFLOW            19

#define EV3_PERIOD   10000000            /* Period in ns */
#define EV3_ITER     10000               /* Number of iterations */
#define EV3_HEATING  100                 /* Number of dummy iterations */

struct ev3_periodic_info {
  int              timer_fd;
  unsigned int     wakeups_missed;
};

static int cntr=0;

enum ev3_usb_ids {
  VENDOR_LEGO   =                   0x0694,
  PRODUCT_EV3   =                   0x0005
};

struct ev3_t {
  struct libusb_device_handle       *hdl;
};

typedef unsigned char               ev3_error_t;
typedef struct ev3_t                ev3_t;



ev3_error_t ev3_init( ev3_t **ev3 )  {
  int status;
  
  status = libusb_init( NULL );
  
  if ( status ) 
    return EV3_USB_ERROR;
  
  *ev3 = calloc( 1, sizeof( **ev3 ) );

  return EV3_OK;
}


ev3_error_t ev3_find_and_open( ev3_t **ev3 )  {
  libusb_device                    *dev, **devs,*tmpdev[NB_OF_EV3];
  struct libusb_device_descriptor  desc;
  int                              i, status,nb=0;

  if ( libusb_get_device_list( NULL, &devs ) < 0 )
    return EV3_USB_ERROR;
  
  /* Go through device list loooking for an EV3 device */
  for ( i = 0; ( dev = devs[i] ) != NULL; i++ ) {
    
    status = libusb_get_device_descriptor( dev, &desc );
    
    if ( status >= 0 ) {
      if (   ( desc.idVendor == VENDOR_LEGO ) &&
            ( desc.idProduct == PRODUCT_EV3 ) )  {
        printf("Found Lego device n°%d\n",desc.iSerialNumber);
			  tmpdev[nb]=dev;
				nb++;
			}
		}
		}
		
		for(i=0;i<nb;i++)
		{
        /* Open the device */
        status = libusb_open( tmpdev[i], &ev3[i]->hdl );

			if ( status < 0 )  {
          libusb_free_device_list( devs, 1 );
          return EV3_USB_ERROR;
        }
        
        /* Detach possible kernel driver bound to interface */
        libusb_detach_kernel_driver( ev3[i]->hdl, EV3_INTERFACE_NUMBER );
        
        /* Claiming the interface */
        status = libusb_claim_interface( ev3[i]->hdl, EV3_INTERFACE_NUMBER );
        if ( status )  {
          libusb_close( ev3[i]->hdl );
          libusb_free_device_list( devs, 1 );
          return EV3_IN_USE;
        }
        

  }
  
  if(nb>0)
	{
		printf("All EV3s are properly Initialized\n");
		libusb_free_device_list( devs, 1 );
    return (nb+1);
	}
	else  
	{
	 libusb_free_device_list( devs, 1 );
	 return EV3_NOT_PRESENT;
	}
}


ev3_error_t ev3_close( ev3_t *ev3 )  {
  
  libusb_release_interface( ev3->hdl, EV3_INTERFACE_NUMBER );
  libusb_close( ev3->hdl );
  libusb_exit( NULL );
  if ( ev3 != NULL )
    free( ev3 );
  ev3 = NULL;

  return EV3_OK;
}

ev3_error_t ev3_send_buf( ev3_t *ev3, char *buf, int len )  {
  int  status, transfered = 0;
  
  status = libusb_bulk_transfer( ev3->hdl, EV3_EP_OUT, buf, len, &transfered, EV3_USB_TIMEOUT );
  
  if ( status  )
    return EV3_USB_WRITE_ERROR;
  
  if ( transfered != len )
    return EV3_USB_PARTIAL_TRANS;

  return EV3_OK;
}

ev3_error_t ev3_recv_buf( ev3_t *ev3, char *buf, int len )  {
  int            i, status, transfered = 0,j=0;
  unsigned char  tmpbuf[EV3_PACKET_SIZE];
  
  if ( len > EV3_PACKET_SIZE )
    return EV3_USB_OVERFLOW;
  
  status = libusb_bulk_transfer( ev3->hdl, EV3_EP_IN, tmpbuf, EV3_PACKET_SIZE, &transfered, EV3_USB_TIMEOUT );

  if ( status  )
    return EV3_USB_WRITE_ERROR;
  
  if ( transfered != EV3_PACKET_SIZE )
    return EV3_USB_PARTIAL_TRANS;
  
  for ( i = 0; i < len; i++ )
    buf[i] = tmpbuf[i];

  return EV3_OK;
}



int main( void )  {
  ev3_t           *ev3[NB_OF_EV3];
  ev3_error_t     status;
  int             i,nb;//nb=number of EV3 detected
  struct timespec ts1, ts2;
	
	struct ev3_measurement_struct ev3_measurement_struct;
	struct ev3_control_struct ev3_control_struct;
  unsigned char 	ev3_measurements[sizeof(ev3_measurement_struct)];
	unsigned char 	ev3_control[sizeof(ev3_control_struct)];
  
  /* Initialize libusb */
	for(i=0;i<NB_OF_EV3;i++)
	{
	status = ev3_init( &ev3[i] );
  if ( status != EV3_OK )  {
    fprintf( stderr, "Libusb initialization error.\n" );
    exit( -1 );
  }
	}
    
  /* Look for an EV3 in USB devices list and open it if found */
  status = ev3_find_and_open( ev3 );
  if ( status )  {
    if ( status == EV3_NOT_PRESENT )
		{
      fprintf( stderr, "EV3 not found. Is it properly plugged in USB port?\n" );
			exit(-2);
		}
    else if(status==EV3_USB_ERROR || status==EV3_IN_USE)
		{
      fprintf( stderr, "Error while scanning for EV3 : %d.\nDid you forget to sudo ?\n",status );
			exit( -2 );
		}
		else
		 nb=status;
  }
 
  i=0;
	static int cnt=0,j=0;
  while(1)
  {
	 /*RECEIVE*/
	 ev3_recv_buf( ev3[0], ev3_measurements, sizeof(ev3_measurement_struct));	 
	 memcpy(&ev3_measurement_struct, ev3_measurements, sizeof(ev3_measurements));
	 
#define DEBUG
	 
	 #ifdef DEBUG 
	 printf("\
Measures : ADC        %4d | %4d | %4d | %4d \n\r\
           Encoders   %4d | %4d | %4d | %4d \n\r\
           Batt : %fmV \n\r",
				 ev3_measurement_struct.input_ADC[0],
				 ev3_measurement_struct.input_ADC[1],
				 ev3_measurement_struct.input_ADC[2],
				 ev3_measurement_struct.input_ADC[3],
				 ev3_measurement_struct.motor_angle[0],
				 ev3_measurement_struct.motor_angle[1],
				 ev3_measurement_struct.motor_angle[2],
				 ev3_measurement_struct.motor_angle[3],
				 ev3_measurement_struct.battery_voltage);//*/
	 
#endif
	 
	 /*COMPUTE*/
	 ev3_control_struct.init_encoders=0;
	 ev3_control_struct.motor_power[0]=-100;	 
	 ev3_control_struct.motor_power[1]=100;	 
	 ev3_control_struct.motor_power[2]=-100;	 
	 ev3_control_struct.motor_power[3]=100;
	 
	 /*SEND*/
	 
	 memcpy(ev3_control, &ev3_control_struct, sizeof(ev3_control_struct));
	 ev3_send_buf( ev3[0], ev3_control, sizeof(ev3_control_struct));
  }
  
  /* Close EV3 USB ports */
	for(i=0;i<NB_OF_EV3;i++)
	 ev3_close( ev3[i] );
  
  return 0;
}