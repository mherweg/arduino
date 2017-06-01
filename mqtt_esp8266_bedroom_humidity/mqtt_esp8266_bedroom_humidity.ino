/*
 ESP8266 MQTT + I2C example
 
 reads temperature & humidity from the AM3220 sensor
 and brightness with an LDR 
 sends those 3 values to the MQTT Server

GND---33 Ohm---*---LDR--- +3.3V
               |
               V
                Analog input of the ESP

 http://www.instructables.com/id/Connecting-AM2320-With-Arduino/
 
 you'll need this library:
 https://github.com/Ten04031977/AM2320-master

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <Wire.h>
#include <AM2320.h>

#include "conf.h"

// I moved this to conf.h
//const char* ssid = "XXXXXXX";
//const char* password = "XXXXXXX";
//const char* mqtt_server = "XXXX";


AM2320 th;
WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[6];
char msg2 [6];
char msg3 [6];
int value = 0;
int brightness;

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  //AM2320
  //SDA -> D3 = GPIO0
  //SCL -> D4 = GPIO2
   Wire.pins(0,2);// just to make sure
   Wire.begin(0,2);// the SDA and SCL

  
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.print("BUILTIN_LED Value: ");
  Serial.println(BUILTIN_LED);
  Serial.print("Connecting to ");
  Serial.println(ssid);


  
WiFi.softAPdisconnect(true);
WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("home/bedroom/hello", "hello world");
      // ... and resubscribe
      //client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();


  // LDR Helligkeits sensor
  
 
  long now = millis();
  if (now - lastMsg > 3000) {
    lastMsg = now;

   // LDR Helligkeits sensor
   brightness = analogRead( A0);

   //AM2320
   switch(th.Read()) {
    case 2:
      Serial.println("CRC failed");
      break;
    case 1:
      Serial.println("Sensor offline");
      break;
    case 0:
      Serial.print("humidity: ");
      Serial.print(th.h);
       snprintf (msg3, 6, "%d",  int(th.h*100));
      Serial.print("%, AM2320 temperature: ");
      Serial.print(th.t);
       snprintf (msg, 6, "%d", int(th.t*100));
      Serial.println("*C");
      break;
  }
   
   snprintf (msg2, 6, "%d", brightness);
    
    Serial.print("brightness: ");
    Serial.println(msg2);
    Serial.print("temp: ");
    Serial.println(msg);
    client.publish("home/bedroom/temperature", msg );
    client.publish("home/bedroom/brightness", msg2 );
    client.publish("home/bedroom/humidity", msg3 );
  }
}

