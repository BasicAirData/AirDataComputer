/*  
Temperature Sensor TEST
Reads an Analog Devices TMP36GT9Z temperature sensor on A9
and converts the voltage value to temperature. 

Connect Pin 1 to 3.3V
Connect Pin 2 to A0
Connect Pin 3 to GND  
*/

int TsensorPin = A0;       // select the input pin for the sensor
int ledPin = 13;           // select the pin for the LED
int waiton = 50;            // wait time between measurements
int waitoff = 450;            // wait time between measurements
double temperature = 0.0;

void setup() {
  // declare the ledPin as an OUTPUT:
  pinMode(TsensorPin, INPUT);                       // and set pins to input.
  pinMode(ledPin, OUTPUT);                          // Set Temperature Sensor Pin
  Serial.begin (115200);                            // Set up the Serial Connection.
}

void loop() {
  temperature = TMP36GT_AI_value_to_Celsius(analogRead(TsensorPin)); // read temperature
  
  Serial.print("Temperature = ");  
  Serial.print(temperature, 1);             // write temperature to Serial
  Serial.println(" °C");
  
  digitalWrite(ledPin, HIGH);               // turn the ledPin on
  delay(waiton);                            // stop the program for <waiton> milliseconds:
  digitalWrite(ledPin, LOW);                // turn the ledPin off:        
  delay(waitoff);                           // stop the program for for <waitoff> milliseconds:             
}

double TMP36GT_AI_value_to_Celsius(int AI_value)
{                                               // Convert Analog-input value to temperature
  float Voltage;
  Voltage = AI_value * (3300.0 / 1024);         // Sensor value in mV:
  return ((Voltage - 750) / 10) + 25;           // Temperature according to datasheet: 750 mV @ 25 °C
                                                // 10 mV / °C
}
