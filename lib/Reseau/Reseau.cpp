#include <Arduino.h>
#include <vector>
#include <SPI.h>
#include <string>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <Reseau.h>
const byte last_mac = ID;
int id = ID;
byte mac[]    = { 0x02, 0x00, 0x00, 0x7A, 0x38, last_mac };
IPAddress ip (192, 168, 8, 100+id);
IPAddress server(SERVEUR_MQTT);

uint32_t lastReconnectAttempt = 0;
unsigned long check;

//Just Header
void callback(char* topic, byte* payload, unsigned int length);
boolean connection();

EthernetClient ethClient;
PubSubClient client(server, PORT, callback, ethClient);

struct TopicParametre {
    String    topic;
    int  parametre;
    int       defaut;
    boolean   subscribe;
    boolean   define;
  };
std::vector<TopicParametre> topic_parametre;
void AddParametre (String topic, int defaut){
  String sujet = String(id)+"/"+topic;
  TopicParametre valeure;
  valeure.topic = sujet;
  valeure.subscribe = false;
  valeure.define = false;
  valeure.defaut = defaut;
  topic_parametre.push_back(valeure);
  Serial.println("add");
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  for (size_t i = 0; i < topic_parametre.size(); i++){
    if(String(topic) == topic_parametre[i].topic){
      String valeure;
      for (size_t j=0;j<length;j++) {
      valeure += ((char)payload[j]);
      }
      topic_parametre[i].parametre = (int)(valeure.toInt());
      topic_parametre[i].define = true;
    }
  }

}

boolean  setup_reseau (){
  Ethernet.init(5);  // ESP32 with Adafruit Featherwing Ethernet


  // start the Ethernet connection:
  Serial.println("Initialize Ethernet:");
  Ethernet.begin(mac,ip);
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet");
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    } else if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    return false;
  }
  //Ethernet.begin(mac,ip);
  // print your local IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());

  connection();

  String msg = "Démarrage de la lampe "+String(id);
  client.publish("outTopic",msg.c_str());


}

boolean connection(){

  Serial.println("connection");
  if (client.connect((String(id)).c_str())) {
    String msg = "Connexion de la lampe "+String(id);
    client.publish("outTopic",msg.c_str());
    client.subscribe("inTopic");
    for(auto str:topic_parametre){
      const char* topic = str.topic.c_str();
      client.subscribe(topic);
      str.subscribe = true;
      Serial.print("sub: ");
      Serial.println(topic);
    }
    Serial.println("subscribe OK");
    return true;
  }

}

boolean loop_reseau(){
  unsigned long now = millis();
  unsigned long intervalle = now - check;
  client.loop();

  if(client.state()==0){
    check = now;
    return true;
  }
  if (intervalle > 20000) {
    ESP.restart();
  }
  if (!client.connected()) {
    Serial.println("=== MQTT NOT CONNECTED ===");
    // Reconnect every 10 seconds
    uint32_t t = millis();
    if (t - lastReconnectAttempt > 10000L) {
      lastReconnectAttempt = t;
      if (connection()) {
        lastReconnectAttempt = 0;
      }
    }
    return false;
  }

  //NO-USE - Pour tester la connexion avec la carte ethernet
  if (Ethernet.linkStatus() == LinkOFF) {
  }
  else if (Ethernet.linkStatus() == LinkON) {
  }
  else {
  }

}

boolean publish(String topic, String msg){
  String sujet = String(id)+"/"+topic;
  return (client.publish(sujet.c_str(),msg.c_str()) == 1);
}

int ReadParametre (String topic){
  //String sujet = String(ID)+"/"+topic;
  for(auto val:topic_parametre){
    String sujet = String(id)+"/"+topic;

    if(sujet == val.topic){
      if(val.define){
        return val.parametre;
      } else {
        return val.defaut;
      }
    }
  }
}

void ResetValues (){
  for(auto& val:topic_parametre){
    val.define = false;
  }
}
