#ifndef LEDS_H_INCLUDED
#define LEDS_H_INCLUDED
  #define LED_NOMBRE 47
  #define LED_PIN 22

  void setup_led();
  void LEDoff();
  void LEDon(int puissance, int r, int g, int b);

#endif
