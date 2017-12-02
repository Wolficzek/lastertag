#include <Arduino.h>
#include <IRremote.h>
#include <SoftwareSerial.h>
#define TRIGGERPIN 9
#define BUZZER 6
#define RECV_PIN 8
//Lasertag for arduino alpha v1.0
//IR diode is connected to pin 3

IRsend irsend;
int ir_pin = 0, ammo = 100, buzz_pin = 0;
unsigned long currentMillis;
long reloadTime = 4000;          //Time to reload in miliseconds
long buzzTime = 300;             //Time buzzing after being shot
SoftwareSerial mySerial(10, 11); // RX, TX
IRrecv irrecv(RECV_PIN);         //Pin where irdetector OUT goes
decode_results results;
char command;

void buzzOn()
{
      digitalWrite(BUZZER, HIGH);
}

void buzzOff()
{
      digitalWrite(BUZZER, LOW);
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
    irsend.sendSony(0xb90, 12);
		delay(20);
		ammo--;
	}
	if(ammo == 0){
    currentMillis = millis(); //delay for reloadTime while handling other events
    if(currentMillis % reloadTime == 0){
          ammo = 100;
    }
	}

  //IR reciever handling
  buzz_pin = digitalRead(BUZZER);
  if (irrecv.decode(&results)) {
    Serial.println(results.value);

    if(results.value == 2960) buzzOn();
    irrecv.resume(); // Receive the next value
  }

  if(buzz_pin == HIGH){
    currentMillis = millis();
    if(currentMillis % buzzTime == 0){
       buzzOff();
     }
  }
  //Bluetooth handling
  //TO DO: Commands, gamemodes
  if (mySerial.available()) {
    command = mySerial.read();
    Serial.write(command);
  }
  if (Serial.available()) {
    mySerial.write(Serial.read());
  }

}
