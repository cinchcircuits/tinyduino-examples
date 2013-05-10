#include <SD.h>

 // The circuit:
 // * analog sensors on analog ins 0, 1, and 2
 // * SD card attached to SPI bus as follows:
 // ** MOSI - pin 11
 // ** MISO - pin 12
 // ** CLK - pin 13
 // ** CS - pin 10

/*
This program reads data from the Pulse Sensor. 
Serial output is designed to mate with Processing sketch "P_PulseSensor_xx" series
Serial Protocol initiates datastring with coded ascii character, ends each message with carriage return
We named the variable that holds the heart rate (BPM) after the group Quantified Self.
They backed our Kickstarter campaing at the $600 level and having a variable named after them is one of their rewards.
Go Count Yourself!!!  http://quantifiedself.com/

by Joel Murphy & Yury Gitman in Brooklyn, Summer 2011. 
*/

// VARIABLES
unsigned long time;      // Holds current time for pulse rate calculation
unsigned long lastTime;  // Used for calculating time between beats
int Sensor;              // Holds current analog Sensor Reading
int lastSensor;          // Used to find waveform direction
int Peak;                // Holds value of peak in waveform
int Trough;              // Holds value of trough in waveform
int beats[10];           // Array to collect time between beats for calculating BPM
int beatCounter = 0;     // Used to hold position in beats array
int QuantifiedSelf;      // Used to hold the heart rate value (BPM)
int drop;                // Holds the amplitude of waveform

int fadeRate = 10;       // when arduino finds a heartbeat, it will fade an LED on pin 11 (PWM)
int Fade = 0;            // Fade variable will set PWM 

const int chipSelect = 10;
boolean falling = false; // used to keep track of waveform direction

// PINS
int LED = 5;         // pin 13 LED blinks on each pulse
int dimLED = 6;      // LED on pin 11 fades with each pulse
int PulseSensor = 5;  // Pulse Sensor purple wire connected to analog pin 5
#define CS    10
#define MOSI    11
#define MISO    12
#define SCK    13
#define CHPSELECT 10

void setup()
{ 
  pinMode(LED, OUTPUT);        // set the LED pins as outputs
  pinMode(dimLED, OUTPUT);     
  Serial.begin(115200);        // start the hardware serial block and set the baud rate
  lastTime = millis();         // initialize lastTime variable
    Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(CS, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(MISO, INPUT);
  pinMode(SCK, OUTPUT);
  pinMode(CHPSELECT, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
}
void loop()
{
  Sensor = analogRead(PulseSensor);      // take a reading
    //Serial.print("s");          // send raw analog data to Processing sketch (or other)
    //Serial.println(Sensor);     // 's' = Raw Sensor Data
//  USE WITH LED ON PIN 11 FOR FADE EFFECT
    Fade -= fadeRate;              // Fade variable set to 255 when heart beat is found
    Fade = constrain(Fade,0,255);  // these lines fade the LED
    analogWrite(dimLED,Fade);
    
// KEEP TRACK OF THE DIRECTION OF THE WAVEFORM
    if (falling == false){                       // if the sensor values are rising 
      if (Sensor < lastSensor-1){                  // if current reading is less than last reading - noise      
         falling = true;                         // a peak has been reached
         Serial.print("P");                      // send peak value to Processing scketch (or other)
         Serial.println(Peak);                   // 'P' = Peak in waveform     
         digitalWrite(LED,LOW);                  // turn off pin 13 LED
      }else if(Sensor > lastSensor){             // otherwise, if current reading is bigger, values are still rising
         Peak = Sensor;                          // record the next potential peak
         lastSensor = Sensor;                    // keep track of rising signal  
      }
    }
    if (falling == true){                        // if the sensor values are falling
      if (Sensor > lastSensor){                  // if current reading is bigger than last reading
        falling = false;                         // a trough has been reached
        Serial.print("T");                       // send trough value to Processing sketch (or other)
        Serial.println(Trough);                  // 'T' = Trough in waveform
        drop = Peak - Trough;                    // difference = signal amplitude
        Peak = 0;                                // setting Peak to 0 here helps get rid of noise
// THIS IF STATEMENT IS HOW THE HEARTBEAT IS FOUND IN PULSE SENSOR WAVEFORM         
        if (drop > 4 && drop <60){               // ignore noise in signal. adjust as needed
          timeBeat();                            // go work out the BPM
          Serial.print("d");                     // send the amplitude to Processing Sketch (or other)
          Serial.println(drop);                  // 'd' = amplitude of waveform
          digitalWrite(LED,HIGH);           // start pin 13 LED blink
          Fade = 255;                            // set fading LED to high brightness
         
        
          writeSD();
          
        }                 
       }else if (Sensor < lastSensor){           // otherwise, if current reading is smaller weʻre still falling 
         Trough = Sensor;                        // record the next potential trough      
         lastSensor = Sensor;                    // keep track of falling signal
       }   
      }
    delay(20);  // break for 20mS. Processing frame-rate = 100.
    
}// END VOID LOOP




void timeBeat(){
        time = millis();                          // take note of the current time
        beats[beatCounter] = time - lastTime;     // record miliseconds since the last pulse in beats array
        lastTime = time;                          // stay up to date! 
        beatCounter ++;                           // move array pointer to next position in array
        if (beatCounter == 10){                   // if we've taken 10 readings, it's time to derive heart rate
          QuantifiedSelf = getBPM();              // go derive the heart rate
          Serial.print("q");                      // send the heart rate to Processing sketch (or other)
          Serial.println(QuantifiedSelf);         // 'q' = heart rate
          beatCounter = 0; 
        }
}// END OF timeBeat FUNCTION


// This function will return a value for heart rate (Beats Per Minute)
int getBPM(){
    int dummy;                          // used in sorting
    int mean;                           // used in averaging
    boolean done = false;               // clear sorting flag    
// this simple sorting routine will arrange values in the beat array from lowest to highest 
    while(done != true){                
    done = true;
    for (int j=0; j<9; j++){           // simple swap sorts numbers from lowest to highest
      if (beats[j] > beats[j + 1]){     // sorting numbers here
        dummy = beats[j + 1];
        beats [j+1] = beats[j] ;
        beats[j] = dummy;
        done = false;
       }
     }
   }
// this FOR loop selects the longer beat time values to avoid incorrect heart rate readings
   for(int k=1; k<9; k++){              // exclude lowest and highest values from averaging 
    mean += beats[k];                   // add beat times together
   }
   mean /=8;                            // averaging
   mean = 60000/mean;                   // devide 60 seconds by average pulse length
   return mean;                         // return beats per minute
}// END OF getBPM function
void writeSD()
{
// open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
 
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.write("HR: ");
    dataFile.print(QuantifiedSelf);
    dataFile.write("\n");
    dataFile.close();
    // print to the serial port too:
    Serial.print("HR: ");
    Serial.println(QuantifiedSelf);
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  } 
}
