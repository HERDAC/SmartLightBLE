#include <Arduino.h>
#include <BLEDevice.h>
#include <vector>
#include <string>
#include <Bluetooth.h>

static BLEAddress *pServerAddress;
BLEScan* pBLEScan;
BLEClient*  pClient;

std::vector<BeaconBLE> beacons;

unsigned long entry;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    //Called for each advertising BLE server.
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      pServerAddress = new BLEAddress(advertisedDevice.getAddress());

      BeaconBLE tag;
      tag.adresse = pServerAddress->toString().c_str();
      tag.qualite = advertisedDevice.getRSSI();
      beacons.push_back(tag);
/*
      Serial.println();
      Serial.print("Device found: ");
      Serial.print(tag.adresse);
      Serial.print(" => ");
      Serial.print(tag.qualite);
      */
    }
}; // MyAdvertisedDeviceCallbacks

void setup_ble(){
  Serial.println("Starting Arduino BLE Client application...");

  BLEDevice::init("");

  pClient  = BLEDevice::createClient();
  Serial.println(" - Created client");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
}

std::vector<BeaconBLE> loop_ble(){
  beacons.clear();
  BLEScanResults scanResults = pBLEScan->start(1);
  return beacons;
}
