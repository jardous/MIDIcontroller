#include <SPI.h>
#include <math.h>

#include <MIDI.h>
#include <midi_Defs.h>
#include <midi_Message.h>
#include <midi_Namespace.h>
#include <midi_Settings.h>

// Include the Bounce2 library found here:
// https://github.com/thomasfredericks/Bounce-Arduino-Wiring

#include <Bounce2.h>

#define LED_PIN 13
#define STRIP_PIN A0
#define DEFAULT_MIDI_CHANNEL 1
#define MIDI_CC 0xB0
#define WAH_CC 0x01
#define BOUNCE_INTERVAL 10
#define SHARP_PIN A4

// MIDI init
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);


// NEOPIXEL LED
#include "NeoPixel.h"
#define ILLUM_DEFAULT strip.Color(0, 5, 0)


#include "BLEModule.h"

#include "LEDEffects.h"

// the controller board is 3x4 buttons
#define BOARD_ROWS 3
#define BOARD_COLS 4

typedef enum { SWITCH, STOMP } ButtonType;

void allSwitchesOff(void);

void sendMIDI(uint8_t cc, uint8_t value, uint8_t channel=DEFAULT_MIDI_CHANNEL) {
    static char *buf = "   ";
    sprintf(buf, "%02d-%02d-%02d", cc, value, channel);
    Serial.println(buf);

    MIDI.sendControlChange(cc, value, channel);
    //bluetooth.BLESendMIDI(MIDI_CC, cc, value);
}

#include "Sharp.h"


class ButtonBase : public Bounce {
public:
  ButtonBase::ButtonBase(int pin, uint8_t midi_channel) : m_midi_channel(midi_channel) {
      pinMode(pin, INPUT_PULLUP);
      attach(pin);
      interval(BOUNCE_INTERVAL);
      ledIndex = ledStripIndexCounter;
      ledStripIndexCounter += 1;
  }

  virtual void update(void) {}
  virtual void setEnabled(bool state) {}  // stomp
  virtual void setState(bool state) {}  // switch
  virtual void resetLED(void) = 0;
  
  uint8_t m_midi_channel;
  int ledIndex;
private:
  static int ledStripIndexCounter;
};
int ButtonBase::ledStripIndexCounter = 0;

class Stomp : public ButtonBase {
public:
  Stomp(int pin, uint8_t cc,
         bool init_enabled=false, uint8_t on_value=1, uint8_t off_value=0, uint8_t midi_channel=DEFAULT_MIDI_CHANNEL)
                                    : m_cc(cc)
                                    , m_onvalue(on_value)
                                    , m_offvalue(off_value)
                                    , ButtonBase(pin, midi_channel) {
    setEnabled(init_enabled);
  }
  virtual void update(void) {
    Bounce::update();
    
    if (Bounce::fell()) {
      // set to off others if this is switch
      setEnabled(!m_enabled);
      //setState(!m_state);

      uint8_t m_value = m_offvalue;
      if (m_enabled == true) { m_value = m_onvalue; }
    
      /*
      if (m_cc == 31) {
        rainbowCycle(5);
      }
      */
      sendMIDI(m_cc, m_value, m_midi_channel);
    }
  }
  
  virtual void setEnabled(bool enabled) {
    m_enabled = enabled;
    resetLED();
  }
  
  virtual void resetLED(void) {
    if (m_enabled == true) {
      strip.setPixelColor(ledIndex, strip.Color(0, pulser.m_brightness*5 + 90, 0));
    } else {
      strip.setPixelColor(ledIndex, strip.Color(pulser.m_brightness*3 + 30, pulser.m_brightness*3 + 30, pulser.m_brightness*3 + 30));
    }
  }

private:
  bool m_enabled;
  uint8_t m_cc;
  uint8_t m_onvalue;
  uint8_t m_offvalue;
};


class Switch : public ButtonBase {
public:
  Switch(int pin, uint8_t cc, bool init_state=false, uint8_t midi_channel=DEFAULT_MIDI_CHANNEL)
                                    : m_cc(cc), ButtonBase(pin, midi_channel) {
    setState(init_state);
  }
  
  virtual void update(void) {
    Bounce::update();
    if (Bounce::fell()) {
      // set to off others if this is switch
      allSwitchesOff();
      setState(true);
      sendMIDI(m_cc, 1, m_midi_channel);
    }
  }
  
  virtual void setState(bool state) {
    m_state = state;
    resetLED();
  }

  virtual void resetLED(void) {
    if (m_state == true) {
      strip.setPixelColor(ledIndex, strip.Color(pulser.m_brightness*5 + 90, 0, 0));
    } else {
      strip.setPixelColor(ledIndex, strip.Color(0, 0, pulser.m_brightness*5 + 20));
    }
  }

private:
  bool m_state;
  uint8_t m_cc;
};

ButtonBase *buttons[BOARD_ROWS * BOARD_COLS] = { NULL };

void allSwitchesOff(void) {
  for (int i=0; i<BOARD_ROWS*BOARD_COLS; i++) {
    if (buttons[i]) {
        buttons[i]->setState(false);
    }
  }
}

void updateLEDs(void) {
  for (int i=0; i<BOARD_ROWS*BOARD_COLS; i++) {
    if (buttons[i]) {
        buttons[i]->resetLED();
    }
  }
  strip.show();
}



void setup() {
  Serial.begin(9600);

  // add buttons to the array and nstantiate the button objects
  
  // first row (bottom)
  buttons[0] = new Switch(/*pin*/2, /*cc*/50, /*init_state */true);  // performance 1
  buttons[1] = new Switch(/*pin*/3, /*cc*/51);   // performance 2
  buttons[2] = new Switch(/*pin*/4, /*cc*/52);  // performance 3
  buttons[3] = new Switch(/*pin*/5, /*cc*/53);   // performance 4
  // second row
  buttons[4] = new Stomp(/*pin*/6, /*cc*/18, /*init_state */false);  // stomp B
  buttons[5] = new Stomp(/*pin*/7, /*cc*/19, /*init_state */false);  // stomp C
  buttons[6] = new Stomp(/*pin*/8, /*cc*/26, /*init_state */true);  // delay
  buttons[7] = NULL;
  // third row (top)
  buttons[8] = new Stomp(/*pin*/A1, /*cc*/31, /*init_state */true);  // Tuner
  buttons[9] = new Stomp(/*pin*/A2, /*cc*/32, /*init_state */true);  // ???
  buttons[10] = NULL;
  buttons[11] = NULL;

  pinMode(SHARP_PIN, INPUT);
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
  
  for (int i=0; i<BOARD_ROWS*BOARD_COLS; i++) {
    if (buttons[i]) {
        buttons[i]->resetLED();
    }
  }
  
  //bluetooth.initialize();
}



void loop() {
  // Update the Bounce instance
  for (int i=0; i<BOARD_ROWS*BOARD_COLS; i++) {
    if (buttons[i] != NULL) {
      buttons[i]->update();
    }
  }
  updateLEDs();
  
  // pulsating effects
  pulser.update();
  
  //sharp.update();
}
