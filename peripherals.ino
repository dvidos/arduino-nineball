typedef struct {
  unsigned char lampNo;
  unsigned int timeMs;
  unsigned int timeLeft;
} blinkingLampsType;

typedef struct {
  unsigned char switchNo: 6; // we have 5 x 8 = 40 switches, 6 bits => 64 values
  unsigned char wasPressed: 1;
} switchInfoType;

typedef struct {
  unsigned char coilNo;
  unsigned int timeLeft;
} coilTimerType;

typedef struct {
  unsigned char two_bcd_digits[4];
} bigNumType;

#define MAX_BLINKING_LAMPS 8

unsigned char lampMatrix[8]; // up to 64 lamps, 8 bytes
blinkingLampsType blinkingLamps[8]; // up to 8 lamps blinking at any time
switchInfoType switchInfo[8]; // up to 8 switch events queue
coilTimerType coilTimers[16]; // 16 coils timers
unsigned char coilStatus[2]; // 16 coils
unsigned char scoreDisplay[14];


int getLamp(int lampNo) {
  return (lampMatrix[lampNo / 8] >> (lampNo % 8)) & 0x01;
}

void lampOn(int lampNo) {
  lampMatrix[lampNo / 8] |= 0x01 << (lampNo % 8);
  stopLampBlinking(lampNo);
}

void startLampBlinking(int lampNo, int timeMs) {
  for (char i = 0; i < MAX_BLINKING_LAMPS; i++) {
    if (!blinkingLamps[i].lampNo) {
      blinkingLamps[i].lampNo = lampNo;
      blinkingLamps[i].timeMs = timeMs;
      blinkingLamps[i].timeLeft = timeMs;
      // don't call lampOn()/lampOff() as they turn off blinking too
      lampMatrix[lampNo / 8] ^= 0x01 << (lampNo % 8);
      break;
    }
  }
}

void stopLampBlinking(unsigned char lampNo) {
  for (char i = 0; i < MAX_BLINKING_LAMPS; i++) {
    if (blinkingLamps[i].lampNo == lampNo) {
      blinkingLamps[i].lampNo = 0;
      lampMatrix[lampNo / 8] &= ~(0x01 << (lampNo % 8));
      break;
    }
  }
}

void lampOff(int lampNo) {
  lampMatrix[lampNo / 8] &= ~(0x01 << (lampNo % 8));
  stopLampBlinking(lampNo);
}

void launchCoil(int coilNo) {
}

void launchSound(int soundNo) {
}


