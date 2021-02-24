#include <string.h>
#include <stdlib.h>
#include <stdio.h>
//
char Rx_Byte = 0;
char Rx_Buf[20];
byte Rx_Buf_Cnt = 0;
byte Rx_State = 0;

char data_buf[10];

int co2_ppm = 0;
int co2_state = 0; // 0 : Normal 1: Warm Up
int co2_error = 0; // 0 : Noraml 1: Co2 Sensor Error

int analogPin = 0;

void setup()
{
  Serial.begin(9600);
  Rx_Byte = 0;
  memset(Rx_Buf, 0x00, 20);
  Rx_Buf_Cnt = 0;
  Rx_State = 0;

  memset(data_buf, 0x00, 10);
}

void loop()
{
  // put your main code here, to run repeatedly:
 //  Serial.println(Serial.available() );
  co2_ppm = analogRead(A0);
  Serial.println(co2_ppm);
  delay(500);
  
/* 이유를 잘 모르겠으나, Serial.available() 값이 계속 0라 아래의 코드가 실행되지 않음... 
   위의 세 줄의 코드로 값이 측정은 되지만 과연 정확한 값인지는 조금 의문..
*/
  while(Serial.available() )
  {
    Rx_Byte = Serial.read();
    //Serial.print(Rx_Byte);

    if(Rx_Byte == '#' && Rx_State == 0) // Start
    {
      Rx_Buf_Cnt =  0;
      Rx_State = 1;
      Rx_Buf[Rx_Buf_Cnt++] = Rx_Byte;
    }
    else if(Rx_State == 1)
    {
      Rx_Buf[Rx_Buf_Cnt++] = Rx_Byte;
      if(Rx_Byte == 0x0d) // End
      {
        Rx_State = 0;

        memset(data_buf, 0x00, 10);
        strncpy(data_buf,Rx_Buf + 2, 4);
        co2_ppm = atoi(data_buf);

        memset(data_buf, 0x00, 10);
        strncpy(data_buf,Rx_Buf + 7, 2);
        if(!strcmp(data_buf,"Nr"))
        {
          //Serial.println("co2_state_Nomarl"); // Test Message
          co2_state = 0;
        }
        else if(!strcmp(data_buf,"WU"))
        {
          //Serial.println("co2_state_WarmUp"); // Test Message
          co2_state = 1;
        }
        
        memset(data_buf, 0x00, 10);
        strncpy(data_buf,Rx_Buf + 10, 2);
        if(!strcmp(data_buf,"Nr"))
        {
          //Serial.println("co2_error_Normal"); // Test Message
          co2_error = 0;
        }
        else if(!strcmp(data_buf,"Er"))
        {
          //Serial.println("co2_error_Error"); // Test Message
          co2_error = 1;
        }
        Serial.print(Rx_Buf);
        Serial.println("Co2 PPM : " + String(co2_ppm,DEC));
        Serial.println("Co2 Status : " + String(co2_state,DEC));
        Serial.println("Co2 Error : " + String(co2_error,DEC));
        
      }
    }
  }
}
