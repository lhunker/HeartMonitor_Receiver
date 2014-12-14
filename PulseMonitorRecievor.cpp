#include "Energia.h"


void setup();
void loop();


#include <Enrf24.h>
#include <nRF24L01.h>
#include <SPI.h>

Enrf24 radio(P2_0, P2_1, P2_2);
const uint8_t rxaddr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x01 };

void setup() {
	Serial.begin(9600);

	SPI.begin();
	SPI.setDataMode(SPI_MODE0);
	SPI.setBitOrder(MSBFIRST);

	radio.begin();  // Defaults 1Mbps, channel 0, max TX power

	radio.setRXaddress((void*)rxaddr);

	pinMode(P1_0, OUTPUT);
	digitalWrite(P1_0, LOW);

	radio.enableRX();  // Start listening
	Serial.println("Started Up");
}

void loop() {
	char inbuf[33];


	  while (!radio.available(true))
	    ;
	  if (radio.read(inbuf)) {
	    Serial.print("Received packet: ");
	    Serial.println(inbuf);

	  }
}


