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
    digitalWrite(SPEAKER_SD, HIGH);
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
