#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>

SoftwareSerial NodeSerial(D2, D3); // RX | TX

const char* ssid = "EFFE";
const char* password = "emerald3282";
const char* mqtt_server = "broker.netpie.io";
const int mqtt_port = 1883;
const char* mqtt_Client = "4828bc1c-c274-4fdd-9a0a-58d869fb94a5";
const char* mqtt_username = "ySLfSqwzgFvTjLoAupUorKWhGyEfGF4k";
const char* mqtt_password = "SHwbMyPdji-Y*qbV_TAAK-oJu9NMbKNs";
WiFiClient espClient;
PubSubClient client(espClient);

char msg[50];
String stat;
int PrefTemp = 25;
int Temp = 25;

int analogPin = A0;

void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection…");
        if (client.connect(mqtt_Client, mqtt_username, mqtt_password)) {
            Serial.println("connected");
            client.subscribe("@msg/status");
            client.subscribe("@msg/preftemp");
            client.subscribe("@msg/temp");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println("try again in 5 seconds");
            delay(5000);
        }
    }
}

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    String message;
    for (int i = 0; i < length; i++) {
        message = message + (char)payload[i];
    }
    Serial.println(message);
    if(String(topic) == "@msg/status") {
        if(message == "on"){
            Serial.println("SWITCH on");
            stat = "on";
        }
        else if (message == "off"){
            Serial.println("SWITCH off");
            stat = "off";
        }
    }
    if(String(topic) == "@msg/preftemp") {
        PrefTemp = message.toInt();
        Serial.println(message);
    }
    if(String(topic) == "@msg/temp") {
        Temp = message.toInt();
        Serial.println(message);
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(250);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");

    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
    NodeSerial.begin(115200);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
    String data = "{\"data\": {\"temperature\":" + String(Temp) + "}}";
    data.toCharArray(msg,(data.length()+1));
    client.publish("@shadow/data/update",msg);
    data = "{\"data\": {\"preftemp\":" + String(PrefTemp) + "}}";
    data.toCharArray(msg,(data.length()+1));
    client.publish("@shadow/data/update",msg);
    data = "{\"data\": {\"status\":" + String(stat) + "}}";
    data.toCharArray(msg,(data.length()+1));
    client.publish("@shadow/data/update",msg);

    //UART
    int NewStat = (stat[1] == 'n' ? 1 : 0);
    NodeSerial.print(NewStat);
    NodeSerial.print(" ");
    NodeSerial.print(Temp);
    NodeSerial.print(" ");
    NodeSerial.print(PrefTemp);
    NodeSerial.print("\n");
    
    delay(1000);
}
