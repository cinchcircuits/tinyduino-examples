#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>

// The shield uses the I2C SCL and SDA pins. On classic Arduinos
// this is Analog 4 and 5 so you can't use those for analogRead() anymore
// However, you can connect other I2C sensors to the I2C bus and share
// the I2C bus.
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// These #defines make it easy to set the backlight color
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

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
 
//#include <SoftwareSerial.h>
 
//SoftwareSerial xbee(2, 3); // RX, TX
char c = 'A';
int  pingPong = 1;
 
void setup()  {
   Serial.begin(9600);
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);

  // Print a message to the LCD. We track how long it takes since
  // this library has been optimized a bit and we're proud of it :)
  int time = millis();
  lcd.print("XBee Receiver");
  lcd.setBacklight(WHITE);
}
 
byte reset = 0;
void loop()  {
  if ( reset == 1 )
  {
    delay(500);
    lcd.clear();
      lcd.setCursor(0, 0);
     lcd.print("XBee Receiver");
     lcd.setCursor(0, 1);
    reset = 0;
    while( !Serial.available()) delay(10);
    
  }
  if (Serial.available()) {
    char temp= Serial.read();
    if (temp == '\0') reset = 1;
    else lcd.print(temp);
    
  }
  
}
