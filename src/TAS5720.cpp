/**
 * @file TAS5720.cpp
 * @brief TAS5720 class implementation for I2C communication
 * @author Axel Chabot
 * @date: 2022-06-16
 * @version 1.0
 */
#include "TAS5720.h"

/**
 * @brief Constructor
 * @param i2c I2C bus (Wire instance)
 * @param addr TAS5720 I2C address
 * @param debug Enable debug mode
 */
TAS5720::TAS5720(TwoWire &i2c, uint8_t addr, bool debug) {
    _i2c = &i2c;
    _addr = addr;
    _debug = debug;
    _i2c->begin();
    _i2c->setClock(TAS5720_I2C_SPEED);
}

/**
 * @brief Destructor
 */
TAS5720::~TAS5720() {
    _i2c->end();
}

/**
 * @brief Write register
 * @param reg Register
 * @param data Data to write
 */
void TAS5720::_write(uint8_t reg, uint8_t data) {
    _i2c->beginTransmission(_addr);
    _i2c->write(reg);
    _i2c->write(data);
    if (_i2c->endTransmission()) {
        if (_debug) {
            Serial.print("TAS5720: Error writing to register ");
            Serial.print(reg, HEX);
            Serial.print(" with data ");
            Serial.println(data, HEX);
        }
    } else {
        if (_debug) {
            Serial.print("TAS5720: Writing to register ");
            Serial.print(reg, HEX);
            Serial.print(" with data ");
            Serial.println(data, HEX);
        }
    }
}

/**
 * @brief Read register
 * @param reg Register
 * @return Data read
 */
uint8_t TAS5720::_read(uint8_t reg) {
    _i2c->beginTransmission(_addr);
    _i2c->write(reg);
    _i2c->endTransmission(false);
    if (_i2c->requestFrom(_addr, (uint8_t) 1)) {
        uint8_t buf = _i2c->read();
        if (_debug) {
            Serial.print("TAS5720: Reading from register ");
            Serial.println(reg, HEX);
            Serial.print(" with data ");
            Serial.println(buf, HEX);
        }
        return buf;
    } else {
        if (_debug) {
            Serial.print("TAS5720: Error reading from register ");
            Serial.println(reg, HEX);
        }
        return 0;
    }
}

/**
 * @brief Get TAS5720 chip ID
 * @return ID
 */
uint8_t TAS5720::getDeviceIdentification() {
    return _read(TAS5720_ADDRESS_DEVICE_IDENTIFICATION);
}

/**
 * @brief Get sleep mode
 * @return Enable ou Disable
 */
bool TAS5720::getSleepMode() {
    return bitRead(_read(TAS5720_ADDRESS_POWER_CONTROL), 1);
}

/**
 * @brief Set sleep mode
 * @param sleep Enable ou Disable
 */
void TAS5720::setSleepMode(bool sleep) {
    _rxData = _read(TAS5720_ADDRESS_POWER_CONTROL);
    _write(TAS5720_ADDRESS_POWER_CONTROL, bitWrite(_rxData, 1, sleep));
}

/**
 * @brief Get Shutdown state
 * @return Enable ou Disable
 */
bool TAS5720::getShutdown() {
    return !bitRead(_read(TAS5720_ADDRESS_POWER_CONTROL), 0);
}

/**
 * @brief Set Shutdown state
 * @param shutdown Enable ou Disable
 */
void TAS5720::setShutdown(bool shutdown) {
    _rxData = _read(TAS5720_ADDRESS_POWER_CONTROL);
    _write(TAS5720_ADDRESS_POWER_CONTROL, bitWrite(_rxData, 0, !shutdown));
}

/**
 * @brief Get Serial Audio Interface Format
 * @return Format (Check SAIFormat struct)
 */
SAIFormat TAS5720::getSerialAudioInterfaceFormat() {
    return (SAIFormat) (_read(TAS5720_ADDRESS_DIGITAL_CONTROL) & 0x07);
}

/**
 * @brief Set Serial Audio Interface Format
 * @param format
 */
void TAS5720::setSerialAudioInterfaceFormat(SAIFormat format) {
    uint8_t tmp = _read(TAS5720_ADDRESS_DIGITAL_CONTROL);
    tmp &= 0xF8;
    tmp |= format;
    _write(TAS5720_ADDRESS_DIGITAL_CONTROL, tmp);
}

/**
 * @brief Get single or double speed
 * @return False single speed, True double speed
 */
bool TAS5720::getSpeed() {
    return bitRead(_read(TAS5720_ADDRESS_DIGITAL_CONTROL), 3);
}

/**
 * @brief Set single or double speed
 * @param doubleSpeed False single speed, True double speed
 */
void TAS5720::setSpeed(bool doubleSpeed) {
    _rxData = _read(TAS5720_ADDRESS_DIGITAL_CONTROL);
    _write(TAS5720_ADDRESS_DIGITAL_CONTROL, bitWrite(_rxData, 3, doubleSpeed));
}

/**
 * @brief Get Digital Boost
 * @return Boost (Check Boost struct)
 */
DigitalBoost TAS5720::getDigitalBoost() {
    return (DigitalBoost) ((_read(TAS5720_ADDRESS_DIGITAL_CONTROL) & 0x30) >> 4);
}

/**
 * @brief Set Digital Boost
 * @param boost
 */
void TAS5720::setDigitalBoost(DigitalBoost boost) {
    uint8_t tmp = _read(TAS5720_ADDRESS_DIGITAL_CONTROL);
    tmp &= 0xCF;
    tmp |= (boost << 4);
    _write(TAS5720_ADDRESS_DIGITAL_CONTROL, tmp);
}

/**
 * @brief Get High Pass Filter Status
 * @return False : Active, True : Bypass
 */
bool TAS5720::getHighPassFilterMode() {
    return bitRead(_read(TAS5720_ADDRESS_DIGITAL_CONTROL), 7);
}

/**
 * @brief Set High Pass Filter Status
 * @param bypass False : Active, True : Bypass
 */
void TAS5720::setHighPassFilterMode(bool bypass) {
    _rxData = _read(TAS5720_ADDRESS_DIGITAL_CONTROL);
    _write(TAS5720_ADDRESS_DIGITAL_CONTROL, bitWrite(_rxData, 7, bypass));
}

/**
 * @brief Mute left channel
 * @param mute true : Mute, false : Unmute
 */
void TAS5720::muteLeft(bool mute) {
    _rxData = _read(TAS5720_ADDRESS_VOLUME_CONTROL_CONFIGURATION);
    _write(TAS5720_ADDRESS_VOLUME_CONTROL_CONFIGURATION,
           bitWrite(_rxData, 0, mute));
}

/**
 * @brief Mute right channel
 * @param mute true : Mute, false : Unmute
 */
void TAS5720::muteRight(bool mute) {
    _rxData = _read(TAS5720_ADDRESS_VOLUME_CONTROL_CONFIGURATION);
    _write(TAS5720_ADDRESS_VOLUME_CONTROL_CONFIGURATION,
           bitWrite(_rxData, 1, mute));
}

/**
 * @brief Set Fade
 * @param fade true : Fade, false : No Fade
 */
void TAS5720::setFade(bool fade) {
    _rxData = _read(TAS5720_ADDRESS_VOLUME_CONTROL_CONFIGURATION);
    _write(TAS5720_ADDRESS_VOLUME_CONTROL_CONFIGURATION,
           bitWrite(_rxData, 7, fade));
}

/**
 * @brief Get Volume
 */
void TAS5720::getVolume() {
    volumeLeft = _read(TAS5720_ADDRESS_VOLUME_CONTROL_LEFT);
    volumeRight = _read(TAS5720_ADDRESS_VOLUME_CONTROL_RIGHT);
}

/**
 * @brief Set Volume
 * @param left
 * @param right
 */
void TAS5720::setVolume(uint8_t left, uint8_t right) {
    _write(TAS5720_ADDRESS_VOLUME_CONTROL_LEFT, left);
    _write(TAS5720_ADDRESS_VOLUME_CONTROL_RIGHT, right);
}

/**
 * @brief Get Channel Select
 * @return Channel (Check ChannelSelection struct)
 */
ChannelSelection TAS5720::getChannelSelection() {
    return (ChannelSelection) bitRead(_read(TAS5720_ADDRESS_ANALOG_CONTROL), 1);
}

/**
 * @brief Set Channel Select
 * @param channel
 */
void TAS5720::setChannelSelection(ChannelSelection channel) {
    _rxData = _read(TAS5720_ADDRESS_ANALOG_CONTROL);
    _write(TAS5720_ADDRESS_ANALOG_CONTROL, 0x80 | bitWrite(_rxData, 1, channel));
}

/**
 * @brief Get Analog Gain
 * @return gain (Check AnalogGain struct)
 */
AnalogGain TAS5720::getAnalogGain() {
    return (AnalogGain) ((_read(TAS5720_ADDRESS_ANALOG_CONTROL) >> 2) & 0x03);
}

/**
 * @brief Set Analog Gain
 * @param gain
 */
void TAS5720::setAnalogGain(AnalogGain gain) {
    uint8_t tmp = _read(TAS5720_ADDRESS_ANALOG_CONTROL);
    tmp &= 0xF3;
    tmp |= (gain << 2);
    tmp |= 0x80;
    _write(TAS5720_ADDRESS_ANALOG_CONTROL, tmp);
}

/**
 * @brief Get PWM Rate
 * @return PWM Rate (Check PWMRate struct)
 */
PWMRate TAS5720::getPWMRate() {
    return (PWMRate) ((_read(TAS5720_ADDRESS_ANALOG_CONTROL) >> 4) & 0x07);
}

/**
 * @brief Set PWM Rate
 * @param rate
 */
void TAS5720::setPWMRate(PWMRate rate) {
    uint8_t tmp = _read(TAS5720_ADDRESS_ANALOG_CONTROL);
    tmp &= 0x8F;
    tmp |= (rate << 4);
    tmp |= 0x80;
    _write(TAS5720_ADDRESS_ANALOG_CONTROL, tmp);
}
/**
 * @brief get Over Temperature Error
 * @return true : Error, false : No Error
 */
bool TAS5720::getOverTemperatureErrorStatus() {
    return bitRead(_read(TAS5720_ADDRESS_FAULT_CONFIGURATION_ERROR_STATUS), 0);
}

/**
 * @brief get Output DC Error
 * @return true : Error, false : No Error
 */
bool TAS5720::getOutputDCErrorStatus() {
    return bitRead(_read(TAS5720_ADDRESS_FAULT_CONFIGURATION_ERROR_STATUS), 1);
}

/**
 * @brief get Over Current Error
 * @return true : Error, false : No Error
 */
bool TAS5720::getOverCurrentErrorStatus() {
    return bitRead(_read(TAS5720_ADDRESS_FAULT_CONFIGURATION_ERROR_STATUS), 2);
}

/**
 * @brief get Clock Error
 * @return true : Error, false : No Error
 */
bool TAS5720::getClockErrorStatus() {
    return bitRead(_read(TAS5720_ADDRESS_FAULT_CONFIGURATION_ERROR_STATUS), 3);
}

/**
 * @brief get Error(s) register
 * @return true : Error(s), false : No Error
 */
bool TAS5720::getErrorStatus() {
    return (_read(TAS5720_ADDRESS_FAULT_CONFIGURATION_ERROR_STATUS) & 0x0F) != 0x00;
}

/**
 * @brief get Over Current Threshold(
 * @return OCE Threshold (Check OCEThreshold struct)
 */
OCEThreshold TAS5720::getOCEThreshold() {
    return (OCEThreshold) ((_read(TAS5720_ADDRESS_FAULT_CONFIGURATION_ERROR_STATUS) >> 4) & 0x03);
}

/**
 * @brief set Over Current Threshold
 * @param threshold
 */
void TAS5720::setOCEThreshold(OCEThreshold threshold) {
    uint8_t tmp = _read(TAS5720_ADDRESS_FAULT_CONFIGURATION_ERROR_STATUS);
    tmp &= 0xCF;
    tmp |= (threshold << 4);
    _write(TAS5720_ADDRESS_FAULT_CONFIGURATION_ERROR_STATUS, tmp);
}

/**
 * @brief get Digita lClipper
 * @return Value (20bits)
 */
uint32_t TAS5720::getDigitalClipper() {
    uint32_t clip = 0;
    clip |= ((_read(TAS5720_ADDRESS_DIGITAL_CLIPPER_1) >> 2) & 0x3F);
    clip |= (_read(TAS5720_ADDRESS_DIGITAL_CLIPPER_2) << 6);
    clip |= (((_read(TAS5720_ADDRESS_POWER_CONTROL) >> 2) & 0x3F) << 14);
    return clip;
}

/**
 * @brief set Digital Clipper
 * @param clip
 */
void TAS5720::setDigitalClipper(uint32_t clip) {
    if (clip < 0x100000) {
        uint8_t tmp = _read(TAS5720_ADDRESS_DIGITAL_CLIPPER_1);
        tmp &= 0x03;
        tmp |= (clip << 2);
        _write(TAS5720_ADDRESS_DIGITAL_CLIPPER_1, tmp);
        _write(TAS5720_ADDRESS_DIGITAL_CLIPPER_2, clip >> 6);
        tmp = _read(TAS5720_ADDRESS_POWER_CONTROL);
        tmp &= 0x03;
        tmp |= ((clip >> 14) << 2);
        _write(TAS5720_ADDRESS_POWER_CONTROL, tmp);
    }
}