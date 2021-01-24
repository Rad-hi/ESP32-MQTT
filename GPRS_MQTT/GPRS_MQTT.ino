#include "GPRS_T.h"
#include "MQTT_T.h"

#include <ArduinoJson.h>

#define SERIAL_MON_BAUD_RATE   115200  // 115.2K baud serial connection to computer
#define GPRS_SETUP_STEPS       4
#define MQTT_CONNECT_ATTEMPS   5
#define MQTT_CONNECT_DELAY     4000    // ms
#define DATA_PUB_FREQUENCY     30000L  // ms -> 30 seconds

#define JSON_BUFFER_SIZE       400     // Calculate the right number using:
                                       // https://arduinojson.org/v5/assistant/

StaticJsonDocument<JSON_BUFFER_SIZE> DATA; // Json file that'll contain all data

unsigned long GPRS_last_sent = 0;
const unsigned long GPRS_setup_delays [] = {0, 1000, 3000, 0, 1000}; // milliseconds
int GPRS_setup_step = 0;
bool GPRS_ready = false;

unsigned long MQTT_last_connection_attempt = 0;
int MQTT_connection_attempt = 0;
bool MQTT_ready = false;
bool MQTT_initialized = false;

unsigned long last_time_published = 0;

int packet_id = 0; // Variable used for demo purposes

void setup() {
  
  Serial.begin(SERIAL_MON_BAUD_RATE);
  
  MQTT_setup(MQTT_INIT); //check MQTT.h for code
  Serial.print("Set MQTT broker: ");
  Serial.println(MQTT_BROKER);

  Serial.println("Modem setup: ");
    
  do{
    //each step of the GPRS setup requires a different delay period,
    //so I made a table that contains the delay period, where its index
    //in the table corresponds to the right step 
    if(GPRS_setup_step < GPRS_SETUP_STEPS
      && (millis() - GPRS_last_sent >= GPRS_setup_delays[GPRS_setup_step])){
        
        ///////////////////////////////////////////////////////////////
        /////         Info for setup steps confirmation           /////  
        ///////////////////////////////////////////////////////////////
        switch(GPRS_setup_step){
          case 1:{
            Serial.println("Step 0: done, serial connection activated!");
            break;
          }
          case 2:{
            Serial.println("Step 1: done, initializing modem...");
            break;
          }
          case 3:{
            Serial.println("Step 2: done, modem: " + modem.getModemInfo());
            Serial.println("Waiting for network...");
            break;
          }
          case 4:{
            Serial.println("Step 3: done, Connecting...");
            break;
          }
        }
        ///////////////////////////////////////////////////////////////
        
        GPRS_init(GPRS_setup_step++);
        GPRS_last_sent = millis();
    }
    
    //if we finished all our steps, but the modem still couldn't connect
    //we reboot, otherwise, we declare the GPRS ready
    if(!GPRS_ready && GPRS_setup_step == GPRS_SETUP_STEPS
      && (millis() - GPRS_last_sent >= GPRS_setup_delays[GPRS_setup_step])){
        if(!modem.isGprsConnected()){
          Serial.println("Rebooting");
          ESP.restart(); //Reboot
        }
        else{
          GPRS_ready = true;
        }
    }
  }while(!GPRS_ready);
  if(GPRS_ready)Serial.println("Connected!");
}

void loop() {
  //Tries to connect every 4 seconds as long as we're not connected yet, 
  //and we haven't exceeded the max number of tries
  if(!MQTT_ready && MQTT_connection_attempt < MQTT_CONNECT_ATTEMPS
    && (millis() - MQTT_last_connection_attempt >= MQTT_CONNECT_DELAY)){
      Serial.print("MQTT Connection attempt n°: ");
      Serial.println(MQTT_connection_attempt);
      MQTT_setup(MQTT_CONNECT);
      MQTT_connection_attempt++;
      MQTT_last_connection_attempt = millis();
  }
  if(!MQTT_ready){
    if(mqtt.connected()){
      MQTT_ready = true;
      Serial.println("Connected to MQTT broker");
      
      // If connected, subscribe
      MQTT_setup(MQTT_SUBSCRIBE);
      Serial.print("Subscribed to: ");
      Serial.println(MQTT_TOPIC);
      
    }

    //if we reached the maximum number of trials and we still haven't connected
    //yet, we reboot
    else if(MQTT_connection_attempt == MQTT_CONNECT_ATTEMPS) ESP.restart();
  }
  
  //if we did send the data and it's been a minute since our last sent
  //(if you wanna change it, change the DATA_PUB_FREQUENCY value) 
  //we get back to the read state -> we read the new data -> we send ...
  if(MQTT_ready && (millis() - last_time_published >= DATA_PUB_FREQUENCY)){
    send_data(packet_id++);
    last_time_published = millis();
  }
  mqtt.loop();
}

void send_data(int id){  
  char buffer[200]; // Container for the data to be sent

  // Construct the Json file you want to send anyway you want
  String data = "Packet n° ";
  data += String(id);
  DATA["1st_Tag"]["Sub_Tag"] = data;
  DATA["2nd_Tag"]["Sub_Tag"] = data;

  // Prepare the Json file for sending
  serializeJson(DATA, buffer);

  // Publish to the topic you want
  Serial.print("Publishing: ");
  mqtt.publish(MQTT_TOPIC, buffer); //publish the data
  Serial.println(data);
}
