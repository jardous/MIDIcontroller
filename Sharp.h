// SHARP sensor
int i;
float val;
int prev_val=0;
int read_pin=A0;
int threshold = 20;


class Sharp {
public:
  void update(void) {
      // Sharp distance sensor
      i = analogRead(SHARP_PIN);
      //Serial.println(i);
      if (i > 500) i=500;
      if (i < 100) i=100;
      //Serial.println(i);

      if (abs(prev_val - i) > threshold) {
        prev_val = i;
        val = (i - 100) / 400.0;
        i = int(val * 127);
        Serial.print("val:");
        Serial.println(i);
        
        sendMIDI(WAH_CC, i, DEFAULT_MIDI_CHANNEL);
      }
    }
};

Sharp sharp = Sharp(); 