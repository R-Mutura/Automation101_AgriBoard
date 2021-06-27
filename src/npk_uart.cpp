#include <Arduino.h>

#include "npk_uart.hpp"


#include <stdint.h>

HardwareSerial uart(1);

 
 //enable pins for the rs232 driver
#define RE 8
#define DE 7

byte values[11];

//constant byte code[]
const byte nitro[] = {0x01,0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c};
const byte phos[] = {0x01,0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc};
const byte pota[] = {0x01,0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0};

 void NPK_uart_init(){
// we ll use hardware uart pins 16 and 17
uart.begin(9600, SERIAL_8N1, 17,16);
//17==rx , 16== tx

 pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);
 }

 byte nitrogen(){
  digitalWrite (RE, HIGH);
  digitalWrite(DE, HIGH);
  vTaskDelay(10/portTICK_PERIOD_MS);

  if (uart.write(nitro, sizeof(nitro)==8)){
       digitalWrite (RE, LOW);
       digitalWrite(DE, LOW);
           for(byte i=0; i<7;i++){
               values[i]=uart.read();
               Serial.print(values[i], HEX);

           }
           Serial.println("");

          }
  return values[4];
 }

byte phosphorous(){
  digitalWrite(DE,HIGH);
  digitalWrite(RE,HIGH);
  delay(10);
  if(uart.write(phos,sizeof(phos))==8){
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);
    for(byte i=0;i<7;i++){
    //Serial.print(uart.read(),HEX);
    values[i] = uart.read();
    Serial.print(values[i],HEX);
    }
    Serial.println();
  }
  return values[4];
}
 
byte potassium(){
  digitalWrite(DE,HIGH);
  digitalWrite(RE,HIGH);
  delay(10);
  if(uart.write(pota,sizeof(pota))==8){
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);
    for(byte i=0;i<7;i++){
    //Serial.print(uart.read(),HEX);
    values[i] = uart.read();
    Serial.print(values[i],HEX);
    }
    Serial.println();
  }
  return values[4];
}

 Nutrient_function NPK_get_values(){
    //get the values of the nutrients

nutrients nutrient;

//byte val1,val2,val3;
  nutrient.nitrogen_val = nitrogen();
  vTaskDelay(250/portTICK_PERIOD_MS);
  nutrient.phosphorous_val = phosphorous();
   vTaskDelay(250/portTICK_PERIOD_MS);
  nutrient.potassium_val = potassium();
   vTaskDelay(250/portTICK_PERIOD_MS);
 //  check the type of value received and convert it from uint8_ts to hexadecimals if necessary
   //to write in a continous string
   //csv format for easire sending over WiFI

return nutrient;
}

  