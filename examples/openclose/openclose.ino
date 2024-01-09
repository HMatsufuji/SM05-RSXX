/**
 * @file openclose.ino
 * @author Hideshi Matsufuji (h-matsufuji@hi-corp.jp)
 * @brief 
 * @version 0.1
 * @date 2022-02-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <Arduino.h>

// main loop
RTC_DATA_ATTR struct st_main {
    uint8_t st;
    uint32_t bootCount;
} main0;


///////////////////////
// open/close sensor //
///////////////////////

#define OPENCLOSE_IO GPIO_NUM_13

RTC_DATA_ATTR struct st_openclose {
    bool st; 
} openclose;

/**
 * @brief Initialize open-close sensor
 */
void openclose_init() {
    pinMode(OPENCLOSE_IO, INPUT);
    openclose.st = 0;
}

/**
 * @brief Interrupt hundler
 */
void IRAM_ATTR opencloseEventHundler() {
    if (openclose.st == 0 && digitalRead(OPENCLOSE_IO) == 0) {
        Serial.println("--- Now CLOSE");
        openclose.st = 1;
        openclose_start();
    } else if (openclose.st == 1 && digitalRead(OPENCLOSE_IO) == 1) {
        Serial.println("--- Now OPEN");
        openclose.st = 0;
        openclose_start();
    } else {
        Serial.println("--- Skip Event");
    }
}

/**
 * @brief open-close sensor interrupt settings
 */
void openclose_start(void) {
    detachInterrupt(OPENCLOSE_IO);
    if (openclose.st == 1) {
        attachInterrupt(OPENCLOSE_IO, opencloseEventHundler, RISING);
    } else {
        attachInterrupt(OPENCLOSE_IO, opencloseEventHundler, FALLING);
    }
}

/**
 * @brief detach interrupt
 */
void openclose_stop() {    
    detachInterrupt(OPENCLOSE_IO);
}


//////////////////
////// main //////
//////////////////


// First event
//
void first_event() {
    main0.bootCount = 0;  
    
    Serial.println("-------- First Boot --------");
    
    // open close initialize
    openclose_init();

    // wakeup settings
    esp_sleep_enable_ext0_wakeup(OPENCLOSE_IO, LOW);
}

// change switch state
//
void openclose_event() {
    openclose_stop();

    if (openclose.st == 1 && digitalRead(OPENCLOSE_IO) == 1) {
        openclose.st = 0;
        Serial.println("+++ Now OPEN");
    } else if (openclose.st == 0 && digitalRead(OPENCLOSE_IO) == 0) {
        openclose.st = 1;
        Serial.println("+++ Now CLOSE");
    } else {
        Serial.println("+++ Skip Event");
    }
    openclose_start();

    if (openclose.st == 0) {
        esp_sleep_enable_ext0_wakeup(OPENCLOSE_IO, LOW);
    } else {
        esp_sleep_enable_ext0_wakeup(OPENCLOSE_IO, HIGH);
    }
}

///////////////////////////////////
/////////////// setup /////////////
///////////////////////////////////
 
void setup() {
    detachInterrupt(OPENCLOSE_IO);

    Serial.begin(115200);
    while (!Serial) delay(100);

    main0.bootCount++;
    Serial.println("Boot Count: " + String(main0.bootCount));

    // get wakeup reason
    esp_sleep_wakeup_cause_t wakeup_reason;
    wakeup_reason = esp_sleep_get_wakeup_cause();

    switch(wakeup_reason){
        case ESP_SLEEP_WAKEUP_EXT0      : Serial.printf("External interrupt (RTC_IO)\n");                                                openclose_event(); break;
        case ESP_SLEEP_WAKEUP_EXT1      : Serial.printf("External interrupt (RTC_CNTL) IO=%llX\n", esp_sleep_get_ext1_wakeup_status());  break;
        case ESP_SLEEP_WAKEUP_TIMER     : Serial.printf("Timer interrupt\n");  break;
        case ESP_SLEEP_WAKEUP_TOUCHPAD  : Serial.printf("Touch interrupu PAD=%d\n", esp_sleep_get_touchpad_wakeup_status());       break;
        case ESP_SLEEP_WAKEUP_ULP       : Serial.printf("ULP booting\n");                                                       break;
        case ESP_SLEEP_WAKEUP_GPIO      : Serial.printf("GPIO interrupt from light sleep\n");                                       break;
        case ESP_SLEEP_WAKEUP_UART      : Serial.printf("UART interrupt from light sleep\n");                                       break;
        default                         : Serial.printf("Booting from other than sleep\n");                                                    first_event(); break;
    }

    // Flush serial
    Serial.print("Bye!\n\n");
    Serial.flush();

    // Into the deep sleep
    esp_deep_sleep_start();
}

//////////////////////////////////////
/////////////// loop /////////////////
//////////////////////////////////////

void loop() {}
