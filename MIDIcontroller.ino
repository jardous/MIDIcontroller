#include <SPI.h>


#include <MIDI.h>
#include <midi_Defs.h>
#include <midi_Message.h>
#include <midi_Namespace.h>
#include <midi_Settings.h>

#include <Adafruit_NeoPixel.h>

// Include the Bounce2 library found here:
// https://github.com/thomasfredericks/Bounce-Arduino-Wiring

#include <Bounce2.h>

#define LED_PIN 13
#define STRIP_PIN A0
#define DEFAULT_MIDI_CHANNEL 1
#define MIDI_CC 0xB0
#define BOUNCE_INTERVAL 10

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);


// NEOPIXEL LED
#include "NeoPixel.h"
#define ILLUM_DEFAULT strip.Color(0, 5, 0)


// BLUETOOTH
#include <SoftwareSerial.h>

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "BluefruitConfig.h"
#define __BLE

#ifdef __BLE
SoftwareSerial bluefruitSS = SoftwareSerial(BLUEFRUIT_SWUART_TXD_PIN, BLUEFRUIT_SWUART_RXD_PIN);
Adafruit_BluefruitLE_UART ble(bluefruitSS, BLUEFRUIT_UART_MODE_PIN,
                      BLUEFRUIT_UART_CTS_PIN, BLUEFRUIT_UART_RTS_PIN);
// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
}

/*
 * Set the new characteristic value
 * format: AT+GATTCHAR=1,AF-80-XX-YY-ZZ
 * where XX is the command, YY is note and ZZ is velocity
*/
void BLESendMIDI(unsigned char command, unsigned char param1, unsigned char param2) {
    static char *buf = "AT+GATTCHAR=1,AF-80-00-00-00";
    sprintf(buf+20, "%X-%02X-%02X", command, param1, param2);
    ble.sendCommandCheckOK(buf);
    Serial.println(buf);
}
#endif


#include "LEDEffects.h"

// the controller board is 3x4 buttons
#define BOARD_ROWS 3
#define BOARD_COLS 4

typedef enum { SWITCH, STOMP } ButtonType;

void allSwitchesOff(void);

class Button : public Bounce
{

public:

  Button(int pin, ButtonType type, uint8_t cc,
         uint8_t value=0, uint8_t channel=DEFAULT_MIDI_CHANNEL)
                                    : m_type(type) , m_state(false)
                                    , m_cc(cc), m_value(value), m_channel(DEFAULT_MIDI_CHANNEL)
  {
    pinMode(pin, INPUT_PULLUP);
    attach(pin);
    interval(BOUNCE_INTERVAL);
  }

  void update(void) {
    Bounce::update();
    if (Bounce::fell()) {
      MIDI.sendControlChange(m_cc, m_value, m_channel);
#ifdef __BLE
      if (ble.isConnected() != 0) {
        Serial.println("sending CC");
        BLESendMIDI(MIDI_CC, m_cc, 127);
      }
#endif
      if (m_cc == 31) {
        rainbowCycle(5);
      }
      
      // set to off others if this is switch
      if (m_type == SWITCH) {  // switch
        allSwitchesOff();
        m_state = true;
      } else {  // stomp
        m_state = !m_state;
      }
      updateLEDs();
    }
  }

  ButtonType m_type;  // 0 - switch, 1 - stomp
  bool m_state;
  uint8_t m_interval;
  uint8_t m_cc;
  uint8_t m_value;
  uint8_t m_channel;
};

Button *buttons[BOARD_ROWS * BOARD_COLS] = { NULL };

void allSwitchesOff(void) {
  for (int i=0; i<BOARD_ROWS*BOARD_COLS; i++) {
    if (buttons[i]) {
      if (buttons[i]->m_type == SWITCH) {
        buttons[i]->m_state = false;
      }
    }
  }
}

void updateLEDs(void) {
  for (int i=0, ledi=0; i<BOARD_ROWS*BOARD_COLS; i++) {
    if (buttons[i]) {
        if (buttons[i]->m_state == true)  {
          if (buttons[i]->m_type == SWITCH) {
            strip.setPixelColor(ledi, strip.Color(pulser.m_brightness*5+90, pulser.m_brightness*5+90, pulser.m_brightness*5+90));
          } else {  // stomp
            strip.setPixelColor(ledi, strip.Color(pulser.m_brightness*5+50, 0, 0));
          }
        } else {
          if (buttons[i]->m_type == SWITCH) {
            strip.setPixelColor(ledi, strip.Color(0, 0, pulser.m_brightness*2+3));
          } else {  // stomp
            strip.setPixelColor(ledi, strip.Color(0, pulser.m_brightness+3, 0));
          }
        }
        ledi++;
    }
  }
  strip.show();
}



void setup() {
  Serial.begin(9600);

  // add buttons to the array and nstantiate the button objects
  // first row (bottom)
  buttons[0] = new Button(/*pin*/2, /*type*/SWITCH, /*cc*/1);
  buttons[1] = new Button(/*pin*/3, /*type*/STOMP, /*cc*/2);
  buttons[2] = new Button(/*pin*/4, /*type*/STOMP, /*cc*/3);
  buttons[3] = new Button(/*pin*/5, /*type*/SWITCH, /*cc*/4);
  // second row
  buttons[4] = new Button(/*pin*/6, /*type*/STOMP, /*cc*/21);
  buttons[5] = new Button(/*pin*/7, /*type*/STOMP, /*cc*/22);
  buttons[6] = new Button(/*pin*/8, /*type*/STOMP, /*cc*/23);
  buttons[7] = NULL;
  // third row (top)
  buttons[8] = new Button(/*pin*/A1, /*type*/STOMP, /*cc*/31);
  buttons[9] = new Button(/*pin*/A2, /*type*/STOMP, /*cc*/32);
  buttons[10] = NULL;
  buttons[11] = NULL;

  //Setup the LED
  pinMode(LED_PIN, OUTPUT);

  MIDI.begin();
  Serial.println("ready");
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'


  // initial effect
  colorWipe(strip.Color(255, 0, 0), 50); // Red
  colorWipe(strip.Color(0, 255, 0), 50); // Green
  colorWipe(strip.Color(0, 0, 255), 50); // Blue

#ifdef __BLE
  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }

  Serial.println( F("OK!") );
  // Perform a factory reset to make sure everything is in a known state
  Serial.println(F("Performing a factory reset: "));
  if (! ble.factoryReset() ){
       error(F("Couldn't factory reset"));
  }

  // Disable command echo from Bluefruit
  ble.echo(false);

  // Print Bluefruit information
  ble.info();

  // configure GATT services and advertising
  ble.sendCommandCheckOK("AT+GATTLIST");
  ble.sendCommandCheckOK("AT+GATTADDSERVICE=UUID128=03-B8-0E-5A-ED-E8-4B-33-A7-51-6C-E3-4E-C4-C7-00");
  ble.sendCommandCheckOK("AT+GATTADDCHAR=UUID128=77-72-E5-DB-38-68-41-12-A1-A9-F2-66-9D-10-6B-F3,PROPERTIES=0x96,MIN_LEN=1,MAX_LEN=20");
  ble.sendCommandCheckOK("AT+GAPINTERVALS=8,15,250,180");
  ble.sendCommandCheckOK("AT+GAPSETADVDATA=02-01-06-11-06-00-C7-C4-4E-E3-6C-51-A7-33-4B-E8-ED-5A-0E-B8-03");
  ble.sendCommandCheckOK("AT+BLEPOWERLEVEL=4");

  ble.reset();

  ble.verbose(false);  // debug info is a little annoying after this point!
#endif __BLE
}


void loop() {
  // Update the Bounce instance
  for (int i=0; i<BOARD_ROWS*BOARD_COLS; i++) {
    if (buttons[i] != NULL) {
      buttons[i]->update();
    }
  }
  pulser.update();
}
