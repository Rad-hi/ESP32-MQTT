#include <PubSubClient.h>

#define MQTT_BROKER       "io.adafruit.com" // Or any other domain
#define MQTT_PORT         1883
#define MQTT_USER         "" // Account info

#define MQTT_PASSWORD     "" // Account info (sometimes known as Key instead of password
                             // Note that MQTT connections could be public; not require a key

#define MQTT_TOPIC        "USER_N/feeds/TOPIC" // "UserName/" + "feeds/" + "TopicName" (if using adafruit.io)

//Setup states
#define MQTT_INIT         0
#define MQTT_CONNECT      1
#define MQTT_SUBSCRIBE    2

TinyGsmClient client(modem);
PubSubClient  mqtt(client);

////Functions' prototypes
void MQTT_setup(int);
void MQTT_callback(char*, byte*, unsigned int);

////Functions declarations
void MQTT_setup(int setup_step){
  switch(setup_step){
  	case MQTT_INIT:{
      mqtt.setServer(MQTT_BROKER, MQTT_PORT);
      mqtt.setCallback(MQTT_callback);
  	  break;
  	}
  	case MQTT_CONNECT:{
      mqtt.connect(MQTT_BROKER, MQTT_USER, MQTT_PASSWORD); //
  	  break;
  	}
  	case MQTT_SUBSCRIBE:{
      mqtt.subscribe(MQTT_TOPIC);
  	  break;
  	}
  	default:
  	  break;
  }
}


//Use this function in case the module is expected to receive data
//as well as send it. In such case, depending on the topic, 
//you can do whatever you want.
void MQTT_callback(char* topic, byte* message, unsigned int len) {
  String recieved_msg = "";
  for (int i = 0; i < len; i++) {
    recieved_msg += (char)message[i];
  }
}
