/*******************************************************************************
 * Copytight (c) 2016 Maarten Westenberg based on work of
 * Thomas Telkamp and Matthijs Kooijman porting the LMIC stack to Arduino IDE
 * and Gerben den Hartog for his tiny stack implementation with the AES library
 * that we used in the LMIC stack.
 *
 * Permission is hereby granted, free of charge, to anyone
 * obtaining a copy of this document and accompanying files,
 * to do whatever they want with them without any restriction,
 * including, but not limited to, copying, modification and redistribution.
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 *
 * This example sends a valid LoRaWAN packet with sensor values read.
 * If no sensor is connected the payload is '{"Hello":"World"}', that
 * will be processed by The Things Network server.
 *
 * Note: LoRaWAN per sub-band duty-cycle limitation is enforced (1% in g1,
 *  0.1% in g2).
 *
 * Change DevAddr to a unique address for your node
 * See http://thethingsnetwork.org/wiki/AddressSpace
 *
 * Do not forget to define the radio type correctly in config.h, default is:
 *   #define CFG_sx1272_radio 1
 * for SX1272 and RFM92, but change to:
 *   #define CFG_sx1276_radio 1
 * for SX1276 and RFM95.
 *
 *******************************************************************************/


// FUTURE: USE https://github.com/adafruit/TinyLoRa

#define WAIT_SECS 120


#include <avr/pgmspace.h>
#include <Arduino.h>
#include "lmic.h"
#include "hal/hal.h"
#include <SPI.h>
//#include <SI7021.h>

//---------------------------------------------------------
// Sensor declarations
//---------------------------------------------------------
//SI7021 envSensor;

// Frame Counter
int count=0;

// LoRaWAN Application identifier (AppEUI)
// Not used in this example
static const u1_t APPEUI[8] PROGMEM = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x00, 0xBB, 0x24 };

// LoRaWAN DevEUI, unique device ID (LSBF)
// Not used in this example
static const u1_t DEVEUI[8] PROGMEM  = { 0x98, 0x76, 0xB6, 0x10, 0x98, 0x5E, 0x00, 0x01 };

// LoRaWAN NwkSKey, network session key
// Use this key for The Things Network
unsigned char NwkSkey[16] = 		{ 0x8D, 0x50, 0x14, 0xF7, 0x57, 0xED, 0x8C, 0x89, 0xCB, 0xBD, 0x7D, 0xEF, 0xD5, 0xC4, 0x58, 0xD3 };

// LoRaWAN AppSKey, application session key
// Use this key to get your data decrypted by The Things Network
unsigned char AppSkey[16] =		{ 0x2F, 0xB5, 0x40, 0xBD, 0xDF, 0xD4, 0x9B, 0x23, 0x2F, 0x96, 0xCA, 0x5E, 0xDB, 0xA5, 0x54, 0x95 };

// LoRaWAN end-device address (DevAddr)
// See http://thethingsnetwork.org/wiki/AddressSpace

#define msbf4_read(p)   (u4_t)((u4_t)(p)[0]<<24 | (u4_t)(p)[1]<<16 | (p)[2]<<8 | (p)[3])
unsigned char DevAddr[4] = { 0x26, 0x01, 0x17, 0x6A };


// ----------------------------------------------------------------------------
// APPLICATION CALLBACKS
// ----------------------------------------------------------------------------


// provide application router ID (8 bytes, LSBF)
void os_getArtEui (u1_t* buf) {
    memcpy(buf, APPEUI, 8);
}

// provide device ID (8 bytes, LSBF)
void os_getDevEui (u1_t* buf) {
    memcpy(buf, DEVEUI, 8);
}

// provide device key (16 bytes)
void os_getDevKey (u1_t* buf) {
    memcpy(buf, NwkSkey, 16);
}

int debug=1;
//uint8_t mydata[64];

static uint8_t mydata[] = "Hello, world!";
static osjob_t sendjob;

// Pin mapping
// These settings should be set to the GPIO pins of the device
// you want to run the LMIC stack on.
//
#define LMIC_UNUSED_PIN 0

lmic_pinmap pins = {
  .nss = 8,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = LMIC_UNUSED_PIN,
  .dio = {7, 9, LMIC_UNUSED_PIN},
};

void onEvent (ev_t ev) {
    //debug_event(ev);

    switch(ev) {
      // scheduled data sent (optionally data received)
      // note: this includes the receive window!
      case EV_TXCOMPLETE:
          // use this event to keep track of actual transmissions
          Serial.print("EV_TXCOMPLETE, time: ");
          Serial.println(millis() / 1000);
          if(LMIC.dataLen) { // data received in rx slot after tx
              //debug_buf(LMIC.frame+LMIC.dataBeg, LMIC.dataLen);
              Serial.println("Data Received");
          }
          break;
       default:
          break;
    }
}



void do_send(osjob_t* j){
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println(F("OP_TXRXPEND, not sending"));
    } else {
        // Prepare upstream data transmission at the next possible time.
        #define VBATPIN 9
        // Read senser values
        float vbat = analogRead(VBATPIN);
        vbat *= 2;    // we divided by 2, so multiply back
        vbat *= 3.3;  // Multiply by 3.3V, our reference voltage
        vbat /= 1024; // convert to voltage
        vbat = 1.0*(rand()%100) / 100;
        int temp = 23; //envSensor.getCelsiusHundredths()/10;
        int humid = 0.80; //envSensor.getHumidityPercent();
        //Serial.print("VBat        : " ); Serial.println(vbat);
        //Serial.print("Temperature : " ); Serial.println(temp);
        //Serial.print("Humidity: " ); Serial.println(humid);
        float data = vbat + temp*100 + humid*100000;
        Serial.print("Code: " ); Serial.println(data);


        LMIC_setTxData2(1, mydata, sizeof(mydata)-1, 0);
        Serial.println(F("Packet queued"));
        // Concert Message
        /*byte buffer[4];
        dtostrf(data, 1, 2, buffer);
        String res = buffer;
        res.getBytes(buffer, res.length() + 1);
        LMIC_setTxData2(1, (uint8_t*) buffer, res.length(), 0);*/
    }

    // Next TX is scheduled after TX_COMPLETE event.

}

// ====================================================================
// SETUP
//
void setup() {
  Serial.begin(9600);
  while ( ! Serial ) {
      delay( 10 );
  }

  // Init Sensor
  /*if (!envSensor.begin()) {
    while (true);
  }*/

  // LMIC init
  os_init();
  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();
  // Set static session parameters. Instead of dynamically establishing a session
  // by joining the network, precomputed session parameters are be provided.
  LMIC_setSession (0x1, msbf4_read(DevAddr), (uint8_t*)NwkSkey, (uint8_t*)AppSkey);
  // Disable data rate adaptation
  LMIC_setAdrMode(0);
  // Disable link check validation
  LMIC_setLinkCheckMode(0);
  // Disable beacon tracking
  LMIC_disableTracking ();
  // Stop listening for downstream data (periodical reception)
  LMIC_stopPingable();
  // Set data rate and transmit power (note: txpow seems to be ignored by the library)
  LMIC_setDrTxpow(DR_SF7,14);
  //

	Serial.println("Ready");

}

// ================================================================
// LOOP
//
void loop() {
	os_runloop_once();
}
