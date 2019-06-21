const int pinSenden = 3;
const int pinEmpfang = 4;

long microsecondsToCentimeters(long microseconds)
{
  return microseconds / 29 / 2;
}

void setup() {
  pinMode(pinSenden, OUTPUT);
  pinMode(pinEmpfang, INPUT);
  Serial.begin(9600);
}

void loop()
{
  long duration, cm;

  digitalWrite(pinSenden, LOW);
  delayMicroseconds(2);
  digitalWrite(pinSenden, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinSenden, LOW);

  duration = pulseIn(pinEmpfang, HIGH);

  cm = microsecondsToCentimeters(duration);
  Serial.println(cm);
  delay(100);
}


