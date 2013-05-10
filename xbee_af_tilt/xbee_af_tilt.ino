/*
  Xbee1
  D. Thiebaut
 
  Makes Arduino send 1 character via XBee wireless to another XBee connected
  to a computer via a USB cable. 
 
  The circuit: 
  * RX is digital pin 8 (connect to TX of XBee)
  * TX is digital pin 9 (connect to RX of XBee)
 
  Based on a sketch created back in the mists of time by Tom Igoe
  itself based on Mikal Hart's example
 
*/
 
#include <SoftwareSerial.h>

SoftwareSerial xbee(8, 9); // RX, TX

//For the RPI-1031 - http://www.sparkfun.com/products/10621 
int tilt_s1 = 6;
int tilt_s2 = 7;

void setup()  {
   Serial.begin(9600);
   pinMode(tilt_s1, INPUT);
   pinMode(tilt_s2, INPUT);
 
   // set the data rate for the SoftwareSerial port
   xbee.begin( 9600 );
}
 
void loop()  {
  // send character via XBee to other XBee connected to Mac
  // via USB cable
  int position = getTiltPos();
  if(position == 0 ){
    return;
  } else if ( position == 1 || position == 2 )
  {
      xbee.print("Fell OVER");
  } else{
    xbee.print("On my head");
  }
 
  xbee.print('\0');
  delay( 1000 );
}
int getTiltPos(){
   int s1 = digitalRead(tilt_s1);
   int s2 = digitalRead(tilt_s2);
   return (s1 << 1) | s2; //bitwise math to combine the values
}
