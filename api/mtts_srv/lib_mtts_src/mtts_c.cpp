
#include "mtts_c.h"
#include "mtts.h"
using namespace mtts;

#include "pcm_play.h"
//callback function, return 0:stop  1:continue
bool getboolvalue(void*){
  return 1;
}

namespace {
    Mtts SS("Mandarin");
}

extern "C" bool tts_init(void)
{
    pcm_prepair();
    return true;
}
extern "C" bool tts_play(bool isGBK, const char *buf)
{
    if (isGBK) {
        return false;
    }

    SS.speak(buf,getboolvalue,NULL);
    return true;
}

extern "C" void tty_setting(uint16_t pitch, uint16_t rate, uint16_t volume)
{
    // TODO: paramters' range fixing and applying
    //SS.setPitch(20);
    //SS.setVolume(20);
    //SS.setRate(20);
    //SS.setSpeed(20);
}
