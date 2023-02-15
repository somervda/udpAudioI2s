#include <Arduino.h>
/**
 * @file example-udp-send.ino
 * @author Phil Schatzmann
 * @brief Sending audio over udp
 * @version 0.1
 * @date 2022-03-09
 *
 * @copyright Copyright (c) 2022
 *
 * ATTENTION: DRAFT - not tested yet
 */

#include "AudioTools.h"
#include "AudioLibs/Communication.h"

uint16_t sample_rate = 10000;
uint8_t channels = 1; // The stream will have 1 channels

I2SStream i2sStream;                                    // Access I2S as stream
ConverterFillLeftAndRight<int16_t> filler(LeftIsEmpty); // fill both channels - or change to RightIsEmpty

UDPStream udp;
Throttle throttle;
IPAddress udpAddress(192, 168, 1, 255);
const int udpPort = 7000;
StreamCopy copier(udp, i2sStream); // copies i2s sound into udp
const char *ssid = "guest24";
const char *password = "backyard";

void setup()
{
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Info);

  // connect to WIFI
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println(WiFi.localIP());

  // start i2s input with default configuration
  Serial.println("starting I2S...");
  // auto config = i2sStream.defaultConfig(RX_MODE);
  // config.i2s_format = I2S_STD_FORMAT; // if quality is bad change to I2S_LSB_FORMAT https://github.com/pschatzmann/arduino-audio-tools/issues/23
  // config.sample_rate = 10000;
  // config.channels = 1;
  // config.bits_per_sample = 16;

  Serial.println("starting I2S...");
  auto config = i2sStream.defaultConfig(RXTX_MODE);
  config.channels = channels;
  config.sample_rate = sample_rate;
  config.bits_per_sample = 16;
  config.i2s_format = I2S_STD_FORMAT;
  config.is_master = true;
  config.port_no = 0;
  config.pin_ws = 18;
  config.pin_bck = 5;
  config.pin_data = 19;
  config.pin_data_rx = 17;
  config.pin_mck = 0;
  config.use_apll = true;

  i2sStream.begin(config);
  Serial.println("I2S started");

  // Define udp address and port
  Serial.println("starting udp...");
  udp.begin(udpAddress, udpPort);
  auto cfg = throttle.defaultConfig();
  cfg.channels = channels;
  cfg.sample_rate = sample_rate;
  throttle.begin(cfg);

  Serial.println("started...");
}

void loop()
{
  throttle.startDelay();
  int bytes = copier.copy();
  throttle.delayBytes(bytes);
}