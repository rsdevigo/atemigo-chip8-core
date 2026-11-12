#include "libretro.h"
#include "libretrocore.h"
#include <memory>
#include <cstring>
#include <cstdlib>

static std::unique_ptr<AtemigoChip8::libretro::LibretroCore> core = nullptr;

static retro_video_refresh_t videoCB;
static retro_audio_sample_t audioCB;
static retro_audio_sample_batch_t audioBatchCB;
static retro_input_poll_t inputPollCB;
static retro_input_state_t inputStateCB;
static retro_environment_t environCb;
static retro_log_printf_t logCb;


static const char *const kOptColor = "atemigo_chip8_pixel_color";
static const char *const kOptSound = "atemigo_chip8_sound_freq";
static const char *const kOptSpeed = "atemigo_chip8_instructions";

static uint32_t parseColor(const char *v)
{
    if (!strcmp(v, "green"))
        return 0x33FF33;
    if (!strcmp(v, "amber"))
        return 0xFFB000;
    return 0xFFFFFF; // white
}

static const char *const kOptEightX = "atemigo_chip8_eightx_behavior";
static const char *const kOptBNNN = "atemigo_chip8_bnnn_behavior";
static const char *const kOptFX1E = "atemigo_chip8_fx1e_behavior";
static const char *const kOptFXs = "atemigo_chip8_fxs_behavior";

static bool readBool(const char *key, bool fallback)
{
    struct retro_variable var = {key, nullptr};
    if (environCb && environCb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
        return strcmp(var.value, "enabled") == 0;
    return fallback;
}

// Lê as opções atuais do frontend e repassa ao core.
static void applyOptions()
{
    if (!core || !environCb)
        return;

    struct retro_variable var = {kOptColor, nullptr};
    uint32_t color = 0xFFFFFF;
    if (environCb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
        color = parseColor(var.value);

    int freq = 440;
    var = {kOptSound, nullptr};
    if (environCb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
        freq = atoi(var.value);

    // Os valores padrão vêm do construtor de CoreOptions.
    AtemigoChip8::core::CoreOptions co;
    var = {kOptSpeed, nullptr};
    if (environCb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
        co.numberOfInstructs = atoi(var.value);
    co.eightXBehavior = readBool(kOptEightX, co.eightXBehavior);
    co.bNNNBehavior = readBool(kOptBNNN, co.bNNNBehavior);
    co.fX1EBehavior = readBool(kOptFX1E, co.fX1EBehavior);
    co.fXsBehavior = readBool(kOptFXs, co.fXsBehavior);

    core->setOptions(color, freq, co);
}

// Os retro_set_* chegam ANTES do retro_init, então os ponteiros ficam
// guardados aqui e são repassados ao core quando ele é criado.
void retro_init(void)
{
    core = std::unique_ptr<AtemigoChip8::libretro::LibretroCore>(new AtemigoChip8::libretro::LibretroCore());
    core->setVideoRefresh(videoCB);
    core->setAudioSampleBatch(audioBatchCB);
    core->setInputPoll(inputPollCB);
    core->setInputState(inputStateCB);
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
    environCb = cb;

    static const struct retro_variable vars[] = {
        {kOptColor, "Cor do pixel; white|green|amber"},
        {kOptSound, "Frequencia do som (Hz); 440|220|880"},
        {kOptSpeed, "Instrucoes por frame; 12|8|10|15|20|30"},
        // O primeiro valor de cada lista é o padrão (igual ao construtor de CoreOptions).
        {kOptEightX, "8XY6/8XYE desloca VX direto (desligado copia VY antes); enabled|disabled"},
        {kOptBNNN, "BNNN usa VX em vez de V0 (estilo SUPER-CHIP); disabled|enabled"},
        {kOptFX1E, "FX1E seta VF se I passar de 0xFFF; enabled|disabled"},
        {kOptFXs, "FX55/FX65 mantem I inalterado (desligado incrementa I); enabled|disabled"},
        {nullptr, nullptr}};
    cb(RETRO_ENVIRONMENT_SET_VARIABLES, (void *)vars);

    // Teclado hexa 0-F mapeado nos botões do RetroPad (ver readKeys no core).
    static const char *const names[16] = {
        "Tecla 0", "Tecla 1", "Tecla 2", "Tecla 3", "Tecla 4", "Tecla 5", "Tecla 6", "Tecla 7",
        "Tecla 8", "Tecla 9", "Tecla A", "Tecla B", "Tecla C", "Tecla D", "Tecla E", "Tecla F"};
    static const unsigned ids[16] = {
        RETRO_DEVICE_ID_JOYPAD_B, RETRO_DEVICE_ID_JOYPAD_UP, RETRO_DEVICE_ID_JOYPAD_DOWN,
        RETRO_DEVICE_ID_JOYPAD_LEFT, RETRO_DEVICE_ID_JOYPAD_RIGHT, RETRO_DEVICE_ID_JOYPAD_A,
        RETRO_DEVICE_ID_JOYPAD_X, RETRO_DEVICE_ID_JOYPAD_Y, RETRO_DEVICE_ID_JOYPAD_L,
        RETRO_DEVICE_ID_JOYPAD_R, RETRO_DEVICE_ID_JOYPAD_L2, RETRO_DEVICE_ID_JOYPAD_R2,
        RETRO_DEVICE_ID_JOYPAD_SELECT, RETRO_DEVICE_ID_JOYPAD_START,
        RETRO_DEVICE_ID_JOYPAD_L3, RETRO_DEVICE_ID_JOYPAD_R3};
    static struct retro_input_descriptor desc[17];
    for (int i = 0; i < 16; i++)
        desc[i] = {0, RETRO_DEVICE_JOYPAD, 0, ids[i], names[i]};
    desc[16] = {0, 0, 0, 0, nullptr};
    cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, desc);

    // Core que roda ROM, não precisa de jogo para iniciar: false (padrão).
    bool noGame = false;
    cb(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &noGame);

    // Logger do frontend (opcional, ajuda a depurar)
    struct retro_log_callback log;
    if (cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &log))
        logCb = log.log;
}

void retro_set_video_refresh(retro_video_refresh_t cb)
{
    videoCB = cb;
}

void retro_set_audio_sample(retro_audio_sample_t cb)
{
    audioCB = cb;
}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
    audioBatchCB = cb;
}

void retro_set_input_poll(retro_input_poll_t cb)
{
    inputPollCB = cb;
}

void retro_set_input_state(retro_input_state_t cb)
{
    inputStateCB = cb;
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
    core->getMachineInfo(info);
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
    enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_XRGB8888;
    if (!environCb || !environCb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
        return false;

    if (!core->loadGame(game))
        return false;
    applyOptions();
    return true;
}

bool retro_load_game_special(unsigned game_type, const struct retro_game_info *info, size_t num_info)
{
    return false;
}

void retro_unload_game(void)
{
     core->unloadGame();
}

void retro_reset(void)
{
    core->reset();
}

void retro_run(void)
{
    bool updated = false;
    if (environCb && environCb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
        applyOptions();

    core->runFrame();

    if (core->takeHaltNotice() && environCb)
    {
        struct retro_message msg = {"Atemigo CHIP-8: a execucao da ROM parou.", 180};
        environCb(RETRO_ENVIRONMENT_SET_MESSAGE, &msg);
    }
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