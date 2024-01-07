#ifndef _ESP_BOOT_REASON_H_
#define _ESP_BOOT_REASON_H_

#define S_APP_NAME            "\n\nesp_boot_lwdt"
#define S_GPIO_INITIALIZED    "GPIO LED pin intialized"

#define S_REASON_FOR_RESTART "\nESP reason for restart: (%d) %s\n\n"

#define S_BAD_RTC             "Given RTC address probably invalid, could not set it"

#define S_REASON_CHANGED      "The restart reason %d was %d on the previous call\n"
#define S_COUNT_CHANGED       "The consecutive count %d was %d on the previous call\n"
#define S_DATA_CHANGED        "The value of data %d was %d on the previous call\n" 

#define S_POWER_ON            "power on"
#define S_HARDWARE_WD         "hardware watchdog timed out"
#define S_EXCEPTION_OCCURED   "software exception %d occured"
#define S_SOFTWARE_WD         "software watchdog timed out"
#define S_RESTART_OR_RESET    "software restart() or reset() invoked"
#define S_ESP_WOKE            "ESP woke from deep sleep"
#define S_EXTERNAL_RESET      "external system reset"
#define S_USER_RESET          "user defined reset, data: %d" 
#define S_USER_RESTART        "user defined restart, data: %d" 
#define S_LWD_RESET           "loop watchdog timed out"
#define S_LWD_LOOP_RESET      "loop watchdog loop() not completed"
#define S_LWD_OVERWRITE_RESET "loop watchdog overwritten"
#define S_NEED_DEBUG_LWD_PART "Need to uncomment #define DEBUG_LWD_PART in mdEspRestart.h to run this test"
#define S_MANGLED_FLAG        "Mangled restart flag to cause restart"
#define S_MANGLED_VARS        "Mangled lwdt variables to cause restart"
#define S_UNKNOWN_REASON      "for an unknown reason"
#define S_RESTART_REASON      "\n\n>>> Restart reason: "
#define S_COUNT               ". Count: %d. <<<\n"

#define S_VISITED_MODULE      ", last visited module (%d) - "
#define S_LOOP_END            "end of the loop() function"
#define S_UNKNOWN_MODULE      "unknown"

#define S_RST_RESTART         "Rebooting with ESP.restart()"
#define S_RST_RESET           "Rebooting with ESP.reset()"
#define S_RST_HWDT            "Rebooting with hardware watchdog timeout reset (wait about 6 seconds)"
#define S_RST_SWDT            "Rebooting with sofware watchdog timeout reset (wait about 3 seconds)"
#define S_RST_EXCP_0          "Rebooting because of exception 0 (divide by 0)"
#define S_RST_EXCP_3          "Rebooting because of exception 3 (LoadStoreErrorCause)"
#define S_RESET_USER          "User defined rebooting with reset and data = %d\n"
#define S_RESTART_USER        "User defined rebooting with restart and data = %d\n"
#define S_RST_LWD             "Rebooting with loop watchdog timeout reset (wait about 12 seconds)"
#define S_RST_LWD_LOOP        "Rebooting with loop watchdog timeout reset outside the loop() function"
#define S_RST_LWD_OVERWRITE   "Rebooting with loop watchdog overwritting its variables (wait up to 12 seconds)"

#define USER_INPUT_MESSAGE_COUNT 13
static const char* userInputMessage[USER_INPUT_MESSAGE_COUNT] = {
  "To select how to reset the ESP, press",
  "  A - for ESP.restart()",
  "  B - for ESP.reset()",
  "  C - for hardware watchdog timeout",
  "  D - for software watchdog timeout",
  "  E - for divide by 0 exception (0)",
  "  F - for LoadStoreErrorCause exception (3)",
  "  G - for user defined reset",
  "  H - for user defined restart",
  "  I - for loop watchdog timeout",
  "  J - for loop watchdog timeout out of the loop() function",
  "  K - for loop watchdog with overwritten variables",
  " or - press the reset button."};

#define S_UART_BOOT "\n*** Press on the restart button because ***\n*** the sketch has just been uploaded. ***"

#endif
