
#ifndef _MDESPRESTART_H_
#define _MDESPRESTART_H_

/*
 * Do not define the first two DEBUG_ directives
 * in the library.
 * 
 * Optionnally INCLUDE_GETRESETREASONEX can be
 * defined in the library if there is a use for
 * 
 *   String getResetReasonEx(void)  
 */
//#define DEBUG_RTC_PART
//#define DEBUG_LWD_PART
//#define INCLUDE_GETRESETREASONEX

#include "Arduino.h"


/* 
 * Loop watchdog version 0.3.2
 *  
 * The first instruction in loop() must reset the loop watch
 * dog:
 * 
 *   lwdtFeed();
 * 
 * The last instruction in loop() must be:
 * 
 *   lwdtStamp();  or lwdtStamp(LOOP_END);
 *   
 * Each module called on between those two instruction, should also
 * begin by stamping the watchdog with its identifier as:
 * 
 * void testModule(void) {
 *   lwdStamp(TEST_MODULE);
 *   
 * Add module identifiers for each module in the sketch: 
 * 
 * #define testModule 0
 * #define ledModule  1
 * ...
 * User defined module identifiers are 32 bit values from 0 to 
 * 4,294,967,294 = 0xffffffff-1. The biggest unsigned 32 bit 
 * value, 0xffffffff = LOOP_END is reserved to mark the end of 
 * the loop() function
 */

/*
 * Type definitions of restart information.
 * 
 */
typedef uint8_t  restartReason_t;
typedef uint16_t  restartCount_t;
typedef uint32_t   restartData_t;
 
#define LOOP_END 0xFFFFFFFF

void lwdtStamp(restartData_t data = LOOP_END); 

/*
 * Resets the loop watch dog timer. This should be called as the 
 * first instruction in the loop() functino and not elsewhere.
 */
void lwdtFeed(void);

/* 
 *  Initializes the loop watch dog timer, setting the watch dog
 *  time out interval. By default this is LWD_TIMEOUT. 
 *  
 *  This function must be called once in the sketch setup() function. 
 */
#define LWD_TIMEOUT 12000  // 12 second lwd timeout = 2 * hardware timeout

void lwdtInit(unsigned long timeout = LWD_TIMEOUT);


/*
 * ESP.getResetInfoPtr()->reason returs a value 0 to 6 as
 * as  defined in "user_interface.h"
 *
 * 0 = REASON_DEFAULT_RST,      normal startup by power on 
 * 1 = REASON_WDT_RST,          hardware watch dog reset 
 * 2 = REASON_EXCEPTION_RST,    exception reset
 * 3 = REASON_SOFT_WDT_RST,     software watch dog reset
 * 4 = REASON_SOFT_RESTART,     software restart ,system_restart
 * 5 = REASON_DEEP_SLEEP_AWAKE, wake up from deep-sleep
 * 6 = REASON_EXT_SYS_RST,      external system reset 
 * 
 * Here 5 more restart reasons are added:
 *     
 *   REASON_USER_RESET         
 *     user initiated reset, see userReset()
 *     
 *   REASON_USER_RESTART         
 *     user initiated restart, see userRestart()
 *     
 *   REASON_LWD_RST
 *     the loop watchdog has timed out within the loop() function
 *     
 *   REASON_LWD_LOOP_RST
 *     the loop watchdog has timed out somewhere between the end of the 
 *     loop() function and its start
 *     
 *   REASON_LWD_OVW_RST   
 *     the loop watchdog variables have been mangled (overwritten)
 */
#define REASON_USER_RESET     7
#define REASON_USER_RESTART   8
#define REASON_LWD_RST        9
#define REASON_LWD_LOOP_RST  10
#define REASON_LWD_OVW_RST   11
#define LAST_VALID_REASON    11

/*
 * Returns the reason for the restart and the number of consecutive times
 * the ESP has been restarted for that reason.
 * 
 * The data value is only significant for loop watchdog events (it
 * returns the last module entered with lwdtStamp) and execptions
 * (it returns the exception value exccause).
 * 
 * Can be called many times without problem. If setResartRtcAddress() was not 
 * explicitly used to set the RTC memory address of the restart structure, 
 * the DEFAULT_ADDRESS will be used.
 */

restartReason_t getRestartReason(restartCount_t &count, restartData_t &data);

/*
 * Reboots the ESP8266 with ESP.resest() saving the content of data to RTC 
 * memory before. After the reboot, the function getRestart(count, data) 
 * will return REASON_USER_RESET with data equal to the value saved to
 * RTC memory before the reboot. The count value will reflect the number of 
 * consecutive restart because of calls to userReset() no matter if data is 
 * the same or changed.
 * 
 */
void userReset(restartData_t data = 0);

/*
 * Reboots the ESP8266 with ESP.restart() saving the content of data to RTC 
 * memory before. After the reboot, the function getRestart(count, data) 
 * will return REASON_USER_RESTART with data equal to the value saved to
 * RTC memory before the reboot. The count value will reflect the number of 
 * consecutive restart because of calls to userRestart() no matter if data is 
 * the same or changed.
 * 
 */
void userRestart(restartData_t data = 0);


/*
 * Extended version of ESP.getResetReason() which also reports the five
 * additional restart reasons.
 */
#ifdef INCLUDE_GETRESETREASONEX
String getResetReasonEx(void);
#endif

/*
 * This routine can be called before using any of the previous
 * functions to set the address where the restart structure will
 * be stored in RTC memory. 
 * 
 * User available RCT memory is divided into 128 4-byte blocks
 * (which amount to 512 bytes of RAM memory), numbered 0 to 127.
 * Hence, valid block address are between 0 and 126 (128 less 
 * the block size of the restart structure saved to the RTC 
 * memory, which is currently 2). By default the block address
 * of the restart structure is at the very end of the RTC
 * memory whatever size the structure is.
 * 
 * Returns false if addr is outside the valid range.
 */
#define DEFAULT_RESTART_ADDRESS 0xFF

bool setRestartRtcAddress(uint8_t addr = DEFAULT_RESTART_ADDRESS);

/*
 * Returns true if the code being executed has just been downloaded
 * over the UART and the ESP cannot be restarted except manually 
 * with the RST pin or by being powered off and on.
 */
bool isUartBoot(void);


//-------------------------------------------------

#ifdef DEBUG_LWD_PART  
void mangleLwdt(void);
void mangleLwdtFlag(void);
#endif

#ifdef DEBUG_RTC_PART 
void dumpInfo(void);
void dumpRestart(char * msg);
#endif

#endif
