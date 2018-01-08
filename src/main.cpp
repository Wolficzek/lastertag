#include <Arduino.h>
#include <IRremote.h>
#include <SoftwareSerial.h>
#define TRIGGERPIN 9
#define BUZZER 6
#define RECV_PIN 8
//Lasertag for arduino alpha v1.3
//IR diode is connected to pin 3

IRsend irsend;
int ir_pin = 0, ammo = 30, buzz_pin = 0;
unsigned long currentMillis;
unsigned long reloadTime = 4000;          //Time to reload in miliseconds
unsigned long buzzTime = 150, sendDelay = 400;             //Time buzzing after being shot
unsigned long lastReloadTime = 0, lastSentMsg = 0, lastBuzz = 0;
SoftwareSerial mySerial(10, 11); // RX, TX
IRrecv irrecv(RECV_PIN);         //Pin where irdetector OUT goes
decode_results results;
int command;
int bullet = 2000; //2000 - undefined, 2960 - RED, 2950 - BLUE
int team = 0; //0 - no team, 1-red, 2-blue

void setup()
{
  //Serials initialization
  Serial.begin(9600);
  mySerial.begin(9600);
  //pinModes initialization
	pinMode(TRIGGERPIN, INPUT_PULLUP);     //Pin where trigger is connected
  pinMode(BUZZER, OUTPUT);

  Serial.println("SerialInit");
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("MySerialInit");
  while (!mySerial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Enabling IRinTEST");
  irrecv.enableIRIn(); // Start the receiver
  Serial.println("Enabled IRin");

}

void buzzOn()
{
      digitalWrite(BUZZER, HIGH);
}

void buzzOff()
{
      digitalWrite(BUZZER, LOW);
}

void sendBtMsg(char* msg){
  Serial.write(msg);
  mySerial.write(msg);
}

void counter(SoftwareSerial mySerial, int bullet){

  if((millis() - lastSentMsg > sendDelay)){
    mySerial.println(bullet);
    lastSentMsg = millis();
  }


}
void loop()
{
  //Shoot handling section
	ir_pin = digitalRead(TRIGGERPIN);
	if(ir_pin == LOW && ammo > 0){
    irsend.sendSony(bullet, 12);
    ammo--;
    if(ammo != 0) counter(mySerial, bullet);
    Serial.println(ammo);
	}
	if(ammo == 0){
    if ((millis() - lastReloadTime) > reloadTime){
      sendBtMsg("reload");
      lastReloadTime = millis();
    }
	}

  //IR reciever handling
  buzz_pin = digitalRead(BUZZER);

  if (irrecv.decode(&results)) {
    Serial.println(results.value);

    if(results.value == 2960 && team == 1){
      buzzOn();
    }
    irrecv.resume(); // Receive the next value

    if(results.value == 2950 && team == 2){
      buzzOn();
    }
    irrecv.resume(); // Receive the next value


    if(results.value == 2000 && team == 0){
      buzzOn();
    }
    irrecv.resume(); // Receive the next value
  }
  //Buzzer handling
  if(buzz_pin == HIGH){
    if(millis() - lastBuzz > buzzTime){
       buzzOff();
       lastBuzz = millis();
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
    }else if(command == '0'){
      bullet = 2000;
      team = 0;
    }else if(command == '4' || command == '5'){
      ammo = 30;
    }
    //Debug purposes serial write
    Serial.write(command);
  }
}
