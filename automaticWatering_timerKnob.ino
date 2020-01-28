// Arduino code for watering timer with cycle and duration control.

// Duration is the length of time for which water is ON.
// Interval is the length of time for which water is OFF.
// Therefore, Cycle = Duration + Interval

// Set maximums
//const unsigned long cycle_max = 86400000; //For 24 hour cycle
//const unsigned long duration_max = 180000; //Maximum duration
const unsigned long cycle_max = 60000; //For 1 min cycle
const unsigned long duration_max = 10000; //Maximum duration. Set this based on quality of the solenoid valve. Solenoid valves can get headed and damaged if kept ON for long time.

// Initialize variables
unsigned long duration = 0;
unsigned long interval = 0;
const unsigned long default_duration = 1000;
const unsigned long default_interval = 10000;

// Variable resistance will be used to set time interval and duration.
float Vin = 5;             // Input voltage value

// Set variables for cycle
const int sensorPin_cycle = A0;  // Analog input pin that senses Vout
int sensorValue_cycle = 0;       // sensorPin default value
float Rmax_cycle = 10000;        // Variable resistance max ohms
float Vout_cycle = 0;            // Vout default value
float Rref_cycle = 5080;         // Reference resistor's value in ohms (you can give this value in kiloohms or megaohms - the resistance of the tested resistor will be given in the same units)
float R_cycle = 0;               // Tested resistors default value

// Set variables for duration
const int sensorPin_duration = A1;  // Analog input pin that senses Vout
int sensorValue_duration = 0;       // sensorPin default value
float Rmax_duration = 12000;        // Variable resistance max ohms
float Vout_duration = 0;            // Vout default value
float Rref_duration = 5080;         // Reference resistor's value in ohms (you can give this value in kiloohms or megaohms - the resistance of the tested resistor will be given in the same units)
float R_duration = 0;               // Tested resistors default value

// Flags
int waterOnFlag = 0;
int initialFlag = 1;

// To track the time since last event fired
unsigned long previousMillis = 0;

//Pin connected to relay
const int pin4 = 4;

void setup() {
  pinMode(pin4, OUTPUT);
  Serial.begin(9600);
  Serial.println("Initialized, setting up timer.");
  setTimer();
}
 
void loop() {
   // Get snapshot of time
   unsigned long currentMillis = millis();
   unsigned long diffMillis = (unsigned long)(currentMillis - previousMillis);

   // How much time has passed, accounting for rollover with subtraction!
   if ((waterOnFlag == 0 && diffMillis == interval) || initialFlag == 1)
   {
      // It's time to do something!
      digitalWrite(13, HIGH); // Toggle the LED on Pin 13
      digitalWrite(pin4, HIGH);
      waterOnFlag = 1;
      char buf[500];
      sprintf(buf, "Water ON after %lu ms. Waiting for %lu ms before turning OFF.",diffMillis,duration);
      Serial.println(buf);
      // Use the snapshot to set track time until next event
      previousMillis = currentMillis;
      initialFlag = 0;
   }
   if (waterOnFlag == 1 && diffMillis == duration)
   {
      // It's time to do something!
      digitalWrite(13, LOW); // Toggle the LED on Pin 13
      digitalWrite(pin4, LOW);
      waterOnFlag = 0;
      char buf[500];
      sprintf(buf, "Water OFF after %lu ms. Turning ON after %lu ms.",diffMillis,interval);
      Serial.println(buf);
      // Use the snapshot to set track time until next event
      previousMillis = currentMillis;
   }
}

void setTimer()
{
   sensorValue_cycle = analogRead(sensorPin_cycle);  // Read Vout on analog input pin A0 (Arduino can sense from 0-1023, 1023 is 5V)
   Vout_cycle = (Vin * sensorValue_cycle) / 1023;    // Convert Vout to volts
   R_cycle = Rref_cycle * (1 / ((Vin / Vout_cycle) - 1));  // Formula to calculate tested resistor's value

   sensorValue_duration = analogRead(sensorPin_duration);  // Read Vout on analog input pin A1 (Arduino can sense from 0-1023, 1023 is 5V)
   Vout_duration = (Vin * sensorValue_duration) / 1023;    // Convert Vout to volts
   R_duration = Rref_duration * (1 / ((Vin / Vout_duration) - 1));  // Formula to calculate tested resistor's value

   // Calculate duration and interval in milliseconds. Note that interval is length of cycle minus the duration value, so that duration + interval = cycle.
   if(R_duration > 0){duration = (duration_max * R_duration) / Rmax_duration;}
   else{duration = default_duration;}
   if(R_cycle > 0.0){interval = ((cycle_max * R_cycle) / Rmax_cycle) - duration;}
   else{interval = default_interval;}

   // Print details to serial, used mainly for debugging (optional)
   Serial.print("R_cycle: ");
   Serial.print(R_cycle);
   Serial.print(" ohms  R_duration: ");
   Serial.print(R_duration);
   /*
   Serial.print(" ohms  Duration: ");
   Serial.print(duration/60000.0);
   Serial.print(" min  Interval: ");
   Serial.print(interval/60000.0);
   Serial.print(" min  Cycle: ");
   Serial.print((duration+interval)/60000.0);
   Serial.println(" min");
   */

   Serial.print(" ohms  Duration:");
   Serial.print(duration);
   Serial.print(" sec  Interval: ");
   Serial.print(interval);
   Serial.print(" sec  Cycle: ");
   Serial.print((duration+interval));
   Serial.println(" sec");

   //Serial.println( typeof(duration));
   //char buf[500];
   //sprintf(buf, "Cycle: %f min, Duration: %lu min, Interval: %lu min",(float)(interval+duration)/60000.0,(unsigned long)duration/60000.0,(unsigned long)interval/60000.0);
   //Serial.println(buf);
}

