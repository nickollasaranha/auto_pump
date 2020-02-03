/*
 * created by Rui Santos, https://randomnerdtutorials.com
 * 
 * Complete Guide for Ultrasonic Sensor HC-SR04
 *
    Ultrasonic sensor Pins:
        VCC: +5VDC
        Trig : Trigger (INPUT) - Pin11
        Echo: Echo (OUTPUT) - Pin 12
        GND: GND
 */
/* PINS DEFINITON HERE */
int trigPin = 11;    // Trigger
int echoPin = 12;    // Echo
int relayPin = 8;    // Relay

/* Global VARS */
const float maximumDistAllowed = 4.5;
const int circularArraySize = 10;
const int pumpActivationTime = 15000;

/* Don't change those */
int index = 0;
bool validSensorRead = false;
long duration;
float sensorReads[circularArraySize], cm;

/* Setup function */
void setup() {
  // Serial Port begin
  Serial.begin (9600);
 
  // Define inputs and outputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(relayPin, OUTPUT);

  // Set the relay OFF
  digitalWrite(relayPin, HIGH);

  // Initialize sensorReads with -1
  for(int i = 0; i<10; i++)
    sensorReads[i] = -1;
}

/* This will return the average mean of sensorReads array */
float readSensorWAVG() {
  float result = 0;
  int i = 0;
 
  /* Read the sensor until you find a -1 */
  for(i=0; i<circularArraySize; i++) {
    if (sensorReads[i] == -1) {
      break;
    }
    result+=sensorReads[i];
  }

  validSensorRead = (i == 10 ? true : false);
  return result/ (float) i;
}

/* This function will handle correct index of the circular array */
void addToArray(float value) {
  sensorReads[index] = value;
  index++;
  index = index % circularArraySize;
}

/* This function will read the sensor and calculate it in cm */
long readSensor() {

  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
 
  // Convert the time into a distance
  // Divide by 29.1 or multiply by 0.0343
  cm = (duration/2.0) / 29.1;

  if (cm > 1 && cm < 20) {
    // Add this value to the sensor
    addToArray(cm);
  }

  Serial.print(cm);
  Serial.print("cm ");
  Serial.println(); 
}

/* Main function */
void loop() {

  /* We read the sensor measurements first */
  readSensor();

  /* Check if we need to activate the pump */
  if ((readSensorWAVG() <= maximumDistAllowed) && validSensorRead) {
    // Activate Pump for 5 seconds
    Serial.println("Activating Pump");
    digitalWrite(relayPin, LOW);
    delay(pumpActivationTime);
    // Turn off the Pump now
    digitalWrite(relayPin, HIGH);

    /* Right after we set this to a very high value so we don't reactivate again unless
    really necessary */
    for (int i = 0; i<circularArraySize; i++)
      sensorReads[i] = 100;
  }

  delay(100);
}