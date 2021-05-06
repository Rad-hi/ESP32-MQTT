#ifndef __GPRS_H__
  #error GPRS.h must be #included before MQTT.h in your main code
#endif

#ifndef __MQTT_H__
  #define __MQTT_H__
  
  #include <PubSubClient.h>  // https://github.com/knolleary/pubsubclient/
  
  #define BROKER            "io.adafruit.com"
  #define MQTT_PORT         1883
  #define MQTT_USER         "" // User name in adafruit.io
  #define MQTT_PASSWORD     "" // MQTT Key in adafruit.io
  #define TOPIC             "" // User_Name/feeds/Topic_Name_Of_Choice
  
  // Create mqtt port (client might be a wifi or gprs client)
  PubSubClient  mqtt(client);
  
  void MQTT_setup();
  void MQTT_connect();
  void MQTT_subscribe();
  void MQTT_callback(char*, byte*, unsigned int);
  void send_data(const char*);
  bool mqtt_state();
  
  
  void MQTT_setup(){
    mqtt.setServer(BROKER, MQTT_PORT);
    //mqtt.setCallback(MQTT_callback);
  }
  
  void MQTT_connect(){
    
    while (!mqtt.connected()) {
      if(mqtt.connect(BROKER, MQTT_USER, MQTT_PASSWORD))return;
      else{
        delay(500);
      }
    }
  }
  
  void MQTT_subscribe(){
     mqtt.subscribe(TOPIC);
  }
  
  // Use this function in case the module is expected to receive data
  // as well as send it. In such case, depending on the topic, 
  // you can do whatever you want.
  void MQTT_callback(char* topic, byte* message, unsigned int len){
    String recieved_msg = "";
    for (int i = 0; i < len; i++) {
      recieved_msg += (char)message[i];
    }
  }
  
  void send_data(const char* buffer){
    mqtt.publish(TOPIC, buffer); // Publish the data
    mqtt.loop();
  }
  
  bool mqtt_state(){
    return mqtt.connected();
  }
#endif
