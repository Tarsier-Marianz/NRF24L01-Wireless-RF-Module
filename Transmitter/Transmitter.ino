#include <String.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7, 8); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};

int PUSH_PIN            = 4;
int JOYX_PIN            = A0;
int JOYY_PIN            = A1;
int JOYZ_PIN            = A2;

/* variable declaration that holds data value */
int X_val               = 0;
int Y_val               = 0;
int Z_val               = 0;
int pushState           = 0;

char cmd[4];                        // array that holds command or a combination of two or more commands
int middlePoint         = 1023 / 2; // 1023 is the default max value of analog
int wiperMiddlePoint    = 512 / 2;  // result will be the middle point of wiper, which added/subtracted by the wiper threshold
int wiperThreshold      = 180;      // it helps reading sensitivity of the Z-axis/Wiper
int axisThreshold       = 80;       // it helps reading sensitivity X & Y axis

int i = 0;
void initPins() {
  pinMode(JOYX_PIN, INPUT);
  pinMode(JOYY_PIN, INPUT);
  pinMode(JOYZ_PIN, INPUT);
  pinMode(PUSH_PIN, INPUT);
}

void setup() {
  initPins();
  Serial.begin(9600);

  radio.begin();
  radio.openWritingPipe(addresses[1]);    // 00001
  radio.openReadingPipe(1, addresses[0]); // 00002
  radio.setPALevel(RF24_PA_MIN);
  //radio.stopListening();
  Serial.println("Transmitter Started");
}

void acknowledge() {
  delay(5);
  radio.stopListening();

}

void loop() {
  delay(2);
  radio.stopListening();

  X_val = analogRead(JOYX_PIN);
  Y_val = analogRead(JOYY_PIN);
  Z_val = analogRead(JOYZ_PIN);
  pushState = digitalRead(PUSH_PIN);

  if (Y_val > (middlePoint + axisThreshold) || Y_val < (middlePoint - axisThreshold)) {
    cmd[0] = (Y_val > (middlePoint + axisThreshold))  ? 'F' : 'B';
  } else {
    cmd[0] = 0;
  }
  if (X_val > (middlePoint + axisThreshold) || X_val < (middlePoint - axisThreshold)) {
    cmd[1] = (X_val > (middlePoint + axisThreshold)) ? 'L' : 'R';
  } else {
    cmd[1] = 0;
  }
  //start transmitting command to receiver
  sendCommand();

  // Start listening receiver's reply
  delay(2);
  radio.startListening();

  //while (!radio.available());
  if (radio.available()) {
    while (radio.available()) {
      char text[32] = "";
      radio.read(&text, sizeof(text));
      Serial.print("received reply: ");
      Serial.println(text);
    }
  }
}

void sendCommand() {
  String cmds;
  for (int i = 0; i < 2; i++) {
    char c = cmd[i];
    if (c != NULL && c != '\0') {
      cmds += c;
    }
  }
  // check if commands is not empty
  if (cmds != NULL && cmds != '\0') {
    //Serial.println(cmds);
    char text[4] = "";
    cmds.toCharArray(text, sizeof(text));
    radio.write(&text, sizeof(text));
  } else {
    /* if no direction command to transmit then let's check the wiper command*/
    if (Z_val > (wiperMiddlePoint + wiperThreshold) || Z_val < (wiperMiddlePoint - wiperThreshold)) {
      if (Z_val > (wiperMiddlePoint + wiperThreshold)) {
        //Serial.println("WR");
        const char text[] = "WR";
        radio.write(&text, sizeof(text));
      } else if (Z_val < (wiperMiddlePoint - wiperThreshold)) {
        //Serial.println("WL");
        const char text[] = "WL";
        radio.write(&text, sizeof(text));
      } else {
        //turn_off();
      }
    } else {
      /* if no wiper command to transmit then let's check the push button state */
      if (pushState == HIGH) {
        //Serial.println("PUSH");
        const char text[] = "PUSH";
        radio.write(&text, sizeof(text));
      } else {
        //turn_off();
      }
    }
  }
}

