#include <SoftwareSerial.h>

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "BluefruitConfig.h"
#define __BLE


SoftwareSerial bluefruitSS = SoftwareSerial(BLUEFRUIT_SWUART_TXD_PIN, BLUEFRUIT_SWUART_RXD_PIN);
Adafruit_BluefruitLE_UART ble(bluefruitSS, BLUEFRUIT_UART_MODE_PIN,
                      BLUEFRUIT_UART_CTS_PIN, BLUEFRUIT_UART_RTS_PIN);
// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
}

class BLEModule {
public:
  BLEModule::BLEModule() {}
  void initialize(void) {
    
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
    ble.sendCommandCheckOK(F("AT+GATTLIST"));
      
    ble.sendCommandCheckOK(F("AT+GATTADDSERVICE=UUID128=03-B8-0E-5A-ED-E8-4B-33-A7-51-6C-E3-4E-C4-C7-00"));
    ble.sendCommandCheckOK(F("AT+GATTADDCHAR=UUID128=77-72-E5-DB-38-68-41-12-A1-A9-F2-66-9D-10-6B-F3,PROPERTIES=0x96,MIN_LEN=1,MAX_LEN=20"));
    ble.sendCommandCheckOK(F("AT+GAPINTERVALS=8,15,250,180"));
    ble.sendCommandCheckOK(F("AT+GAPSETADVDATA=02-01-06-11-06-00-C7-C4-4E-E3-6C-51-A7-33-4B-E8-ED-5A-0E-B8-03"));
    ble.sendCommandCheckOK(F("AT+BLEPOWERLEVEL=4"));
    
    ble.reset();

    ble.verbose(false);  // debug info is a little annoying after this point!

  }


  /*
   * Set the new characteristic value
   * format: AT+GATTCHAR=1,AF-80-XX-YY-ZZ
   * where XX is the command, YY is note and ZZ is velocity
  */
  void BLESendMIDI(unsigned char command, unsigned char param1, unsigned char param2) {
    if (ble.isConnected() != 0) {
      static char *buf = "AT+GATTCHAR=1,AF-80-00-00-00";
      sprintf(buf+20, "%X-%02X-%02X", command, param1, param2);
      ble.sendCommandCheckOK(buf);
      Serial.println(buf);
    }
  }
};

BLEModule bluetooth = BLEModule();
