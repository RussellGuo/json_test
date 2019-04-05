
#include "mtts_c.h"
#include "mtts.h"
using namespace mtts;

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "pcm_play.h"

//callback function, return 0:stop  1:continue
bool getboolvalue(void*){
  return 1;
}

namespace {
    Mtts *SS = nullptr;
}

extern "C" bool mtts_init(void)
{
    pcm_prepair();
    delete SS;
    SS = new Mtts("Mandarin");
    return true;
}
extern "C" bool mtts_play(const char *buf)
{

    SS->speak(buf,getboolvalue,NULL);
    return true;
}

extern "C" void mtts_setting(uint16_t pitch, uint16_t rate, uint16_t volume)
{
    // TODO: paramters' range fixing and applying
    //SS.setPitch(20);
    //SS.setVolume(20);
    //SS.setRate(20);
    //SS.setSpeed(20);
}
