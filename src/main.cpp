/**
 * @file streams-i2s-filter-i2s.ino
 * @brief Copy audio from I2S to I2S using an FIR filter
 * @author Phil Schatzmann
 * @copyright GPLv3
 */

#include "AudioTools.h"
#include "AudioLibs/Communication.h"

uint16_t sample_rate = 10000;
uint16_t channels = 1;
I2SStream pModI2s;

UDPStream udp;
Throttle throttle;
IPAddress udpAddress(192, 168, 1, 255);
const int udpPort = 7000;
StreamCopy copier(udp, pModI2s); // copies i2s sound into udp
const char *ssid = "guest24";
const char *password = "backyard";

// Arduino Setup
void setup(void)
{
  // Open Serial

  Serial.begin(115200);
  // change to Warning to improve the quality
  AudioLogger::instance().begin(Serial, AudioLogger::Error);

  // connect to WIFI
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println(WiFi.localIP());

  // start I2S in
  auto config = pModI2s.defaultConfig(RXTX_MODE);
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
  pModI2s.begin(config);

  // Define udp address and port
  Serial.println("starting udp...");
  udp.begin(udpAddress, udpPort);

  auto cfg = throttle.defaultConfig();
  cfg.channels = channels;
  cfg.sample_rate = sample_rate;
  throttle.begin(cfg);
}

// Arduino loop - copy sound to out
void loop()
{
  throttle.startDelay();
  int bytes = copier.copy();
  throttle.delayBytes(bytes);
}