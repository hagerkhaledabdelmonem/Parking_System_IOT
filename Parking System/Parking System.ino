#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

/* Put your SSID & Password */
const char* ssid = "";  // Enter SSID here
const char* password = "";  //Enter Password here

ESP8266WebServer server(80);

#include <Servo.h>

unsigned long time_1;
int price_1 = 0;

unsigned long time_2;
int price_2 = 0;

int IR_1 = D0;
int IR_2 = D1;
int IR_3 = D5;  // door

int LED_1 = D6;
int LED_2 = D7;

bool LEDstatus_1;
bool LEDstatus_2;

int enteringCount_1 = 0 ;
int enteringCount_2 = 0 ;

#define servo  D3


Servo myservo;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  LEDstatus_1 = LOW;
  LEDstatus_2 = LOW;
  pinMode(IR_1, INPUT);
  pinMode(IR_2, INPUT);
  pinMode(IR_3, INPUT);

  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);

  myservo.attach(servo);
  myservo.write(90);

  time_1 = millis();
  time_2 = millis();

  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();

  Serial.println("HTTP server started");



}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
  int value_1 = digitalRead(IR_1);
  int value_2 = digitalRead(IR_2);

  LEDstatus_1 = IR_Read(value_1);
  LEDstatus_2 = IR_Read(value_2);

  if (value_1 == 0 && enteringCount_1 == 0 ) {
    enteringCount_1++;
  }

  if (value_2 == 0 && enteringCount_2 == 0 ) {
    enteringCount_2++;
  }

  if (value_1 == 0 && millis() - time_1 >= 1000) {
    price_1++;
    time_1 = millis();
  }
    if (value_2 == 0 && millis() - time_2 >= 1000) {
    price_2++;
    time_2 = millis();
  }
  digitalWrite(LED_1, LEDstatus_1);
  digitalWrite(LED_2, LEDstatus_2);

  int value_3 = digitalRead(IR_3);

  if ((LEDstatus_1 == HIGH || LEDstatus_2 == HIGH) && (value_3 == 0)) {
    myservo.write(0);
    delay(1000);
    myservo.write(100);

  } else if ((value_1 == 1 && enteringCount_1 == 1)) {
    enteringCount_1 = 0 ;
    price_1=0;
    myservo.write(0);
    delay(1000);
    myservo.write(100);

  } else if ((value_2 == 1 && enteringCount_2 == 1)) {
    enteringCount_2 = 0 ;
    price_2=0;
    myservo.write(0);
    delay(1000);
    myservo.write(100);

  }

}

bool IR_Read (int value) {
  bool LEDstatus = LOW;
  if (value == 1 ) {
    LEDstatus = HIGH;

  }
  return LEDstatus;
}



void handle_OnConnect() {
  server.send(200, "text/html", SendHTML(LEDstatus_1, LEDstatus_2));
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

String SendHTML(bool LEDstatus_1, bool LEDstatus_2) {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
  ptr += "<title>Parking Sa3dia</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr += ".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr += ".button-on {background-color: #1abc9c;}\n";
  ptr += ".button-on:active {background-color: #16a085;}\n";
  ptr += ".button-off {background-color: #34495e;}\n";
  ptr += ".button-off:active {background-color: #2c3e50;}\n";
  ptr += "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<h1>Parking Sa3dia</h1>\n";  
  ptr +="<h2>Parking Area 1</h2> ";
  ptr += "<p>Parking 1 Status is: " + Status_LED(LEDstatus_1) ;
  ptr +="<p>Price : ";
  ptr += price_1 ;
  ptr +=" $";
  ptr += "</p>\n " ; 
  ptr +="<h2>Parking Area 2</h2>";
  ptr += "<p>Parking 2 Status is: " + Status_LED(LEDstatus_2);
  ptr +="<p>Price : ";
  ptr += price_2 ;
  ptr +=" $";
  ptr += "</p>\n " ;

  ptr += "</body>\n";
  ptr += "</html>\n";

  return ptr;
}

String Status_LED(bool led_status) {
  String Status;
  if (led_status == LOW) {
    Status = "Busy";
  }
  else {
    Status = "Available";
  }
  return Status;
}
