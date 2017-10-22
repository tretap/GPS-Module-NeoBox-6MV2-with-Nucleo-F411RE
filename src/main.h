#include <mbed.h>

Serial pc(USBTX,USBRX);
Serial device(PA_9,PA_10);

DigitalOut myled(LED1);
DigitalIn button(USER_BUTTON);

union float_buff {
  float f;
  char fBuff[sizeof(float)];
};

int i_command[10];
int ID = 0x00;

//variable for protocol
int id_sate[12]; //GPGSA
int svs[4]; // GPGSV
float_buff longitude,latitude; // GPGLL
char lg = 0x00, lt = 0x00; // GPGLL

int check_number_point(char word);

void send_protocol(char pn);
void check_error();
void receive_command();
void check_PNC(char *cmd);
void show_data(char *cmd,char *data);
