
#include <Arduino.h>
#include <WiFi.h>
#include "Audio.h"

//==================================================================
//firmware

//wifi placeholders (replace later with real wifi network)
#define WIFI_SSID "wifiname"
#define WIFI_PASSWORD "wifipassword"

//audio (placeholder)
#define AUDIO_STREAM_URL "streamurl"


//i25 pins from schematic
#define I2S_LRC 10
#define I2S_BCLK 11
#define I2S_DOUT 12

//button pins 
#define BTN_PLAY 4
#define BTN_UP 5
#define BTN_DOWN 6

//probably 0-21 in volunme right?
#define MIN_VOLUME 0
#define MAX_VOLUME 21
#define START_VOLUME 12

#define DEBOUNCE_MS 40

class DebouncedButton{
  private: 
    uint8_t pin;
    bool stableState;
    bool lastReading;
    unsigned long lastChangeTime;

  public:
    DebouncedButton(uint8_t buttonPin)
    : pin(buttonPin), stableState(HIGH), lastReading(HIGH), lastChangeTime(0) {

    }
    void begin(){
      pinMode(pin, INPUT_PULLUP);
      stableState = digitalRead(pin);
      lastReading = stableState;
    }

    bool wasPressed(){
        bool reading = digitalRead(pin);

        if(reading != lastReading){
          lastChangeTime = millis();
          lastReading = reading;
        }

        if((millis()- lastChangeTime) > DEBOUNCE_MS && reading!= stableState){
          stableState = reading;
        
          if(stableState == LOW){
            return true;
          }
        }
        return false;
    }
};

class HeadphoneStandPlayer{
  private:
    Audio audio;
    DebouncedButton playButton;
    DebouncedButton volumeUpButton;
    DebouncedButton volumeDownButton;

    int volume;
    bool streamStarted;
    bool streamPaused;

    public:
      HeadphoneStandPlayer()
      : playButton(BTN_PLAY), volumeUpButton(BTN_UP), volumeDownButton(BTN_DOWN),volume(START_VOLUME), streamStarted(false),
      streamPaused(false){}

      void begin(){
        playButton.begin();
        volumeUpButton.begin();
        volumeDownButton.begin();

        audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
        audio.setVolume(volume);

        Serial.println("Audio hardware ready ヽ(✿ﾟ▽ﾟ)ノ");
        Serial.println("Press PLAY to start the Wi-Fi stream o(*^＠^*)o");

      }

      void update(){
        audio.loop();
        if(playButton.wasPressed()){
          handlePlayButton();
        }

        if(volumeUpButton.wasPressed()){
          changeVolume(1);
        }

        if(volumeDownButton.wasPressed()){
          changeVolume(-1);
        }
      }
  private: 
    void handlePlayButton(){
      if(!streamStarted){
        Serial.println("PLAY pressed: starting stream (☞ﾟヮﾟ)☞ ☜(ﾟヮﾟ☜)");
        audio.connecttohost(AUDIO_STREAM_URL);
        streamStarted = true;
        streamPaused = false;
        return;
      }

      audio.pauseResume();
      streamPaused = !streamPaused;
    }
    void changeVolume(int amount){
      volume+= amount;

      if(volume<MIN_VOLUME){
        volume = MIN_VOLUME;
      }

      if(volume>MAX_VOLUME){
        volume = MAX_VOLUME;
      }

      audio.setVolume(volume);
      Serial.print("Volume: ");
      Serial.println(volume);
    }
};

HeadphoneStandPlayer player;

void connectToWifi(){
  Serial.print("Connecting to Wi-Fi o(￣▽￣)ｄ");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while(WiFi.status() != WL_CONNECTED){
    delay(250);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup(){
  Serial.begin(115200);
  delay(500);

  connectToWifi();
  player.begin();
}

void loop(){
  player.update();
}