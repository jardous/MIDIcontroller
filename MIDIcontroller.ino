
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
#define STRIP_PIN 11
#define DEFAULT_MIDI_CHANNEL 1
#define BOUNCE_INTERVAL 10

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

#include "NeoPixel.h"
#define ILLUM_DEFAULT strip.Color(0, 5, 0)


// the controller board is 4x3 buttons
#define BOARD_ROWS 4
#define BOARD_COLS 3

class Button : public Bounce
{

public:

  // constructor
  Button(uint8_t index, int pin, uint8_t cc,
         uint8_t value=0, uint8_t channel=DEFAULT_MIDI_CHANNEL)
                                    : m_index(index)
                                    , m_cc(cc)
                                    , m_value(value)
                                    , m_channel(DEFAULT_MIDI_CHANNEL)

  {
    pinMode(pin, INPUT_PULLUP);
    attach(pin);
    interval(BOUNCE_INTERVAL);
  }

  void update(void) {
    Bounce::update();
    if (Bounce::fell()) {
      //Serial.print(pin);
      //Serial.println(" felt");
      MIDI.sendControlChange(m_cc, m_value, m_channel);

      if (m_index == 8) {
        rainbowCycle(5);
      }
      
      for (int i=0; i<4; i++) {
        strip.setPixelColor(i, ILLUM_DEFAULT);
      }
      strip.setPixelColor(m_index, strip.Color(50, 0, 0));
      strip.show();
    }
  }

private:
  uint8_t m_index;
  uint8_t m_interval;
  uint8_t m_cc;
  uint8_t m_value;
  uint8_t m_channel;
};


Button *buttons[BOARD_ROWS * BOARD_COLS] = {NULL,};



void setup() {
  Serial.begin(9600);
  /*
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }*/

  // add buttons to the array and nstantiate the button objects
  // first row (bottom)
  buttons[0] = new Button(/* index */0, /* pin */2, /* cc */1);  // PRESET A
  buttons[1] = new Button(/* index */1, /* pin */3, /* cc */2);  // PRESET B
  buttons[2] = new Button(/* index */2, /* pin */4, /* cc */3);  // PRESET C
  buttons[3] = new Button(/* index */3, /* pin */5, /* cc */4);  // PRESET D
  // second row
  buttons[4] = new Button(/* index */4, /* pin */6, /* cc */21);  // STOMP 1
  buttons[5] = new Button(/* index */5, /* pin */7, /* cc */22);  // STOMP 2
  buttons[6] = new Button(/* index */6, /* pin */8, /* cc */23);  // STOMP 3
  buttons[7] = NULL;
  // third row (top)
  buttons[8] = new Button(/* index */7, /* pin */9, /* cc */31);  // TUNER
  buttons[9] = new Button(/* index */8, /* pin */10, /* cc */32); // BYPASS
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

  for (int i=0; i<4; i++) {
    strip.setPixelColor(i, ILLUM_DEFAULT);
  }
  strip.show();
}

void loop() {
  // Update the Bounce instance
  for (int i=0; i<BOARD_ROWS*BOARD_COLS; i++) {
    if (buttons[i] != NULL) {
      buttons[i]->update();
    }
  }
}
