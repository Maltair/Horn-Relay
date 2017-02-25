

// SimpleTx - the master or the transmitter

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Bounce2.h>

//----- Create a Radio
  #define CE_PIN   9
  #define CSN_PIN 10
  const byte slaveAddress[5] = {'R','x','A','A','A'};
  RF24 radio(CE_PIN, CSN_PIN); 

//-------Constants
  char dataToSend[10] = "Message 0";
  char txNum = '0';
  

//----Timer Constants---
  unsigned long currentMillis;
  unsigned long prevMillis;
  unsigned long txIntervalMillis = 1000; // send once per second

//------Buttons and switches
  #define hornButton 2
  #define switchCheck 3


//------- Instantiate a Bounce object
    Bounce bHornButton = Bounce();
    Bounce bSwitchCheck = Bounce ();

//===========================
void setup() {
//-----Buttons and Swithes
    pinMode(hornButton, INPUT_PULLUP);
    pinMode(switchCheck, INPUT_PULLUP);

//-----After setting up the button, setup the Bounce instance
    bHornButton.attach(hornButton);
    bHornButton.interval(5);  // interval in ms
    bSwitchCheck.attach(switchCheck);
    bSwitchCheck.interval(5);

//-----Serial
    Serial.begin(9600);
    Serial.println("SimpleTx Starting");
    
//----Radio
    radio.begin();
    radio.setDataRate( RF24_250KBPS );
    radio.setRetries(3,5); // delay, count
    radio.openWritingPipe(slaveAddress);
}
//====================

void loop() {
// Update the Bounce instance
    bHornButton.update();
    bSwitchCheck.update();

// Get the updated value
    int x1 = bHornButton.read();
    int x2 = bSwitchCheck.read();

//--Set conditions for testing
  if ( x1 == LOW) {
     dataToSend[8] = 'H';
  }
  if (x2 == LOW) { 
      dataToSend[8] = 'S';
  }

timer ();

}
void timer(){
  currentMillis = millis();
    if (currentMillis - prevMillis >= txIntervalMillis) {
        send();
        prevMillis = millis();
    }
}

//====================

void send() {

// some implementations automatically shut down the radio after a transmission: this ensures the radio is powered up before sending data
radio.powerUp();


    bool rslt;
    rslt = radio.write( &dataToSend, sizeof(dataToSend) );
        // Always use sizeof() as it gives the size as the number of bytes.
        // For example if dataToSend was an int sizeof() would correctly return 2

    Serial.print("Data Sent ");
    Serial.print(dataToSend);
    if (rslt) {
        Serial.println("  Acknowledge received");
        updateMessage();
    }
    else {
        Serial.println("  Tx failed");
    }
}

//================

void updateMessage() {
        // so you can see that new data is being sent
    txNum += 1;
    if (txNum > '9' == 'H' == 'S') {
        txNum = '0';
    }
    
    
    dataToSend[8] = txNum;
}

