#include "SoilSensor.h"

DHT22::DHT22(uint8_t pin) : _pin(pin) {
    // Constructor
}

void DHT22::begin() {
    pinMode(_pin, INPUT_PULLUP);
}

/**
 * @brief Reads the temperature from the DHT22 sensor.
 * 
 * This function reads the temperature data from the DHT22 sensor. If the read operation
 * is successful, it processes the raw data to compute the temperature in degrees Celsius.
 * If the read operation fails, it returns NaN.
 * 
 * @return float The temperature in degrees Celsius, or NaN if the read operation fails.
 */
float DHT22::readTemperature() {
    if (read()) {
        int16_t t = data[2] & 0x7F;
        t <<= 8;
        t |= data[3];
        if (data[2] & 0x80) {
            t = -t;
        }
        return t * 0.1;
    }
    return NAN;
}

/**
 * @brief Reads the humidity value from the DHT22 sensor.
 *
 * This function reads the humidity data from the DHT22 sensor. If the read operation
 * is successful, it processes the raw data and returns the humidity as a float value
 * in percentage. If the read operation fails, it returns NaN.
 *
 * @return float The humidity value in percentage if the read operation is successful,
 *               otherwise NaN.
 */
float DHT22::readHumidity() {
    if (read()) {
        int16_t h = data[0];
        h <<= 8;
        h |= data[1];
        return h * 0.1;
    }
    return NAN;
}

/**
 * @brief Reads data from the DHT22 sensor.
 *
 * This function reads the data from the DHT22 sensor and stores it in the data array.
 * It requests a sample from the sensor, waits for the response, and then reads the
 * 40-bit data stream. The function also checks the checksum to ensure data integrity.
 *
 * @return true if the read was successful and the checksum is correct, false otherwise.
 */
bool DHT22::read() {

    // Request sample
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
    delay(20);
    digitalWrite(_pin, HIGH);
    delayMicroseconds(40);
    pinMode(_pin, INPUT_PULLUP);

    // Read the response
    if (expectPulse(LOW) == 0) return false;
    if (expectPulse(HIGH) == 0) return false;

    // Read the data
    for (int i = 0; i < 40; ++i) {
        if (expectPulse(LOW) == 0) return false;
        uint32_t pulseLength = expectPulse(HIGH);
        if (pulseLength == 0) return false;
        data[i / 8] <<= 1;
        if (pulseLength > 40) {
            data[i / 8] |= 1;
        }
    }

    // Check the checksum
    if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
        return true;
    }
    return false;
}

/**
 * @brief Waits for a pulse on the specified pin and counts the duration of the pulse.
 *
 * This function waits for a pulse on the pin associated with the DHT22 sensor. It counts the duration
 * of the pulse in a loop until the pulse level changes or a timeout occurs.
 *
 * @param level The pulse level to wait for (HIGH or LOW).
 * @return The duration of the pulse in loop counts, or 0 if the pulse duration exceeds the timeout.
 */
uint32_t DHT22::expectPulse(bool level) {
    uint32_t count = 0;
    while (digitalRead(_pin) == level) {
        if (count++ >= 1000) {
            return 0;
        }
    }
    return count;
}