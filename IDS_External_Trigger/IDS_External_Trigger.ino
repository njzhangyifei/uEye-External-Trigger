#include <TimerOne.h>  //too lazy to write everything from ground up

#define TRIGGER_PIN_P 9
#define TRIGGER_PIN_N 10

#define HEADER 0
#define INDEX  1
#define VALUE  2
#define CHKSUM 3

#define INIT_INTERVAL 25000  // 20 hzat startup
#define ONE_SEC_IN_MICROSEC 1000000 // 1 second in microseconds

void updateTriggerRate(uint8_t rate){
  if (rate == 0){
    //not possible
    rate = 1;
  }
  unsigned long interval = ONE_SEC_IN_MICROSEC / rate / 2;
  Timer1.setPeriod(interval);
}

volatile uint8_t triggerState = LOW;
void trigger(){
  if (triggerState == LOW) {
    triggerState = HIGH;
  } else {
    triggerState = LOW;
  }
  digitalWrite(TRIGGER_PIN_P, triggerState);
}

void setup(void){
  Serial.begin(115200);
  pinMode(TRIGGER_PIN_P, OUTPUT);
  pinMode(TRIGGER_PIN_N, OUTPUT);
  digitalWrite(TRIGGER_PIN_P, LOW);
  digitalWrite(TRIGGER_PIN_N, LOW);
  Timer1.initialize(INIT_INTERVAL);  
  Timer1.attachInterrupt(trigger);
  while (!Serial){
    ;  //wait until the serial port is started
    //Leonardo specific code
  }
  Serial.println("TRIGGER CONTROLLER STARTED");
}

/*
 * Serial Port Protocol Definition
 * BAUD 115200, 1 STOP BITS, 0 PARITY, 8 DATA BITS
 *
 * +========+============+=========+==========+
 * | Header |   Index    |  Value  | Checksum |
 * +========+============+=========+==========+
 * | 0xA0   | 0xF0..0xFF | uint8_t | uint8_t  |
 * +--------+------------+---------+----------+
 * 
 * Header - 0xA0 
 * Index  - 0xF0..0xFF
 * Value  - Frame rate
 * Chksum - Checksum for the entire pack
 *
 */
byte serialInput[4];
void loop(){
  if (Serial.available()>=4){
    for (int i = 0; i < 4; i++){
      serialInput[i] = Serial.read();
    }
    if (isValidPack(serialInput, sizeof(serialInput))){
      uint8_t triggerRate = 0;
      // valid input
      triggerRate = serialInput[VALUE];
      updateTriggerRate(triggerRate);
    } else if (isResetPack(serialInput, sizeof(serialInput))) {
      // reset the board, another method is set BAUD to 1200
      Timer1.stop();
      digitalWrite(TRIGGER_PIN_P, LOW);
      digitalWrite(TRIGGER_PIN_N, LOW);
      Serial.println("RESET");
      delay(1);
      slapTheWatchdog();
    }
  }
  delay(100);
}

