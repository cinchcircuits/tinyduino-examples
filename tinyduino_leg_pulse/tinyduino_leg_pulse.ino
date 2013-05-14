// @tamberg, 14.04.2013

// Released into the public domain.

// TinyDuino + Charlieplexed 16 LED shield + Proto2 shield with LDR 
// (e.g. http://www.adafruit.com/products/161) connected to pin A3 and GND
// (based on http://tiny-circuits.com/wp-content/uploads/2013/03/TinyShield_LED_Demo.ino)


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


boolean falling = false; // used to keep track of waveform direction

// PINS

int PulseSensor = 5;  // Pulse Sensor purple wire connected to analog pin 5


void setLedRaw (int px, int wx, int py, int wy, 
  int m5, int m6, int m7, int  m8, int m9) 
{
  digitalWrite(px, wx);
  digitalWrite(py, wy);
  pinMode(5, m5);
  pinMode(6, m6);
  pinMode(7, m7);
  pinMode(8, m8);
  pinMode(9, m9);      
}

void setLed (int i) {
  switch (i) {
    case 1: setLedRaw(5, HIGH, 6, LOW, OUTPUT, OUTPUT, INPUT, INPUT, INPUT); break;
    case 2: setLedRaw(5, LOW, 6, HIGH, OUTPUT, OUTPUT, INPUT, INPUT, INPUT); break;
    case 3: setLedRaw(5, HIGH, 7, LOW, OUTPUT, INPUT, OUTPUT, INPUT, INPUT); break;
    case 4: setLedRaw(7, HIGH, 5, LOW, OUTPUT, INPUT, OUTPUT, INPUT, INPUT); break;
    case 5: setLedRaw(6, HIGH, 7, LOW, INPUT, OUTPUT, OUTPUT, INPUT, INPUT); break;
    case 6: setLedRaw(7, HIGH, 6, LOW, INPUT, OUTPUT, OUTPUT, INPUT, INPUT); break;
    case 7: setLedRaw(6, HIGH, 8, LOW, INPUT, OUTPUT, INPUT, OUTPUT, INPUT); break;
    case 8: setLedRaw(8, HIGH, 6, LOW, INPUT, OUTPUT, INPUT, OUTPUT, INPUT); break;
    case 9: setLedRaw(5, HIGH, 8, LOW, OUTPUT, INPUT, INPUT, OUTPUT, INPUT); break;
    case 10: setLedRaw(8, HIGH, 5, LOW, OUTPUT, INPUT, INPUT, OUTPUT, INPUT); break;
    case 11: setLedRaw(8, HIGH, 7, LOW, INPUT, INPUT, OUTPUT, OUTPUT, INPUT); break;
    case 12: setLedRaw(7, HIGH, 8, LOW, INPUT, INPUT, OUTPUT, OUTPUT, INPUT); break;
    case 13: setLedRaw(9, HIGH, 7, LOW, INPUT, INPUT, OUTPUT, INPUT, OUTPUT); break;
    case 14: setLedRaw(7, HIGH, 9, LOW, INPUT, INPUT, OUTPUT, INPUT, OUTPUT); break;
    case 15: setLedRaw(9, HIGH, 8, LOW, INPUT, INPUT, INPUT, OUTPUT, OUTPUT); break;
    case 16: setLedRaw(8, HIGH, 9, LOW, INPUT, INPUT, INPUT, OUTPUT, OUTPUT); break;
    default: setLedRaw(5, LOW, 6, LOW, OUTPUT, OUTPUT, INPUT, INPUT, INPUT); // off
  }
}

void setLedGauge (int value) {
 
  
    for (int i = 1; i <= value; i++) {
      setLed(i);
      delay(60);
    }
    
  for (int i = value; i >= 0; i--) {
      setLed(i);
      delay(60);
    }
    
  
}


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

void setup () {
  Serial.begin(9600);
  
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  lastTime = millis();  
  
}


void loop()
{
  Sensor = analogRead(PulseSensor);      // take a reading
    //Serial.print("s");          // send raw analog data to Processing sketch (or other)
    //Serial.println(Sensor);     // 's' = Raw Sensor Data
//  USE WITH LED ON PIN 11 FOR FADE EFFECT
    Fade -= fadeRate;              // Fade variable set to 255 when heart beat is found
    Fade = constrain(Fade,0,255);  // these lines fade the LED

    
// KEEP TRACK OF THE DIRECTION OF THE WAVEFORM
    if (falling == false){                       // if the sensor values are rising 
      if (Sensor < lastSensor-1){                  // if current reading is less than last reading - noise      
         falling = true;                         // a peak has been reached
         Serial.print("P");                      // send peak value to Processing scketch (or other)
         Serial.println(Peak);                   // 'P' = Peak in waveform     
        
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
          
          Fade = 255;                            // set fading LED to high brightness
         
        
         
          
        }                 
       }else if (Sensor < lastSensor){           // otherwise, if current reading is smaller weʻre still falling 
         Trough = Sensor;                        // record the next potential trough      
         lastSensor = Sensor;                    // keep track of falling signal
       }   
      }
       int darknessValue = map(QuantifiedSelf, 45, 120, 0, 16);
       setLedGauge(darknessValue);
      delay(10);  // break for 20mS. Processing frame-rate = 100.
      setLedGauge(0);
      delay(10);
    
}// END VOID LOOP
