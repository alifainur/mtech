// Potentiometer is connected to GPIO 34 (Analog ADC1_CH6) 
const int potPin = 4;

// variable for storing the potentiometer value
int inputVal = 0;

void setup() {
 Serial.begin(9600);
 delay(1000);
}

void loop() {
  // Reading potentiometer value
 inputVal = analogRead (potPin);
 Serial.println(inputVal);
 delay(500);
}
