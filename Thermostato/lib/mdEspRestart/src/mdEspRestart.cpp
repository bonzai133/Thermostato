#include <Ticker.h>
#include "mdEspRestart.h"

extern "C" {
#include "user_interface.h"
}

/*
 * Returns true if the code being executed has
 * just been downloaded over the UART
 */
bool isUartBoot(void) {
  int bootmode = 0;
  asm (
    "movi %0, 0x60000200\n\t"
    "l32i %0, %0, 0x118\n\t"
    : "+r" (bootmode) /* Output */
    : /* Inputs (none) */                
    : "memory" /* Clobbered */           
  );
  return (((bootmode >> 0x10) & 0x7) == 1); 
}


/*
 * Structure in which restart information will be saved
 * using RTC memory. The latter is divided into 4 byte
 * blocks, the first 64 blocks (256 bytes) are used by the 
 * system, the last 128 (512 bytes) are available to the
 * user. 
 * 
 * Two attributes are defined to control the allocation of
 * memory and alignment of the global structure.
 * 
 * First the members of the structure are ordered to ensure
 * that it occupies exactly two blocks. The packed attribute 
 * is not needed, but put reason after count and there will 
 * padding will be added to align data on a 4 byte boundary.
 * 
 * The second attribute aligns the whole structure on a 
 * 4 byte boundary because  system_rtc_mem_read and 
 * system_rtc_write will not work otherwise. Actually, the
 * aligned attribute is not needed here because the 
 * structure contains a 32 bit member and hence will be 
 * aligned on a 32 bit boundary.
 * 
 * GCC online documentation on attributes:
 * 
 * Specifying Attributes of Variables  
 * https://gcc.gnu.org/onlinedocs/gcc-3.3/gcc/Variable-Attributes.html#Variable%20Attributes
 * 
 * Espressif, ESP8266 Non-OS SDK API Reference, Version 2.2.1 (2018/05)
 * "3.3.23 system_rtc_mem_write" p. 19
 * https://www.espressif.com/sites/default/files/documentation/2c-esp8266_non_os_sdk_api_reference_en.pdf#page=1&zoom=auto,-13,792
 *
 */

struct __attribute__((packed)) {
  uint8_t           flag;   // 1 byte restart structure flag 
  restartReason_t reason;   // 1 byte restart reason
  restartCount_t   count;   // 2 byte (16 bit) count of consecutive boots for same reason
  restartData_t     data;   // 4 byte (32 bit) data (module id or exception cause or user defined)
} restart __attribute__((aligned(4)));

#define RESTART_MARKER    0xA5  // 1010 0101
#define LWD_USR_MARKER    0xA1  // 1010 0001
#define MARKER_MASK       0xFB  // 1111 1011

/*
 * By default the restart structure will be stored at the
 * very end of the RTC memory
 * 
 * The 127 blocks of user RTC memory follow 64 4-byte blocks 
 * (256 bytes) reserved by the system
 */
#define RESTART_BLOCK_SIZE ((sizeof(restart) + 3) & (~3))
#define LAST_VALID_BLOCK_ADDRESS 64 + 128 - (RESTART_BLOCK_SIZE / 4)

uint8 rtcAddress = LAST_VALID_BLOCK_ADDRESS;

bool setRestartRtcAddress(uint8_t addr) {
  bool result = true;
  if (addr == DEFAULT_RESTART_ADDRESS) {
    rtcAddress = LAST_VALID_BLOCK_ADDRESS;
  } else { 
    addr = addr + 64;
    result = (addr <= LAST_VALID_BLOCK_ADDRESS);
    if (result) rtcAddress = addr;
  } 
  return result;
}   

void initRestart(void) {
  restart.flag = RESTART_MARKER;
  restart.reason = 0;
  restart.count = 0;
  restart.data = LOOP_END;
}


void loadRestart(void) {
  system_rtc_mem_read(rtcAddress, &restart, sizeof(restart));
  if ( ( (restart.flag & MARKER_MASK) != LWD_USR_MARKER) || ( restart.reason > LAST_VALID_REASON) ) {
    initRestart();
  }
}    

void saveRestart(void)  {
  system_rtc_mem_write(rtcAddress, &restart, sizeof(restart));
}

void setLwdtRestartReason(restartReason_t reason) {
  if (restart.reason != reason) {
    restart.count = 0;  // it will be incremented by getRestartReason
  }  
 restart.reason = reason;
 restart.flag = LWD_USR_MARKER;  
 saveRestart();
}


/*
 * Flag to avoid reading restart struct from memory more than once,
 * in other words, getRestartReason can be called more than once, on  
 * second or later calls, the result will be the same as the first
 * call.
 */
boolean restartUpdated = false;   // 

restartReason_t getRestartReason(restartCount_t &count, restartData_t &data) {
  if (!restartUpdated) {
    
    loadRestart();
    
    restartReason_t reason = ESP.getResetInfoPtr()->reason;    
    restartReason_t restartreason = restart.reason;
  
    if ( (restart.flag == LWD_USR_MARKER) && (reason == REASON_SOFT_RESTART) )  {
        reason = restartreason;
    }  
  
    boolean isSameReason = (reason == restartreason);
    if (reason == REASON_EXCEPTION_RST) {
      restartData_t exceptionCause = ESP.getResetInfoPtr()->exccause;
      isSameReason = isSameReason && (exceptionCause == restart.data);
      restart.data = exceptionCause;
    }
    
    if (isSameReason) {
      restart.count++;
    } else {
      restart.count = 1;
    }    
    restart.flag = RESTART_MARKER;
    restart.reason = reason;
    saveRestart();
    restartUpdated = true;  
  }
  data = restart.data;
  count = restart.count;
  return restart.reason;
}


void userRestart(restartData_t data) {
  restart.data = data;
  setLwdtRestartReason(REASON_USER_RESTART);
  ESP.restart();
}


void userReset(restartData_t data) {
  restart.data = data;
  setLwdtRestartReason(REASON_USER_RESET);
  ESP.reset();
}

#ifdef INCLUDE_GETRESETREASONEX
String getResetReasonEx(void) {
  char buff[80];
  restartCount_t count;
  restartData_t data;
  restartReason_t restartReason = getRestartReason(count, data);
  if (restartReason == REASON_EXCEPTION_RST) {
    snprintf(buff, sizeof(buff), "%s %d, count %d", ESP.getResetReason().c_str(), data, count);
  } else if (restartReason <= REASON_EXT_SYS_RST) {
    snprintf(buff, sizeof(buff), "%s, count %d", ESP.getResetReason().c_str(), count);
  } else if (restartReason == REASON_USER_RESET) {
    snprintf_P(buff, sizeof(buff), PSTR("User reset, data %d, count %d"), data, count);
  } else if (restartReason == REASON_USER_RESTART) {
    snprintf_P(buff, sizeof(buff), PSTR("User restart, data %d, count %d"), data, count);
  } else if (restartReason == REASON_LWD_RST) {
    snprintf_P(buff, sizeof(buff), PSTR("Loop watchdog in %d, count %d"), data, count);
  } else if (restartReason == REASON_LWD_LOOP_RST) {
    snprintf_P(buff, sizeof(buff), PSTR("Loop watchdog incomplete in %d, count %d"), data, count);
  } else if (restartReason == REASON_LWD_OVW_RST) {
    snprintf_P(buff, sizeof(buff), PSTR("Loop watchdog overwritten in %d, count %d"), data, count);
  } else {
    return ESP.getResetReason();  
  }
  return String(buff);  
}
#endif

Ticker lwdTicker;
volatile unsigned long lwdTime;
volatile unsigned long lwdTimeout;
volatile unsigned long lwdDifference;
volatile unsigned long lwdNotDifference;

/*
 * N O T E
 * 
 * clearRestart() should be invoked when user chooses to restart the
 * system manually ????
 * 
void clearRestart(void) {
 initRestart();
 saveRestart();
}
 */
 
void lwdtStamp(restartData_t data) {
  restart.data = (restartData_t) data;
}
  
void lwdtFeed(void) {
  lwdTime = millis();
  lwdTimeout = lwdTime + lwdDifference;
  if (restart.data != LOOP_END) {
    setLwdtRestartReason(REASON_LWD_LOOP_RST);
    ESP.restart(); 
  }
}

void ICACHE_RAM_ATTR lwdtcb(void) {
  if ( ((lwdTimeout - lwdTime) != lwdDifference) || (lwdDifference != - lwdNotDifference) )
    setLwdtRestartReason(REASON_LWD_OVW_RST);
  else if ( (restart.flag & MARKER_MASK) != LWD_USR_MARKER) 
    setLwdtRestartReason(REASON_LWD_OVW_RST);           // where variable mangled    
  else if (millis() - lwdTime < LWD_TIMEOUT)
    return;
  else
    setLwdtRestartReason(REASON_LWD_RST);  
  ESP.restart();   // lwd timed out or lwd overwritten
}

void lwdtInit(unsigned long timeout) {
  lwdNotDifference = -timeout;
  lwdDifference = timeout;
  lwdTicker.attach_ms(timeout / 2, lwdtcb); 
  lwdtStamp(LOOP_END); //= start of loop()
  lwdtFeed();
}

#ifdef DEBUG_RTC_PART
void dumpInfo(void) {
  Serial.println("mdEspRestart info");
  Serial.printf("  sizeof(restart): %d\n", sizeof(restart));
  Serial.printf("  RESTART_BLOCK_SIZE: %d\n", RESTART_BLOCK_SIZE);
  Serial.printf("  sizeof(restart) is a multiple of 4 bytes: %s\n", (sizeof(restart)==RESTART_BLOCK_SIZE) ? "true" : "*** FALSE ***");
  Serial.printf("  rtcAddress: %d\n",  rtcAddress);
  Serial.printf("  (rtcAddress-64)*4 + sizeof(restart): %d\n", (rtcAddress-64)*4 + sizeof(restart));
  Serial.printf("  (rtcAddress-64)*4 + RESTART_BLOCK_SIZE: %d\n", (rtcAddress-64)*4 + RESTART_BLOCK_SIZE);

  Serial.printf("  memory location of restart: %p\n", &restart);
  Serial.printf("  restart is 32 bit aligned: %s\n", (((uint32_t) &restart & (uint32_t) 0b11) == 0) ? "true" : "*** FALSE ***");
  boolean ok = system_rtc_mem_read(rtcAddress, &restart, sizeof(restart));
  Serial.printf("  can do system_rtc_mem_read %s\n", (ok) ? "yes" : "**** NO ***");   
  Serial.println();
}

void dumpRestart(char * msg){
  Serial.printf("%s:: flag: 0x%0x, reason: 0x%0x, count: %d, data: 0x%0x\n", msg, restart.flag, restart.reason, restart.count, restart.data);
}
#endif

#ifdef DEBUG_LWD_PART
void mangleLwdt(void) {
  lwdTime = 0;
  lwdTimeout = 0;
  lwdDifference = 0;
}
void mangleLwdtFlag(void) {
  restart.flag = 0xF0;
}
#endif
