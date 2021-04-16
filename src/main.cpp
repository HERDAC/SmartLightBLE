/*******************************
 * Ce code est créé par HERDAC *
 ******************************/

//Ajout des bibliothèques
#include <Arduino.h>
#include <vector>
#include <string>
#include <Reseau.h>
#include <Bluetooth.h>
#include <DFPlayer.h>
#include <Leds.h>
#include <Wire.h>
#include <uFire_SHT20.h>
uFire_SHT20 sht20;

//Définition des PINs
#define LED 02
#define SON1 A15
#define SON2 14
#define PIR1 26
#define PIR2 27
#define INT1 34
#define INT2 35
#define TEST 04
#define CHAUF 25


int WatchDog;
std::vector<BeaconBLE> tag;
int volume_past;
boolean play_sound;

unsigned long tps_max;
int tps_last_max;
int analog_past(0);
boolean clign(false);

int color_r;
int color_g;
int color_b;
int LED_max;
int LED_ampli;
int vol_sound;
boolean chauffage;
boolean pb_test;
float humidity;
float temperature;
float dewpoint;


void setup() {
  delay(5000);

  //Définition des PINs
  pinMode(LED, OUTPUT);
  digitalWrite(LED,HIGH);
  pinMode(PIR1,INPUT);
  pinMode(PIR2,INPUT);
  pinMode(TEST,INPUT_PULLUP);
  pinMode(INT1,INPUT);
  pinMode(INT2,INPUT);
  pinMode(CHAUF, OUTPUT);

  //Ouverture du port série pour l'ordi
  Serial.begin(115200);

  //Ajout des paramètres d'entrées
  AddParametre("BLE/recherche", 1);
  AddParametre("LED/ON", 0);
  AddParametre("LED/color/r", 245);
  AddParametre("LED/color/g", 255);
  AddParametre("LED/color/b",  20);
  AddParametre("LED/MAX", 40);
  AddParametre("LED/ampli", 20);
  AddParametre("SON/volume", 20);
  AddParametre("TEMP/CHAUFFAGE", 0);
  AddParametre("WDG/ESP");

  //Setup des différentes fonctions
  setup_ble();
  while (!setup_reseau()) {
    delay(500);
    Serial.println("Echec réseau");
  }
  setup_led();
  Wire.begin(15,13);
  sht20.begin();


  delay(random(5000));

  //Check première connection
  WatchDog = random(1000);
  publish("WDG/NR", String(WatchDog));
  while (WatchDog != ReadParametre("WDG/ESP")){
    Serial.println("Pas encore connecté");
    loop_reseau();
    delay(100);
  }

  //Reset valeurs des paramètres
  //ResetValues();

  //Setup du DFPlayer
  setup_dfp();


  //Paramètre fixe
  color_r = ReadParametre("LED/color/r");
  color_g = ReadParametre("LED/color/g");
  color_b = ReadParametre("LED/color/b");
  LED_max = ReadParametre("LED/MAX");
  LED_ampli = ReadParametre("LED/ampli");
  vol_sound = ReadParametre("SON/volume");
}

void loop() {
  if(loop_reseau()){
    digitalWrite(LED,LOW);
  } else {
    digitalWrite(LED,HIGH);
  }


  publish("INT1", String(digitalRead(INT1)));
  publish("INT2", String(digitalRead(INT2)));


  //Lecture des paramètres BLE + LED
  int recherche = ReadParametre("BLE/recherche");
  int actif = ReadParametre("LED/ON");

  if(!actif){
    //Paramètre fixe
    color_r = ReadParametre("LED/color/r");
    color_g = ReadParametre("LED/color/g");
    color_b = ReadParametre("LED/color/b");
    LED_max = ReadParametre("LED/MAX");
    LED_ampli = ReadParametre("LED/ampli");
    vol_sound = ReadParametre("SON/volume");

    chauffage = ReadParametre("TEMP/CHAUFFAGE");
    pb_test = !digitalRead(TEST);
    humidity    = sht20.humidity();
    temperature = sht20.temperature();
    dewpoint = sht20.dew_point();

    Serial.print("humidité: ");
    Serial.println(humidity);
    publish("TEMP/HR",String(humidity));
    Serial.print("Température: ");
    Serial.println(temperature);
    publish("TEMP/T",String(temperature));
    //Serial.print("Dewpoint: ");
    //Serial.println(dewpoint);
    publish("TEMP/DP",String(dewpoint));

    if(volume_past != vol_sound){
      volume(vol_sound);
      volume_past = vol_sound;
    }

    digitalWrite(CHAUF, chauffage);

  } else {
    chauffage = false;
  }
  
  boolean pir_1 = digitalRead(PIR1);
  boolean pir_2 = digitalRead(PIR2);
  int son1 = random(1000);//analogRead(SON1);
  int son2 = analogRead(SON2);
  

  // envoie des capteurs de mouvements
  publish("PIR1", String(pir_1));
  publish("PIR2", String(pir_2));

  Serial.println(son1);

  unsigned long now = millis();
  if((son1-analog_past)>0){
    analog_past = son1;
    clign = true;
  } else if((son1-analog_past)<0){
    analog_past = son1;
    clign = false;
  }

  if(clign){
    tps_last_max = now - tps_max;
    tps_max = now;
  }
  int x = son1 -2500;
  int y = map(x, 0, 1500, 0, LED_ampli);
  int val = map(y, 0, LED_ampli, 0, LED_max);

  //Serial.println(son1);
  //Serial.println(LED_max);
  //Serial.println(LED_ampli);
  //Serial.println(val);
  //Serial.println(tps_last_max);
  //publish("test", String(val));

//--------------------

  if(recherche){
    tag = loop_ble();
    for(auto val:tag){
      publish("BLE/TAG",val.adresse + " => " + val.qualite);
    }

    
  }

  if(actif){
    Serial.println("TEST");
    //LEDon(val,color_r,color_g,color_b);
    delay(100);
    if(clign){
      LEDon(LED_max,color_r,color_g,color_b);
      Serial.println("ON");
    } else {
      LEDon(LED_max-(LED_ampli*LED_max)/100,color_r,color_g,color_b);
      Serial.println("OFF");
    }
    if(!play_sound){
      play(1);
      play_sound = true;
    }
    digitalWrite(CHAUF, 0);
  } else {
    LEDoff();
    stop();
    play_sound = false;
  }

Serial.println("--- - --- - ---");



}
