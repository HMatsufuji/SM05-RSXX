/**
 * @file onewire_2buses.ino
 * @author Hideshi Matsufuji (h-matsufuji@hi-corp.jp)
 * @brief 
 * @version 0.1
 * @date 2022-02-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <Arduino.h>

// DS18B20
#include <OneWire.h>
#include <DallasTemperature.h>


/////////////
// DS18B20 //
/////////////

#define ONE_WIRE_BUS_1 12
#define ONE_WIRE_BUS_2 13

struct st_ds18b20 {
    float temp1;
    float temp2;
} ds18b20;

struct st_values {
    float temp1;
    float temp2;
};

OneWire oneWire_1(ONE_WIRE_BUS_1);
OneWire oneWire_2(ONE_WIRE_BUS_2);

DallasTemperature sensor_1(&oneWire_1);
DallasTemperature sensor_2(&oneWire_2);


void ds18b20_init(void)
{
    sensor_1.begin();
    sensor_2.begin();
}

void ds18b20_get(struct st_ds18b20 *a)
{
    sensor_1.requestTemperatures();
    sensor_2.requestTemperatures();
    a->temp1 = sensor_1.getTempCByIndex(0);
    a->temp2 = sensor_2.getTempCByIndex(0);
}

// Measurement
void measure(struct st_values *a) {

    // DS18B20
    ds18b20_init();
    ds18b20_get(&ds18b20);

    a->temp1 = ds18b20.temp1;
    a->temp2 = ds18b20.temp2;
}

///////////////
/// main ////
//////////////


void setup() {
    int error;

    // Set console baud rate
    Serial.begin(115200);
    while (! Serial) delay(50);
        

}

void loop() {
    struct st_values x;
    measure(&x);
    Serial.print("TEMP1: ");
    Serial.print(x.temp1);
    Serial.print(" TEMP2: ");
    Serial.println(x.temp2);
    delay(1000);
}