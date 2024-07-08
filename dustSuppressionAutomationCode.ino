#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include<SoftwareSerial.h>




const char* ssid = "";
const char* password = "";
const char* firebaseFunctionUrl = "";




const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;




#define threshold_value_of_dust_level 30
#define threshold_value_of_moisture_level 50
#define checker_led 27
#define dust_level_limit_exceeded 26
#define moisture_level_under_limit 25
#define movement_of_vehical_stoped 33
#define valve_status 32
#define moisture_level_analog_input 35
#define relayon 6
#define trigPin 4 //RX
#define echoPin 16  //TX
SoftwareSerial pmsSerial(17,5); //(TX,RX)






double dust_level_value=0;
double moisture_level_value=0;
double distance=0;
bool movement_of_vehical=0;
bool valve_status_value=0;




struct pms5003data {
 uint16_t framelen;
 uint16_t pm10_standard, pm25_standard, pm100_standard;
 uint16_t pm10_env, pm25_env, pm100_env;
 uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
 uint16_t unused;
 uint16_t checksum;
};




struct pms5003data data;




double returnMoistureLevel(){
 double sensorValue = analogRead(moisture_level_analog_input);
 sensorValue/=4096.0;
 sensorValue = 100-(100*sensorValue);
 delay(100);
 return sensorValue;
}




double returnDistance(){
 digitalWrite(trigPin, LOW);
 delayMicroseconds(5);
 digitalWrite(trigPin, HIGH);
 delayMicroseconds(10);
 digitalWrite(trigPin, LOW);
 double  duration = pulseIn(echoPin, HIGH);
 double distance = duration*0.034/2;
 return distance;
}




void setup() {
 Serial.begin(115200);
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
   delay(1000);
   Serial.println("Connecting to WiFi...");
 }
 Serial.println("Succesfully connected to WiFi...");
 // Init and get the time
 configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);


 pinMode(dust_level_limit_exceeded, OUTPUT);
 pinMode(moisture_level_under_limit, OUTPUT);
 pinMode(movement_of_vehical_stoped, OUTPUT);
 pinMode(valve_status, OUTPUT);
 pinMode(checker_led, OUTPUT);
 pinMode(moisture_level_analog_input,INPUT);
 pinMode(trigPin, OUTPUT);
 pinMode(echoPin, INPUT);
 pmsSerial.begin(9600);
 // pinMode(relayon,OUTPUT);
}




void loop() {
 while (!readPMSdata(&pmsSerial)) ;
   //reading data was successful!
   Serial.println();
   Serial.println("---------------------------------------");
   Serial.println("Concentration Units (standard)");
   Serial.print("PM 1.0: "); Serial.print(data.pm10_standard);
   Serial.print("\t\tPM 2.5: "); Serial.print(data.pm25_standard);
   Serial.print("\t\tPM 10: "); Serial.println(data.pm100_standard);
   Serial.println("---------------------------------------");
   Serial.println("Concentration Units (environmental)");
   Serial.print("PM 1.0: "); Serial.print(data.pm10_env);
   Serial.print("\t\tPM 2.5: "); Serial.print(data.pm25_env);
   Serial.print("\t\tPM 10: "); Serial.println(data.pm100_env);
   Serial.println("---------------------------------------");
   Serial.print("Particles > 0.3um / 0.1L air:"); Serial.println(data.particles_03um);
   Serial.print("Particles > 0.5um / 0.1L air:"); Serial.println(data.particles_05um);
   Serial.print("Particles > 1.0um / 0.1L air:"); Serial.println(data.particles_10um);
   Serial.print("Particles > 2.5um / 0.1L air:"); Serial.println(data.particles_25um);
   Serial.print("Particles > 5.0um / 0.1L air:"); Serial.println(data.particles_50um);
   Serial.print("Particles > 10.0 um / 0.1L air:"); Serial.println(data.particles_100um);
   Serial.println("---------------------------------------");
 
  dust_level_value=data.pm100_env;
  Serial.print("dust_level_value  : ");
  Serial.println(dust_level_value);


   moisture_level_value = returnMoistureLevel();
   Serial.print("moisture_level_value  : ");
   Serial.println(moisture_level_value);


  distance = returnDistance();
  if(distance<150)movement_of_vehical=1;
  else movement_of_vehical=0;




  Serial.print("Distance of Obstacle  : ");
  Serial.println(distance);
 
  Serial.print("movement_of_vehical  : ");
  Serial.println(movement_of_vehical);


  digitalWrite(checker_led, HIGH);
  int flg = 0;




  if(dust_level_value>=threshold_value_of_dust_level){
     digitalWrite(dust_level_limit_exceeded, HIGH);
     if(moisture_level_value<=threshold_value_of_moisture_level){
       digitalWrite(moisture_level_under_limit, HIGH);
       if(movement_of_vehical==LOW){
         digitalWrite(movement_of_vehical_stoped, HIGH);
         digitalWrite(valve_status, HIGH);
         // digitalWrite(relayon, LOW);
         flg = 1;
       }
       else{
          digitalWrite(movement_of_vehical_stoped, LOW);
       }
    }
    else{
     digitalWrite(moisture_level_under_limit, LOW);
    }
  }
  else{
   digitalWrite(dust_level_limit_exceeded, LOW);
  }




  if(flg==0){
     digitalWrite(dust_level_limit_exceeded, LOW);
     digitalWrite(moisture_level_under_limit, LOW);
     digitalWrite(movement_of_vehical_stoped, LOW);
     digitalWrite(valve_status, LOW);
     // digitalWrite(relayon, HIGH);
  }


   Serial.print("valve_status  : ");
   Serial.println(flg);




 valve_status_value = (valve_status>0);
 valve_status_value = flg;


 delay(1000);
 digitalWrite(checker_led, LOW);


   // Create the HTTP POST request
   int value = 1;
   HTTPClient http;
   http.begin(firebaseFunctionUrl);
   http.addHeader("Content-Type", "application/json");
   StaticJsonDocument<200>jsonDoc;
   jsonDoc["dust"]=dust_level_value;
   jsonDoc["moisture"]=moisture_level_value;
   jsonDoc["distance"]=distance;
   jsonDoc["vehice"]=movement_of_vehical;
   jsonDoc["valve"]=valve_status_value;
   jsonDoc["timeStamp"]=currentTime();
 
   String jsonStr;
   serializeJson(jsonDoc,jsonStr);
 
   int httpResponseCode = http.POST(jsonStr);
   Serial.println(httpResponseCode);
   // Handle the response
   if (httpResponseCode == 200) {
     Serial.println("Data added successfully");
   } else {
     Serial.println("Error adding data to database");
   }
   Serial.println(" ");
}




boolean readPMSdata(Stream *s) {
 if (! s->available()) {
//    Serial.println("1");
   return false;}
 if (s->peek() != 0x42) {
//    Serial.println("2");
   s->read();
   return false;
 }
 if (s->available() < 32) {
//    Serial.println("3")
 ;return false;}
 uint8_t buffer[32];   
 uint16_t sum = 0;
 s->readBytes(buffer, 32);
 for (uint8_t i=0; i<30; i++) {sum += buffer[i];}
 uint16_t buffer_u16[15];
 for (uint8_t i=0; i<15; i++) {
   buffer_u16[i] = buffer[2 + i*2 + 1];
   buffer_u16[i] += (buffer[2 + i*2] << 8);
 }
 memcpy((void *)&data, (void *)buffer_u16, 30);
 if (sum != data.checksum) {
   Serial.println("Checksum failure");
   return false;
 }
 return true;
}




String currentTime(){
 struct tm timeinfo;
 if(!getLocalTime(&timeinfo)){
   Serial.println("Failed to obtain time");
   return "Failed";
 }




 // Add the UTC offset for IST (5 hours and 30 minutes)
 timeinfo.tm_hour += 4;
 timeinfo.tm_min += 30;
 // Handle rollover if minutes exceed 60
 if (timeinfo.tm_min >= 60) {
   timeinfo.tm_min -= 60;
   timeinfo.tm_hour += 1;
 }
 // Handle rollover if hours exceed 24
 if (timeinfo.tm_hour >= 24) {
   timeinfo.tm_hour -= 24;
 }
 Serial.println(&timeinfo,"%B %d %Y %H:%M:%S");




 char timeString[40];
 strftime(timeString, sizeof(timeString), "%B %d %Y %H:%M:%S", &timeinfo);




 String currTime = timeString;
 return currTime; 
}