
#include "mtts_c.h"
#include "mtts.h"
using namespace mtts;

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "pcm_play.h"
#include "ipc_cmd.h"

//callback function, return 0:stop  1:continue
bool getboolvalue(void*){
  return 1;
}

namespace {
    Mtts *SS = nullptr;
}

extern "C" bool tts_init(void)
{
    pcm_prepair();
    delete SS;
    SS = new Mtts("Mandarin");
    return true;
}
extern "C" bool tts_play(bool isGBK, const char *buf)
{
    if (isGBK) {
        send_ipc_reply("ERR INIT", 0);
        return false;
    }

    SS->speak(buf,getboolvalue,NULL);
    send_ipc_reply("ERR OK", 0);
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
