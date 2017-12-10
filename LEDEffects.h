#define BRIGHT_MAX 10
#define BRIGHT_MIN 0

void updateLEDs(void);

class Pulser {
public:
  Pulser() : m_brightness(BRIGHT_MIN), m_inc(1), tstamp(millis()), tnext(millis()) {
    
  }
  void update(void) {
    if (tnext > millis()) return;
    
    if ((millis() - tstamp) > 40) {

      m_brightness += m_inc;
  
      if (m_brightness == BRIGHT_MAX) {
        m_inc = -1;
      } else if (m_brightness == BRIGHT_MIN) {
        m_inc = 1;
        tnext = millis() + 2000;
      }
      tstamp = millis();
      //updateLEDs();
    }
  }
  uint8_t m_brightness;
  uint8_t m_inc;
  long tstamp;
  long tnext;
};

Pulser pulser;
