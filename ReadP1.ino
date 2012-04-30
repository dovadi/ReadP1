/*
  Reading P1 Companion Standard of Dutch Smart Meter through external RX with an Arduino.
  You can't use SoftwareSerial because you need to change the default settings of the USART like partity and stop bits.
  EN_TX should be enabled during reading of data, but disabled if you want to upload your sketch!
  You need to convert the data signal with an 7404.
*/

const int ledPin =  6; //Red LED on the Nanode 5
const int requestPin =  4;

void setup()
{
  Serial.begin(9600);
  UCSR0C = (2<<UPM00)|(0<<USBS0)|(2<<UCSZ00)|(0<<UCPOL0); // 7-E-1

  pinMode(ledPin, OUTPUT);
  pinMode(requestPin, OUTPUT);
  digitalWrite(requestPin, HIGH);
}

void loop()
{
  digitalWrite(ledPin, HIGH);

  if (Serial.available() > 0) {
    digitalWrite(ledPin, LOW);
    byte incomingByte = Serial.read();
    Serial.write(incomingByte);
  }

}

