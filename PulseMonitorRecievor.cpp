#include "Energia.h"

String dump_radio_status_to_serialport(uint8_t);
void setup();
void loop();


#include <Enrf24.h>
#include <nRF24L01.h>
#include <SPI.h>

//TODO Set these params
#define pktdiff 10000
#define errorlow 20
#define errorhigh 200
#define alarmlevel 90
#define errorLED P1_6
#define statusLED P1_7
#define alarmLED P2_5
#define buzzer P2_4

//Globals
volatile long previouspkt = 0;

Enrf24 radio(P2_1, P2_0, P1_3);
//Enrf24 radio(P2_0, P2_1, P2_2);
const uint8_t rxaddr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x01 };

void setup() {
//	Serial.begin(9600);

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
//	Serial.println("Started Up");
}

void loop() {
	char inbuf[33];

	  dump_radio_status_to_serialport(radio.radioState());
	  while (!radio.available(true)){
	  	long currenttime = millis();
	  	if (currenttime - previouspkt >= pktdiff){
	  		digitalWrite(errorLED, HIGH);
	  	}
	  }
//	  Serial.println(inbuf);
	  previouspkt = millis();
	  if (radio.read(inbuf)) {
//	    Serial.print("Received packet: ");
//	    Serial.println(inbuf);
	    int bpm = (int)inbuf[0] -'0';
	    bpm *= 10;
	    bpm += (int)inbuf[1] - '0';
	    if (inbuf[2]!= 0){
	    	bpm *= 10;
	    	bpm += (int)inbuf[2] - '0';
	    }

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

String dump_radio_status_to_serialport(uint8_t status)
{
//	Serial.print("Enrf24 radio transceiver status: ");
	switch (status) {
	case ENRF24_STATE_NOTPRESENT:
		return "NO TRANSCEIVER PRESENT";
		break;

	case ENRF24_STATE_DEEPSLEEP:
		return "DEEP SLEEP <1uA power consumption";
		break;

	case ENRF24_STATE_IDLE:
		return "IDLE module powered up w/ oscillators running";
		break;

	case ENRF24_STATE_PTX:
		return "Actively Transmitting";
		break;

	case ENRF24_STATE_PRX:
		return "Receive Mode";
		break;

	default:
		return "UNKNOWN STATUS CODE";
	}
}


