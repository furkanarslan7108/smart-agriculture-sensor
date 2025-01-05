#ifndef DHT22_H
#define DHT22_H

#include <Arduino.h>

class DHT22 {
public:
    DHT22(uint8_t pin);
    void begin();
    float readTemperature();
    float readHumidity();

private:
    uint8_t _pin;
    uint8_t data[5];
    bool read();

    uint32_t expectPulse(bool level);
};

#endif // DHT22_H