/**
 * @file TAS5720.cpp
 * @brief TAS5720L/M class implementation for I2C communication.
 *
 * Register map and bit fields per SLOS903B (TAS5720L/M datasheet).
 */
#include "TAS5720.h"

#ifndef _SERIAL
    #define _SERIAL Serial
#endif

TAS5720::TAS5720(TwoWire &i2c, uint8_t addr, bool debug) {
    _i2c = &i2c;
    _addr = addr;
    _debug = debug;
    volume = 0;
}

TAS5720::~TAS5720() {
}

void TAS5720::_write(uint8_t reg, uint8_t data) {
    _i2c->beginTransmission(_addr);
    _i2c->write(reg);
    _i2c->write(data);
    if (_i2c->endTransmission()) {
        if (_debug) {
            _SERIAL.print("TAS5720: Error writing to register ");
            _SERIAL.print(reg, HEX);
            _SERIAL.print(" with data ");
            _SERIAL.println(data, HEX);
        }
    } else {
        if (_debug) {
            _SERIAL.print("TAS5720: Writing to register ");
            _SERIAL.print(reg, HEX);
            _SERIAL.print(" with data ");
            _SERIAL.println(data, HEX);
        }
    }
}

uint8_t TAS5720::_read(uint8_t reg) {
    _i2c->beginTransmission(_addr);
    _i2c->write(reg);
    _i2c->endTransmission(false);
    if (_i2c->requestFrom(_addr, (uint8_t) 1)) {
        uint8_t buf = _i2c->read();
        if (_debug) {
            _SERIAL.print("TAS5720: Reading from register ");
            _SERIAL.println(reg, HEX);
            _SERIAL.print(" with data ");
            _SERIAL.println(buf, HEX);
        }
        return buf;
    } else {
        if (_debug) {
            _SERIAL.print("TAS5720: Error reading from register ");
            _SERIAL.println(reg, HEX);
        }
        return 0;
    }
}

uint8_t TAS5720::getDeviceIdentification() {
    return _read(TAS5720_ADDR_DEVICE_ID);
}

// ---------- Power Control (0x01) ----------

bool TAS5720::getSleepMode() {
    return bitRead(_read(TAS5720_ADDR_POWER_CONTROL), 1);
}

void TAS5720::setSleepMode(bool sleep) {
    uint8_t tmp = _read(TAS5720_ADDR_POWER_CONTROL);
    _write(TAS5720_ADDR_POWER_CONTROL, bitWrite(tmp, 1, sleep));
}

bool TAS5720::getShutdown() {
    return !bitRead(_read(TAS5720_ADDR_POWER_CONTROL), 0);
}

void TAS5720::setShutdown(bool shutdown) {
    uint8_t tmp = _read(TAS5720_ADDR_POWER_CONTROL);
    _write(TAS5720_ADDR_POWER_CONTROL, bitWrite(tmp, 0, !shutdown));
}

// ---------- Digital Control 1 (0x02) ----------

SAIFormat TAS5720::getSerialAudioInterfaceFormat() {
    return (SAIFormat) (_read(TAS5720_ADDR_DIGITAL_CONTROL_1) & 0x07);
}

void TAS5720::setSerialAudioInterfaceFormat(SAIFormat format) {
    uint8_t tmp = _read(TAS5720_ADDR_DIGITAL_CONTROL_1);
    tmp &= ~0x07;
    tmp |= (format & 0x07);
    _write(TAS5720_ADDR_DIGITAL_CONTROL_1, tmp);
}

bool TAS5720::getSpeed() {
    return bitRead(_read(TAS5720_ADDR_DIGITAL_CONTROL_1), 3);
}

void TAS5720::setSpeed(bool doubleSpeed) {
    uint8_t tmp = _read(TAS5720_ADDR_DIGITAL_CONTROL_1);
    _write(TAS5720_ADDR_DIGITAL_CONTROL_1, bitWrite(tmp, 3, doubleSpeed));
}

bool TAS5720::getHighPassFilterMode() {
    return bitRead(_read(TAS5720_ADDR_DIGITAL_CONTROL_1), 7);
}

void TAS5720::setHighPassFilterMode(bool bypass) {
    uint8_t tmp = _read(TAS5720_ADDR_DIGITAL_CONTROL_1);
    _write(TAS5720_ADDR_DIGITAL_CONTROL_1, bitWrite(tmp, 7, bypass));
}

// ---------- Digital Control 2 (0x03) ----------
// Layout: RSV[7:5]=100, MUTE[4], RSV[3]=0, TDM_SLOT_SELECT[2:0].

bool TAS5720::getMute() {
    return bitRead(_read(TAS5720_ADDR_DIGITAL_CONTROL_2), 4);
}

void TAS5720::mute(bool m) {
    uint8_t tmp = _read(TAS5720_ADDR_DIGITAL_CONTROL_2);
    // Force RSV[7:5]=100 and RSV[3]=0 per datasheet.
    tmp = (tmp & 0x07) | 0x80;
    bitWrite(tmp, 4, m);
    _write(TAS5720_ADDR_DIGITAL_CONTROL_2, tmp);
}

// ---------- Volume Control (0x04) ----------

void TAS5720::getVolume() {
    volume = _read(TAS5720_ADDR_VOLUME_CONTROL);
}

void TAS5720::setVolume(uint8_t v) {
    _write(TAS5720_ADDR_VOLUME_CONTROL, v);
}

// ---------- Analog Control (0x06) ----------
// Layout: RSV[7]=0, PWM_RATE[6:4], ANALOG_GAIN[3:2], RSV[1:0]=01.

AnalogGain TAS5720::getAnalogGain() {
    return (AnalogGain) ((_read(TAS5720_ADDR_ANALOG_CONTROL) >> 2) & 0x03);
}

void TAS5720::setAnalogGain(AnalogGain gain) {
    uint8_t tmp = _read(TAS5720_ADDR_ANALOG_CONTROL);
    // Clear bit 7 (RSV must be 0) and ANALOG_GAIN[3:2].
    tmp &= 0x73;
    tmp |= ((gain & 0x03) << 2);
    // Force RSV[1:0] = 01.
    tmp = (tmp & 0xFC) | 0x01;
    _write(TAS5720_ADDR_ANALOG_CONTROL, tmp);
}

PWMRate TAS5720::getPWMRate() {
    return (PWMRate) ((_read(TAS5720_ADDR_ANALOG_CONTROL) >> 4) & 0x07);
}

void TAS5720::setPWMRate(PWMRate rate) {
    uint8_t tmp = _read(TAS5720_ADDR_ANALOG_CONTROL);
    // Clear bit 7 (RSV must be 0) and PWM_RATE[6:4].
    tmp &= 0x0F;
    tmp |= ((rate & 0x07) << 4);
    // Force RSV[1:0] = 01.
    tmp = (tmp & 0xFC) | 0x01;
    _write(TAS5720_ADDR_ANALOG_CONTROL, tmp);
}

// ---------- Fault Configuration / Error Status (0x08) ----------

bool TAS5720::getOverTemperatureErrorStatus() {
    return bitRead(_read(TAS5720_ADDR_FAULT_CONFIG_ERROR_STATUS), 0);
}

bool TAS5720::getOutputDCErrorStatus() {
    return bitRead(_read(TAS5720_ADDR_FAULT_CONFIG_ERROR_STATUS), 1);
}

bool TAS5720::getOverCurrentErrorStatus() {
    return bitRead(_read(TAS5720_ADDR_FAULT_CONFIG_ERROR_STATUS), 2);
}

bool TAS5720::getClockErrorStatus() {
    return bitRead(_read(TAS5720_ADDR_FAULT_CONFIG_ERROR_STATUS), 3);
}

bool TAS5720::getErrorStatus() {
    return (_read(TAS5720_ADDR_FAULT_CONFIG_ERROR_STATUS) & 0x0F) != 0x00;
}

uint8_t TAS5720::getFaultStatus() {
    return _read(TAS5720_ADDR_FAULT_CONFIG_ERROR_STATUS);
}

OCEThreshold TAS5720::getOCEThreshold() {
    return (OCEThreshold) ((_read(TAS5720_ADDR_FAULT_CONFIG_ERROR_STATUS) >> 4) & 0x03);
}

void TAS5720::setOCEThreshold(OCEThreshold threshold) {
    uint8_t tmp = _read(TAS5720_ADDR_FAULT_CONFIG_ERROR_STATUS);
    tmp &= 0xCF;
    tmp |= ((threshold & 0x03) << 4);
    _write(TAS5720_ADDR_FAULT_CONFIG_ERROR_STATUS, tmp);
}

// ---------- Digital Clipper (split across 0x01, 0x10, 0x11) ----------

uint32_t TAS5720::getDigitalClipper() {
    uint32_t clip = 0;
    clip |= ((_read(TAS5720_ADDR_DIGITAL_CLIPPER_1) >> 2) & 0x3F);
    clip |= (_read(TAS5720_ADDR_DIGITAL_CLIPPER_2) << 6);
    clip |= (((_read(TAS5720_ADDR_POWER_CONTROL) >> 2) & 0x3F) << 14);
    return clip;
}

void TAS5720::setDigitalClipper(uint32_t clip) {
    if (clip < 0x100000) {
        uint8_t tmp = _read(TAS5720_ADDR_DIGITAL_CLIPPER_1);
        tmp &= 0x03;
        tmp |= (clip << 2);
        _write(TAS5720_ADDR_DIGITAL_CLIPPER_1, tmp);
        _write(TAS5720_ADDR_DIGITAL_CLIPPER_2, clip >> 6);
        tmp = _read(TAS5720_ADDR_POWER_CONTROL);
        tmp &= 0x03;
        tmp |= ((clip >> 14) << 2);
        _write(TAS5720_ADDR_POWER_CONTROL, tmp);
    }
}
