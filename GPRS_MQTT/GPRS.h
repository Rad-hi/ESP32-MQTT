#ifndef __GPRS_H__
  #define __GPRS_H__
  
  // GSM module specification
  #define TINY_GSM_MODEM_SIM800
  #include <TinyGsmClient.h> //https://github.com/vshymanskyy/TinyGSM
  
  // TTGO T-Call pin definitions
  #define MODEM_RST               5
  #define MODEM_PWRKEY            4
  #define MODEM_POWER_ON          23
  #define MODEM_TX                27
  #define MODEM_RX                26
  #define I2C_SDA                 21
  #define I2C_SCL                 22
  #define MODEM_BAUD_RATE         115200  //115.2 Kb/s baud rate
  
  // GPRS init states
  #define GPRS_RESET              0
  #define GPRS_START_SERIAL       1
  #define GPRS_INIT_MODULE        2
  #define GPRS_INIT_MODEM         3
  #define GPRS_CONNECT            4
  #define GPRS_READY_             5
  
  // GPRS data
  #define GPRS_USER               ""
  #define GPRS_PASS               ""
  #define APN                     ""      // APN (google the apn of your network provider in your country)
  #define SIM_PIN                 ""      // If SIM is locked, provide the PUK code
  
  // Timeout connection before automatically restarting
  #define GPRS_CONN_TIMEOUT       10000UL // 10 seconds
  
  // Hardware Serial for builtin GSM Module
  #define SerialAT Serial1 
  
  TinyGsm modem(SerialAT);
  TinyGsmClient client(modem);
  
  void setup_modem();
  void GPRS_init();
  void GPRS_wake_up();
  void GPRS_put_to_sleep();
  bool GPRS_connectivity_status();
  
  
  
  // Setup variables
  unsigned long GPRS_last_update = 0;
  int  gprs_setup_state = 0;
  bool serial_ready  = false;
  bool module_ready  = false;
  bool connect_ready = false;
  
  // Functions' declarations
  void GPRS_init(){
    switch(gprs_setup_state){
      case GPRS_RESET:{
        
        gprs_setup_state = 0;
        serial_ready  = false;
        module_ready  = false;
        connect_ready = false;
        
        gprs_setup_state = GPRS_START_SERIAL;
        GPRS_last_update = millis();
        break;
      }
      case GPRS_START_SERIAL:{
        if(!serial_ready) {
          serial_ready = true;
          SerialAT.begin(MODEM_BAUD_RATE, SERIAL_8N1, MODEM_RX, MODEM_TX);
        }
        if(millis() - GPRS_last_update >= 100){
          gprs_setup_state = GPRS_INIT_MODULE;
          GPRS_last_update = millis();
        }
        break;
      }
      case GPRS_INIT_MODULE:{
        if(!module_ready) {
          module_ready = true;
          setup_modem();
        }
        if(millis() - GPRS_last_update >= 3000){
          gprs_setup_state = GPRS_INIT_MODEM;
          GPRS_last_update = millis();
        }
        break;
      }
      case GPRS_INIT_MODEM:{
        modem.restart();
        gprs_setup_state = GPRS_CONNECT;
        GPRS_last_update = millis();
        break;
      }
      case GPRS_CONNECT:{
        if(!connect_ready) {
          connect_ready = true;
          
          // Unlock SIM if PIN is required (;provided)
          if (SIM_PIN && modem.getSimStatus() != 3 ) {
            modem.simUnlock(SIM_PIN);
          }
          // Connect
          modem.gprsConnect(APN, GPRS_USER, GPRS_PASS);
        }
        if(millis() - GPRS_last_update >= 500){
          gprs_setup_state = GPRS_READY_;
        }
        break;
      }
      case GPRS_READY_:{
        break;
      }
      default: break;
    }
  }
  
  // Please refer to the datasheet of the SIM800 for more details about this process
  void setup_modem(){
    pinMode(MODEM_RST, OUTPUT);
    digitalWrite(MODEM_RST, HIGH);    
    pinMode(MODEM_PWRKEY, OUTPUT);
    pinMode(MODEM_POWER_ON, OUTPUT);
    digitalWrite(MODEM_POWER_ON, HIGH);
    // Pull down PWRKEY for 1 second or more according to manual requirements
    digitalWrite(MODEM_PWRKEY, HIGH);
    delay(10);
    digitalWrite(MODEM_PWRKEY, LOW);
    delay(1000);
    digitalWrite(MODEM_PWRKEY, HIGH);
  }
  
  void GPRS_put_to_sleep(){
     SerialAT.write("AT+CSCLK=2\r"); // Enable sleep mode until external interrupt
                                     // Consult datasheet: https://www.elecrow.com/download/SIM800%20Series_AT%20Command%20Manual_V1.09.pdf
                                     // (Page 160, AT+CSCLK Configure Slow Clock)
  }
  
  void GPRS_wake_up(){
    unsigned long started_init_ = millis();
    do{
      GPRS_init();
    }while(gprs_setup_state != GPRS_READY_
           && (millis() - started_init_ <= GPRS_CONN_TIMEOUT)); 
           
    if(!modem.isGprsConnected()){ // Couldn't connect
      ESP.restart();
    }
  }
  
  bool GPRS_connectivity_status(){
    return modem.isGprsConnected();
   }
#endif
