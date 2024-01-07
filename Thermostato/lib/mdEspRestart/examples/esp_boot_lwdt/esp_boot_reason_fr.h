#ifndef _ESP_BOOT_REASON_H_
#define _ESP_BOOT_REASON_H_

#define S_APP_NAME            "\n\nesp_boot_lwdt"
#define S_GPIO_INITIALIZED    "Broche GPIO de la DEL initialisée"

#define S_REASON_FOR_RESTART "\nCause ESP du démarrage: (%d) %s\n\n"

#define S_BAD_RTC             "L'adresse RTC ne peut être changée, valeur donnée probablement incorrecte"

#define S_REASON_CHANGED      "La raison du démarrage %d était %d à l'appel précédent\n"
#define S_COUNT_CHANGED       "Le compte consécutif %d était %d à l'appel précédent\n"
#define S_DATA_CHANGED        "La valeur de data %d était %d à l'appel précédent\n" 

#define S_POWER_ON            "mis sous tension"
#define S_HARDWARE_WD         "déclenchement du chien de garde matériel"
#define S_EXCEPTION_OCCURED   "occurence de l'exception %d"
#define S_SOFTWARE_WD         "déclenchement du chien de garde logiciel"
#define S_RESTART_OR_RESET    "fonction restart() ou reset() invoquée"
#define S_ESP_WOKE            "réveil du ESP après un profond sommeil"
#define S_EXTERNAL_RESET      "réinitialisation externe du système"
#define S_USER_RESET          "démarrage (reset) déclanché par l'utilisateur, data: %d"
#define S_USER_RESTART        "démarrage (restart) déclanché par l'utilisateur, data: %d"
#define S_LWD_RESET           "déclenchement du chien de garde loop"
#define S_LWD_LOOP_RESET      "déclenchement du chien de garde loop sans finir loop()"
#define S_LWD_OVERWRITE_RESET "déclenchement du chien de garde loop après écrasement de ses variables"
#define S_UNKNOWN_REASON      "raison inconnue"
#define S_NEED_DEBUG_LWD_PART "Activez #define DEBUG_LWD_PART dans mdEspRestart.h pour faire ce test"
#define S_MANGLED_FLAG        "Redémarrage en écrasant le marqueur de l'enregistrement restart"
#define S_MANGLED_VARS        "Redémarrage en écrasant les variables du chien de garde loop"
#define S_RESTART_REASON      "\n\n>>> Cause du démarrage: "
#define S_COUNT               ". Compte: %d. <<<\n"

#define S_VISITED_MODULE      ", dernier module visité (%d) - "
#define S_LOOP_END            "fin de la fonction loop()"
#define S_UNKNOWN_MODULE      "inconnu"

#define S_RST_RESTART         "Redémarrage avec ESP.restart()"
#define S_RST_RESET           "Redémarrage avec ESP.resest()"
#define S_RST_HWDT            "Redémarrage avec déclenchement du chien de garde matériel (attendre 6 secondes)"
#define S_RST_SWDT            "Redémarrage avec déclenchement du chien de garde logiciel (attendre 3 seconds)"
#define S_RST_EXCP_0          "Redémarrage en vertu de l'exception 0 (division par 0)"
#define S_RST_EXCP_3          "Redémarrage en vertu de l'exception 3 (LoadStoreErrorCause)"
#define S_RST_LWD             "Déclenchement du chien de garde loop (attendre 12 secondes)"
#define S_RST_LWD_LOOP        "Déclenchement du chien de garde loop sans finir la fonction loop()"
#define S_RST_LWD_OVERWRITE   "Déclenchement du chien de garde loop par écrasement de ses variables (attendre 12 secondes)"
#define S_RESET_USER          "Redémarrage utilisateur avec reset et data = %d\n"
#define S_RESTART_USER        "Redémarrage utilisateur avec restart et data = %d\n"

#define USER_INPUT_MESSAGE_COUNT 13
static const char* userInputMessage[USER_INPUT_MESSAGE_COUNT] = {
  "Pour choisir comment redémarrer le ESP, appuyer sur",
  "  A - avec ESP.restart()",
  "  B - avec ESP.reset()",
  "  C - avec déclenchement du chien de garde matériel",
  "  D - avec déclenchement du chien de garde logiciel",
  "  E - avec l'exception 0 - division avec 0",
  "  F - avec l'exception 3 - LoadStoreErrorCause",
  "  G - avec redémarrage (reset) utilisateur",
  "  H - avec redémarrage (restart) utilisateur",
  "  I - avec déclenchement du chien de garde loop",
  "  J - avec déclenchement du chien de garde loop en fin de la fonction",
  "  K - avec déclenchement du chien de garde loop par écrasement de ses variables",
  " ou - appuyer le bouton de réinitialisation."};

#define S_UART_BOOT "\n*** Appuyer le bouton de réinitialisation  ***\n*** car le croquis vient d'être téléversé. ***"

#endif
