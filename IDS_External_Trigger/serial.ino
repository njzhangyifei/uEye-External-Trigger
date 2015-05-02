byte lastIndex = 0xFF;  // init value, 0xF0 .. 0xFF
boolean checkIndex(byte index){
  if (index == lastIndex+1){
    // in sync
    lastIndex = index;  // update current index
    return true;
  } else {
    // not correct or overflow
    if (lastIndex == 0xFF && index == 0xF0){
      // overflow
      lastIndex = 0xF0;
      return true;
    }
    // not correct
    if (index <= 0xFF && index >= 0xF0){
      //in range, out of sync
      lastIndex = index;
    } else {
      lastIndex = 0xFF;
    }
    return false;
  }
}

boolean isValidPack(byte * pack, int packSize){
  if (pack[HEADER] == 0xA0 ){
    //correct header
    if (checkIndex(pack[INDEX])){
      //correct index
      if (pack[CHKSUM] == chksum(pack, packSize)){
        return true;
      }
    }
  }
  return false;
}

boolean isResetPack(byte * pack, int packSize){
  boolean resetCommand = false;
  if (pack[HEADER] == 0xB0){
    //correct header
    resetCommand = true;
    for (int i = 1; i < packSize; i++){
      resetCommand &= (pack[i] == 0xFF);
    }
  }
  return resetCommand;
}

byte chksum(byte * pack, int packSize){
  byte sum = 0;
  for (int i = 0; i < (packSize-1); i++){
    sum ^= pack[i];
  }
  return sum;
}
