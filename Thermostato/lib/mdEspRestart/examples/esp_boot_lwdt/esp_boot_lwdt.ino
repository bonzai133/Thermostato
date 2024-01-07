/*
 * esp_boot_lwdt.ino
 * 
 */

#include <mdEspRestart.h>

// Include one and only one of the following
// language resource file
//
#include "esp_boot_reason_fr.h"
//#include "esp_boot_reason_en.h"

// If defined, the reason for the ESP restart is
// repeatedly sent for 4 minutes at startup. This
// is a way to test POWER ON restarts.
//
//#define MULTI_ANNOUNCEMENT
 
extern "C" {
#include "user_interface.h"
}

#define KEY_MODULE 0
#define LED_MODULE 1
#define BAD_MODULE 2 
#define LOOP_SWITCH 3 
#define MANGLE_MODULE 4

/*
  * Strings to report where the loop watchdog
  * bit.
  */
static const char* moduleNames[] = {
  "keyModule",           
  "ledModule",                
  "badModule", 
  "loopSwitch",
  "mangleModule"
};

void printModuleName(int index) {
  Serial.printf(S_VISITED_MODULE, index);
  if (index == LOOP_END) 
    Serial.print(S_LOOP_END);
  else if ( (index < 0) || (index > 4) )
    Serial.print(S_UNKNOWN_MODULE);
  else
    Serial.print(moduleNames[index]);
}

// LED flasher

#define LED_SHORT_INTERVAL  50  // 50 ms on/off period
#define LED_LONG_INTERVAL  400  // 400 ms on/off period
#define LED_PIN            D4   // = 2 for Arduino 

uint16_t ledInterval = LED_LONG_INTERVAL;
unsigned long ledTime = 0;

void ledModule(void) {
  lwdtStamp(LED_MODULE); // always first instruction of a module
  if (millis() - ledTime > ledInterval) {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    ledTime = millis();
  }
}

void badModule(void) {
  lwdtStamp(BAD_MODULE); // always first instruction of a module
  while(1) { delay(5); }       // the loop watchdog will bite
}

void mangleModule(void) {
  lwdtStamp(MANGLE_MODULE); // always first instruction of a module
  #ifdef DEBUG_LWD_PART
  if (millis() % 2) {
    mangleLwdtFlag();
    Serial.println(S_MANGLED_FLAG);
  } else {
    mangleLwdt();
    Serial.println(S_MANGLED_VARS);
  }  
  #else
  Serial.println(S_NEED_DEBUG_LWD_PART);
  #endif
}
 
char keyModule(void) {
  lwdtStamp(KEY_MODULE); // always first instruction of a module
  if (Serial.available()) {
    while (Serial.available()) {  
      int ich = Serial.read();
      char ch = toupper(ich);
      if ((ch >= 'A') && (ch <= 'K')) { 
        return ch; 
      } 
    }  
  } 
  return 0;  
}



void setup() {
  restartReason_t firstReason;
  restartData_t   firstData;
  restartCount_t  firstCount;
  restartReason_t reason;
  restartData_t   data;
  restartCount_t  count;
  
  Serial.begin(115200);
  delay(100);
  Serial.println(S_APP_NAME);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  Serial.println(S_GPIO_INITIALIZED);  

  if (isUartBoot()) {
    Serial.println(S_UART_BOOT);
    ledInterval = LED_SHORT_INTERVAL;
    while (1) { delay(2); ledModule(); }
  }


  #ifdef MULTI_ANNOUNCEMENT
  for (int i=0; i < 240; i++) {
    Serial.printf(S_REASON_FOR_RESTART, ESP.getResetInfoPtr()->reason, ESP.getResetReason().c_str()); 
    delay(1000);
  }  
  #endif

  //Serial.println("Setting the rtc address");
  if (!setRestartRtcAddress(8)) {
    Serial.println(S_BAD_RTC); 
  }

  // If the sketch is not working do the following and look 
  // carefully at the output on the IDE serial monitor.
  // Don't forget to enable DEBUG_RTC_PART in mdEspRestart.h
  #ifdef DEBUG_RTC_PART
  dumpInfo();
  #endif

  firstReason = getRestartReason(firstCount, firstData);

  //dbg Serial.printf("\ngetRestartReason:: reason: %d, count: %d, data: %d\n", firstReason, firstCount, firstData);

  // Checking that getRestartReason can be called more than once
  // None of the 3 messages should show up.
  reason = getRestartReason(count, data);
    if (reason != firstReason) 
      Serial.printf(S_REASON_CHANGED, reason, firstReason);
    if (count != firstCount) 
      Serial.printf(S_COUNT_CHANGED, count, firstCount);
    if (data != firstData) 
      Serial.printf(S_DATA_CHANGED, data, firstData);

  #ifdef INCLUDE_GETRESETREASONEX
  Serial.printf("\ngetResetReasonEx(): %s\n\n", getResetReasonEx().c_str());
  #endif
    
  Serial.print(S_RESTART_REASON);
  switch (reason) {
    
    case REASON_DEFAULT_RST: 
      Serial.print(S_POWER_ON);
      break;
      
    case REASON_WDT_RST:
      Serial.print(S_HARDWARE_WD);
      break;
      
    case REASON_EXCEPTION_RST:
      Serial.printf(S_EXCEPTION_OCCURED, data);
      break;
      
    case REASON_SOFT_WDT_RST:
      Serial.print(S_SOFTWARE_WD);
      break;
      
    case REASON_SOFT_RESTART: 
      Serial.print(S_RESTART_OR_RESET);
      break;
      
    case REASON_DEEP_SLEEP_AWAKE:
      Serial.print(S_ESP_WOKE);
      break;
      
    case REASON_EXT_SYS_RST:
      Serial.print(S_EXTERNAL_RESET);
      break;
       
    case REASON_USER_RESET:
      Serial.printf(S_USER_RESET, data);
      break;

    case REASON_USER_RESTART:
      Serial.printf(S_USER_RESTART, data);
      break;

    case REASON_LWD_RST:
      Serial.print(S_LWD_RESET);
      printModuleName(data);
      break;
       
    case REASON_LWD_LOOP_RST:
      Serial.print(S_LWD_LOOP_RESET);
      printModuleName(data);
      break;
       
    case REASON_LWD_OVW_RST:
      Serial.print(S_LWD_OVERWRITE_RESET);
      printModuleName(data);
      break;
      
    default:  
      Serial.print(S_UNKNOWN_REASON);     
      break;
  }
  Serial.printf(S_COUNT, count);

  Serial.println();
  for (int i=0; i < USER_INPUT_MESSAGE_COUNT; i++) {
    Serial.println(userInputMessage[i]);
  }  

  ledTime = millis();
  lwdtInit();
}


static const char xyz[] PROGMEM = "This is a string stored in flash";

int res;
restartData_t dat;

boolean skipLastStamp = false;

void loop() {
  lwdtFeed();
    
  ledModule();
  char inch = keyModule();
  
  if (inch != 0)
    Serial.println();

  lwdtStamp(LOOP_SWITCH);  // can be inserted in loop() also
    
  switch (inch) {
    case 'A':
      Serial.println(S_RST_RESTART);
      ESP.restart();
      break;

    case 'B':  
      Serial.println(S_RST_RESET);
      ESP.reset();
      break;
 
    case 'C':  
      Serial.println(S_RST_HWDT);
      ESP.wdtDisable();
      while (1) {} // timeout
      break;
      
    case 'D':
      Serial.println(S_RST_SWDT);
      while (1) {} // timeout
      break;
      
    case 'E':
      Serial.println(S_RST_EXCP_0);
      res = 5 / (2 - 2); 
      // the following is needed otherwise compiler optimisation gets rid of res and the
      // divide by 0 error never occurs. Could also declare res as volatile 
      //     volatile int res;  
      // just before the switch commmand and that would stop optimisation also
      Serial.printf("res = %d", res);  
      break;  
      
    case 'F':
      Serial.println(S_RST_EXCP_3);
      char buff[80];
      strlcpy(buff, xyz, 80);
      break;  

    case 'G':
      dat = random(0xFFFF);
      Serial.printf(S_RESET_USER, dat);
      Serial.flush(); // a reset is not a restart!
      userReset(dat);
      break;  

    case 'H':
      dat = random(0xFFFF);
      Serial.printf(S_RESTART_USER, dat);
      userRestart(dat);
      break;  
      
    case 'I':
      Serial.println(S_RST_LWD);
      badModule();
      break;  

    case 'J':
      Serial.println(S_RST_LWD_LOOP);
      skipLastStamp = true;
      break;  

    case 'K':
      Serial.println(S_RST_LWD_OVERWRITE);
      mangleModule();
      break;  
  } 
   
  if (!skipLastStamp) {
    lwdtStamp();
  }  
}
