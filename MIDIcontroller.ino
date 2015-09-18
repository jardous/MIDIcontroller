
#include <MIDI.h>
#include <midi_Defs.h>
#include <midi_Message.h>
#include <midi_Namespace.h>
#include <midi_Settings.h>

// Include the Bounce2 library found here:
// https://github.com/thomasfredericks/Bounce-Arduino-Wiring

#include <Bounce2.h>

#define LED_PIN 13
#define DEFAULT_MIDI_CHANNEL 1
#define BOUNCE_INTERVAL 10  // [ms]

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

// the controller board is 4x3 buttons
#define BOARD_ROWS 4
#define BOARD_COLS 3

Bounce *buttons[BOARD_ROWS * BOARD_COLS] = {NULL,};

class Button : public Bounce
{

public:

  // constructor
  Button(uint8_t pin, uint8_t cc, uint8_t value=0, uint8_t channel=DEFAULT_MIDI_CHANNEL)
                                    : m_cc(cc)
                                    , m_value(value)
                                    , m_channel(DEFAULT_MIDI_CHANNEL)

  {
    Bounce::pin = pin;
    pinMode(pin, INPUT_PULLUP);
    attach(pin);
    interval(BOUNCE_INTERVAL);
  }

  bool update() {
    Bounce::update();
    if (fell()) {
      MIDI.sendControlChange(m_cc, m_value, m_channel);
    }
  }

private:
  uint8_t m_interval;
  uint8_t m_cc;
  uint8_t m_value;
  uint8_t m_channel;
};




void setup() {

  // add buttons to the array and nstantiate the button objects
  // first row (bottom)
  buttons[0] = new Button(/* pin */2, /* cc */1);  // PRESET A
  buttons[1] = new Button(/* pin */3, /* cc */2);  // PRESET B
  buttons[2] = new Button(/* pin */4, /* cc */3);  // PRESET C
  buttons[3] = new Button(/* pin */5, /* cc */4);  // PRESET D
  // second row
  buttons[4] = new Button(/* pin */6, /* cc */21);  // STOMP 1
  buttons[5] = new Button(/* pin */7, /* cc */22);  // STOMP 2
  buttons[6] = new Button(/* pin */8, /* cc */23);  // STOMP 3
  buttons[7] = NULL;
  // third row (top)
  buttons[8] = new Button(/* pin */9, /* cc */31);  // TUNER
  buttons[9] = new Button(/* pin */10, /* cc */32); // BYPASS
  buttons[10] = NULL;
  buttons[11] = NULL;

  //Setup the LED
  pinMode(LED_PIN, OUTPUT);

  MIDI.begin();
}

void loop() {
  // Update the Bounce instance
  for (int i=0; i<BOARD_ROWS*BOARD_COLS; i++) {
    if (buttons[i] != NULL) {
      buttons[i]->update();
    }
  }
}
