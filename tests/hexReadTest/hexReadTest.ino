void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.setTimeout(10);
}

void loop() {
  String colorString;
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    colorString = Serial.readStringUntil('\n');
    long color = colorString.toInt();
    Serial.println(colorString);
    Serial.println(color);
  }
}
