/*
 * PulseMonitorReciever
 * Author: Lukas Hunker
 * The reciever module for our heartrate sensor
 * Recieves signal over SPI and Detects an abnormality
 * Uses Energia libraries and Enrf24 Library
 * SPI Library modified to use USCI A instead of USCI B
*/


void setup();
void loop();

#include "Energia.h"
#include <Enrf24.h>
#include <nRF24L01.h>
#include <SPI.h>

//Params (these are set for demo purposes)
#define pktdiff 10000
#define errorlow 20
#define errorhigh 200
#define alarmlevel 90
#define errorLED P1_6
#define statusLED P1_7
#define alarmLED P2_5
#define buzzer P2_4

//Globals
volatile long previouspkt = 0;	//time the last packet was recived in miliseconds

Enrf24 radio(P2_1, P2_0, P1_3);
const uint8_t rxaddr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x01 };

void setup() {
	SPI.begin();
	SPI.setDataMode(SPI_MODE0);
	SPI.setBitOrder(MSBFIRST);

	radio.begin();  // Defaults 1Mbps, channel 0, max TX power

	radio.setRXaddress((void*)rxaddr);

	pinMode(statusLED, OUTPUT);
	digitalWrite(statusLED, HIGH);
	pinMode(errorLED, OUTPUT);
	pinMode(alarmLED, OUTPUT);
	pinMode(buzzer, OUTPUT);
	digitalWrite(errorLED, LOW);
	digitalWrite(alarmLED, LOW);
	digitalWrite(buzzer, LOW);

	radio.enableRX();  // Start listening
}

void loop() {
	char inbuf[33];

	  //Wait for a packet
	  while (!radio.available(true)){
	  	long currenttime = millis();
	  	//if packet not recieved recently - error
	  	if (currenttime - previouspkt >= pktdiff){
	  		digitalWrite(errorLED, HIGH);
	  	}
	  }
	  previouspkt = millis();	//set last pkt recieved time

	  //concert input to an int
	  if (radio.read(inbuf)) {
	    int bpm = (int)inbuf[0] -'0';
	    bpm *= 10;
	    bpm += (int)inbuf[1] - '0';
	    if (inbuf[2]!= 0){
	    	bpm *= 10;
	    	bpm += (int)inbuf[2] - '0';
	    }

	    //Set LEDs and Buzzer
	    digitalWrite(errorLED, LOW);
	    if (bpm < errorlow || bpm > errorhigh){
	    	digitalWrite(errorLED, HIGH);
	    	digitalWrite(alarmLED, LOW);
	    } else if (bpm < alarmlevel){
	    	digitalWrite(alarmLED, HIGH);
	        digitalWrite(buzzer, HIGH);
	    }else{
	    	//turn off alarms and error stuffs
	    	digitalWrite(errorLED, LOW);
	    	digitalWrite(alarmLED, LOW);
	    	digitalWrite(buzzer, LOW);
	    }
	  }
}


