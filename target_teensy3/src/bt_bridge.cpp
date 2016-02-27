#include <stdint.h>
#include <WProgram.h>

#define BYTE_BUF_SIZE 64

int ledPin = 13;
int ctsPin = 2;
int rtsPin = 3;
int cmdLowPin = 4;
int stateChangePin = 5;

extern "C" int main(void)
{
  setup();
  while (1) {
    loop();
    yield();
  }
}

#define HWSERIAL Serial1

String gReadLine; 
uint8_t gReadLineDone;

void readUntilNewline()
{
  char incomingByte;
  if (HWSERIAL.available() > 0) {
    incomingByte = HWSERIAL.read();
    gReadLine.concat(incomingByte);
    
    if (gReadLine.endsWith('\n')) {
      gReadLineDone =  1;
    }
  }
}


void setCmdMode(void)
{
  uint8_t done = 0;

  while(!done) {
    digitalWrite(cmdLowPin, HIGH);
    delay(100);
    digitalWrite(cmdLowPin, LOW);
    gReadLineDone = 0;
    while (!gReadLineDone) {
      readUntilNewline();
    }
    if (gReadLine.startsWith("CMD")) {
      done = 1;
    } 
  }    
}

void reconnectBluetooth()
{
  HWSERIAL.println("B");
}

void setup()
{
  pinMode(ledPin, OUTPUT);
  pinMode(cmdLowPin, OUTPUT);
  pinMode(stateChangePin, INPUT);
  digitalWrite(ledPin, LOW);
  HWSERIAL.begin(115200);
  Serial.begin(115200);
  setCmdMode();
  reconnectBluetooth();
}

void loop()
{
  int incomingByte;
        
  if (Serial.available() > 0) {
    digitalWrite(ledPin, HIGH);
    incomingByte = Serial.read();
    HWSERIAL.write(incomingByte);
    digitalWrite(ledPin, LOW);
  }
  if (HWSERIAL.available() > 0) {
    digitalWrite(ledPin, HIGH);
    incomingByte = HWSERIAL.read();
    Serial.write(incomingByte);
    digitalWrite(ledPin, LOW);
  }
}
