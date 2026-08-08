/**
 * @file TAS5720.h
 * @brief Driver for the TI TAS5720L/M Class-D amplifier.
 *
 * Originally based on axoulc/TAS5720-Arduino (which targets the TAS5720A).
 * Forked and adapted for the TAS5720L/M register map (SLOS903B).
 *
 * Key M-variant differences vs. the upstream library:
 *  - Single Volume Control register at 0x04 (no separate L/R at 0x05).
 *  - Mute is bit 4 of reg 0x03 (Digital Control 2), not bits 0/1.
 *  - No Fade, Digital Boost, or Channel Select bit in 0x06.
 *  - Analog gain steps are 19.2 / 20.7 / 23.5 / 26.3 dBV.
 *  - Reg 0x06 bit 7 is RSV and must be 0; bits[1:0] RSV must be 01.
 */
#ifndef __TAS5720_H__
#define __TAS5720_H__

#include <Arduino.h>
#include <Wire.h>

#define TAS5720_I2C_SPEED 100000

#define TAS5720_ADDR_DEVICE_ID                 0x00
#define TAS5720_ADDR_POWER_CONTROL             0x01
#define TAS5720_ADDR_DIGITAL_CONTROL_1         0x02
#define TAS5720_ADDR_DIGITAL_CONTROL_2         0x03
#define TAS5720_ADDR_VOLUME_CONTROL            0x04
#define TAS5720_ADDR_ANALOG_CONTROL            0x06
#define TAS5720_ADDR_FAULT_CONFIG_ERROR_STATUS 0x08
#define TAS5720_ADDR_DIGITAL_CLIPPER_2         0x10
#define TAS5720_ADDR_DIGITAL_CLIPPER_1         0x11

// Fault/Error status bits in register 0x08 (see datasheet Table 22).
// CLKE is non-latching/self-clearing (reads high whenever the part is not in
// active mode — e.g. PVDD/16V rail absent or a halted SAIF clock); OTE/DCE/OCE
// are latching and clear only on an SDZ toggle.
#define TAS5720_FAULT_OTE  0x01  // bit 0: over-temperature (latching)
#define TAS5720_FAULT_DCE  0x02  // bit 1: output DC error (latching)
#define TAS5720_FAULT_OCE  0x04  // bit 2: over-current / output short (latching)
#define TAS5720_FAULT_CLKE 0x08  // bit 3: SAIF clock error (non-latching)

typedef enum : uint8_t {
    SAI_24_BITS_RIGHT_JUSTIFIED   = 0,
    SAI_20_BITS_LEFT_JUSTIFIED    = 1,
    SAI_18_BITS_RIGHT_JUSTIFIED   = 2,
    SAI_16_BITS_LEFT_JUSTIFIED    = 3,
    SAI_I2S                       = 4,
    SAI_16_24_BITS_LEFT_JUSTIFIED = 5,
} SAIFormat;

typedef enum : uint8_t {
    GAIN_19_2_DBV = 0,
    GAIN_20_7_DBV = 1,
    GAIN_23_5_DBV = 2,
    GAIN_26_3_DBV = 3,
} AnalogGain;

typedef enum : uint8_t {
    // Single-speed values; in double-speed mode these halve (see datasheet
    // Table 6). Names track the single-speed multiplier for readability.
    RATE_6_LRCK  = 0,
    RATE_8_LRCK  = 1,
    RATE_10_LRCK = 2,
    RATE_12_LRCK = 3,
    RATE_14_LRCK = 4,
    RATE_16_LRCK = 5,
    RATE_20_LRCK = 6,
    RATE_24_LRCK = 7,
} PWMRate;

typedef enum : uint8_t {
    OCE_DEFAULT           = 0,
    OCE_REDUCE_75_PERCENT = 1,
    OCE_REDUCE_50_PERCENT = 2,
    OCE_REDUCE_25_PERCENT = 3,
} OCEThreshold;

class TAS5720 {
private:
    TwoWire *_i2c;
    uint8_t _addr;
    bool _debug;

    void _write(uint8_t reg, uint8_t data);
    uint8_t _read(uint8_t reg);

public:
    // Last value read by getVolume().
    uint8_t volume;

    TAS5720(TwoWire &i2c, uint8_t addr, bool debug = false);
    ~TAS5720();

    uint8_t getDeviceIdentification();

    // Raw register read, for diagnostics that need a register this API does not
    // model (SendAudioI2S dumps 0x02/0x03/0x04/0x06 when chasing a TAS5720 CLKE).
    uint8_t readReg(uint8_t reg);

    // Power Control (0x01) as a raw byte — the bit accessors above decode single
    // bits, but the CLKE diagnostics want the whole register before/after a
    // shutdown->active cycle.
    uint8_t readPowerControl();

    bool getSleepMode();
    void setSleepMode(bool sleep);

    bool getShutdown();
    void setShutdown(bool shutdown);

    SAIFormat getSerialAudioInterfaceFormat();
    void setSerialAudioInterfaceFormat(SAIFormat format);

    bool getSpeed();
    void setSpeed(bool doubleSpeed);

    bool getHighPassFilterMode();
    void setHighPassFilterMode(bool bypass);

    // True = muted (volume ramped down, idle audio played).
    bool getMute();
    void mute(bool mute);

    // Reads reg 0x04 into `volume`.
    void getVolume();
    // Writes reg 0x04 directly; 0xCF = 0 dB, 0xFF = +24 dB, < 0x07 = mute.
    void setVolume(uint8_t v);

    AnalogGain getAnalogGain();
    void setAnalogGain(AnalogGain gain);

    PWMRate getPWMRate();
    void setPWMRate(PWMRate rate);

    bool getOverTemperatureErrorStatus();
    bool getOutputDCErrorStatus();
    bool getOverCurrentErrorStatus();
    bool getClockErrorStatus();
    bool getErrorStatus();

    // Raw read of reg 0x08; decode with the TAS5720_FAULT_* masks. One I2C
    // transaction (vs. one per per-bit getter).
    uint8_t getFaultStatus();

    OCEThreshold getOCEThreshold();
    void setOCEThreshold(OCEThreshold threshold);

    uint32_t getDigitalClipper();
    void setDigitalClipper(uint32_t clipper);
};

#endif // __TAS5720_H__
