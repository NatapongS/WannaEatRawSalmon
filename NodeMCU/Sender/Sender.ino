#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DallasTemperature.h>

const char* ssid = "EFFE";
const char* password = "emerald3282";
const char* mqtt_server = "broker.netpie.io";
const int mqtt_port = 1883;
const char* mqtt_Client = "0dedf919-a387-4be2-a3bb-a33bb185bab0";
const char* mqtt_username = "UjfiSdoyjW85LR3T7jDT8hJizs9kLZRs";
const char* mqtt_password = "MM5y2GkgFIIx0_$ax3m1xBUA!~JbX2R4";
WiFiClient espClient;
PubSubClient client(espClient);
#define ONE_WIRE_BUS D2
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);

char msg[1000];
long lastMag = 0;

int analogPin = A0;
int Temperature = 0;
char s ;

void reconnect() {
    while (!client.connected()) {
      Serial.print("Attempting MQTT connection…");
      if (client.connect(mqtt_Client, mqtt_username, mqtt_password)) {
        Serial.println("connected");
      }
      else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println("try again in 5 seconds");
        delay(5000);
      }
    }
  }
  
void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid,password);
  while(WiFi.status()!= WL_CONNECTED){
    delay(250);
    Serial.print(".");
  }
  Serial.print("WiFi connect");
  pinMode(2, INPUT);
  client.setServer(mqtt_server,mqtt_port);
  sensors.begin(); 
}

void loop() {
  //read data from sensor
  Serial.print("A0 = ");
  Serial.println(digitalRead(2));
  Serial.print(" Requesting temperatures..."); 
  sensors.requestTemperatures(); //อ่านข้อมูลจาก library
  Serial.print("Temperature is: ");
  Temperature = sensors.getTempCByIndex(0);//analogRead(analogPin);  
  Serial.println(Temperature);

  //transmit data to website (all data from sensor & springkle status)
  if(!client.connected()){
    reconnect();
  }
  client.loop();
  String data = "{\"data\": {\"Temperature\":" + String(Temperature) + "}}";
  data.toCharArray(msg,(data.length()+1));
  client.publish("@shadow/data/update",msg);
  data = String(Temperature);
  data.toCharArray(msg,(data.length()+1));
  client.publish("@msg/temp",msg);
  delay(1000);
}
