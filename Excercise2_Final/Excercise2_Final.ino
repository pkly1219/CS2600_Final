#include <WiFi.h>
#include <PubSubClient.h>


const char* ssid = "Xai3GDi";
const char* password = "khongbiet";

// MQTT Broker
const char *mqtt_broker = "broker.mqttdashboard.com";
const char *topic = "esp32_joystick";
const char *mqtt_username = "phungly";
const char *mqtt_password = "19621961";
const int mqtt_port = 1883;
const char *x = "Coordinate_x";
const char *y = "Coordinate_y";
const char *z = "Coordinate_z";






#define LEDPIN 2




unsigned long previousMillis = 0; 
const long interval = 5000;
int current_ledState = LOW;
int last_ledState = LOW;

WiFiClient wifiClient;
PubSubClient client(wifiClient);


void setup_led()
{
  //initialize digital pin PIN_LED as an output
  pinMode(LEDPIN,OUTPUT);
  pinMode(current_ledState,INPUT);
}
void setup_wifi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
   WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.println("Connecting to WiFi..");
 }
 Serial.println("Connected to the WiFi network");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void connect_to_broker() {
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
 while (!client.connected()) {
     String client_id = "esp32-client-";
     client_id += String(WiFi.macAddress());
     Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
     if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
         Serial.println("Public emqx mqtt broker connected");
     } else {
         Serial.print("failed with state ");
         Serial.print(client.state());
         delay(2000);
     }
 }
 client.subscribe(topic);
}

void callback(char* topic, byte *payload, unsigned int length) {
  Serial.println("-------new message from broker-----");
  Serial.print("topic: ");
  Serial.println(topic);
  Serial.print("message: ");
  Serial.write(payload, length);
  Serial.println();
  if (*payload == '1') current_ledState = HIGH;
  if (*payload == '0') current_ledState = LOW;
}
int xyzPins[] = {12, 13, 14}; //x, y, x pins
void setup() {
  Serial.begin(115200);
  Serial.setTimeout(500);
  setup_wifi();
  connect_to_broker();
   pinMode(xyzPins[2], INPUT_PULLUP);
  setup_led();
}


void loop() {
  client.loop();
  if (!client.connected()) {
    connect_to_broker();
  }
  if (last_ledState != current_ledState) {
    last_ledState = current_ledState;
    digitalWrite(LEDPIN, current_ledState);
    Serial.print("LED state is ");
    Serial.println(current_ledState);
  }

  int xVal = analogRead(xyzPins[0]);
  int yVal = analogRead(xyzPins[1]);
  int zVal = digitalRead(xyzPins[2]);
  Serial.printf("X,Y,Z: %d,\t%d,\t%d\n", xVal, yVal, zVal);
  delay(5000);
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
  
  
    
   
      client.publish(x, String(xVal).c_str());
      client.publish(y, String(yVal).c_str());
       client.publish(z, String(zVal).c_str());
      previousMillis = currentMillis;
    
  
}
}
