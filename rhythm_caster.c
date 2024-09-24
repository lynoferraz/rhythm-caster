#include <riv.h>
#include <string.h>
#include <math.h>
#include <stdio.h> 
#include <stdlib.h>
// #include <spng.h>
#define SEQT_IMPL
#include "seqt.h"

enum {
    N_MESSAGE_ROWS = 16,
    MAX_MESSAGE_SIZE = 50,
    MAGIC_SIZE = 4,
    MAP_SIZE = 16,
    // TARGET_FPS = 30,
    TILE_SIZE = 24,
    WIZARD_SIZE = 50,
    WIZARD_WALK_FRAMES = 4,
    SCREEN_SIZE = TILE_SIZE*MAP_SIZE,
    CASTING_TOP_Y = TILE_SIZE*14+2,
    POSSIBLE_CAST_Y = TILE_SIZE*13,
    POSSIBLE_LEFT_X = TILE_SIZE,

    MAP_PLAYER_GROUND = 0,
    MAP_LAYER_BLOCKABLE = 1,
    MAP_LAYER_ITEMS = 2,
    MAP_LAYER_DECORATIONS = 3,
    // MAP_LAYER_MONSTERS = 4,
    MAP_LAYERS = 4, //5,

    TIME_SIG = 4,

    BEAT_TICKS = 6,

    MAX_CAST = 8,
    MAX_SPELLBOOK = 30,
    HELP_FRAMES = 480,
    MAX_SPELL_REACH_TILES = 25,
    
    PROTECTION_CIRCLES = 6,
    NOVA_CIRCLES = 4,
    FROST_PARTICLES = 10,
    FIREBALL_REACH = 5,
    WINDSLASH_REACH = 2,

    HEAD_COLORS = 4,

};

enum {
    DIR_DOWN,
    DIR_LEFT,
    DIR_UP,
    DIR_RIGHT,
};


////
// Souds

riv_waveform_desc error_sfx = {
    .type = RIV_WAVEFORM_PULSE,
    .attack = 0.01f, .decay = 0.03f, .sustain = 0.2f, .release = 0.01f,
    .start_frequency = RIV_NOTE_A3, .end_frequency = RIV_NOTE_A2,
    .amplitude = 0.3f, .sustain_level = 0.5f,
};

riv_waveform_desc frostbite_sfx = {
    .type = RIV_WAVEFORM_PULSE,
    .attack = 0.025f, .decay = 0.075f, .sustain = 0.05f, .release = 0.2f,
    .start_frequency = RIV_NOTE_B7, .end_frequency = RIV_NOTE_D5,
    .amplitude = 0.15f, .sustain_level = 0.425f,
};

riv_waveform_desc windslash_sfx = {
    .type = RIV_WAVEFORM_NOISE,
    .attack = 0.125f, .decay = 0.5f, .sustain = 0.0f, .release = 1.0f,
    .start_frequency = RIV_NOTE_A3, .end_frequency = RIV_NOTE_D5,
    .amplitude = 0.15f, .sustain_level = 0.425f,
};

riv_waveform_desc fireball_sfx = {
    .type = RIV_WAVEFORM_NOISE,
    .attack = 0.2f, .decay = 0.075f, .sustain = 0.05f, .release = 0.2f,
    .start_frequency = RIV_NOTE_Db5, .end_frequency = RIV_NOTE_Db7,
    .amplitude = 0.15f, .sustain_level = 0.425f,
};

riv_waveform_desc nova_sfx = {
    .type = RIV_WAVEFORM_NOISE,
    .attack = 0.225f, .decay = 0.275f, .sustain = 0.2f, .release = 0.2f,
    .start_frequency = RIV_NOTE_C4, .end_frequency = RIV_NOTE_Eb5,
    .amplitude = 0.15f, .sustain_level = 0.425f,
};

riv_waveform_desc protection_sfx = {
    .type = RIV_WAVEFORM_TRIANGLE,
    .attack = 0.125f, .decay = 0.275f, .sustain = 0.45f, .release = 0.275f,
    .start_frequency = RIV_NOTE_Bb4, .end_frequency = RIV_NOTE_Gb4,
    .amplitude = 0.1f, .sustain_level = 0.425f,
};

riv_waveform_desc secret_sfx = {
    .type = RIV_WAVEFORM_NOISE,
    .attack = 0.49f, .decay = 0.5f, .sustain = 0.35f, .release = 0.639f,
    .start_frequency = RIV_NOTE_A2, .end_frequency = RIV_NOTE_Bb4,
    .amplitude = 0.25f, .sustain_level = 0.34f, .duty_cycle = 1.0f
};

////
// types


////
// Conf

int message_row_dist = SCREEN_SIZE / N_MESSAGE_ROWS;


static int16_t map[MAP_LAYERS][MAP_SIZE][MAP_SIZE] = {
    {
        {182,182,182,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,182,182,182,},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
        {-1,-1,-1,-1,212,223,223,223,223,223,223,223,-1,-1,-1,-1,},
        {-1,222,223,224,225,238,238,238,238,238,238,238,227,228,229,-1,},
        {-1,235,238,192,238,238,205,238,238,192,238,238,238,238,242,-1,},
        {-1,235,238,238,238,238,238,238,238,238,238,238,238,238,242,-1,},
        {-1,235,238,192,238,192,238,238,238,238,206,238,238,238,242,-1,},
        {-1,235,238,238,238,238,238,238,238,238,193,238,238,238,242,-1,},
        {-1,235,238,238,219,238,238,238,238,238,193,193,193,238,242,-1,},
        {-1,235,238,238,238,238,238,238,238,238,238,238,193,193,242,-1,},
        {-1,235,238,238,238,192,238,238,238,238,238,238,238,238,242,-1,},
        {-1,235,238,238,205,238,238,238,238,193,238,238,238,238,242,-1,},
        {-1,235,218,238,238,238,238,238,238,238,238,238,238,238,242,-1,},
        {-1,235,238,238,238,238,238,238,238,238,238,238,238,238,242,-1,},
        {-1,-1,-1,-1,252,251,252,252,252,252,252,252,-1,-1,-1,-1,},
        {182,182,182,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,182,182,182,},
    },
    {
        {-1,-1,-1,185,186,197,197,231,186,232,233,197,188,-1,-1,-1,},
        {195,196,197,198,199,210,210,244,199,245,246,210,201,202,203,204,},
        {208,209,210,211,-1,-1,-1,-1,-1,-1,-1,3,214,215,216,217,},
        {221,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,230,},
        {234,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,243,},
        {234,-1,-1,-1,-1,-1,-1,6,-1,-1,-1,-1,-1,-1,-1,243,},
        {234,-1,-1,-1,-1,-1,-1,19,-1,-1,-1,-1,0,-1,-1,243,},
        {234,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,243,},
        {234,-1,-1,14,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,243,},
        {234,-1,-1,-1,-1,15,-1,-1,5,-1,-1,-1,-1,-1,-1,243,},
        {234,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,6,-1,243,},
        {234,-1,-1,6,-1,-1,-1,-1,-1,-1,-1,-1,-1,19,-1,243,},
        {234,-1,-1,19,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,243,},
        {234,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,243,},
        {247,248,249,250,-1,-1,-1,-1,-1,-1,-1,-1,253,254,255,256,},
        {-1,-1,-1,263,265,264,265,265,265,265,265,265,266,-1,-1,-1,},
    },
    {
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,1,-1,-1,},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
    },
    {
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
        {-1,-1,-1,-1,-1,32,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
        {-1,40,-1,2,-1,-1,-1,-1,-1,-1,-1,-1,45,40,-1,-1,},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
    },
    // {
    //     {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
    //     {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
    //     {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
    //     {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
    //     {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
    //     {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
    //     {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
    //     {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
    //     {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
    //     {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
    //     {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
    //     {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
    //     {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
    //     {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
    //     {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
    //     {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
    // }
};

static int arrows_positions[4][6] = {
    {0,0,  20,0, 10,20},
    {0,10, 20,0, 20,20},
    {0,20, 10,0, 20,20},
    {0,0,  0,20, 20,10},
};

enum {
    FROSTBITE1_SPELL,
    FROSTBITE2_SPELL,
    FROSTBITE3_SPELL,
    FIREBALL_SPELL,
    WINDSLASH_SPELL,
    NOVA_SPELL,
    PROTECTION_SPELL,
    SECRET_SPELL,
};

static int16_t spellbook[][MAX_CAST+1] = {
    {DIR_RIGHT,DIR_UP,-1,-2,-2,-2,-2,-2,PROTECTION_SPELL},
    {DIR_LEFT,DIR_DOWN,-1,-2,-2,-2,-2,-2,PROTECTION_SPELL},
    {DIR_DOWN,DIR_LEFT,-1,-2,-2,-2,-2,-2,PROTECTION_SPELL},
    {DIR_UP,DIR_RIGHT,-1,-2,-2,-2,-2,-2,PROTECTION_SPELL},
    {DIR_LEFT,DIR_LEFT,-1,-2,-2,-2,-2,-2,FROSTBITE1_SPELL},
    {DIR_RIGHT,DIR_RIGHT,-1,-2,-2,-2,-2,-2,FROSTBITE1_SPELL},
    {DIR_UP,DIR_UP,-1,-2,-2,-2,-2,-2,FROSTBITE1_SPELL},
    {DIR_DOWN,DIR_DOWN,-1,-2,-2,-2,-2,-2,FROSTBITE1_SPELL},
    {DIR_LEFT,DIR_LEFT,DIR_LEFT,-1,-2,-2,-2,-2,FROSTBITE2_SPELL},
    {DIR_RIGHT,DIR_RIGHT,DIR_RIGHT,-1,-2,-2,-2,-2,FROSTBITE2_SPELL},
    {DIR_UP,DIR_UP,DIR_UP,-1,-2,-2,-2,-2,FROSTBITE2_SPELL},
    {DIR_DOWN,DIR_DOWN,DIR_DOWN,-1,-2,-2,-2,-2,FROSTBITE2_SPELL},
    {DIR_LEFT,DIR_LEFT,DIR_LEFT,DIR_LEFT,-1,-2,-2,-2,FROSTBITE3_SPELL},
    {DIR_RIGHT,DIR_RIGHT,DIR_RIGHT,DIR_RIGHT,-1,-2,-2,-2,FROSTBITE3_SPELL},
    {DIR_UP,DIR_UP,DIR_UP,DIR_UP,-1,-2,-2,-2,FROSTBITE3_SPELL},
    {DIR_DOWN,DIR_DOWN,DIR_DOWN,DIR_DOWN,-1,-2,-2,-2,FROSTBITE3_SPELL},
    {DIR_RIGHT,DIR_RIGHT,DIR_LEFT,DIR_LEFT,-1,-2,-2,-2,FIREBALL_SPELL},
    {DIR_LEFT,DIR_LEFT,DIR_RIGHT,DIR_RIGHT,-1,-2,-2,-2,FIREBALL_SPELL},
    {DIR_UP,DIR_UP,DIR_DOWN,DIR_DOWN,-1,-2,-2,-2,FIREBALL_SPELL},
    {DIR_DOWN,DIR_DOWN,DIR_UP,DIR_UP,-1,-2,-2,-2,FIREBALL_SPELL},
    {DIR_UP,DIR_DOWN,DIR_UP,-1,-2,-2,-2,-2,WINDSLASH_SPELL},
    {DIR_RIGHT,DIR_LEFT,DIR_RIGHT,-1,-2,-2,-2,-2,WINDSLASH_SPELL},
    {DIR_LEFT,DIR_RIGHT,DIR_LEFT,-1,-2,-2,-2,-2,WINDSLASH_SPELL},
    {DIR_DOWN,DIR_UP,DIR_DOWN,-1,-2,-2,-2,-2,WINDSLASH_SPELL},
    {DIR_LEFT,DIR_DOWN,DIR_RIGHT,DIR_UP,-1,-2,-2,-2,NOVA_SPELL},
    {DIR_RIGHT,DIR_DOWN,DIR_LEFT,DIR_UP,-1,-2,-2,-2,NOVA_SPELL},
    {DIR_LEFT,DIR_RIGHT,DIR_LEFT,DIR_RIGHT,DIR_DOWN,DIR_DOWN,DIR_UP,DIR_UP,SECRET_SPELL},
};

static float spell_beats_duration[] = {
    2.0, //FROSTBITE1_SPELL,
    3.0, //FROSTBITE2_SPELL,
    4.0, //FROSTBITE3_SPELL,
    1.0, //FIREBALL_SPELL,
    0.75, //WINDSLASH_SPELL,
    4.0, //NOVA_SPELL,
    8.0, //PROTECTION_SPELL,
    5.0, //SECRET_SPELL,
};

static bool unlocked_spells[] = {
    true, //FROSTBITE1_SPELL,
    false, //FROSTBITE2_SPELL,
    false, //FROSTBITE3_SPELL,
    false, //FIREBALL_SPELL,
    false, //WINDSLASH_SPELL,
    false, //NOVA_SPELL,
    true, //PROTECTION_SPELL,
    false, //SECRET_SPELL,
};

static int frostbite_colors[] = {
    RIV_COLOR_WHITE,RIV_COLOR_LIGHTGREY, RIV_COLOR_LIGHTSLATE, RIV_COLOR_LIGHTBLUE,RIV_COLOR_LIGHTTEAL,RIV_COLOR_WHITE,RIV_COLOR_LIGHTGREY
};

static int spells_effect_area[][MAX_SPELL_REACH_TILES][3] = { // model {active,facing_axis,side_axis}
    { // ATTACK_SPELL - frost 1
        {1,1,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}
    },
    { // ATTACK_SPELL - frost 2
        {1,1,0},{1,2,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}
    },
    { // ATTACK_SPELL - frost 3
        {1,1,0},{1,2,0},{1,3,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}
    },
    { // FIREBALL_SPELL - fireball
        {1,1,0},{1,2,0},{1,3,0},{1,4,0},{1,5,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}
    },
    { // WINDSLASH_SPELL - wind slash
        {1,1,0},{1,2,0},{1,1,1},{1,1,-1},{1,0,1},{1,0,-1},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}
    },
    { // NOVA_SPELL
        {1,1,0},{1,-1,0},{1,0,1},{1,0,-1},{1,2,0},{1,-2,0},{1,0,2},{1,0,-2},{1,1,1},{1,-1,1},{1,1,-1},{1,-1,-1},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}
    },
    { // PROTECTION_SPELL
        {1,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}
    },
    { // SECRET_SPELL
        {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}
    },
};

static uint8_t head_colors[4];

////
// State

// Sprite sheets
int wizard_sps;
int tileset_sps;


bool started = false;
bool ended = false;
// Music music;
int start_frame;

int music_bpm = 0;
int last_note_frame = -1;
int last_beat_frame;
// int note_frame;
// int note_frame_start = 0;
// int note_frame_end = 0;
float note_time = 0.0;
float hits_per_second;
float note_period;
float beat_guide_tick_size;
float frames_per_beat;

int synch_hit;
int moving_frames;
int moved_frames;
int speed;

riv_vec2f player_pos;
int player_direction;
bool chain_cast = false;
static int cast_queue[MAX_CAST];
int last_cast_key_press_frame;
bool casting = false;
int released_spell = -1;
int completed_spell = -1;
int release_spell_frame = 0;
int release_spell_anim_frames = 0;
riv_vec2f release_spell_pos;
int release_spell_direction;
int n_spells;
int n_frostbite_colors;
int last_move_frame = 0;
int last_move_dir = 0;
riv_recti wizard_bbox;

bool board_help;
bool chest_help;
int help_frame;

////
// Aux functions

void start_game() {
    started = true;
    start_frame = riv->frame;
    hits_per_second = music_bpm*((1.0*TIME_SIG)/60);
    note_period = hits_per_second/riv->target_fps;
    frames_per_beat = riv->target_fps/(music_bpm/60.0);
    beat_guide_tick_size = (1.0 * SCREEN_SIZE) / (BEAT_TICKS * frames_per_beat);
    synch_hit = (int)(frames_per_beat * 0.25);
    moving_frames = (int)ceil(frames_per_beat / 4);
    speed = (int)ceil((1.0*TILE_SIZE)/moving_frames);
    // release_spell_anim_frames = (int)ceil(frames_per_beat * 6 / 4);

    int minx = WIZARD_SIZE;
    int maxw = 0;
    int miny = WIZARD_SIZE;
    int maxh = 0;
    for (int i=0; i<16; i++)  {
        riv_recti r = riv_get_sprite_bbox(i,wizard_sps,1,1);
        if (r.x < minx) minx = r.x;
        if (r.width > maxw) maxw = r.width;
        if (r.y < miny) miny = r.y;
        if (r.height > maxh) maxh = r.height;
    }
    int got_colors = 0;
    int sps_width = riv->images[riv->spritesheets[wizard_sps].image_id].width;
    for (int i=0; i<riv->spritesheets[wizard_sps].cell_height; i++)  {
        if (got_colors >= HEAD_COLORS) break;
        for (int j=0; j<riv->spritesheets[wizard_sps].cell_width; j++)  {
            if (got_colors >= HEAD_COLORS) break;
            uint8_t color = *(riv->images[riv->spritesheets[wizard_sps].image_id].pixels + j + i*sps_width);
            if (color < 255) {
                int c = 0;
                bool has_color = false;
                for (; c<HEAD_COLORS; c++) {
                    if (head_colors[c] == 0) break;
                    if (head_colors[c] == color) {
                        has_color = true;
                        break;
                    }

                }
                if (has_color) continue;
                if (c<HEAD_COLORS) { // new color
                    head_colors[c] = color;
                }
            }
        }
    }

    minx = minx + maxw/2 - TILE_SIZE/2;
    maxw = TILE_SIZE;

    wizard_bbox.x = minx;
    wizard_bbox.y = miny+maxh-maxw;
    wizard_bbox.width = maxw;
    wizard_bbox.height = maxw;
    int start_x = 4;
    int start_y = 6;
    // for (int x=3; start_x < 0 || start_y < 0 || x<MAP_SIZE; x++)  {
    //     for (int y=3; y<MAP_SIZE; y++)  {
    //         if (map[MAP_LAYER_BLOCKABLE][y][x] > 0) {
    //             start_x = x;
    //             start_y = y;
    //             break;
    //         }
    //     }
    // }

    player_pos.x = start_x*TILE_SIZE - wizard_bbox.x + (TILE_SIZE-wizard_bbox.width)/2 ;
    player_pos.y = start_y*TILE_SIZE - wizard_bbox.y + (TILE_SIZE-wizard_bbox.height)/2;
    player_direction = DIR_DOWN;
    last_cast_key_press_frame = 0;
    for (int i=0; i<MAX_CAST; i++) cast_queue[i] = -1;

    n_spells = sizeof(spellbook) / sizeof(spellbook[0]);
    n_frostbite_colors = sizeof(frostbite_colors) / sizeof(frostbite_colors[0]);
    riv_printf("START\n");
}

int64_t clamp(int64_t v, int64_t min, int64_t max) { v = v < min ? min : v; v = v > max ? max : v; return v; }

int16_t collides_with_layer(riv_vec2f pos, int l) {
    int y = pos.y / TILE_SIZE;
    int x = pos.x / TILE_SIZE;
    if (map[l][y][x] > 0) {
        return map[l][y][x];
    }
    return 0;
}

void initialize() {


    if (riv->incard_len > 0) {

        read_incard_data(riv->incard,0,riv->incard_len);

    }

    if (!music_bpm) {
        seqt_init();
        seqt_play(seqt_make_source_from_file("04.seqt.rivcard"), -1);
        seqt_sound *sound = &seqt.sounds[1];
        music_bpm = sound->source->bpm;

    }

    riv->outcard_len = riv_snprintf((char*)riv->outcard, RIV_SIZE_OUTCARD, 
        "JSON{\"score\":0}");

}

void end_session() {
    riv_printf("SESSION OVER\n");
    ended = true;
    // Quit in 2 seconds
    riv->quit_frame = riv->frame + 1*riv->target_fps;
}

void read_incard_data(uint8_t *data,int from,int size) {
    char magic[5];
    for (int i=0; i<MAGIC_SIZE ; i++) magic[i] = data[from + i];
    magic[MAGIC_SIZE] = '\0';
    // riv_printf("MAGIC: %s\n",magic);
    if (!strcmp(magic,"SEQT")) {
        if (music_bpm) {
            return;
        }
        seqt_init();
        // music_bpm = music->bpm;
        seqt_play((seqt_source*)(data + from),-1);
        seqt_sound *sound = &seqt.sounds[1];
        seqt_source* music = sound->source;
        music_bpm = sound->source->bpm;

        // music = (Music)(data + from);

        // riv_printf("music magic %s\n",music->magic);
        // riv_printf("music version %d\n",music->version);
        // riv_printf("music flags %d\n",music->flags);
        // riv_printf("music bpm %d\n",music->bpm);


    // } else if (!strcmp(magic,"SPRS")) {

    //     spng_ctx *ctx = spng_ctx_new(0);

    //     if(ctx == NULL) {
    //         riv_printf("spng_ctx_new() failed\n");
    //         return;
    //     }

    //     int ret = spng_decode_image(ctx, image, image_size, SPNG_FMT_RGBA8, 0);
    //     riv->images;

    //     wizard_sps = riv_make_spritesheet(riv_make_image("wizard-spritesheet-53.png", 255), WIZARD_SIZE, WIZARD_SIZE);
    //     spng_ctx_free(ctx);

    } else if (!strcmp(magic,"MICS")) {
        int wi = from + MAGIC_SIZE; // curr_word_index

        uint32_t n_incards = data[from + wi+3] | (uint32_t)data[from + wi+2] << 8
            | (uint32_t)data[from + wi+1] << 16 | (uint32_t)data[from + wi] << 24;
        wi += MAGIC_SIZE;

        for (int i=0; i<n_incards ; i++) {

            uint32_t from_incard_i = data[from + wi+3] | (uint32_t)data[from + wi+2] << 8
                | (uint32_t)data[from + wi+1] << 16 | (uint32_t)data[from + wi] << 24;
            wi += MAGIC_SIZE;

            uint32_t size_incard_i =data[from + wi+3] | (uint32_t)data[from + wi+2] << 8
                | (uint32_t)data[from + wi+1] << 16 | (uint32_t)data[from + wi] << 24;
            wi += MAGIC_SIZE;

            read_incard_data(data,from_incard_i,size_incard_i);
        }

    }
}


void play_music() {
    if (!music_bpm) return;
    seqt_poll();

    note_time = note_time + note_period;
    int note_base_frame = (int)note_time;

    if (note_base_frame != last_note_frame) {
        if (note_base_frame%TIME_SIG==0)
            last_beat_frame = riv->frame;
        last_note_frame = note_base_frame;
    }

}


void check_completed_spell() {

    completed_spell = -1;
    bool possible_spells[MAX_SPELLBOOK];
    for (int s=0; s<n_spells; s++) possible_spells[s] = false;

    for (int i=0; i<MAX_CAST; i++) {
        for (int s=0; s<n_spells; s++) {
            if (i == 0) {
                if (spellbook[s][i] == cast_queue[i]) {
                    possible_spells[s] = true;
                }
                continue;
            }
            if (!possible_spells[s] || spellbook[s][i] == -2) continue;
            if (spellbook[s][i] == -1) {
                if (!unlocked_spells[spellbook[s][MAX_CAST]]) continue;
                completed_spell = spellbook[s][MAX_CAST];
                continue;
            }
            if (spellbook[s][i] != cast_queue[i]) {
                possible_spells[s] = false;
            }
            if (possible_spells[s] && i == MAX_CAST - 1) {
                if (!unlocked_spells[spellbook[s][MAX_CAST]]) continue;
                completed_spell = spellbook[s][MAX_CAST];
                continue;
            }
        }
    }
}

////
// Update functions


void update_state() {
    if (riv->keys[RIV_GAMEPAD1_SELECT].press) {
        end_session();
        return;
    }
    play_music();


    int diff_frames = riv->frame - last_beat_frame;
    bool in_synch_frame = diff_frames < synch_hit || diff_frames > frames_per_beat - synch_hit;
    bool move = false;


    if (riv->keys[RIV_GAMEPAD_RIGHT].down) {
        player_direction = DIR_RIGHT;
    }
    if (riv->keys[RIV_GAMEPAD_LEFT].down) {
        player_direction = DIR_LEFT;
    }
    if (riv->keys[RIV_GAMEPAD_DOWN].down) {
        player_direction = DIR_DOWN;
    }
    if (riv->keys[RIV_GAMEPAD_UP].down) {
        player_direction = DIR_UP;
    }
    
    if (riv->keys[RIV_GAMEPAD_A2].down) {
        casting = true;
    } else {
        casting = false;
    }

    bool key_press = false;
    if (in_synch_frame && 
            (riv->keys[RIV_GAMEPAD_RIGHT].press || riv->keys[RIV_GAMEPAD_LEFT].press || 
                riv->keys[RIV_GAMEPAD_DOWN].press || riv->keys[RIV_GAMEPAD_UP].press)) {
        key_press = true;
    }

    // reset casting if pressed out of synch
    if (!in_synch_frame && 
            (riv->keys[RIV_GAMEPAD_RIGHT].press || riv->keys[RIV_GAMEPAD_LEFT].press || 
                riv->keys[RIV_GAMEPAD_DOWN].press || riv->keys[RIV_GAMEPAD_UP].press)) {
        chain_cast = false;
        for (int i=0; i<MAX_CAST; i++) cast_queue[i] = -1;
        completed_spell = -1;
        // released_spell = -1;
        riv_waveform(&error_sfx);
    }

    // reset casting if missed synch
    if (casting && cast_queue[0] != -1 && riv->frame > last_cast_key_press_frame + frames_per_beat + synch_hit ) {
        chain_cast = false;
        for (int i=0; i<MAX_CAST; i++) cast_queue[i] = -1;
        completed_spell = -1;
    }

    if (key_press && casting && riv->frame > last_cast_key_press_frame + frames_per_beat/2) {
        int i=0;
        for (; i<MAX_CAST; i++) {
            if (cast_queue[i] == -1) break;
        }
        if (i < MAX_CAST) {
            cast_queue[i] = player_direction;
            chain_cast = true;
            check_completed_spell();
        }
        
        last_cast_key_press_frame = riv->frame;
    }

    if (key_press && !casting && riv->frame > last_move_frame + moving_frames) {
        int dx = 0;
        int dy = 0;
        if (player_direction == DIR_LEFT) dx = -TILE_SIZE;
        else if (player_direction == DIR_RIGHT) dx = TILE_SIZE;
        else if (player_direction == DIR_UP) dy = -TILE_SIZE;
        else if (player_direction == DIR_DOWN) dy = TILE_SIZE;
        riv_vec2f new_postl = {
            player_pos.x+dx+wizard_bbox.x,
            player_pos.y+dy+wizard_bbox.y};
        if (!(collides_with_layer(new_postl, MAP_LAYER_BLOCKABLE) > 0)) {
            move = true;
            last_move_frame = riv->frame;
            last_move_dir = player_direction;
            moved_frames = 0;
        }
    }

    if (released_spell > -1 && riv->frame > release_spell_frame + release_spell_anim_frames) {
        released_spell = -1;
        check_completed_spell();
    }

    if (!casting && riv->keys[RIV_GAMEPAD_A1].press) {
        if (completed_spell > -1) {
            released_spell = completed_spell;
            release_spell_frame = riv->frame;
            release_spell_anim_frames = (int)ceil(frames_per_beat * spell_beats_duration[released_spell]);
            completed_spell = -1;
             // top left of player base
            release_spell_pos.x = player_pos.x+wizard_bbox.x;
            release_spell_pos.y = player_pos.y+wizard_bbox.y;
            release_spell_direction = player_direction;
        }
        
        // release
        chain_cast = false;
        for (int i=0; i<MAX_CAST; i++) cast_queue[i] = -1;
    }


    if (riv->frame <= last_move_frame + moving_frames && moved_frames < TILE_SIZE) {
        int final_speed = speed;
        if (moved_frames + final_speed > TILE_SIZE)
            final_speed = TILE_SIZE - moved_frames;
        
        int dx = 0;
        int dy = 0;
        if (last_move_dir == DIR_LEFT) dx = -final_speed;
        else if (last_move_dir == DIR_RIGHT) dx = final_speed;
        else if (last_move_dir == DIR_UP) dy = -final_speed;
        else if (last_move_dir == DIR_DOWN) dy = final_speed;

        player_pos.x += dx;
        player_pos.y += dy;
        moved_frames += final_speed;

        if (moved_frames == TILE_SIZE) {
            riv_vec2f player_base = {
                player_pos.x+wizard_bbox.x,
                player_pos.y+wizard_bbox.y};
            // collision with chest
            if (collides_with_layer(player_base, MAP_LAYER_ITEMS) == 1) {
                map[MAP_LAYER_ITEMS][(int)(player_base.y/TILE_SIZE)][(int)(player_base.x/TILE_SIZE)] = 4; // open chest
                // do something
                riv_printf("  On chest!!\n");
                unlocked_spells[FROSTBITE2_SPELL] = true;
                unlocked_spells[FIREBALL_SPELL] = true;
                help_frame = riv->frame;
                chest_help = true;
            }
            
            // collision with front of board
            if (player_base.y >= TILE_SIZE) {
                riv_vec2f pos_up = {
                    player_base.x,
                    player_base.y - TILE_SIZE};
                // riv_printf("  pos %f %f %i %i\n",pos_up.y/TILE_SIZE,pos_up.x/TILE_SIZE,(int)(pos_up.y/TILE_SIZE),(int)(pos_up.x/TILE_SIZE));
                if (collides_with_layer(pos_up, MAP_LAYER_DECORATIONS) == 2) {
                    // do something
                    riv_printf("  Reading board!!\n");
                    help_frame = riv->frame;
                    board_help = true;
                }
            }
        }
    }

}



////
// Draw functions

void draw_start_screen() {
    // Draw title
    riv_draw_text(
        "Rhythm Spell Caster",       // text to draw
        RIV_SPRITESHEET_FONT_5X7,   // sprite sheet id of the font
        RIV_CENTER,                 // anchor point on the text bounding box
        SCREEN_SIZE/2,              // anchor x
        SCREEN_SIZE/2,              // anchor y
        2,                          // text size multiplier
        RIV_COLOR_LIGHTGREEN        // text color
    );
    // Make "press to start blink" by changing the color depending on the frame number
    uint32_t col = ((riv->frame / 15) % 2 == 0) ? RIV_COLOR_LIGHTRED : RIV_COLOR_DARKRED;
    // Draw press to start
    riv_draw_text("PRESS TO START", RIV_SPRITESHEET_FONT_5X7, RIV_CENTER, SCREEN_SIZE/2, SCREEN_SIZE/2+32, 1, col);
}

void draw_beat_guide() {

    int diff_frames = riv->frame - last_beat_frame;
    int dx = diff_frames;
    int delta_x = round(diff_frames * beat_guide_tick_size);

    riv_draw_rect_fill(0,SCREEN_SIZE - TILE_SIZE, SCREEN_SIZE, 2, RIV_COLOR_SLATE);

    for (int i=0; i<BEAT_TICKS; i++)  {
        riv_draw_rect_fill(SCREEN_SIZE - delta_x - i * SCREEN_SIZE/6,SCREEN_SIZE - 1.5 * TILE_SIZE, 2, TILE_SIZE, RIV_COLOR_SLATE);
    }

    riv_draw_rect_fill(SCREEN_SIZE/2 - TILE_SIZE/2 + dx/2,SCREEN_SIZE - 1.5 * TILE_SIZE, TILE_SIZE - dx, TILE_SIZE, RIV_COLOR_SLATE);
    riv_draw_rect_fill(SCREEN_SIZE/2 - 1,SCREEN_SIZE - 1.5 * TILE_SIZE, 2, TILE_SIZE, RIV_COLOR_LIGHTSLATE);
}


void draw_cast_moves() {

    for (int i=0; i<MAX_CAST; i++) {
        if (cast_queue[i] == -1) break;
        
        riv_draw_triangle_fill(
            2+(1+i)*TILE_SIZE + arrows_positions[cast_queue[i]][0], CASTING_TOP_Y + arrows_positions[cast_queue[i]][1],  // p0
            2+(1+i)*TILE_SIZE + arrows_positions[cast_queue[i]][2], CASTING_TOP_Y + arrows_positions[cast_queue[i]][3],  // p1
            2+(1+i)*TILE_SIZE + arrows_positions[cast_queue[i]][4], CASTING_TOP_Y + arrows_positions[cast_queue[i]][5],  // p2
            RIV_COLOR_WHITE
        );
    }
}

void draw_spell() {

    //riv_draw_rect_line(player_pos.x+wizard_bbox.x,player_pos.y+wizard_bbox.y,wizard_bbox.width,wizard_bbox.height,RIV_COLOR_YELLOW);
    if (released_spell > -1 && riv->frame <= release_spell_frame + release_spell_anim_frames) {

        // draw spell effect
        riv_vec2f player_base = {
            player_pos.x+wizard_bbox.x,
            player_pos.y+wizard_bbox.y};
        int release_spell_x = released_spell != PROTECTION_SPELL ? release_spell_pos.x : player_base.x;
        int release_spell_y = released_spell != PROTECTION_SPELL ? release_spell_pos.y : player_base.y;
        int release_spell_tile_x = release_spell_x/TILE_SIZE;
        int release_spell_tile_y = release_spell_y/TILE_SIZE;
        for(uint8_t effect_tile = 0; effect_tile < MAX_SPELL_REACH_TILES; effect_tile++) {
            if (!spells_effect_area[released_spell][effect_tile][0]) break;
            int x;
            int y;

            if (release_spell_direction == DIR_DOWN) {
                x = spells_effect_area[released_spell][effect_tile][2];
                y = spells_effect_area[released_spell][effect_tile][1];
            } else if (release_spell_direction == DIR_LEFT) {
                x = -spells_effect_area[released_spell][effect_tile][1];
                y = spells_effect_area[released_spell][effect_tile][2];
            } else if (release_spell_direction == DIR_UP) {
                x = spells_effect_area[released_spell][effect_tile][2];
                y = -spells_effect_area[released_spell][effect_tile][1];
            } else if (release_spell_direction == DIR_RIGHT) {
                x = spells_effect_area[released_spell][effect_tile][1];
                y = spells_effect_area[released_spell][effect_tile][2];
            }
            riv_draw_rect_line(release_spell_tile_x*TILE_SIZE+x*TILE_SIZE,release_spell_tile_y*TILE_SIZE+y*TILE_SIZE,TILE_SIZE,TILE_SIZE,RIV_COLOR_YELLOW);
        }

        // char buf[128];
        if (released_spell == FROSTBITE1_SPELL || released_spell == FROSTBITE2_SPELL || released_spell == FROSTBITE3_SPELL ) {
            // riv_snprintf(buf, sizeof(buf), "Attack 1");
            int curr_anim_frames = riv->frame-release_spell_frame;
            if (curr_anim_frames%((int)frames_per_beat/4) == 0) 
                riv_waveform(&frostbite_sfx);

            int base_x;
            int base_y;
            int coef_x;
            int coef_y;
            int frostbite_reach = 1;
            if (released_spell == FROSTBITE2_SPELL) {
                frostbite_reach = 2;
            } else if (released_spell == FROSTBITE3_SPELL) {
                frostbite_reach = 3;
            }

            if (release_spell_direction == DIR_DOWN) {
                base_x = release_spell_x;
                base_y = release_spell_y+wizard_bbox.height;
                coef_x = 0;
                coef_y = 1;
            } else if (release_spell_direction == DIR_LEFT) {
                base_x = release_spell_x;
                base_y = release_spell_y+wizard_bbox.height;
                coef_x = -1;
                coef_y = 0;
            } else if (release_spell_direction == DIR_UP) {
                base_x = release_spell_x+wizard_bbox.width;
                base_y = release_spell_y;
                coef_x = 0;
                coef_y = -1;
            } else if (release_spell_direction == DIR_RIGHT) {
                base_x = release_spell_x+wizard_bbox.width;
                base_y = release_spell_y;
                coef_x = 1;
                coef_y = 0;
            }

            for (int i = 0;i<(frostbite_reach)*FROST_PARTICLES;i++) {
                double x = base_x + coef_y*sin((i%12)*0.1308996939) * TILE_SIZE + coef_x*((curr_anim_frames*i)%12) * frostbite_reach * TILE_SIZE/12;
                double y = base_y + coef_x*sin((i%12)*0.1308996939) * TILE_SIZE + coef_y*((curr_anim_frames*i)%12) * frostbite_reach * TILE_SIZE/12;
                // riv_draw_point(x, y, frostbite_colors[i%n_frostbite_colors]);
                riv_draw_circle_fill(x, y, 1+riv_rand_uint(2), frostbite_colors[i%n_frostbite_colors]);
            }
        } else if (released_spell == WINDSLASH_SPELL) {
            int curr_anim_frames = riv->frame-release_spell_frame;
            if (curr_anim_frames%((int)frames_per_beat) == 0) 
                riv_waveform(&windslash_sfx);
            int center_x = release_spell_x + wizard_bbox.width/2;
            int center_y = release_spell_y + wizard_bbox.height/2;

            int inner_radius = TILE_SIZE/2;
            int outer_radius = (TILE_SIZE)/2 + WINDSLASH_REACH*TILE_SIZE;

            int delta_frames = riv->frame - release_spell_frame;
            int delta_pixels = (outer_radius-inner_radius) * delta_frames / release_spell_anim_frames;
            // double delta_degrees = delta_frames < release_spell_anim_frames/2 ? 2*M_PI * delta_frames / release_spell_anim_frames : -2*M_PI * delta_frames / release_spell_anim_frames/2 ;
            double delta_degrees = 3*M_PI_4 * delta_frames / release_spell_anim_frames;
            double delta_degrees2 = M_PI_2 * delta_frames / release_spell_anim_frames;

            int rotation_degrees;
            int coef_x;
            int coef_y;
            if (release_spell_direction == DIR_DOWN) {
                rotation_degrees = M_PI_2;
                coef_x = 0;
                coef_y = 1;
            } else if (release_spell_direction == DIR_LEFT) {
                rotation_degrees = M_PI;
                coef_x = -1;
                coef_y = 0;
            } else if (release_spell_direction == DIR_UP) {
                rotation_degrees = -M_PI_2;
                coef_x = 0;
                coef_y = -1;
            } else if (release_spell_direction == DIR_RIGHT) {
                rotation_degrees = 0;
                coef_x = 1;
                coef_y = 0;
            }

            double base_X = sin(rotation_degrees+delta_degrees);
            double base_y = -sin(M_PI_2+rotation_degrees+delta_degrees);

            double base2_X = sin(M_PI_4+rotation_degrees+delta_degrees);
            double base2_y = -sin(M_PI_4+M_PI_2+rotation_degrees+delta_degrees);

            double inv_base_X = sin(M_PI+rotation_degrees-delta_degrees);
            double inv_base_y = -sin(M_PI+M_PI_2+rotation_degrees-delta_degrees);

            double inv_base2_X = sin(M_PI+rotation_degrees-delta_degrees);
            double inv_base2_y = -sin(M_PI+M_PI_2-M_PI_4+rotation_degrees-delta_degrees);

            riv_draw_triangle_fill(
                center_x+base_X*inner_radius+coef_x*delta_pixels,
                center_y+base_y*inner_radius+coef_y*delta_pixels,
                center_x+base2_X*inner_radius+coef_x*delta_pixels,
                center_y+base2_y*inner_radius+coef_y*delta_pixels,
                center_x+base_X*outer_radius,
                center_y+base_y*outer_radius,
                RIV_COLOR_LIGHTYELLOW
            );

            riv_draw_triangle_fill(
                center_x+inv_base_X*inner_radius+coef_x*delta_pixels,
                center_y+inv_base_y*inner_radius+coef_y*delta_pixels,
                center_x+inv_base2_X*inner_radius+coef_x*delta_pixels,
                center_y+inv_base2_y*inner_radius+coef_y*delta_pixels,
                center_x+inv_base_X*outer_radius,
                center_y+inv_base_y*outer_radius,
                RIV_COLOR_LIGHTYELLOW
            );

            riv_draw_triangle_fill(
                center_x+base_X*(inner_radius + 8)+coef_x*delta_pixels,
                center_y+base_y*(inner_radius + 8)+coef_y*delta_pixels,
                center_x+base2_X*inner_radius+coef_x*delta_pixels,
                center_y+base2_y*inner_radius+coef_y*delta_pixels,
                center_x+base_X*(outer_radius - 8),
                center_y+base_y*(outer_radius - 8),
                RIV_COLOR_LIGHTGREEN
            );

            riv_draw_triangle_fill(
                center_x+inv_base_X*(inner_radius + 8)+coef_x*delta_pixels,
                center_y+inv_base_y*(inner_radius + 8)+coef_y*delta_pixels,
                center_x+inv_base2_X*inner_radius+coef_x*delta_pixels,
                center_y+inv_base2_y*inner_radius+coef_y*delta_pixels,
                center_x+inv_base_X*(outer_radius - 8),
                center_y+inv_base_y*(outer_radius - 8),
                RIV_COLOR_LIGHTGREEN
            );

            // riv_snprintf(buf, sizeof(buf), "Attack 2");
            // for (int i = 0.0;i<6;i++) {
            //     double x = release_spell_x+wizard_bbox.width/2 + sin(riv->time*4 + i*1.0471975511966) * TILE_SIZE;
            //     double y = release_spell_y+wizard_bbox.height/2 + cos(riv->time*4 + i*1.0471975511966) * TILE_SIZE;
            //     riv_draw_circle_fill(x, y, 4, RIV_COLOR_LIGHTRED + (int)i);
            // }
        } else if (released_spell == FIREBALL_SPELL) {
            int curr_anim_frames = riv->frame-release_spell_frame;
            if (curr_anim_frames == 0) 
                riv_waveform(&fireball_sfx);
            // riv_snprintf(buf, sizeof(buf), "Attack 3");

            int delta_frames = riv->frame - release_spell_frame;
            int delta_pixels = FIREBALL_REACH * TILE_SIZE * delta_frames / release_spell_anim_frames;

            int x;
            int y;
            if (release_spell_direction == DIR_DOWN) {
                x = release_spell_x + wizard_bbox.width/2;
                y = release_spell_y+wizard_bbox.height + delta_pixels;
            } else if (release_spell_direction == DIR_LEFT) {
                x = release_spell_x - delta_pixels;
                y = release_spell_y+wizard_bbox.height/2;
            } else if (release_spell_direction == DIR_UP) {
                x = release_spell_x+wizard_bbox.width/2;
                y = release_spell_y - delta_pixels;
            } else if (release_spell_direction == DIR_RIGHT) {
                x = release_spell_x+wizard_bbox.width + delta_pixels;
                y = release_spell_y+wizard_bbox.height/2;
            }
            riv_draw_circle_fill(x, y, 20+riv_rand_uint(2), RIV_COLOR_LIGHTYELLOW);
            riv_draw_circle_fill(x, y, 16+riv_rand_uint(2), RIV_COLOR_YELLOW);
            riv_draw_circle_fill(x, y, 12+riv_rand_uint(2), RIV_COLOR_ORANGE);
            riv_draw_circle_fill(x, y, 8+riv_rand_uint(2), RIV_COLOR_RED);
        } else if (released_spell == PROTECTION_SPELL) {
            int curr_anim_frames = riv->frame-release_spell_frame;
            if (curr_anim_frames%((int)frames_per_beat) == 0) 
                riv_waveform(&protection_sfx);
            // riv_snprintf(buf, sizeof(buf), "Protection");
            for (int i = 0.0;i<PROTECTION_CIRCLES;i++) {
                double x = release_spell_x+wizard_bbox.width/2 + sin(riv->time*4 + i*1.0471975511966) * TILE_SIZE;
                double y = release_spell_y+wizard_bbox.height/2 + cos(riv->time*4 + i*1.0471975511966) * TILE_SIZE;
                riv_draw_circle_fill(x, y, 4, RIV_COLOR_LIGHTRED + (int)i);
            }
        } else if (released_spell == NOVA_SPELL) {
            int curr_anim_frames = riv->frame-release_spell_frame;
            if (curr_anim_frames%((int)frames_per_beat) == 0) 
                riv_waveform(&nova_sfx);
            // riv_snprintf(buf, sizeof(buf), "NOVA!!");
            for (int i = 0;i<NOVA_CIRCLES;i++) {
                double radius = (2 + 2*cos(riv->time*4 + i*1.0471975511966)) * TILE_SIZE;
                riv_draw_circle_line(
                    release_spell_x+wizard_bbox.width/2, 
                    release_spell_y+wizard_bbox.height/2, 
                    radius, RIV_COLOR_LIGHTRED + (int)i);
            }
        } else if (released_spell == SECRET_SPELL) {
            // riv_snprintf(buf, sizeof(buf), "secret...");
            // int center_x = release_spell_x + wizard_bbox.width/2;
            // int center_y = release_spell_y + wizard_bbox.height/2;
            // riv_draw_circle_fill(center_x, center_y, TILE_SIZE*SECRET_REACH+riv_rand_uint(2), RIV_COLOR_LIGHTYELLOW);
            int curr_anim_frames = riv->frame-release_spell_frame;
            if (curr_anim_frames%((int)frames_per_beat) == 0) {
                riv_clear(RIV_COLOR_BLACK);
                riv_waveform(&secret_sfx);
            }
            else if (curr_anim_frames%((int)(frames_per_beat+frames_per_beat/2)) == 0) riv_clear(RIV_COLOR_WHITE);
        }

        // if (player_direction == DIR_DOWN) {
        //     riv_draw_text(buf, RIV_SPRITESHEET_FONT_5X7, RIV_TOP, player_pos.x+wizard_bbox.x+wizard_bbox.width/2,player_pos.y+wizard_bbox.y+wizard_bbox.height+1, 1, RIV_COLOR_WHITE);
        // } else if (player_direction == DIR_LEFT) {
        //     riv_draw_text(buf, RIV_SPRITESHEET_FONT_5X7, RIV_RIGHT, player_pos.x+wizard_bbox.x-1,player_pos.y+wizard_bbox.y+wizard_bbox.height/2, 1, RIV_COLOR_WHITE);
        // } else if (player_direction == DIR_UP) {
        //     riv_draw_text(buf, RIV_SPRITESHEET_FONT_5X7, RIV_BOTTOM, player_pos.x+wizard_bbox.x+wizard_bbox.width/2,player_pos.y+wizard_bbox.y-1, 1, RIV_COLOR_WHITE);
        // } else if (player_direction == DIR_RIGHT) {
        //     riv_draw_text(buf, RIV_SPRITESHEET_FONT_5X7, RIV_LEFT, player_pos.x+wizard_bbox.x+wizard_bbox.width+1,player_pos.y+wizard_bbox.y+wizard_bbox.height/2, 1, RIV_COLOR_WHITE);
        // }


    }

    if (completed_spell > -1) {
        char buf[128];
        if (completed_spell == FROSTBITE1_SPELL) {
            riv_snprintf(buf, sizeof(buf), "Frostbite");
        } else if (completed_spell == FROSTBITE2_SPELL) {
            riv_snprintf(buf, sizeof(buf), "Frostbite 2");
        } else if (completed_spell == FROSTBITE3_SPELL) {
            riv_snprintf(buf, sizeof(buf), "Frostbite 3");
        } else if (completed_spell == WINDSLASH_SPELL) {
            riv_snprintf(buf, sizeof(buf), "Wind Slash");
        } else if (completed_spell == FIREBALL_SPELL) {
            riv_snprintf(buf, sizeof(buf), "Fireball");
        } else if (completed_spell == PROTECTION_SPELL) {
            riv_snprintf(buf, sizeof(buf), "Shield of Light");
        } else if (completed_spell == NOVA_SPELL) {
            riv_snprintf(buf, sizeof(buf), "Lightning Nova");
        } else if (completed_spell == SECRET_SPELL) {
            riv_snprintf(buf, sizeof(buf), "secret...");
        }

        riv_draw_text(buf, RIV_SPRITESHEET_FONT_5X7, RIV_LEFT, POSSIBLE_LEFT_X, POSSIBLE_CAST_Y, 1, RIV_COLOR_WHITE);
    }

}

void draw_map() {
    // Draw every layer
    for (int l=0;l<MAP_LAYERS;++l) {
        riv->draw.color_key_disabled = l == 0; // Optimize rendering for first layer
        // Draw tile grid
        for (int y=0;y<MAP_SIZE;++y) {
            for (int x=0;x<MAP_SIZE;++x) {
                int id = map[l][y][x];
                if (id > 0) {
                    // Draw object sprite
                    riv_draw_sprite(id, tileset_sps, x*TILE_SIZE, y*TILE_SIZE, 1, 1, 1, 1);
                }
            }
        }
    }
}

void draw_player() {
    // Draw wizard sprite
    riv->draw.pal_enabled = true;
    if (chain_cast) { // Colorize hat
        for (int c=0; c<HEAD_COLORS; c++) {
            riv->draw.pal[head_colors[c]] = 16 + (riv->frame / 2) % 16 + c;
        }
    }
    int base_frame = player_direction * WIZARD_WALK_FRAMES;
    int anim_frame = ((riv->frame / ((int)frames_per_beat/TIME_SIG)) % WIZARD_WALK_FRAMES);
    riv_draw_sprite(base_frame + anim_frame, wizard_sps, player_pos.x, player_pos.y, 1, 1, 1, 1);
    riv_draw_rect_line(player_pos.x+wizard_bbox.x,player_pos.y+wizard_bbox.y,wizard_bbox.width,wizard_bbox.height,RIV_COLOR_PEACH);
    if (chain_cast) { // Reset color palette swap
        for (int c=0; c<HEAD_COLORS; c++) {
            riv->draw.pal[head_colors[c]] = head_colors[c];
        }
    }
    riv->draw.pal_enabled = false;
}

void draw_help() {

    if (board_help) {
        if (riv->frame < help_frame + HELP_FRAMES) {
            char buf[128];
            riv_snprintf(buf, sizeof(buf), "Frostbite: %c %c.\nShield of light: %c %c",'f','f','f','u');//16,16,16,31);//26,26,26,25);
            riv_draw_text(buf, RIV_SPRITESHEET_FONT_3X5, RIV_TOP, SCREEN_SIZE/2,TILE_SIZE/2, 2, RIV_COLOR_WHITE);
        } else {
            board_help = false;
        }
    } else if (chest_help) {
        if (riv->frame < help_frame + HELP_FRAMES) {
            char buf[128];
            riv_snprintf(buf, sizeof(buf), "Frostbite 2: %c %c %c\nFireball: %c %c %c %c",'f','f','f','f','f','b','b');
            riv_draw_text(buf, RIV_SPRITESHEET_FONT_3X5, RIV_TOP, SCREEN_SIZE/2,TILE_SIZE/2, 2, RIV_COLOR_WHITE);
        } else {
            chest_help = false;
        }
    } else if (riv->frame < start_frame + HELP_FRAMES) {
        riv_draw_text("Move on the Rhythm. Cast on the rhythm.", RIV_SPRITESHEET_FONT_3X5, RIV_TOP, SCREEN_SIZE/2,TILE_SIZE/2, 2, RIV_COLOR_WHITE);
        riv_draw_text("Uhmm... there's something writen on the board.", RIV_SPRITESHEET_FONT_3X5, RIV_BOTTOM, SCREEN_SIZE/2,SCREEN_SIZE - 2*TILE_SIZE, 2, RIV_COLOR_WHITE);
    }
}
////
// Main Draw and update functions


// Called every frame to update game state
void update() {
    if (!started) { // Game not started yet
        // Let game start whenever a key has been pressed
        if (riv->key_toggle_count > 0) {
            start_game();
        }
    } else if (!ended) { 
        update_state();
    }

}

// Called every frame to draw the game
void draw() {

    if (!started) { 
        riv_clear(RIV_COLOR_DARKSLATE);
        draw_start_screen();
    } else if (!ended) { 
        riv_clear(RIV_COLOR_DARKSLATE);
        draw_map();
        draw_player();
        draw_cast_moves();
        draw_beat_guide();
        draw_help();
        draw_spell();
    } else {
        riv_draw_text("Game Over", RIV_SPRITESHEET_FONT_5X7, RIV_CENTER, SCREEN_SIZE/2,SCREEN_SIZE/2, 2, RIV_COLOR_WHITE);
    }
}

////
// Main loop

int main(int argc, char* argv[]) {
    riv->width = SCREEN_SIZE;
    riv->height = SCREEN_SIZE;
    // riv->target_fps = 30;

    // Load sprites
    riv_load_palette("palette.png", 32);
    wizard_sps = riv_make_spritesheet(riv_make_image("wizard-spritesheet-6362.png", 255), WIZARD_SIZE, WIZARD_SIZE);
    tileset_sps = riv_make_spritesheet(riv_make_image("tileset.png", 255), TILE_SIZE, TILE_SIZE);

    initialize();

    do { // main loop
        update();
        draw();
    } while(riv_present());
    return 0;
}
