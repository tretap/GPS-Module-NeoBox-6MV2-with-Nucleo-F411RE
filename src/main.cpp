/*This Program support only single command because i'm single by trethep.*/

#include "main.h"


int main() {
  pc.baud(9600);
  device.baud(9600);

  char word;
  char data_income[100];

  for(int i = 0; i < sizeof(data_income);i++){
    data_income[i] = 0x00;
  }

  char command[5];

  myled = 1;
  //char buffer[128];

  pc.printf("System Starts.\n"); // for debug.

  while(1){
    //check status working ?
    myled = !myled;

    // Check income information
    if(device.readable()){
      //pc.printf("%c",device.getc());
      if (device.getc() == '$'){

        for (int i = 0; i < sizeof(data_income); i++){
          word = device.getc();

          if (word == '*'){
            //pc.printf("%s .\n",data_income);
            show_data(command,data_income);
            i = sizeof(data_income);

          } else {
            data_income[i] = word;
          }
        }
      }
    }

    if (button == 0){ // for manual receive command
      while(button != 1);
      pc.printf("System has income commands.\n");// for debug.

      for(int count = 0; count < 5; count++){
        command[count] = pc.getc();
      }

      pc.printf("Your Command 's : %s\n" , command);// for debug.
    }

    if (pc.readable()){
      receive_command();
      if (i_command[1] == 0x01 ){
        check_PNC(command);
        //pc.printf("%s",command); // for debug.
      }
    }
  }
}

int check_number_point(char word){
  if ((word >= 48 and word <= 57) or word == 46 ){
    return 1;
  }
  return 0;
}

void receive_command(){ // for save income command protocol to i_command
  char c_word;

  if (pc.getc() == ':'){
    if (pc.getc() == ';'){
      //pc.printf("Come in!"); // Debug.but have same problem like debug in readable from pc (up)
      for(int i=0;i < sizeof(i_command); i++){
        c_word = pc.getc();

        if (c_word == 0xFF){ // 0x7A for debug , real's 0xFF.
          //pc.printf("%s .\n",i_command);
          i = sizeof(i_command);

          //pc.printf("%02X %02X %02X %02X %02X\n",i_command[0],i_command[1],i_command[2],i_command[3],i_command[4]);
        } else {
          i_command[i] = c_word;
        }
      }
    }
  }
}

void check_error(){
  //Not in this version .
}

//This function create for spilt type of PN in command protocol in to command variable
void check_PNC(char *cmd){ // {check package name command}
  char* table_pn[6]; // Table package name
  char cmd_local[6];

  table_pn[0] = "GPGGA";
  table_pn[1] = "GPGSA";
  table_pn[2] = "GPGSV";
  table_pn[3] = "GPGLL";
  table_pn[4] = "GPRMC";
  table_pn[5] = "GPVTG";

  strcpy(cmd_local,table_pn[i_command[2] - 1]);

  for(int count = 0; count < 5; count++){
    cmd[count] = cmd_local[count];
  }

}

//function for check equaltion of income information
void show_data(char *cmd,char *data){
  char title_data[5];
  char data_local[100];

  for(int i = 0;i < sizeof(data_local);i++){
    data_local[i] = 0x00;
  }

  //for swap Like "GPGSV" to check
  for(int j= 0;j < sizeof(title_data);j++){
    title_data[j] = data[j];
  }


  //Check and return information to serial pc
  if((strncmp(cmd,"GPGSA",5) == 0) and (strncmp(title_data,"GPGSA",5) == 0)){
    //pc.printf("\n%s\n",data); //for debug.

    char save_information[12][3];

    for(int i= 0; i < 12;i++){
        for(int j= 0;j <3;j++){
            save_information[i][j] = NULL;
        }
    }

    int count_gll = 1, count_array = 0;

    sscanf(data,"GPGSA,%s",data);

    //pc.printf("%s\n",data);for Debug.

    for(int i=0; i < sizeof(data);i++){
      if(data[i] == 44 or data[i] == 42){
        count_gll++;
        count_array = 0;
      }else{ // edit here for set information .
        if(count_gll >= 3 and count_gll <= 14){
          if(check_number_point(data[i])){
              save_information[count_gll-3][count_array] = data[i];
              count_array++;
          }
        }
      }
    }

    for(int i=0;i < 12;i++){
        sscanf(save_information[i],"%d",&id_sate[i]);
    }

    /*
    for(int i =0;i < 12;i++){
      pc.printf("%d",id_sate[i]);
      if(i == 12){pc.printf("\n");}
    }*/

    send_protocol(0x02);
    strcpy(cmd,"ABCD");// for reset command variable protect bug.
  }

  if((strncmp(cmd,"GPGSV",5) == 0) and (strncmp(title_data,"GPGSV",5) == 0)){
    //pc.printf("\n%s\n",data); //for debug.
    char id_s[4][3]; // Id of satellites

    for(int i= 0; i < 4;i++){
      for(int j= 0;j <3;j++){
          id_s[i][j] = 0x00;
      }
    }

    char ev_s[4][3]; // Elevation degrees

    for(int i= 0; i < 4;i++){
      for(int j= 0;j <3;j++){
          ev_s[i][j] = 0x00;
      }
    }

    char azm_s[4][4];// Azimuth degrees

    for(int i= 0; i < 4;i++){
      for(int j= 0;j < 4 ;j++){
          azm_s[i][j] = 0x00;
      }
    }

    char snr_s[4][3];//Signal Strength

    for(int i= 0; i < 4;i++){
      for(int j= 0;j <3;j++){
          snr_s[i][j] = 0x00;
      }
    }

    char total_msg ,p_msg, all_svs;// p_msg(Present msg)

    int count_gll = 1, count_array = 0;

    sscanf(data,"GPGSV,%c,%c",&total_msg,&p_msg);
    sscanf(data,"GPGSV,%s",data_local);

    //pc.printf("%s\n",data);

    for(int i=0; i < sizeof(data_local);i++){
      if(data_local[i] == 44 or data_local[i] == 42){
        count_gll++;
        count_array = 0;
      }else{ // edit here for set information .
        if(count_gll == 4 or count_gll == 8 or count_gll == 12 or count_gll == 16 ){
          if(check_number_point(data_local[i])){
               id_s[(count_gll/4)-1][count_array] = data_local[i];
               count_array++;
          }
        }

        if(count_gll == 5 or count_gll == 9 or count_gll == 13 or count_gll == 17){
          if(check_number_point(data_local[i])){
                if(count_gll == 5){ev_s[0][count_array] = data_local[i];}
                else if(count_gll == 9){ev_s[1][count_array] = data_local[i];}
                else if(count_gll == 13){ev_s[2][count_array] = data_local[i];}
                else if(count_gll == 17){ev_s[3][count_array] = data_local[i];}
                count_array++;
          }
        }

        if(count_gll == 6 or count_gll == 10 or count_gll == 14 or count_gll == 18){
          if(check_number_point(data_local[i])){
              if(count_gll ==6){azm_s[0][count_array] = data_local[i];}
              else if(count_gll ==10){azm_s[1][count_array] = data_local[i];}
              else if(count_gll ==14){azm_s[2][count_array] = data_local[i];}
              else if(count_gll ==18){azm_s[3][count_array] = data_local[i];}

              count_array++;
          }
        }

        if(count_gll == 7 or count_gll == 11 or count_gll == 15 or count_gll == 19){
          if(check_number_point(data_local[i])){
              snr_s[((count_gll-3)/4) -1][count_array] = data_local[i];
              count_array++;
          }
        }
      }
    }

    //pc.printf("ID : %s, %s, %s, %s\n",id_s[0],id_s[1],id_s[2],id_s[3]);//for test
    //pc.printf("EVS : %s, %s, %s, %s\n",ev_s[0],ev_s[1],ev_s[2],ev_s[3]);//for test
    //pc.printf("AZM_S : %s, %s, %s, %s\n",azm_s[0],azm_s[1],azm_s[2],azm_s[3]);// for test
    //pc.printf("SNR_S : %s, %s, %s, %s\n",snr_s[0],snr_s[1],snr_s[2],snr_s[3]); // for test

    int id_s_int[4];

    for(int i = 0;i < 4;i++){
      sscanf(id_s[i],"%d",id_s_int[i]);
    }

    for(int i = 0;i < sizeof(id_s); i++){
      if(id_s_int[i] == i_command[3]){
        svs[0] = id_s_int[i];
        sscanf(ev_s[i],"%d",svs[1]);
        sscanf(azm_s[i],"%d",svs[2]);
        sscanf(snr_s[i],"%d",svs[3]);
        send_protocol(0x03);
        strcpy(cmd,"ABCD");
      }
    }

    if (total_msg == p_msg){
      send_protocol(0x03);
      strcpy(cmd,"ABCD");// for reset command variable protect bug.
    }
  }

  if((strncmp(cmd,"GPGLL",5) == 0) and (strncmp(title_data,"GPGLL",5) == 0)){
    //pc.printf("\n%s\n",data); //for debug.
    latitude.f = 0x00;
    longitude.f = 0x00;

    char lat[50], lot[50];
    int count_gll = 1, count_lt = 0, count_lg = 0;

    char * pch;
    char * now;

    int count =1;

    strncpy(data_local,data,sizeof(data_local));
    sscanf(data_local,"GPGLL,%s",&data_local);

    for(int i=0; i < sizeof(data_local);i++){
      if(data_local[i] == 44 or data_local[i] == 42){
        count_gll++;
      }else{
        if(count_gll == 1){
          if(check_number_point(data_local[i])){
              lat[count_lt] = data_local[i];
              count_lt++;
          }
        }

        if(count_gll == 2){
          lt = data_local[i];
        }

        if(count_gll == 3){
          if(check_number_point(data_local[i])){
              lot[count_lg] = data_local[i];
              count_lg++;
          }
        }

        if(count_gll == 4){
          lg = data_local[i];
        }
      }
    }

    sscanf(lat,"%f",&latitude.f);
    sscanf(lot,"%f",&longitude.f);

    //pc,printf("Latitude : %f , Longuitude : %f , Lt : %c , Lg : %c",latitude.f,longitude.f,lt,lg );
    //pc.printf("Count : %d\n",count); // for debug.

    send_protocol(0x04);
    strcpy(cmd,"ABCD");// for reset command variable protect bug.
  }
}

void send_protocol(char pn){
  // wait for edition version.

  //Header Package send : and ; (:;)
  //ID Device variable : ID
  // Calculate Length of package
  char len_package = 0x00;
  char check_sum = 0x00;

  // Calculate Check sum
  // Package Name receive from argument
  // Package Data id_sate or svs or longitude,latitude and lg , lt

  if(pn == 0x02){
    len_package = 0x0C;
    check_sum = pn + len_package + ID;

    for(int i = 0; i < 12;i++){
      check_sum = check_sum + id_sate[i];
    }
    check_sum = ~check_sum;

    // Sending Section //
    pc.putc(':');
    pc.putc(';');
    pc.putc(ID);
    pc.putc(len_package);
    pc.putc(pn);

    for(int i =0; i < 12;i++){
      pc.putc(id_sate[i]);
    }

    pc.putc(check_sum);
    pc.putc(0xFF);

  }

  else if(pn == 0x03){
    len_package = 0x04;
    check_sum = pn + len_package + ID;

    for(int i = 0; i < 4;i++){
      check_sum = check_sum + svs[i];
    }
    check_sum = ~check_sum;

    // Sending Section //
    pc.putc(':');
    pc.putc(';');
    pc.putc(ID);
    pc.putc(len_package);
    pc.putc(pn);

    for(int i =0; i < 4;i++){
      pc.putc(svs[i]);
    }

    pc.putc(check_sum);
    pc.putc(0xFF);
  }

  else if(pn == 0x04){
    len_package = 0x0A;
    check_sum = pn + len_package + ID + lg +lt;

    for(int i = 0; i < 4;i++){
      check_sum = check_sum + latitude.fBuff[i];
    }

    for(int i = 0; i < 4;i++){
      check_sum = check_sum + longitude.fBuff[i];
    }
    check_sum = ~check_sum;

    // Sending Section //
    pc.putc(':');
    pc.putc(';');
    pc.putc(ID);
    pc.putc(len_package);
    pc.putc(pn);

    for(int i =0; i < 4;i++){
      pc.putc(latitude.fBuff[i]);
    }

    for(int i =0; i < 4;i++){
      pc.putc(longitude.fBuff[i]);
    }

    pc.putc(check_sum);
    pc.putc(0xFF);
  }
}
