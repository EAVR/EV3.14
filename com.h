/* Common definitions for client and server */

#define EV3_NB_MOTORS               4
#define EV3_NB_SENSORS              4
#define TIMEOUT_MS							10
#define MAX_CONTROL							100.0

#define USB_NOT_READ		255

struct ev3_control_struct	{
	unsigned char	init_encoders;
	signed char 	motor_power[EV3_NB_MOTORS];
};

struct ev3_measurement_struct	{
  float		battery_voltage;	
	unsigned int		input_ADC[EV3_NB_SENSORS];
	signed int		motor_angle[EV3_NB_MOTORS];
};

 
void generate_gf();
void gen_poly();
void encode_rs();
void decode_rs();
char* encode_string(char* string); 
char* decode_string(char* string);
