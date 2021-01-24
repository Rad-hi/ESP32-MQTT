////GSM module specification
#define TINY_GSM_MODEM_SIM800
#include <TinyGsmClient.h> //https://github.com/vshymanskyy/TinyGSM

// TTGO T-Call pin definitions
#define MODEM_RST            		5
#define MODEM_PWRKEY         		4
#define MODEM_POWER_ON       		23
#define MODEM_TX             		27
#define MODEM_RX             		26
#define MODEM_BAUD_RATE      		115200  //115.2 Kb/s baud rate

////GPRS init states
#define GPRS_START_SERIAL			  0
#define GPRS_INIT_MODULE			  1
#define GPRS_INIT_MODEM				  2
#define GPRS_CONNECT            3

////APNs 

#define APN                     "XXXXXXX" // Search for your internet provider's APN online 
#define GPRS_USER               ""
#define GPRS_PASS               ""

// Hardware Serial for builtin GSM Module
#define SerialAT Serial1

TinyGsm modem(SerialAT);


////Functions' prototypes
void setup_modem();
void GPRS_init(int);

////Functions' declarations
void GPRS_init(int init_step){
  switch(init_step){
  	case GPRS_START_SERIAL:{
  	  SerialAT.begin(MODEM_BAUD_RATE, SERIAL_8N1, MODEM_RX, MODEM_TX);
  	  break;
    }
  	case GPRS_INIT_MODULE:{
      setup_modem();
  	  break;
    }
  	case GPRS_INIT_MODEM:{
  	  modem.restart();
  	  break;
    }
  	case GPRS_CONNECT:{
      modem.gprsConnect(APN, GPRS_USER, GPRS_PASS);
  	  break;
    }
  }
}

void setup_modem(){
  pinMode(MODEM_RST, OUTPUT);
  digitalWrite(MODEM_RST, HIGH);    
  pinMode(MODEM_PWRKEY, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);
  digitalWrite(MODEM_POWER_ON, HIGH);
  // Pull down PWRKEY for more than 1 second according to manual requirements
  digitalWrite(MODEM_PWRKEY, HIGH);
  delay(100);
  digitalWrite(MODEM_PWRKEY, LOW);
  delay(1000);
  digitalWrite(MODEM_PWRKEY, HIGH);
}
