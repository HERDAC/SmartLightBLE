#ifndef RESEAU_H_INCLUDED
#define RESEAU_H_INCLUDED
  #define ID 18
   //de 0 à 255
  #define SERVEUR_MQTT 192,168,8,100
  #define PORT 1883

  boolean setup_reseau ();
  boolean loop_reseau();

  boolean publish(String topic, String msg);
  void AddParametre (String topic, int defaut = 0);
  int ReadParametre (String topic);
  void ResetValues ();

#endif
