#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7, 8); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(addresses[0]); // 00002
  radio.openReadingPipe(1, addresses[1]); // 00001
  radio.setPALevel(RF24_PA_MIN);
  //radio.startListening();
}
void loop() {
  delay(5);
  radio.startListening();

  if ( radio.available()) {
    char text[32] = "";
    //while (radio.available()) {
    radio.read(&text, sizeof(text));
    //Serial.println(text);
    //}
    delay(5);
    radio.stopListening();

    sendReply(text);
  }
}

void sendReply(char *receive_command) {
  String cmds = String(receive_command);

  // check if commands is not empty
  if (cmds != NULL && cmds != '\0') {
    String reply_msg = "" ;

    if (cmds == "F") {
      reply_msg = "FORWARD";
    } else if (cmds == "B") {
      reply_msg = "BACKWARD";
    } else if (cmds == "L") {
      reply_msg = "LEFT";
    } else if (cmds == "R") {
      reply_msg = "RIGHT";
    } else if (cmds == "FR") {
      reply_msg = "FORWARD RIGHT";
    } else if (cmds == "FL") {
      reply_msg = "FORWARD LEFT";
    } else if (cmds == "BL") {
      reply_msg = "BACKWARD LEFT";
    } else if (cmds == "BR") {
      reply_msg = "BACKWARD RIGHT";
    } else if (cmds == "WL") {
      reply_msg = "WIPER LEFT";
    } else if (cmds == "WR") {
      reply_msg = "WIPER RIGHT";
    } else if (cmds == "ON") {
      reply_msg = "ON received";
    } else if (cmds == "PUSH") {
      reply_msg = "PUSH received";
    } else {
    }
    char text[32] =  "";
    reply_msg.toCharArray(text, sizeof(text));
    radio.write(&text, sizeof(text));

    //radio.write(&reply_msg, sizeof(reply_msg));
    Serial.print("received: ");
    Serial.print(cmds);
    Serial.print("\t reply :");
    Serial.println(reply_msg);
  }

}

