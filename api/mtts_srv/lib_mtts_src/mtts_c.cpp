
#include "mtts_c.h"
#include "mtts.h"
using namespace mtts;

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace {
    Mtts *SS = nullptr;
}

extern "C" bool mtts_init(void)
{
    delete SS;
    SS = new Mtts("Mandarin");
    return true;
}
extern "C" bool mtts_play(const char *buf, tts_playing_callback_t cb, void* user_ptr)
{
    SS->speak(buf, cb, user_ptr);
    return true;
}

extern "C" void mtts_setting(uint16_t pitch, uint16_t rate, uint16_t volume)
{
    return;
    // TODO: paramters' range fixing and applying
    SS->setPitch(pitch);
    SS->setVolume(volume);
    SS->setRate(rate);
    SS->setSpeed(20);
}

