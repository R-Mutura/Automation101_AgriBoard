#include <Arduino.h>


// Import required libraries
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include<SPIFFS.h>
#include <FS.h>
#include <Wire.h>
//all libraries for sensor use
#include<SPI.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define DHTPIN 34     // Digital pin connected to the DHT sensor 
#define DHTTYPE    DHT22     // DHT 22 (AM2302)

DHT dht(DHTPIN, DHTTYPE);

struct dht22
{
    float humidity;
    float temperature;
    float heatindex;

};
 typedef struct dht22 dht22_function;
 //DHT dht(DHTPIN, DHTTYPE);
dht22_function get_dht22();
dht22 TH_values;
dht22 dth_values;

  /*NPK INCLUDE FUNCTIONS AND USE*/
  #include "npk_uart.hpp"
int nit=0;//variables to store npk values globally for all function access.
int pho=0;
int pot=0;
nutrients nutrient_vals;
//tank level read function decleration
//variable for the get level use
#define Trigger 4
#define Echo 5
const int tank_height = 300;//cm
 long water_level=0;
long get_level();
int get_ph(int a);
// wifi credentials
const char *ssid = "profinet";
  const char *password = "roberto1234";
// Set LED GPIO
const int lightpin = 2;
// Stores LED state
String lightstate="off";
int light_value=0;
 bool light_state=false;
const int ldrpin= 21;
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

//define actuator outputs
#define heater 26
#define humidifier 27
#define pump 14
const int k=100;//constant of propotionality for use with pump
#define  ph_meter_pin 19
#define fan 23
// Replaces placeholder with actual state value
int PH=0;

String processor(const String& var){
  Serial.println(var);
  if(var == "light_status"){
    if(digitalRead(lightpin)){
      lightstate = "ON";
    }
    else{
      lightstate = "OFF";
    }
    Serial.print(lightstate);
    return lightstate;
  }
  else if (var == "temperature_h"){
    //return string(dth_values.temperature);
    return String(random(10,30));
  }
   else if(var == "heater_status"){
    if(digitalRead(heater)){
      return "ON";
    }
    else{
      return "OFF";
    }
  }
  else if(var == "fan_status"){
    if(digitalRead(fan)){
      return "ON";
    }
    else{
      return "OFF";
    }
  }
 else if(var == "humidity" ) {
 //return (String (dth_values.humidity)); 
 return (String(random(0,100)));
 }
 else if(var == "humidifier_status" ){
     if(digitalRead(humidifier)){
      return "ON";
      
     }
     else{
       return "OFF";
       
     }
 }
 else if(var == "ph" ) {
 //return String(PH); 
 return (String(random(10,30)));
 }
 else if(var == "ph_status" )
 {
    if (PH < 5) {
        return "Acidic";
  }
    else if(PH > 9) {
        return "Basic";
    }

    else{
        return "Good";
    }
 }
 else if(var == "nitrogen" ) {
 //return String(nit); 
 return (String(random(0,255)));
 }
 else if(var == "phosphorus" ) {
 //return String(pho); 
 return (String(random(0,255)));
 }

 else if(var == "potassium" ) {
 //return String(pot); 
 return (String(random(0,255)));
 }

 else if(var == "nutrients_status" )
 {
  if (nit==pho && nit == pot) {
        return "Balanced";
  }

  else{
        return "Imbalanced";
  }
  }
  else if(var == "water_level"){
  //return (String(water_level));
  return (String(random(0,100)));
  }
  else if(var == "pump_status" ){
     if(digitalRead(pump)){
      return "ON";
      
     }
     else{
       return "OFF";
       
     }
 } 
  return String(0);
}

void my_handles(){

    // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });
  //load all images and icons
  // /machada logo.png image
server.on("/machada logo.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/machada logo.png", "image/png");
  });
  server.on("/light icon.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/light icon.png", "image/png");
  });
  server.on("/temperature icon.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/temperature icon.png", "image/png");
  });
  server.on("/heater icon.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/heater icon.png", "image/png");
  });
   server.on("/fan icon.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/fan icon.png", "image/png");
  });
  server.on("/humidity icon.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/humidity icon.png", "image/png");
  });
  server.on("/humidifier icon.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/humidifier icon.png", "image/png");
  });
  server.on("/ph icon.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/ph icon.png", "image/png");
  });
  server.on("/nutrients icon.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/nutrients icon.png", "image/png");
  });
  server.on("/water level icon.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/water level icon.png", "image/png");
  });
  server.on("/pump icon.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/pump icon.png", "image/png");
  });//end of icons
//all GPIOS on/off controls
  // Route to set GPIO to HIGH
  server.on("/lighton", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(lightpin, HIGH);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to set GPIO to LOW
  server.on("/lightoff", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(lightpin, LOW);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

   server.on("/heateron", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(heater, HIGH);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to set GPIO to LOW
  server.on("/heateroff", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(heater, LOW);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
   server.on("/fanon", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(fan, HIGH);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to set GPIO to LOW
  server.on("/fanoff", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(fan, LOW);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
   server.on("/humidifieron", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(humidifier, HIGH);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to set GPIO to LOW
  server.on("/humidifieroff", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(humidifier, LOW);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
 server.on("/pumpon", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(pump, HIGH);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to set GPIO to LOW
  server.on("/pumpoff", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(pump, LOW);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
}

 //for loop sensor timing operation
long currenttime=0;
long previoustime=0;
long period =2000;
//for dht timing operation
long current_time=0;
long prev_time =0;
long dhtWait=2000;

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  pinMode(lightpin, OUTPUT);
//set outputs
  pinMode(heater, OUTPUT);
 pinMode(humidifier, OUTPUT);
 pinMode(pump, OUTPUT);
 pinMode(fan, OUTPUT);
//define them as low
 digitalWrite(heater, LOW);
 digitalWrite(humidifier, LOW);
 digitalWrite(pump, LOW);
 digitalWrite(fan, LOW);

 dht.begin();
  
  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

//include all handles

  my_handles();
  // Start server
  server.begin();
}
 
void loop(){
  
  current_time=millis();
  if((currenttime - previoustime)>= period){


 dth_values = get_dht22();
 Serial.print("humidity ="); Serial.println(dth_values.humidity);
  Serial.print("temperature ="); Serial.println(dth_values.temperature);
 if(dth_values.temperature<20 && (digitalRead(heater))==0){
   digitalWrite(heater, HIGH);
 }
  else{
   digitalWrite(heater, LOW);
 }

 if(dth_values.temperature>30 && (digitalRead(fan))==0){
   digitalWrite(fan, HIGH);
 }
  else{
   digitalWrite(fan, LOW);
 }
 

 nutrient_vals=NPK_get_values();
  Serial.print("nitrogen level = "); Serial.print(nutrient_vals.nitrogen_val, DEC); Serial.println(" mg/kg");
  Serial.print("phosphurous level = "); Serial.print(nutrient_vals.phosphorous_val, DEC); Serial.println(" mg/kg");
  Serial.print("potassium level = "); Serial.print(nutrient_vals.potassium_val, DEC); Serial.println(" mg/kg");
  Serial.println("...............");
   nit=nutrient_vals.nitrogen_val;
   pho=nutrient_vals.phosphorous_val;
   pot=nutrient_vals.potassium_val;
  //USE YELLOW LEDS FOR WARNING

 PH = get_ph(ph_meter_pin);
 Serial.printf("PH value is = %d\n",PH);//ph ranges from 0-14

//4. LDR PIN ANALOG READ TO SEE STATE OF LIGHT
  light_value= analogRead(ldrpin);
  light_state = (light_value>=3500)? true: false;

  water_level = get_level();
 Serial.print("water tank level"); Serial.println(water_level);
 ledcWrite(pump, ((tank_height-water_level)*k));
  }
  
 
 
}

dht22_function get_dht22(){
   
   current_time= millis();

   if(current_time-prev_time >= dhtWait){
    TH_values.temperature= dht.readTemperature();
    TH_values.humidity = dht.readHumidity();
    TH_values.heatindex = 0;
    
    if (isnan(TH_values.temperature) || isnan(TH_values.humidity)) {
   Serial.println(F("Failed to read from DHT sensor!"));
   return TH_values={0,0,0};
    }
    prev_time=current_time;
   }
 return TH_values;
}
//function to process ph reading from ph meter and probe
int get_ph(int a){

    //local variable
int phValue =0 ;
int my_buf [10];
 int temp=0;
int totalValue=0;
 Serial.print("getting PH value..");
 
 for(int i=0;i<=10;i++){


 my_buf [i]= analogRead(a);
 }
 //sorting algorithm
 for(int i =0; i<9;i++){
     for (int j = i+1; j < 10; i++)
     {
        if(my_buf [i]>my_buf [j]){
            temp=my_buf [i];
            my_buf [i]=my_buf [j];
            my_buf [j]=temp;
        }
     }
     

 }
 //totalValue=0;
 for(int i=2; i<8;i++){//to read and add the mid 6 values.
     totalValue+=my_buf [i];
   }
 int averageValue=totalValue/6;
 phValue= (map(averageValue,0,4095,0,14));
 Serial.print("Ph_value =");
 Serial.println(phValue);

return phValue;

}

long get_level(){
digitalWrite(Trigger, LOW);  
  delayMicroseconds(2); 
  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10); 
  digitalWrite(Trigger, LOW);
  long duration = pulseIn(Echo, HIGH);
  long height_data = ((duration/2) / 29.09)+6;
  return height_data-= tank_height;
 /* consider the instruction of palcement her in
 https://www.instructables.com/Measuring-water-level-with-ultrasonic-sensor/

 */
}