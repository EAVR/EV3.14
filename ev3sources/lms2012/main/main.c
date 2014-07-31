/* This is the main program that runs on the EV3.14. It handles USB communication, LCD screen, actuators 
 * and sensors of the EV3 and is monitored by a watchdog.
 * 
 * To compile it : arm-none-linux-gnueabi-gcc main.c -o main -lpthread -lrt
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <ioctl.h>
#include <fb.h>
#include <sys/timerfd.h>
#include <sys/wait.h>
#include <sys/file.h>

#include "../lms2012/source/lms2012.h"

#include "charset.h"
#include "com.h"

#define WATCHDOG_PERIOD 100000			//in us
#define SCREEN_PERIOD  1000000

#define SCREEN_SIZE (8*16*12*5)

#define EV3_BATTLEVEL_1 70	//value in cV (centiVolt, accuracy is not critical for this measure, and so it fits on 8 bits)
#define EV3_BATTLEVEL_2 65
#define EV3_BATTLEVEL_3 60
#define EV3_BATTLEVEL_4 55
#define EV3_BATTLEVEL_5 50


//The macros used for the LCD display

#define clear_screen() {int hsulf; for(hsulf=0;hsulf<60*128;hsulf++) dbuf[hsulf]=0;}

#define draw(row,col,ch,style){\
for(theline=0;theline<ligne;theline++) \
 {\
	for(thecolumn=0;thecolumn<colonne;thecolumn++)\
	{\
	 dbuf[LINE_LENGTH*(row*ligne+theline)+col*colonne+thecolumn]=charset[((ch=='\'')?96:((ch==' ')?'*':(ch=='|')?'+':ch))-'*'][style][theline][thecolumn];\
	}\
 }\
}

#define printscr(therow,thecol,str,style) {\
charindex=0,colvar=thecol,rowvar=therow;\
while(str[charindex++])\
{\
 if(str[charindex-1]=='\n') {rowvar++;continue;}\
 if(str[charindex-1]=='\r') {colvar=0;continue;}\
 draw(rowvar,colvar,str[charindex-1],style);\
 colvar++;\
 if(colvar==12) {colvar=0; rowvar++;}\
 }\
}


#define DEBUG
#undef DEBUG


int usb_fd, encoder_fd, motor_fd, lcd_fd, ui_fd, analog_fd;
char motor_command[3];

// The motor operations use a single bit to determine which motor(s) will be used
const char MOTOR_PORT_A = 0x01;
const char MOTOR_PORT_B = 0x02;
const char MOTOR_PORT_C = 0x04;
const char MOTOR_PORT_D = 0x08;

//Shared memory between the 3 processes
char* ptr;
#define WD 							ptr[0]
#define conn_state 			ptr[1]
#define WDflag 					ptr[2]
#define motor_allow 		ptr[3]
#define startflag 			ptr[4]
#define resetEncoders		ptr[5]
#define lbattery				ptr[6]
#define halt						ptr[7]


//timer used by the watchdog and the LCD
struct ev3_periodic_info
{
	int timer_fd;
	uint wakeups_missed;
};

static int ev3_make_periodic( uint period, struct ev3_periodic_info *info )
 {
 int ret;
 uint ns;
 uint sec;
 int fd;
 struct itimerspec itval;

 /* Create the timer */
 fd = timerfd_create (CLOCK_MONOTONIC, 0);
 info->wakeups_missed = 0;
 info->timer_fd = fd;
 if (fd == -1)
 return fd;

 /* Make the timer periodic */
 sec = period/1000000;
 ns = (period - (sec * 1000000)) * 1000;
 itval.it_interval.tv_sec = sec;
 itval.it_interval.tv_nsec = ns;
 itval.it_value.tv_sec = sec;
 itval.it_value.tv_nsec = ns;
 ret = timerfd_settime (fd, 0, &itval, NULL);
 return ret;
  }


static int ev3_wait_period( struct ev3_periodic_info *info )
{
 unsigned long long missed;
 int ret;

 /* 
	 Wait for the next timer event. 
	 If we have missed any the
	 number is written to "missed"
 */
 ret = read(info->timer_fd, &missed, sizeof (missed));
 if (ret == -1)
 {
	 //fprintf(stderr,"** timer read error **\n");
	 return -1;
 }
 if (missed > 1)
 {
	 info->wakeups_missed += (int)(missed-1);
	 //fprintf(stderr,"** clock tic missed (total=%d) **\n",info->wakeups_missed); 
	 return -2;
 }
	 
 return 0;
		 
} 

/* The only function that accesses motor device, to avoid using mutexes. 2 of the processes use it,
 * it uses some flags for additional communication with the motor device (reset encoders, stop mode ...).
 * All motor operations use the first command byte to indicate the type of operation
 * and the second one to indicate the motor(s) port(s)
 */
int setPower(int chan, int val)
{  
 if(resetEncoders)
 {
	  //opOUTPUT_CLR_COUNT = reset encoders
	  resetEncoders=0;
		motor_command[0] = opOUTPUT_CLR_COUNT;
		motor_command[1] = MOTOR_PORT_A;
		write(motor_fd,motor_command,2);
		motor_command[0] = opOUTPUT_CLR_COUNT;
		motor_command[1] = MOTOR_PORT_B;
		write(motor_fd,motor_command,2);
		motor_command[0] = opOUTPUT_CLR_COUNT;
		motor_command[1] = MOTOR_PORT_C;
		write(motor_fd,motor_command,2);
		motor_command[0] = opOUTPUT_CLR_COUNT;
		motor_command[1] = MOTOR_PORT_D;
		write(motor_fd,motor_command,2);
 }
 if(motor_allow && !WDflag)
 {	
	motor_allow=0; 
	if(startflag)
	{
	 motor_command[0] = opOUTPUT_START;
	 motor_command[1] = MOTOR_PORT_A;
	 write(motor_fd,motor_command,2);
	 motor_command[0] = opOUTPUT_START;
	 motor_command[1] = MOTOR_PORT_B;
	 write(motor_fd,motor_command,2);
	 motor_command[0] = opOUTPUT_START;
	 motor_command[1] = MOTOR_PORT_C;
	 write(motor_fd,motor_command,2);
	 motor_command[0] = opOUTPUT_START;
	 motor_command[1] = MOTOR_PORT_D;
	 write(motor_fd,motor_command,2);
	}

	motor_command[0] = opOUTPUT_POWER;
	motor_command[1] = chan;
	motor_command[2] = val;
	write(motor_fd,motor_command,3);
	motor_allow=1;
 }
 else if (motor_allow && WDflag)
 {
	motor_allow=0; 
	motor_command[0] = opOUTPUT_POWER;
	motor_command[1] = chan;
	motor_command[2] = 0;
	write(motor_fd,motor_command,3);

	motor_command[0] = opOUTPUT_STOP;
	motor_command[1] = chan;
	motor_command[2] = 2;
	write(motor_fd,motor_command,3);
	motor_allow=1;
 }
 return 0;
}

//puts a string into USB buffer
int USBPutString(char* s, int l)
{
return write(usb_fd,s,l);
return write(usb_fd,s,l);
}

//gets a string from USB. Sets Watchdog flag to 0 if communication is received : let the watchdog asleep.
int USBGetString(char* buf,int l)
{
 if(read(usb_fd,buf,l))
 {
	buf[l]='\0';
	WD=0;
	return 0;
 }
 else
	return USB_NOT_READ;
}


/*
 * Homemade crc8 method to get a "unique" identifier for the EV3s (there is  still
 * 1 chance out of 255 that 2 EV3s may have the same id) -> see the d_usbdev.c file
 * for more details
 */

static unsigned char crc8_table[] = {
    0x00, 0x3e, 0x7c, 0x42, 0xf8, 0xc6, 0x84, 0xba, 0x95, 0xab, 0xe9, 0xd7,
    0x6d, 0x53, 0x11, 0x2f, 0x4f, 0x71, 0x33, 0x0d, 0xb7, 0x89, 0xcb, 0xf5,
    0xda, 0xe4, 0xa6, 0x98, 0x22, 0x1c, 0x5e, 0x60, 0x9e, 0xa0, 0xe2, 0xdc,
    0x66, 0x58, 0x1a, 0x24, 0x0b, 0x35, 0x77, 0x49, 0xf3, 0xcd, 0x8f, 0xb1,
    0xd1, 0xef, 0xad, 0x93, 0x29, 0x17, 0x55, 0x6b, 0x44, 0x7a, 0x38, 0x06,
    0xbc, 0x82, 0xc0, 0xfe, 0x59, 0x67, 0x25, 0x1b, 0xa1, 0x9f, 0xdd, 0xe3,
    0xcc, 0xf2, 0xb0, 0x8e, 0x34, 0x0a, 0x48, 0x76, 0x16, 0x28, 0x6a, 0x54,
    0xee, 0xd0, 0x92, 0xac, 0x83, 0xbd, 0xff, 0xc1, 0x7b, 0x45, 0x07, 0x39,
    0xc7, 0xf9, 0xbb, 0x85, 0x3f, 0x01, 0x43, 0x7d, 0x52, 0x6c, 0x2e, 0x10,
    0xaa, 0x94, 0xd6, 0xe8, 0x88, 0xb6, 0xf4, 0xca, 0x70, 0x4e, 0x0c, 0x32,
    0x1d, 0x23, 0x61, 0x5f, 0xe5, 0xdb, 0x99, 0xa7, 0xb2, 0x8c, 0xce, 0xf0,
    0x4a, 0x74, 0x36, 0x08, 0x27, 0x19, 0x5b, 0x65, 0xdf, 0xe1, 0xa3, 0x9d,
    0xfd, 0xc3, 0x81, 0xbf, 0x05, 0x3b, 0x79, 0x47, 0x68, 0x56, 0x14, 0x2a,
    0x90, 0xae, 0xec, 0xd2, 0x2c, 0x12, 0x50, 0x6e, 0xd4, 0xea, 0xa8, 0x96,
    0xb9, 0x87, 0xc5, 0xfb, 0x41, 0x7f, 0x3d, 0x03, 0x63, 0x5d, 0x1f, 0x21,
    0x9b, 0xa5, 0xe7, 0xd9, 0xf6, 0xc8, 0x8a, 0xb4, 0x0e, 0x30, 0x72, 0x4c,
    0xeb, 0xd5, 0x97, 0xa9, 0x13, 0x2d, 0x6f, 0x51, 0x7e, 0x40, 0x02, 0x3c,
    0x86, 0xb8, 0xfa, 0xc4, 0xa4, 0x9a, 0xd8, 0xe6, 0x5c, 0x62, 0x20, 0x1e,
    0x31, 0x0f, 0x4d, 0x73, 0xc9, 0xf7, 0xb5, 0x8b, 0x75, 0x4b, 0x09, 0x37,
    0x8d, 0xb3, 0xf1, 0xcf, 0xe0, 0xde, 0x9c, 0xa2, 0x18, 0x26, 0x64, 0x5a,
    0x3a, 0x04, 0x46, 0x78, 0xc2, 0xfc, 0xbe, 0x80, 0xaf, 0x91, 0xd3, 0xed,
    0x57, 0x69, 0x2b, 0x15};

unsigned crc8(unsigned crc, unsigned char *data, size_t len)
{
    unsigned char *end;

    if (len == 0)
        return crc;
    crc ^= 0xff;
    end = data + len;
    do {
        crc = crc8_table[crc^*++data];
    } while (data < end);
    return crc ^ 0xff;
}


/* The LCD screen is accessible via the character device /dev/fb0. A char represents a matrix of 8x1 px. To use it
 * we map the device into a char* and access it directly. 
 * 
 * 3 macros do the direct access to this file. These are :
 * 	-draw(row,col,character,style) for a single character 
 *	-printscr(row, col, string, style) for strings, The escape sequences '\n' and '\r' are recognised. 
 * 	-clear_screen()
 * 
 * Since we only use text messages, we divided the screen into 5x16 byte regions to print one character. We have 12x8
 * such regions. The characters used are from a homemade, ugly, charset. Some additionnal characters are used for the 
 * battery gauge. 
 * 
 * 
 * 
 * User Interface also includes the LEDs and the buttons of the EV3. 
 * The button at the bottom left of the screen is used to cleanly shutdown this program and also the whole EV3 by setting the
 * halt flag.
 * The LEDs are used in a complement to the screen to inform the user :
 * 	- in mode 1 (green static light) everything is OK, program works fine
 * 	- in mode 2 (red static), the connection is down
 * 	- in mode 3 (orange static), the EV3 is shutting down
 * 	- in mode 5 (regularly blinking red), the watchdog is awaken
 * 	- in mode 9 (frenetically blinking orange light), the battery is empty
 */
void *Ui()
{ 
 static unsigned char* screen, *dbuf;
 static int charindex,colvar,rowvar,theline,thecolumn,ch;//these are the variables used by the macros that handle the display
 char batterie [13],led_command[2]= { 0, 0};
 int id, transcounter=0;
 UI *pButtons, *pLeds;
 char led=0;
 
 struct fb_var_screeninfo var;
 struct fb_fix_screeninfo fix;
 
 //set a clock for screen update
 struct ev3_periodic_info info_ui;
 ev3_make_periodic(SCREEN_PERIOD,&info_ui);
 
 /*OPEN CONTROL PANEL DEVICE*/
 if((ui_fd = open(UI_DEVICE_NAME, O_RDWR | O_SYNC)) == -1)
 {
	printf("Failed to open control panel device\n");
	exit(1);
 }
 //Map kernel device to be used at the user space level
 pButtons = (UI*)mmap(0, sizeof(UI), PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, ui_fd, 0);
 if (pButtons == MAP_FAILED)
 {
	printf("Failed to map buttons\n");
	exit(1);
 }
 printf("Control panel device ready\n");
 
 
 
 /* OPEN LCD DEVICE*/
 //Standard POSIX calls to access the /dev file
 lcd_fd = open(LCD_DEVICE_NAME, O_RDWR);
 if (lcd_fd < 0)
	printf("Failed to open LCD device\n"); //Lauro Ojeda added this instead of LogErrorNumber
	
 dbuf=malloc(SCREEN_SIZE*sizeof(char)); //initialise dbuf, the local buffer
 screen=malloc(SCREEN_SIZE*sizeof(char));//initialise screen, the buffer where the device is mapped

 screen = (unsigned char *)mmap(0, var.yres * fix.line_length, PROT_WRITE | PROT_READ, MAP_SHARED, lcd_fd, 0);
 if (screen == MAP_FAILED)
	printf("Mapping function failed\n");
 
 //IOCTLs to get the screen dimensions
 if(ioctl(lcd_fd, 0x4600  , &var ) == -1)  
	printf("IOCTL function call failed\n"); 	 
 if(ioctl(lcd_fd, 0x4602  , &fix ) == -1) 
	printf("IOCTL function call failed\n");
 
 /* This file contains the unique identifier BD_ADDR of the EV3 module in the field SerialStr
	* The byte that is used as unique identifier iSerialNumber in the USB communication protocol
	* is a CRC8 of this value. We perform the CRC8 operation here again to show the identifier of
	* the device on the screen.
	*/
 int id_fd=open("/home/root/lms2012/sys/settings/UsbInfo.dat",O_RDONLY);
 if (id_fd < 0)
	 printf("Failed to get device ID\n");

 //read and parse the file to get the value, get the id as a byte
 char *_id=malloc(50*sizeof(char));
 read(id_fd,_id,50);
 while(*++_id!='=') continue;
 while(*++_id!='=') continue;
 *_id++,_id[12]=0;
 id = crc8( 0L, 0, 0 );
 id = crc8( id, _id, 12 );
		
 //performs an atoi for an 8 bits integer
 _id[0]=(id>=200)?'2':(id>=100)?'1':' ';
 _id[1]=(id<10)?' ':48+(int)((id%100)/10);
 _id[2]=(id%10)+48;
 _id[3]=0;



 clear_screen();

 printscr(0,0,"\
 RPI DRIVER\r\n\n\
   ID: \r\n\
 CONN: [ ]\r\n\
TRANS: [ ]\r\n\
   WD: [ ]"
				 ,WHITE);
 printscr(0,1,"RPI DRIVER\r\n",BLACK);
 printscr(2,7,_id,WHITE);

 printf("finished screen init\n");
 close(id_fd);
 
 while(1)
 {	
	
	ev3_wait_period(&info_ui);	
	 
 //Display a battery gauge
	if(lbattery>= EV3_BATTLEVEL_1)
	{
	 sprintf(batterie,"%s%s%s\n\r     %s%s%s",BAT0_2_HIGH,BAT1_2_HIGH,BAT2_2_HIGH,BAT0_1_HIGH,BAT1_1_HIGH,BAT2_1_HIGH);
	}
	else if(lbattery<EV3_BATTLEVEL_1 && lbattery >=EV3_BATTLEVEL_2)
	{
	 sprintf(batterie,"%s%s%s\n\r     %s%s%s",BAT0_2_HIGH,BAT1_2_HIGH,BAT2_2_HALF,BAT0_1_HIGH,BAT1_1_HIGH,BAT2_1_HALF);
	}
	else if(lbattery<EV3_BATTLEVEL_2 && lbattery >=EV3_BATTLEVEL_3)
	{
	 sprintf(batterie,"%s%s%s\n\r     %s%s%s",BAT0_2_HIGH,BAT1_2_HIGH,BAT2_2_LOW,BAT0_1_HIGH,BAT1_1_HIGH,BAT2_1_LOW);
	}
	else if(lbattery<EV3_BATTLEVEL_3 && lbattery >=EV3_BATTLEVEL_4)
	{
	 sprintf(batterie,"%s%s%s\n\r     %s%s%s",BAT0_2_HIGH,BAT1_2_HALF,BAT2_2_LOW,BAT0_1_HIGH,BAT1_1_HALF,BAT2_1_LOW);
	}
	else if(lbattery<EV3_BATTLEVEL_4 && lbattery >=EV3_BATTLEVEL_5)
	{
	 sprintf(batterie,"%s%s%s\n\r     %s%s%s",BAT0_2_HIGH,BAT1_2_LOW,BAT2_2_LOW,BAT0_1_HIGH,BAT1_1_LOW,BAT2_1_LOW);
	}
	else
	{
	 sprintf(batterie,"%s%s%s\n\r     %s%s%s",BAT0_2_LOW,BAT1_2_LOW,BAT2_2_LOW,BAT0_1_LOW,BAT1_1_LOW,BAT2_1_LOW);
	 led=9;
	}

	printscr(6,5,batterie,WHITE);

 //display if connection is still available
 if(conn_state)
 {
	draw(3,8,'X',WHITE);//CONN
 }
 else
 {
	draw(3,8,' ',WHITE); //CONN
 }

 //"animation" to show that the EV3 is still alive
 switch(transcounter++%4)
	{
	 case 0:
		draw(4,8,'-',WHITE);
		break;
	 case 1:
		draw(4,8,'\\',WHITE);
		break;
	 case 2:
		draw(4,8,'|',WHITE);
		break;
	 case 3:
		draw(4,8,'/',WHITE);
		break;
	 default:
		break;
	}
		
	//show if Watchdog is activated (if USB communication is pending)
	if(WDflag)
	{
	draw(5,8,'X',WHITE);
	}
	else
	{
	draw(5,8,' ',WHITE); 
	}
	
	//Handle the control panel device
	if(led!=9)
	{
	 if(!conn_state) led=2;
	 else if(WDflag) led=5;
	 else led=1;
	}
	
		
	if(pButtons->Pressed[BACK_BUTTON - 1])
	{
	 halt=1;
	 led=3;
	 clear_screen();
	 printscr(4,2,"CLOSING",WHITE);
	}
	
 led_command[0] = '0' + led;
 write(ui_fd, led_command, 2);
 memcpy(screen,dbuf,SCREEN_SIZE);
 
 if(halt)
	break;
 } 
 
 munmap(screen,var.yres * fix.line_length);
 munmap(pButtons, sizeof(UI));
 
 close(ui_fd);
 close(lcd_fd);
 printf("User interface closed\n");
}

/* The aim of this task is to ensure that the USB communication is always active. After the WATCHDOG_PERIOD,
 * if nothing has been received, the Watchdog flag WD still is down  and watchdog awakens. It stops 
 * the motors and waits until the communication starts again. 
 * The Wtachdog of course has a higher priority than the threads it watches. 
 */
void* Watchdog()
{
 nice(-5);
 
 struct ev3_periodic_info info;
 ev3_make_periodic(WATCHDOG_PERIOD,&info);
 
 while(1)
 {
	ev3_wait_period(&info);
	if(WD)
	{
	 WD=1;	 
	 WDflag=1;
	 printf("Wuf\n");
	}
	else
	{
	 if(WDflag) startflag=1;
	 WDflag=0;
	 WD=1;
	}
	if(halt)
	 break;
 }
 printf("Watchdog closed\n");
}

/* The Loop function, executed by a fork of main, does the principal work, it handles the USB communication, 
 * the sensors and the actuators. 
 * First it opens and handles the /dev files of all these devices, then does a loop where it sends the measurements
 * of the analog sensors and the encoders by USB and applies the commands it receives to the actuators.
 * This process communicates with the others through shared memory.
 */

void* Loop()
{
 struct ev3_measurement_struct com_measures;
 struct ev3_control_struct com_control;
 static char USBMeasures [sizeof(com_measures)];

 MOTORDATA *pMotorData;
 unsigned char buf [0x0800];

 int sensorvalues [4];
 ANALOG *pAnalog;
 int i, l=sizeof(com_measures), l2=sizeof(com_control),flag=0;
 float battery;

 /*USB COMMUNICATION*/
 usb_fd = open(USBDEV_DEVICE_NAME, O_RDWR | O_NONBLOCK | O_ASYNC);
 if(usb_fd < 0)
 {
	printf("Failed to open USB device\n");
	exit(1);
 }
 
 printf("Opened USB device\n");
 
 /*MOTORS*/
 motor_fd = open(PWM_DEVICE_NAME, O_WRONLY);
 if(motor_fd < 0)
 {
	printf("Failed to open motor device\n");
	exit(1);
 }

 printf("Opened motor device\n");
 
 /*ENCODERS*/
 encoder_fd= open(MOTOR_DEVICE_NAME, O_RDWR | O_SYNC);  
 if(encoder_fd < 0 )
 {
	printf("Failed to open encoder device\n");
	exit(1);
 }

 printf("Opened encoder device\n");

 pMotorData = (MOTORDATA*)mmap(0, sizeof(MOTORDATA)*vmOUTPUTS, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, encoder_fd, 0);
 if (pMotorData == MAP_FAILED)
 {
	printf("Mapping encoders failed\n");
	exit(1);
 }


 /*ANALOG SENSORS*/
 analog_fd = open(ANALOG_DEVICE_NAME, O_RDWR | O_SYNC);
 if(analog_fd  < 0)
 {
	printf("Failed to open Analog device\n");
	exit(1);
 }

 printf("Opened analog device\n");

 pAnalog = (ANALOG*)mmap(0, sizeof(ANALOG), PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, analog_fd, 0);
 if (pAnalog == MAP_FAILED)
 {
	printf("Mapping device failed\n");
	exit(1);
 }

 printf("Ready...\n");
    
 // Start the motors
 startflag=1;
 motor_allow=1;
		 
 printf("Initialization finished...\n");

 battery=2*pAnalog->Cell123456;

 while(1)
 {
 /*ANALOG MEASUREMENTS*/
	for(i = 0;i<4;++i)
	{
		com_measures.input_ADC[i]=pAnalog->Pin6[i][pAnalog->Actual[i]];
		battery=(battery+2*pAnalog->Cell123456)/2;//battery voltage in mV, with a 5 value average for better accuracy
	}
	
	//load the battery value into the shared memory for use in Lcd(). It will be a 8bit value, enough for our use
	
	lbattery=(int)battery/100;  
	com_measures.battery_voltage=battery;	
  com_measures.motor_angle[0]=pMotorData[0].TachoSensor;
	com_measures.motor_angle[1]=pMotorData[1].TachoSensor;
	com_measures.motor_angle[2]=pMotorData[2].TachoSensor;
	com_measures.motor_angle[3]=pMotorData[3].TachoSensor;

	#ifdef DEBUG
	printf("\
	Measures : ADC        %3d | %3d | %3d | %3d  \n\r\
						 Encoders   %3d | %3d | %3d | %3d \n\r\
						 Batt : %fV \n\n\n\r",
					 com_measures.input_ADC[0],
					 com_measures.input_ADC[1],
					 com_measures.input_ADC[2],
					 com_measures.input_ADC[3],
					 com_measures.motor_angle[0],
					 com_measures.motor_angle[1],
					 com_measures.motor_angle[2],
					 com_measures.motor_angle[3],
					 com_measures.battery_voltage);
	#endif
	
	//if the length of the string got is null, there is no more communication, appear as disconnected
	if(USBGetString(buf,l2))
	{
	 memcpy(&com_control,buf,l2);
	 conn_state=1;
	}
	else
	{
	 conn_state=0;
	}
	
	#ifdef DEBUG
	printf("\
	Control  : consigne %2d | %2d | %2d | %2d\n\r\
						 reset encoders %d\n\n\r",
				 com_control.motor_power[0],
				 com_control.motor_power[1],
				 com_control.motor_power[2],
				 com_control.motor_power[3],
				 com_control.init_encoders);
				 
	#endif

	/*MOTOR COMMAND*/

	if(com_control.init_encoders)
	 resetEncoders=1;
	
	setPower(MOTOR_PORT_A,com_control.motor_power[0]);
	setPower(MOTOR_PORT_B,com_control.motor_power[1]);
	setPower(MOTOR_PORT_C,com_control.motor_power[2]);
	setPower(MOTOR_PORT_D,com_control.motor_power[3]);
	
	//transform the struct com_measures into a char* for the transfer via USB
	memcpy(USBMeasures, &com_measures, sizeof(com_measures));	
	write(usb_fd,USBMeasures,l);	
	
	if(halt)
	 break;
 }
 
 WDflag=1;
 setPower(MOTOR_PORT_A,0);
 setPower(MOTOR_PORT_B,0);
 setPower(MOTOR_PORT_C,0);
 setPower(MOTOR_PORT_D,0);
 
 munmap(pAnalog,sizeof(ANALOG));
 munmap(pMotorData, sizeof(MOTORDATA));
 
 close(analog_fd);
 close(motor_fd);
 close(encoder_fd);
 close(usb_fd);
 printf("Loop stopped\n");
}

//only initialises the shared memory and then forks into the 3 processes
int main(int argc, char**argv)
{
 int pid;
 
 //Shared memory region, is mapped into the char* ptr
 const char *memname = "shared"; 
 const size_t region_size = sysconf(8*sizeof(char));

 int shm_fd = shm_open(memname, O_CREAT | O_TRUNC | O_RDWR, 0666);
 int r = ftruncate(shm_fd, region_size); 
 ptr=mmap(0, region_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
 
 //values at start
 halt=0;
 WD=1;
 WDflag=0;
 conn_state=0;
 motor_allow=0;
 startflag=1;
 resetEncoders=0;
 lbattery=0;
 
 pid=fork();
 
 if(pid==0)
 {
	pid=fork();
	if(pid==0)
	 Watchdog();
	else
	{
	 Ui();
	 wait(0);
	}
 }
 else
 {
	sleep(1);//otherwise if the USB is not plugged, the motors have a little bump at start
	Loop(); 
  wait(0); 
	printf("All processes have exited properly, ready to shut down\n");
	sleep(1);
 } 
 return 0;
}
