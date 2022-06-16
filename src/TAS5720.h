/**
 * @file TAS5720.h
 * @brief Header of TAS5720.cpp
 * @author Axel Chabot
 * @date: 2022-06-16
 * @version 1.0
 */
#ifndef __TAS5720_H__
#define __TAS5720_H__

#include <Arduino.h>
#include <Wire.h>

#define TAS5720_I2C_SPEED 100000

#define TAS5720_ADDRESS_DEVICE_IDENTIFICATION               0x00
#define TAS5720_ADDRESS_POWER_CONTROL                       0x01
#define TAS5720_ADDRESS_DIGITAL_CONTROL                     0x02
#define TAS5720_ADDRESS_VOLUME_CONTROL_CONFIGURATION        0x03
#define TAS5720_ADDRESS_VOLUME_CONTROL_LEFT                 0x04
#define TAS5720_ADDRESS_VOLUME_CONTROL_RIGHT                0x05
#define TAS5720_ADDRESS_ANALOG_CONTROL                      0x06
#define TAS5720_ADDRESS_FAULT_CONFIGURATION_ERROR_STATUS    0x08
#define TAS5720_ADDRESS_DIGITAL_CLIPPER_2                   0x10
#define TAS5720_ADDRESS_DIGITAL_CLIPPER_1                   0x11

typedef enum : uint8_t {
    SAI_24_BITS_RIGHT_JUSTIFIED = 0,
    SAI_20_BITS_LEFT_JUSTIFIED = 1,
    SAI_18_BITS_RIGHT_JUSTIFIED = 2,
    SAI_16_BITS_LEFT_JUSTIFIED = 3,
    SAI_I2S = 4,
    SAI_16_24_BITS_LEFT_JUSTIFIED = 5,
} SAIFormat;

typedef enum : uint8_t {
    DIGITAL_BOOST_0DB = 0,
    DIGITAL_BOOST_6DB = 1,
    DIGITAL_BOOST_12DB = 2,
    DIGITAL_BOOST_18DB = 3
} DigitalBoost;

typedef enum : uint8_t {
    RIGHT = 0,
    LEFT = 1
} ChannelSelection;

typedef enum : uint8_t {
    GAIN_19_2_DBV = 0,
    GAIN_22_6_DBV = 1,
    GAIN_25_DBV = 2
} AnalogGain;

typedef enum : uint8_t {
    RATE_6_LRCK = 0,
    RATE_8_LRCK = 1,
    RATE_10_LRCK = 2,
    RATE_12_LRCK = 3,
    RATE_14_LRCK = 4,
    RATE_16_LRCK = 5,
    RATE_20_LRCK = 6,
    RATE_24_LRCK = 7
} PWMRate;

typedef enum : uint8_t {
    OCE_DEFAULT = 0,
    OCE_REDUCE_75_PERCENT = 1,
    OCE_REDUCE_50_PERCENT = 2,
    OCE_REDUCE_25_PERCENT = 3
} OCEThreshold;

/**
 * @brief Class for TAS5720
 */
class TAS5720 {
private:
    TwoWire *_i2c;
    uint8_t _addr;
    bool _debug;
    uint8_t _rxData;

    void _write(uint8_t reg, uint8_t data);

    uint8_t _read(uint8_t reg);

public:
    uint8_t volumeLeft, volumeRight;

    TAS5720(TwoWire &i2c, uint8_t addr, bool debug = false);

    ~TAS5720();

    uint8_t getDeviceIdentification();

    bool getSleepMode();

    void setSleepMode(bool sleep);

    bool getShutdown();

    void setShutdown(bool shutdown);

    SAIFormat getSerialAudioInterfaceFormat();

    void setSerialAudioInterfaceFormat(SAIFormat format);

    bool getSpeed();

    void setSpeed(bool doubleSpeed);

    DigitalBoost getDigitalBoost();

    void setDigitalBoost(DigitalBoost boost);

    bool getHighPassFilterMode();

    void setHighPassFilterMode(bool bypass);

    void muteLeft(bool mute);

    void muteRight(bool mute);

    /**
     * @brief Mute both channels
     * @param mute true to mute, false to unmute
     */
    void mute(bool mute) {
        muteLeft(mute);
        muteRight(mute);
    }

    void setFade(bool fade);

    void getVolume();

    void setVolume(uint8_t left, uint8_t right);

    /**
     * @brief Set volume for both channels
     * @param volume
     */
    void setVolume(uint8_t volume) {
        setVolume(volume, volume);
    }

    ChannelSelection getChannelSelection();

    void setChannelSelection(ChannelSelection channel);

    AnalogGain getAnalogGain();

    void setAnalogGain(AnalogGain gain);

    PWMRate getPWMRate();

    void setPWMRate(PWMRate rate);

    bool getOverTemperatureErrorStatus();

    bool getOutputDCErrorStatus();

    bool getOverCurrentErrorStatus();

    bool getClockErrorStatus();

    bool getErrorStatus();

    OCEThreshold getOCEThreshold();

    void setOCEThreshold(OCEThreshold threshold);

    uint32_t getDigitalClipper();

    void setDigitalClipper(uint32_t clipper);
};

#endif //__TAS5720_H__
