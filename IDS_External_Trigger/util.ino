void byte2int(byte LSB, byte MSB,int &INTEGER){
  INTEGER = ((int)MSB<<8)|LSB;
}

void int2byte(int INTEGER,byte &LSB, byte &MSB){
  MSB = INTEGER>>8;
  LSB = INTEGER;
}

typedef union{
    float f;
    byte  b[4];
}FLOAT_BYTES;
  
void byte2float(byte BYTES[],float &FLOAT){
  FLOAT_BYTES b2f;
  b2f.b[0] = BYTES[0];
  b2f.b[1] = BYTES[1];
  b2f.b[2] = BYTES[2];
  b2f.b[3] = BYTES[3];
  FLOAT = b2f.f;
}

void float2byte(float FLOAT,byte BYTES[]){
  FLOAT_BYTES f2b;
  f2b.f = FLOAT;
  BYTES[0] = f2b.b[0];
  BYTES[1] = f2b.b[1];
  BYTES[2] = f2b.b[2];
  BYTES[3] = f2b.b[3];
}

uint8_t getBit(byte b, uint8_t bitno){
  return ((b & (1 << bitno))>>bitno);
}

void setBit(byte &b, uint8_t bitno){
  b |= (1<<bitno);
}

void resetBit(byte &b, uint8_t bitno){
  b &= ~(1<<bitno);
}

void toggleBit(byte &b, uint8_t bitno){
  b ^= (1<<bitno);
}

void softwareReset(){
  asm volatile ("  jmp 0");  
}

/*
 * Leonardo Specific
 *    Because Leonardo uses internal USB controller to implement
 *    serial port Serial, a watchdog reset of the controller &
 *    register status is required
 *    Serial Port Connection WILL be interrupted
 */
void slapTheWatchdog(){ 
    WDTCSR=0x18; 
    WDTCSR=0x08; 
    asm volatile ("  wdr");
    while(1); 
} 
