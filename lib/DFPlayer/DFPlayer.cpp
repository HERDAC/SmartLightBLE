#include <Arduino.h>
#include <DFPlayer.h>
#include <SoftwareSerial.h>
#include <DFMiniMp3.h>


class Mp3Notify{
public:
  static void PrintlnSourceAction(DfMp3_PlaySources source, const char* action)
  {
    if (source & DfMp3_PlaySources_Sd){
        Serial.print("SD Card, ");
    }
    Serial.println(action);
  }
  static void OnError(uint16_t errorCode)
  {
    // see DfMp3_Error for code meaning
    Serial.println();
    Serial.print("Com Error ");
    Serial.println(errorCode);
  }
  static void OnPlayFinished(DfMp3_PlaySources source, uint16_t track)
  {
    Serial.print("Play finished for #");
    Serial.println(track);
  }
  static void OnPlaySourceOnline(DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "online");
  }
  static void OnPlaySourceInserted(DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "inserted");
  }
  static void OnPlaySourceRemoved(DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "removed");
  }
};

SoftwareSerial secondarySerial(32, 33); // RX, TX
DFMiniMp3<SoftwareSerial, Mp3Notify> mp3(secondarySerial);

void setup_dfp(){
  mp3.begin();

  uint16_t volume = mp3.getVolume();
  Serial.print("volume ");
  Serial.println(volume);
  mp3.setVolume(10);

  uint16_t count = mp3.getTotalTrackCount(DfMp3_PlaySource_Sd);
  Serial.print("files ");
  Serial.println(count);
}

void play(int fichier){
  mp3.loop();
  mp3.playMp3FolderTrack(fichier);
}

void stop(){
  mp3.loop();
  mp3.stop();
}

boolean statut(){
  return (mp3.getStatus() == 513);
}

void volume(int vol){
  mp3.setVolume(vol);
}
