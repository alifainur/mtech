const int analog_ip = 4;
int inputValue = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  inputValue = analogRead(analog_ip);
  Serial.println(inputValue);
  delay(1000);
}
