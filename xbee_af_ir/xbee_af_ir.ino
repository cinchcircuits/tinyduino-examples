#include <DHT.h>

/*
  Xbee1
  D. Thiebaut
 
  Makes Arduino send 1 character via XBee wireless to another XBee connected
  to a computer via a USB cable. 
 
  The circuit: 
  * RX is digital pin 2 (connect to TX of XBee)
  * TX is digital pin 3 (connect to RX of XBee)
 
  Based on a sketch created back in the mists of time by Tom Igoe
  itself based on Mikal Hart's example
  
 
*/
 
#include <SoftwareSerial.h>
 
SoftwareSerial xbee(5, 6); // RX, TX
#define DHTPIN 7     // what pin we're connected to

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11 
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);
 
void setup()  {
   Serial.begin(9600);
  
   //Serial.println( "Arduino started sending bytes via XBee" );
 
   // set the data rate for the SoftwareSerial port
   xbee.begin( 9600 );
}
 
void loop()  {
    // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (!isnan(t) && !isnan(h)) {
    
    xbee.print("H:"); 
    xbee.print(h);
    xbee.print("% ");
    xbee.print("T:"); 
    xbee.print(int(t));
    xbee.print("C");
    xbee.print('\0');
  }

  
  delay( 2000 );
}


