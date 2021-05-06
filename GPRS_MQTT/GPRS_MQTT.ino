#include "GPRS.h"
#include "MQTT.h"

#include <ArduinoJson.h>
#define JSON_BUFFER_SIZE       400     // Calculate the right number using:
                                       // https://arduinojson.org/v5/assistant/

#define SERIAL_MON_BAUD_RATE   115200  // 115.2K baud serial connection to computer
#define DATA_PUB_FREQUENCY     30000L  // ms -> 30 seconds

StaticJsonDocument<JSON_BUFFER_SIZE> DATA; // Json file that'll contain all data

char mqtt_buffer[JSON_BUFFER_SIZE]; // Container for the data to be sent
unsigned long last_time_published = 0;
int packet_id = 0; // Variable used for demo purposes

void setup() {
  Serial.begin(SERIAL_MON_BAUD_RATE);
  // This function call sets up the module but in a blocking matter!
  GPRS_wake_up();
}

void loop() {
  // Send data each DATA_PUB_FREQUENCY (ms)
  if(millis() - last_time_published >= DATA_PUB_FREQUENCY){
  
    // Construct the Json file you want to send anyway you want
    String data_ = "Packet n° ";
    data_ += String(packet_id++);
    DATA["1st_Tag"]["Sub_Tag"] = data_;
    DATA["2nd_Tag"]["Sub_Tag"]["Sub-Sub-Tag"] = data_;
  
    communicate_();
    
    last_time_published = millis();
  }
}

void communicate_(){

  MQTT_setup();
  MQTT_connect();

  // Prepare the Json file for sending
  serializeJson(DATA, mqtt_buffer);
  
  // Consult MQTT.cpp for send_data()
  send_data(mqtt_buffer);

  Serial.print("Published: ");
  Serial.println(mqtt_buffer);
}
