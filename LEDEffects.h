
class Pulser {
public:
  Pulser() : m_brightness(0), m_inc(1), tstamp(millis()), tnext(millis()) {
    
  }
  void update(void) {
    if (tnext > millis()) return;
    
    if ((millis() - tstamp) > 20) {

      m_brightness += m_inc;
  
      if (m_brightness == 20) {
        m_inc = -1;
      } else if (m_brightness == 0) {
        m_inc = 1;
        tnext = millis() + 2000;
      }
      tstamp = millis();
      updateLEDs();
    }
  }
  int m_brightness;
  int m_inc;
  long tstamp;
  long tnext;
};

Pulser pulser;