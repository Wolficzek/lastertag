#include <Arduino.h>
#include <IRremote.h>
#include <SoftwareSerial.h>
#define TRIGGERPIN 9
#define BUZZER 6
#define RECV_PIN 8
//Lasertag for arduino alpha v1.1
//IR diode is connected to pin 3

IRsend irsend;
int ir_pin = 0, ammo = 100, buzz_pin = 0;
unsigned long currentMillis;
long reloadTime = 4000;          //Time to reload in miliseconds
long buzzTime = 100;             //Time buzzing after being shot
SoftwareSerial mySerial(10, 11); // RX, TX
IRrecv irrecv(RECV_PIN);         //Pin where irdetector OUT goes
decode_results results;
char command;
int bullet = 2000; //2000 - undefined, 2960 - RED, 2950 - BLUE
char team = "UNDEFINED";


void buzzOn()
{
      digitalWrite(BUZZER, HIGH);
}

void buzzOff()
{
      digitalWrite(BUZZER, LOW);
}

void sendBtMsg(char* msg){
  if (mySerial.available()) {
    mySerial.write(msg);
  }
}


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

void loop()
{
  //Shoot handling section
	ir_pin = digitalRead(TRIGGERPIN);
	if(ir_pin == LOW && ammo > 0){
    irsend.sendSony(bullet, 12);
		delay(20);
		ammo--;
	}
	if(ammo == 0){
    currentMillis = millis(); //delay for reloadTime while handling other events
    if(currentMillis % reloadTime == 0){
          ammo = 100;
          sendBtMsg("reload");
    }
	}

  //IR reciever handling
  buzz_pin = digitalRead(BUZZER);
  if (irrecv.decode(&results)) {
    Serial.println(results.value);

    if(results.value == 2960 && team == "RED"){
      buzzOn();
    }
    irrecv.resume(); // Receive the next value

    if(results.value == 2950 && team == "BLUE"){
      buzzOn();
    }
    irrecv.resume(); // Receive the next value

    if(results.value == 2000 && team == "UNDEFINED"){
      buzzOn();
    }
    irrecv.resume(); // Receive the next value
  }

  if(buzz_pin == HIGH){
    currentMillis = millis();
    if(currentMillis % buzzTime == 0){
       buzzOff();
     }
  }

  if (mySerial.available()) {
    command = mySerial.read();
    //Setup teams and bullet IR codes
    //Handle commands
    if(command == "BLUE"){
      bullet = 2960;
      team = "BLUE";
      sendBtMsg("Blue");
    }else if(command =="RED"){
      bullet = 2950;
      team = "RED";
      sendBtMsg("Red");
    }else if(command =="DEATMATCH"){
      bullet = 2000;
      team = "UNDEFINED";
      sendBtMsg("Deadmatch");
    }else if(command =="reset"){
      ammo = 100;
    }
    //Debug purposes serial write
    Serial.write(command);
  }
}
