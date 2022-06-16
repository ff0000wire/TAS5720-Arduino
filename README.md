<h1 align="center">Welcome to TAS5720 Arduino Library 👋</h1>
<p>
  <a href="https://github.com/axoulc/TAS5720-Arduino/blob/main/LICENSE" target="_blank">
    <img alt="License: MIT License" src="https://img.shields.io/badge/License-MIT License-yellow.svg" />
  </a>
</p>

> Arduino library to configure TI TAS5720(A) Class D amplifier

## What is implemented ?
- I2C communication
- Configuration of the amplifier
- Volume control
- Mute control
- Power control
- Errors states

## Install
Use the source files

Install it with platform io : [https://platformio.org/lib/show/13307/RGB%20Driver%20ESP](https://platformio.org/lib/show/13307/RGB%20Driver%20ESP)
```
pio lib install "lefrenchpoc/RGB Driver ESP"
```
or with the `platformio.ini` file :

```
[env:myenv]
;...

lib_deps = lefrenchpoc/RGB Driver ESP @ ^0.2
```

## Usage

```cpp
#include <TAS5720.h>

TAS5720 amp(Wire, 0x6D, true);
uint8_t left = 0, right = 0;

void setup() {
    pinMode(SPEAKER_SD, OUTPUT);  // /SPK_SD Pin
    digitalWrite(SPEAKER_SD, LOW);
    // Start I2S

    amp.mute(true);
    amp.setSerialAudioInterfaceFormat(SAI_I2S);
    amp.setChannelSelection(RIGHT);
    amp.setAnalogGain(GAIN_25_DBV);
    amp.setDigitalBoost(DIGITAL_BOOST_6DB);
    amp.setVolume(200);
    // Do Anything you want here
    digitalWrite(SPEAKER_SD, LOW);
    amp.mute(false);
    // Play some music
}

void loop() {
    amp.getVolume();
    left = amp.volumeLeft;
    right = amp.volumeRight;
    amp.setVolume(++left, ++right);
    delay(1000);
}
```

## Author

👤 **Axel Chabot**

* Github: [@axoulc](https://github.com/axoulc)

## Show your support

Give a ⭐️ if this project helped you!

## 📝 License

Copyright © 2022 [Axel Chabot](https://github.com/axoulc).<br />
This project is [MIT](https://github.com/axoulc/TAS5720-Arduino/blob/main/LICENSE) licensed.