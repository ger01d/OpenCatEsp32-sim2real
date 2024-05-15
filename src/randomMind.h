#define POWER_SAVER 120  //make the robot rest after a certain period, unit is seconds
#define IDLE_SHORT 5
#define IDLE_LONG 15
#define EVERY_X_SECONDS 10
int idleThreshold = IDLE_SHORT;
#define RANDOM_MIND false//true  //let the robot do random stuffs. use token 'z' to activate/deactivate
int randomInterval = 5000;
const char *randomMindList[] = { "iRand", "i", "ksit", "kscrh", "ksnf", "kcmh",  //"u",
#ifdef CUB
                                 "kfd", "krt",
#else
                                 "kck", "m0 -45 0 45 0 0",
#endif
                                 NULL };
byte choiceWeight[] = {
  15,
  30,
  20,
  3,
  5,
  3,  // 5,
#ifdef CUB
  1,
  1,
#else
  2,
  1,
#endif
};

int randomMindListLength;
int randomBase = 0;
long idleTimer;
long randTimer;

void allRandom() {
  char tokenSet[] = { T_INDEXED_SIMULTANEOUS_BIN, T_INDEXED_SEQUENTIAL_BIN };
  int8_t jointSet[] = { 0, 1, 2, 8, 9, 12, 13, 14, 15 };
  byte rangeSet[] = { 30, 30, 30, 2, 2, 5, 5, 2, 2 };
  //  byte rangeSet[] = {90, 90, 180, 50, 50, 100, 100, 50, 50};

  token = tokenSet[rand() % 2];
  cmdLen = rand() % 4 + 4;
  for (byte r = 0; r < cmdLen; r++) {
    byte j = rand() % sizeof(jointSet);
    newCmd[r * 2] = jointSet[j];
    newCmd[r * 2 + 1] = (int8_t)min(max(int(currentAng[jointSet[j]] - currentAdjust[jointSet[j]] + rand() % rangeSet[j] - rangeSet[j] / 2), -90), 90);
    //    PT(jointSet[j]); PT('\t'); PTL(int(newCmd[r * 2 + 1]));
  }
  cmdLen *= 2;
  newCmd[cmdLen] = '~';
}

void randomMind() {
  if (token != T_CALIBRATE && token != T_REST && idleTimer && (millis() - idleTimer) > idleThreshold * 1000) {  //in idle state
    if (millis() - randTimer > randomInterval) {                                                                //every randomInterval(ms) throw a dice
      randTimer = millis();
      int randomNum = esp_random() % randomBase;
      byte randomChoice = -1;
      while (randomNum >= 0) {
        randomChoice++;
        randomNum -= choiceWeight[randomChoice];
      }
      if (randomChoice == 0)
        allRandom();
      else {
        token = randomMindList[randomChoice][0];
        strcpy(newCmd, randomMindList[randomChoice] + 1);  // this is duable only because newCmd+1 is after newCmd!
      }
      newCmdIdx = 100;
      transformSpeed = 0.2;
    }
  } else
    transformSpeed = 1;
}

void powerSaver(int idleThreshold = 10) {  //unit is second
  if (idleTimer && (millis() - idleTimer) > POWER_SAVER * 1000) {
    if (token != T_REST) {
      token = T_REST;
      newCmdIdx = 4;
      idleTimer = 0;
      //      shutEsp32Servo = true;
    }
  }
}
