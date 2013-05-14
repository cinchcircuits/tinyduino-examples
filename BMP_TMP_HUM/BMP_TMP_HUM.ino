
#include <Wire.h>
// HIH_6130_1  - Arduino
// 
// Arduino                HIH-6130
// SCL (Analog 5) ------- SCL (term 3)
// SDA (Analog 4) ------- SDA (term 4)
//
// Note 2.2K pullups to 5 VDC on both SDA and SCL
//
// Pin4 ----------------- Vdd (term 8) 
//
// Illustrates how to measure relative humidity and temperature.
//
// copyright, Peter H Anderson, Baltimore, MD, Nov, '11
// You may use it, but please give credit.  
    
#include <Wire.h> //I2C library

byte fetch_humidity_temperature(unsigned int *p_Humidity, unsigned int *p_Temperature);
char * float2str(float f, int num_digits, char *retbuff, int bufflen);

#define TRUE 1
#define FALSE 0


/* BMP085 Extended Example Code
  by: Jim Lindblom
  SparkFun Electronics
  date: 1/18/11
  license: CC BY-SA v3.0 - http://creativecommons.org/licenses/by-sa/3.0/
  
  Get pressure and temperature from the BMP085 and calculate altitude.
  Serial.print it out at 9600 baud to serial monitor.
*/




//bmp Sensor
#define BMP085_ADDRESS 0x77  // I2C address of BMP085

const unsigned char OSS = 0;  // Oversampling Setting

// Calibration values
int ac1;
int ac2; 
int ac3; 
unsigned int ac4;
unsigned int ac5;
unsigned int ac6;
int b1; 
int b2;
int mb;
int mc;
int md;

// b5 is calculated in bmp085GetTemperature(...), this variable is also used in bmp085GetPressure(...)
// so ...Temperature(...) must be called before ...Pressure(...).
long b5; 

short temperature;
float pressure=101000;
float firstread;
//BMP End

char num_buff[20];

//BMP
void fmtDouble(double val, byte precision, char *buf, unsigned bufLen = 0xffff);
unsigned fmtUnsigned(unsigned long val, char *buf, unsigned bufLen = 0xffff, byte width = 0);

// From http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1207226548/11#11
// Produce a formatted string in a buffer corresponding to the value provided.
// If the 'width' parameter is non-zero, the value will be padded with leading
// zeroes to achieve the specified width.  The number of characters added to
// the buffer (not including the null termination) is returned.
//
unsigned
fmtUnsigned(unsigned long val, char *buf, unsigned bufLen, byte width)
{
  if (!buf || !bufLen)
    return(0);

  // produce the digit string (backwards in the digit buffer)
  char dbuf[10];
  unsigned idx = 0;
  while (idx < sizeof(dbuf))
  {
    dbuf[idx++] = (val % 10) + '0';
    if ((val /= 10) == 0)
      break;
  }

  // copy the optional leading zeroes and digits to the target buffer
  unsigned len = 0;
  byte padding = (width > idx) ? width - idx : 0;
  char c = '0';
  while ((--bufLen > 0) && (idx || padding))
  {
    if (padding)
      padding--;
    else
      c = dbuf[--idx];
    *buf++ = c;
    len++;
  }

  // add the null termination
  *buf = '\0';
  return(len);
}

// From http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1207226548/11#11
// Format a floating point value with number of decimal places.
// The 'precision' parameter is a number from 0 to 6 indicating the desired decimal places.
// The 'buf' parameter points to a buffer to receive the formatted string.  This must be
// sufficiently large to contain the resulting string.  The buffer's length may be
// optionally specified.  If it is given, the maximum length of the generated string
// will be one less than the specified value.
//
// example: fmtDouble(3.1415, 2, buf); // produces 3.14 (two decimal places)
//
void
fmtDouble(double val, byte precision, char *buf, unsigned bufLen)
{
  if (!buf || !bufLen)
    return;

  // limit the precision to the maximum allowed value
  const byte maxPrecision = 6;
  if (precision > maxPrecision)
    precision = maxPrecision;

  if (--bufLen > 0)
  {
    // check for a negative value
    if (val < 0.0)
    {
      Serial.println("neg");
      val = -val;
      *buf = '-';
      bufLen--;
    }

    // compute the rounding factor and fractional multiplier
    double roundingFactor = 0.5;
    unsigned long mult = 1;
    for (byte i = 0; i < precision; i++)
    {
      roundingFactor /= 10.0;
      mult *= 10;
    }

    if (bufLen > 0)
    {
      // apply the rounding factor
      val += roundingFactor;

      // add the integral portion to the buffer
      unsigned len = fmtUnsigned((unsigned long)val, buf, bufLen);
      buf += len;
      bufLen -= len;
    }

    // handle the fractional portion
    if ((precision > 0) && (bufLen > 0))
    {
      *buf++ = '.';
      if (--bufLen > 0)
        buf += fmtUnsigned((unsigned long)((val - (unsigned long)val) * mult), buf, bufLen, precision);
    }
  }

  // null-terminate the string
  *buf = '\0';
} 


// Stores all of the bmp085's calibration values into global variables
// Calibration values are required to calculate temp and pressure
// This function should be called at the beginning of the program
void bmp085Calibration()
{
  Serial.println("calibration start");
  ac1 = bmp085ReadInt(0xAA);
  //Serial.println("ac1");
  ac2 = bmp085ReadInt(0xAC);
  //Serial.println("ac2");
  ac3 = bmp085ReadInt(0xAE);
  //Serial.println("ac3");
  ac4 = bmp085ReadInt(0xB0);
  //Serial.println("ac4");
  ac5 = bmp085ReadInt(0xB2);
  //Serial.println("ac5");
  ac6 = bmp085ReadInt(0xB4);
  //Serial.println("ac6");
  b1 = bmp085ReadInt(0xB6);
  //Serial.println("b1");
  b2 = bmp085ReadInt(0xB8);
  //Serial.println("b2");
  mb = bmp085ReadInt(0xBA);
  //Serial.println("mb");
  mc = bmp085ReadInt(0xBC);
  //Serial.println("mc");
  md = bmp085ReadInt(0xBE);
  Serial.println("Calibration end");
}

// Calculate temperature given ut.
// Value returned will be in units of 0.1 deg C
short bmp085GetTemperature(unsigned int ut)
{
  long x1, x2;
  
  x1 = (((long)ut - (long)ac6)*(long)ac5) >> 15;
  x2 = ((long)mc << 11)/(x1 + md);
  b5 = x1 + x2;

  return ((b5 + 8)>>4);  
}

// Calculate pressure given up
// calibration values must be known
// b5 is also required so bmp085GetTemperature(...) must be called first.
// Value returned will be pressure in units of Pa.
long bmp085GetPressure(unsigned long up)
{
  long x1, x2, x3, b3, b6, p;
  unsigned long b4, b7;
  
  b6 = b5 - 4000;
  // Calculate B3
  x1 = (b2 * (b6 * b6)>>12)>>11;
  x2 = (ac2 * b6)>>11;
  x3 = x1 + x2;
  b3 = (((((long)ac1)*4 + x3)<<OSS) + 2)>>2;
  
  // Calculate B4
  x1 = (ac3 * b6)>>13;
  x2 = (b1 * ((b6 * b6)>>12))>>16;
  x3 = ((x1 + x2) + 2)>>2;
  b4 = (ac4 * (unsigned long)(x3 + 32768))>>15;
  
  b7 = ((unsigned long)(up - b3) * (50000>>OSS));
  if (b7 < 0x80000000)
    p = (b7<<1)/b4;
  else
    p = (b7/b4)<<1;
    
  x1 = (p>>8) * (p>>8);
  x1 = (x1 * 3038)>>16;
  x2 = (-7357 * p)>>16;
  p += (x1 + x2 + 3791)>>4;
  
  return p;
}

// Read 1 byte from the BMP085 at 'address'
char bmp085Read(unsigned char address)
{
  unsigned char data;
  
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();
  
  Wire.requestFrom(BMP085_ADDRESS, 1);
  while(!Wire.available())
    ;
    
  return Wire.read();
}

// Read 2 bytes from the BMP085
// First byte will be from 'address'
// Second byte will be from 'address'+1
int bmp085ReadInt(unsigned char address)
{
  unsigned char msb, lsb;
  
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();
  
  Wire.requestFrom(BMP085_ADDRESS, 2);
  while(Wire.available()<2)
    ;
  msb = Wire.read();
  lsb = Wire  .read();
  
  return (int) msb<<8 | lsb;
}

// Read the uncompensated temperature value
unsigned int bmp085ReadUT()
{
  unsigned int ut;
  
  // Write 0x2E into Register 0xF4
  // This requests a temperature reading
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x2E);
  Wire.endTransmission();
  
  // Wait at least 4.5ms
  delay(5);
  
  // Read two bytes from registers 0xF6 and 0xF7
  ut = bmp085ReadInt(0xF6);
  return ut;
}

// Read the uncompensated pressure value
unsigned long bmp085ReadUP()
{
  unsigned char msb, lsb, xlsb;
  unsigned long up = 0;
  
  // Write 0x34+(OSS<<6) into register 0xF4
  // Request a pressure reading w/ oversampling setting
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x34 + (OSS<<6));
  Wire.endTransmission();
  
  // Wait for conversion, delay time dependent on OSS
  delay(2 + (3<<OSS));
  
  // Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF6);
  Wire.endTransmission();
  Wire.requestFrom(BMP085_ADDRESS, 3);
  
  // Wait for data to become available
  while(Wire.available() < 3)
    ;
  msb = Wire.read();
  lsb = Wire.read();
  xlsb = Wire.read();
  
  up = (((unsigned long) msb << 16) | ((unsigned long) lsb << 8) | (unsigned long) xlsb) >> (8-OSS);
  
  return up;
}

// Read the pressure from the device and print it to the screen in feet
float return_pressure_f(float pmeters)
{
  float pressure, powm;
  

  pressure = pow(pmeters, 0.1902632);
  pmeters = 44330.8 - (4946.54 * pressure);
  // now altitude in meters will be stored in pmeters, convert it to feet
  pressure = pmeters * 3.2808399;

  return pressure;

}
//BMP


//hih_6130
byte fetch_humidity_temperature(unsigned int *p_H_dat, unsigned int *p_T_dat)
{
      byte address, Hum_H, Hum_L, Temp_H, Temp_L, _status;
      unsigned int H_dat, T_dat;
      address = 0x27;;
      Wire.beginTransmission(address); 
      Wire.endTransmission();
      delay(100);
      
      Wire.requestFrom((int)address, (int) 4);
      Hum_H = Wire.read();
      Hum_L = Wire.read();
      Temp_H = Wire.read();
      Temp_L = Wire.read();
      Wire.endTransmission();
      
      _status = (Hum_H >> 6) & 0x03;
      Hum_H = Hum_H & 0x3f;
      H_dat = (((unsigned int)Hum_H) << 8) | Hum_L;
      T_dat = (((unsigned int)Temp_H) << 8) | Temp_L;
      T_dat = T_dat / 4;
      *p_H_dat = H_dat;
      *p_T_dat = T_dat;
      return(_status);
}
   
char * float2str(float f, int num_digits, char *retbuff, int bufflen)
{
    int f_int;
    int pows_of_ten[4] = {1, 10, 100, 1000};
    long multiplier, whole, fract, d, n;
    String num = "";
    
    
    f = f - firstread; 
    
    multiplier = pows_of_ten[num_digits];
    if (f < 0.0)
    {
        f = -f;
       // Serial.print("-");
        num = num + '-';;
    }
    whole = (long) f;
    fract = (long) (multiplier * (f - (float)whole));
    
    //Serial.print(whole);
    //Serial.print(".");
    num = num + String(whole) + '.';
    
    for (n=num_digits-1; n>=0; n--) // print each digit with no leading zero suppression
    {
         d = fract / pows_of_ten[n];
         //Serial.print(d);
         num = num + String(d);
         fract = fract % pows_of_ten[n];
    }
    num.toCharArray(retbuff, bufflen);
    //Serial.print("strarr:");
    //Serial.println(retbuff);
    return retbuff;
}  

void setup()
{
  Serial.begin(57600);
  delay(1000);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
  Serial.println("starting wire");
  Wire.begin();
  Serial.println("starting up");
  bmp085Calibration();
  
  
  //hih_6130 setup
   //pinMode(4, OUTPUT);
   //digitalWrite(4, HIGH); // this turns on the HIH3610
   //delay(2000);
   Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>");  // just to be sure things are working 
   
   //settle the sensor
   for(int ii = 0 ; ii < 10; ii++){
     temperature = bmp085GetTemperature(bmp085ReadUT());
     pressure =(0.9 * pressure) + ( .1 * bmp085GetPressure(bmp085ReadUP()));
     delay(500);
   }  
   double tot_init_readings = 0.0;
   for(int ii = 0 ; ii < 10; ii++){
     temperature = bmp085GetTemperature(bmp085ReadUT());
     pressure =(0.9 * pressure) + ( .1 * bmp085GetPressure(bmp085ReadUP()));
     tot_init_readings += pressure;
     delay(500);
   }  
   
   firstread = return_pressure_f((tot_init_readings/10.0));
   firstread -= 4.0;
}


void loop()
{
  temperature = bmp085GetTemperature(bmp085ReadUT());
  //Filter to normalize data a bit.
  pressure =(0.9 * pressure) + ( .1 * (float)bmp085GetPressure(bmp085ReadUP()));
  //pressure = (float)bmp085GetPressure(bmp085ReadUP());
  

  float pmeters = return_pressure_f(pressure);
  //Serial.print("Temperature: ");
  //Serial.print(temperature, DEC);
  //Serial.println(" *0.1 deg C");
  
  
  
  //hih_6130
   byte _status;
   unsigned int H_dat, T_dat;
   float RH, T_C;
  
      _status = fetch_humidity_temperature(&H_dat, &T_dat);
      
      switch(_status)
      {
          case 0:  //Serial.println("Normal.");
                 RH = (float) H_dat * 6.10e-3;
                 T_C = (float) T_dat * 1.007e-2 - 40.0;
                   break;
          //case 1:  Serial.println("Stale Data.");
          //         break;
          //case 2:  Serial.println("In command mode.");
          //         break;
          default: //Serial.println("Diagnostic."); 
                   break; 
      }       
    
      Serial.println("-");
      Serial.print("H:");
      Serial.print(float2str(RH, 1, num_buff, 20));
      Serial.print(" Tmp:");
      Serial.print(float2str(T_C, 2, num_buff, 20));
      Serial.println();
  
      Serial.print("Pressure:");
      Serial.print(float2str(pressure, 2, num_buff, 20));
      Serial.print(" Pa  Feet:");
      Serial.print(float2str(pmeters,2, num_buff, 20));
      Serial.println();
      delay(2000);
  
}

