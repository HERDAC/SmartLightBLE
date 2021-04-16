#ifndef BLUETOOTH_H_INCLUDED
#define BLUETOOTH_H_INCLUDED

  struct BeaconBLE {
    String  adresse;
    int     qualite;
  };

  void setup_ble();
  std::vector<BeaconBLE> loop_ble();

#endif
