#include <Arduino.h>
#include <IRremote.h>
#include <SoftwareSerial.h>
#define TRIGGERPIN 9
#define BUZZER 6
#define RECV_PIN 12
//Lasertag for arduino alpha v2.0
//IR diode is connected to pin 3

IRsend irsend;
int ir_pin = 0, buzz_pin = 0;
unsigned long reloadTime = 2500, shotDelay = 500;          //Time to reload in miliseconds
unsigned long buzzTime = 200, sendDelay = 400;             //Time buzzing after being shot
unsigned long lastReloadTime = 0, lastSentMsg = 0, lastBuzz = 0, lastShot = 0;
SoftwareSerial mySerial(10, 11); // RX, TX
IRrecv irrecv(RECV_PIN);         //Pin where irdetector OUT goes
decode_results results;
int command;
int bullet = 2000; //2000 - undefined, 2960 - RED, 2950 - BLUE
int team = 0; //0 - no team, 1-red, 2-blue
bool isBuzzOn = false;
bool isReloading = false;


void debug(String msg){
  Serial.println(msg);
}

void setup()
{
  //Serials initialization
  Serial.begin(9600);
  mySerial.begin(9600);
  //pinModes initialization
	pinMode(TRIGGERPIN, INPUT_PULLUP);     //Pin where trigger is connected
  pinMode(BUZZER, OUTPUT);

  Serial.println("Enabling IRinTEST");
  irrecv.enableIRIn(); // Start the receiver
  Serial.println("Enabled IRin");

}

void buzzOn()
{
      digitalWrite(BUZZER, HIGH);
      isBuzzOn = true;
      lastBuzz = millis();
}
void buzzOff()
{
      digitalWrite(BUZZER, LOW);
      isBuzzOn = false;
}

void sendMsg(SoftwareSerial mySerial, int bullet){
  if((millis() - lastSentMsg > sendDelay)){
    mySerial.println(bullet);
    lastSentMsg = millis();
  }
}
void loop()
{
  //Shoot handling section
	ir_pin = digitalRead(TRIGGERPIN);
	if(ir_pin == LOW && isReloading == false){
     if(millis() - lastShot > shotDelay){
      sendMsg(mySerial, bullet);
      irsend.sendSony(bullet, 12);
      irrecv.enableIRIn();
      lastShot = millis();
      debug("Shot");
    }
	}

  //IR reciever handling
  if (irrecv.decode(&results)) {
    Serial.println(results.value);

    if(results.value == 2960 && team == 1){
      sendMsg(mySerial, 500);
      buzzOn();
    }
    irrecv.resume(); // Receive the next value

    if(results.value == 2950 && team == 2){
      sendMsg(mySerial, 500);
      buzzOn();
    }
    irrecv.resume(); // Receive the next value

    if(results.value == 2000 && team == 0){
      sendMsg(mySerial, 500);
      buzzOn();
      irrecv.resume(); // Receive the next value
    }

  }
  //Buzzer handling
  if(isBuzzOn){
    if(millis() - lastBuzz > buzzTime){
       buzzOff();
     }
  }

  //Reloading handling
  if(isReloading){
    if(millis() - lastReloadTime > reloadTime){
      debug("Finished reloading");
       isReloading = false;
     }
  }

  if (mySerial.available()) {
    command = mySerial.read();
    //Setup teams and bullet IR codes
    //Handle commands
    //Set team to blue
    if(command == '2'){
      bullet = 2960;
      team = 2;
      //Set team to red
    }else if(command == '1'){
      bullet = 2950;
      team = 1;
      //set no team - deadmatch mode
    }else if(command == '0' || command == '3'){
      bullet = 2000;
      team = 0;
    }else if(command == '4' || command == '5'){
      lastReloadTime = millis();
      debug("Reloading");
      isReloading = true;
    }
    //Debug purposes serial write
    Serial.write(command);
  }
}
