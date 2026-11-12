#include "libretro.h"
#include "libretrocore.h"
#include <memory>

static std::unique_ptr<AtemigoChip8::libretro::LibretroCore> core = nullptr;

void retro_init(void)
{
    core = std::unique_ptr<AtemigoChip8::libretro::LibretroCore>(new AtemigoChip8::libretro::LibretroCore());
}

void retro_deinit(void)
{
    core.reset();
}

// ---------------------------------------------------------------------------
// Assinaturas obrigatórias: o RetroArch recusa a DLL se faltar qualquer uma.
// Corpos vazios de propósito; os retornos são só o mínimo para compilar.
// ---------------------------------------------------------------------------

// Callbacks entregues pelo front-end
void retro_set_environment(retro_environment_t cb)
{
}

void retro_set_video_refresh(retro_video_refresh_t cb)
{
}

void retro_set_audio_sample(retro_audio_sample_t cb)
{
}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
}

void retro_set_input_poll(retro_input_poll_t cb)
{
}

void retro_set_input_state(retro_input_state_t cb)
{
}

// Informações do core
unsigned retro_api_version(void)
{
    return RETRO_API_VERSION;
}

void retro_get_system_info(struct retro_system_info *info)
{
    info->library_name = "Atemigo CHIP-8";
    info->library_version = "0.1";
    info->need_fullpath = false;
    info->valid_extensions = "ch8";
    info->block_extract = false;
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
}

void retro_set_controller_port_device(unsigned port, unsigned device)
{
}

unsigned retro_get_region(void)
{
    return RETRO_REGION_NTSC;
}

// Ciclo de vida do jogo
bool retro_load_game(const struct retro_game_info *game)
{
    return core->loadGame(game);
}

bool retro_load_game_special(unsigned game_type, const struct retro_game_info *info, size_t num_info)
{
    return false;
}

void retro_unload_game(void)
{
}

void retro_reset(void)
{
    core->reset();
}

void retro_run(void)
{
}

// Save state
size_t retro_serialize_size(void)
{
    return core->getStateSize();
}

bool retro_serialize(void *data, size_t size)
{
    return core->saveState(data, size);
}

bool retro_unserialize(const void *data, size_t size)
{
    return core->loadState(data, size);
}

// Cheats
void retro_cheat_reset(void)
{
}

void retro_cheat_set(unsigned index, bool enabled, const char *code)
{
}

// Memória exposta ao front-end (cheats e conquistas)
void *retro_get_memory_data(unsigned id)
{
    return core->getMachineMemory(id);
}

size_t retro_get_memory_size(unsigned id)
{
    return core->getMachineMemorySize(id);
}