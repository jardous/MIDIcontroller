
#include <MIDI.h>
#include <midi_Defs.h>
#include <midi_Message.h>
#include <midi_Namespace.h>
#include <midi_Settings.h>

// Include the Bounce2 library found here:
// https://github.com/thomasfredericks/Bounce-Arduino-Wiring

#include <Bounce2.h>
// 1st row
#define BUTTON_1x1 2
#define BUTTON_1x2 3
#define BUTTON_1x3 4
#define BUTTON_1x4 5
// 2nd row
#define BUTTON_2x1 6
#define BUTTON_2x2 7
#define BUTTON_2x3 8
// 3rd row
#define BUTTON_3x1 9
#define BUTTON_3x2 10

#define LED_PIN 13
#define MIDI_CHANNEL 1
#define INTERVAL 10

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

// Instantiate a Bounce object
Bounce debouncer1x1 = Bounce();
Bounce debouncer1x2 = Bounce();
Bounce debouncer1x3 = Bounce();
Bounce debouncer1x4 = Bounce();

Bounce debouncer2x1 = Bounce();
Bounce debouncer2x2 = Bounce();
Bounce debouncer2x3 = Bounce();

Bounce debouncer3x1 = Bounce();
Bounce debouncer3x2 = Bounce();

void setup() {

  // Setup the button with an internal pull-up :
  pinMode(BUTTON_1x1, INPUT_PULLUP);
  pinMode(BUTTON_1x2, INPUT_PULLUP);
  pinMode(BUTTON_1x3, INPUT_PULLUP);
  pinMode(BUTTON_1x4, INPUT_PULLUP);

  pinMode(BUTTON_2x1, INPUT_PULLUP);
  pinMode(BUTTON_2x2, INPUT_PULLUP);
  pinMode(BUTTON_2x3, INPUT_PULLUP);

  pinMode(BUTTON_3x1, INPUT_PULLUP);
  pinMode(BUTTON_3x2, INPUT_PULLUP);

  // After setting up the button, setup the Bounce instance :
  debouncer1x1.attach(BUTTON_1x1);
  debouncer1x2.attach(BUTTON_1x2);
  debouncer1x3.attach(BUTTON_1x3);
  debouncer1x4.attach(BUTTON_1x4);

  debouncer2x1.attach(BUTTON_2x1);
  debouncer2x2.attach(BUTTON_2x2);
  debouncer2x3.attach(BUTTON_2x3);

  debouncer3x1.attach(BUTTON_3x1);
  debouncer3x2.attach(BUTTON_3x2);

  debouncer1x1.interval(INTERVAL); // interval in ms
  debouncer1x2.interval(INTERVAL);
  debouncer1x3.interval(INTERVAL);
  debouncer1x4.interval(INTERVAL);

  debouncer2x1.interval(INTERVAL);
  debouncer2x2.interval(INTERVAL);
  debouncer2x3.interval(INTERVAL);

  debouncer3x1.interval(INTERVAL);
  debouncer3x2.interval(INTERVAL);

  //Setup the LED
  pinMode(LED_PIN, OUTPUT);

  MIDI.begin();
}

void loop() {
/*
JamUp
4  Wah depth
7 Master volume
10  Previous preset
11  Next preset
15  Toggle metronome
20  Toggle selected effect
21  Toggle noisegate effect
22  Toggle stomp effect
23  Toggle filter effect
24  Toggle modulation effect
25  Toggle delay effect
26  Toggle reverb effect
27  Toggle amplifier
30  Toggle jam player
31  Jam player increase speed
32  Jam player decrease speed
33  Jam player increase pitch
34  Jam player decrease pitch
35  Jam player increase volume
36  Jam player decrease volume
40  Sampler toggle record
41  Sampler undo
42  Sampler toggle play
43  Sampler toggle dub
64  Tap tempo
  */
  // Update the Bounce instance
  debouncer1x1.update();
  debouncer1x2.update();
  debouncer1x3.update();
  debouncer1x4.update();

  debouncer2x1.update();
  debouncer2x2.update();
  debouncer2x3.update();

  debouncer3x1.update();
  debouncer3x2.update();

  if (debouncer1x1.fell()) {  // PRESET A
    MIDI.sendControlChange(1, 0, MIDI_CHANNEL);
  }
  if (debouncer1x2.fell()) {  // PRESET B
    MIDI.sendControlChange(2, 0, MIDI_CHANNEL);
  }
  if (debouncer1x3.fell()) {  // PRESET C
    MIDI.sendControlChange(3, 0, MIDI_CHANNEL);
  }
  if (debouncer1x4.fell()) {  // PRESET C
    MIDI.sendControlChange(4, 0, MIDI_CHANNEL);
  }


  if (debouncer2x1.fell()) { // STOMP
    MIDI.sendControlChange(22, 0, MIDI_CHANNEL);
  }
  if (debouncer2x2.fell()) {  // DELAY
    MIDI.sendControlChange(25, 0, MIDI_CHANNEL);
  }
  if (debouncer2x3.fell()) {  // MOD
    MIDI.sendControlChange(24, 0, MIDI_CHANNEL);
  }
}
