#ifndef MTTS_H
#define MTTS_H

#include <string>


using namespace std;

namespace mtts {

class MttsImpl;

class Mtts {
 private:
  MttsImpl *m_pImpl;

 public:
  Mtts(string voice);
  int blockSpeak(string text);
  int speak(string text, bool (*pCallback)(void *) = NULL,
            void *pCallbackArgs = NULL);
  void setPitch(int pitch_delta);/*pitch_delta (-100 .. 100, in percent)*/
  void setVolume(int volume_delta);/*volume_delta (-100 .. 100, in percent)*/
  void setRate(int rate_delta);/*rate_delta (-50 .. 100, in percent)*/
  void setSpeed(int tempo_delta);/*tempo_delta (-50 .. 100, in percent)*/
 };
}

#endif
