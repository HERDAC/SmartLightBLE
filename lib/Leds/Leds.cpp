#include <Arduino.h>
#include <Leds.h>
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_NOMBRE, LED_PIN, NEO_GRB + NEO_KHZ800);
#ifdef __AVR__
  #include <avr/power.h>
#endif

int last_puissance;
int lr;
int lg;
int lb;
boolean firstOFF(true);
boolean change(true);

/*
 *  IMPORTANT !!!!  Pour réduire les risque sur le bandeau, ajoutez un condensateur de 1000 uF sur l'alimentation
 *  Mettez une résistance entre 300 et 500 Ohm sur le DataIn de la première LED
 *  Minimiser la distance entre l'Arduino et la première led
 *  Pour préserver le circuit... si vous le voulez, brancher le GND en premier
 */

void setup_led(){
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
 #if defined (__AVR_ATtiny85__)
   if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
 #endif
 // End of trinket special code

 strip.begin();
 strip.show(); // Éteint toutes les LEDs
}

void LEDoff(){
  if (firstOFF){
    strip.clear();
    strip.show();
    firstOFF = false;
    Serial.println("OFF");
    last_puissance = 0;
    lr = 0;
    lg = 0;
    lb = 0;
  }
}

void LEDon(int puissance, int r, int g, int b){

  if(r!=lr || g!=lg || b!=lb){
    for (int LED=0; LED<LED_NOMBRE; LED++){
      strip.setPixelColor(LED, r, g, b);
    }
    lr = r;
    lg = g;
    lb = b;
    change = true;
  }


  if(puissance != last_puissance){
    strip.setBrightness(puissance);
    last_puissance = puissance;
    change = true;
  }

  if(change) {
    strip.show();
    change = false;
  }
  firstOFF = true;
}
