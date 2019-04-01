#include "mtts.h"
using namespace mtts;

//callback function, return 0:stop  1:continue
bool getboolvalue(void* p){
  return 1;
}

int main(int argc, char **argv) {
  Mtts SS("Mandarin");
  //SS.setPitch(20);
  //SS.setVolume(20);
  //SS.setRate(20);
  //SS.setSpeed(20);
  bool (*p)(void*);
  p = getboolvalue;
  SS.speak("华勤通讯技术有限公司",p,NULL);
  return 0;
}
