const int pinSenden = 3;
const int pinEmpfang = 4;
const int pinMotor1a = 5;
const int pinMotor1b = 6;
const int pinMotor2a = 7;
const int pinMotor2b = 8;

long microsecondsToCentimeters(long microseconds)
{
  return microseconds / 29 / 2;
}

void fahre()
{
  digitalWrite(pinMotor1a,HIGH);
  digitalWrite(pinMotor1b,LOW);
  digitalWrite(pinMotor2a,HIGH);
  digitalWrite(pinMotor2b,LOW);
}

void halte()
{
  digitalWrite(pinMotor1a,LOW);
  digitalWrite(pinMotor1b,LOW);
  digitalWrite(pinMotor2a,LOW);
  digitalWrite(pinMotor2b,LOW);
}

void setup() {
  pinMode(pinSenden, OUTPUT);
  pinMode(pinEmpfang, INPUT);
  pinMode(pinMotor1a, OUTPUT);
  pinMode(pinMotor1b, OUTPUT);
  pinMode(pinMotor2a, OUTPUT);
  pinMode(pinMotor2b, OUTPUT);
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
  if (cm < 25)
  {
    halte();
  }
  else
  {
    fahre();
  }
  delay(100);
}
