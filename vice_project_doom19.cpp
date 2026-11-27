#include <iostream>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace sf;
using namespace std;

int screen_x = 1136;
int screen_y = 896;

// ===================== LEVEL DATA (4 screens - Rigid Construction Site Layout) =====================
const int TILE = 32;
const int MAX_PIECES = 25;
int piece_count[4] = { 10, 10, 10, 10 };
float level_data[4][MAX_PIECES][5] = {
    {   // ---------------- SCREEN 1 (level1_screen1.png) ----------------
        { 1, 0, 18, 36, 3 },   // Continuous lower girder bridge across full screen
        { 1, 28, 13, 8, 3 },   // Elevated right platform
        { 3, 28, 16, 2, 5 },   // Support post under right platform
        { 4, 4, 18, 5, 2.5 },  // Hazard striped warning block on lower left
        { 2, 0, 12, 2, 9 },    // Left structural pillar
        { 2, 34, 12, 2, 9 },   // Right structural pillar
    },
    {   // ---------------- SCREEN 2 (level1_screen2.png) ----------------
        { 1, 0, 18, 36, 3 },   // Continuous lower girder bridge across full screen
        { 1, 18, 12, 15, 3 },  // Higher-right platform
        { 3, 9, 15, 2, 6 },    // Support under left platform
        { 3, 24, 15, 2, 6 },   // Support under right platform
        { 4, 10, 15, 6, 3 },   // Hazard striped warning block
        { 2, 0, 12, 2, 9 },    // Left pillar
        { 2, 34, 12, 2, 9 },   // Right pillar
    },
    {   // ---------------- SCREEN 3 (level1_screen3.png) ----------------
        { 1, 0, 18, 36, 3 },   // Continuous lower girder bridge across full screen
        { 1, 13, 16, 8, 3 },   // Center platform
        { 3, 8, 15, 2, 6 },    // Rigid support post 1
        { 3, 16, 15, 2, 6 },   // Rigid support post 2
        { 3, 34, 15, 2, 6 },   // Rigid support post 3
        { 2, 0, 12, 2, 9 },    // Left pillar
        { 2, 34, 12, 2, 9 },   // Right pillar
    },
    {   // ---------------- SCREEN 4 (level1_screen4.png) ----------------
        { 1, 0, 18, 36, 3 },   // Continuous lower girder bridge across full screen
        { 1, 16, 13, 20, 3 },  // Main elevated platform on right
        { 6, 16, 13, 2, 11 },  // Vertical ladder on left of platform to climb up
        { 3, 26, 15, 2, 6 },   // Support post 1 under platform
        { 3, 34, 15, 2, 6 },   // Support post 2 under platform
        { 4, 2, 15, 5, 3 },    // Hazard block 1
        { 4, 10, 15, 5, 3 },   // Hazard block 2
        { 2, 0, 12, 2, 9 },    // Left pillar
    },
};

void display_background(RenderWindow&, Sprite&);
void play_hit_sound(Sound&);
void player_kneel(float&, int&, const int, const int, bool&, bool, float, int, int, bool);

bool check_solid_obstacle(int screen, float left, float right, float top, float bottom) {
    int s = screen - 1;
    for (int i = 0; i < piece_count[s]; i++) {
        int type = (int)level_data[s][i][0];
        if (type < 2 || type > 5) {
            continue;
        }

        float ox0 = level_data[s][i][1] * TILE;
        float oy0 = level_data[s][i][2] * TILE;
        float ox1 = ox0 + level_data[s][i][3] * TILE;
        float oy1 = oy0 + level_data[s][i][4] * TILE;

        if (right > ox0 + 2.0f && left < ox1 - 2.0f && bottom > oy0 + 2.0f && top < oy1 - 2.0f) {
            return true;
        }
    }
    return false;
}

void player_movement(float&, float&, float, const int, float&, bool&, bool, bool&, bool&, bool&, Sound&, const float, const float, const int, int&, const int, bool&);
void player_gravity(float&, float&, bool&, Sound&, const float, const float, const int, float, int, int, bool&, int);
void update_player_whip(bool&, bool&, int&, int&, const int, Sound&);
void update_player_sprite(Sprite&, Texture&, Texture&, Texture&, Texture&, bool, bool, bool, bool, int, int&, int&, const int, int&, int&, int&);
void update_whip_hitbox(bool, bool, bool, bool, int, float, float, int, int, bool&, float&, float&, float&, float&);
void check_whip_hits(bool, float, float, float, float,
                     float[], float[], bool[], const int, int, int,
                     float[], float[], bool[], const int, int, int,
                     float[], float[], bool[], const int, int, int,
                     float[], float[], float[], int[], int[], int[], bool[], int&, const int,
                     float[], float[], float[], int[], bool[], int&, const int, Sound&);
void init_crawlers(bool[], const int);
void spawn_next_crawler(int&, int&, float[], float[], float[], float[], bool[], bool[], bool[], const int, const int, const int, const int);
void update_crawlers(float[], float[], float[], float[], bool[], bool[], bool[], const int, const int, const int, const float, const float, const int);
void display_crawlers(RenderWindow&, Sprite&, Texture&, float[], float[], bool[], bool[], const int, const int, const float);
void init_blue_enemies(bool[], const int);
void spawn_next_blue_enemy(int&, int&, float, float[], float[], float[], float[], int[], bool[], bool[], bool[], const int, const int, const int, const int);
void update_blue_enemies(float, float[], float[], float[], float[], int[], bool[], bool[], bool[], const int, const int, const int, const float, const float, const int);
void display_blue_enemies(RenderWindow&, Sprite&, Texture&, Texture&, float[], float[], bool[], bool[], bool[], const int, const int, const float);
void init_flying_insects(bool[], const int);
void spawn_next_flying_insect(int&, int&, float[], float[], float[], float[], float[], bool[], bool[], const int, const int, const int, const int);
void update_flying_insects(float[], float[], float[], float[], float[], bool[], bool[], const int, const int, const int, const int);
void display_flying_insects(RenderWindow&, Sprite&, Texture&, float[], float[], bool[], bool[], const int, const int, const float);
void init_coins(bool[], int[], int[], int[], const int);
void spawn_coin(float[], float[], float[], int[], int[], int[], bool[], int&, const int, float, float);
void update_coins(float[], float[], float[], int[], int[], int[], bool[], const int, const float, const int, const float, float, float, int, int, int&, Sound&, int);
void display_coins(RenderWindow&, Sprite&, Texture&, float[], float[], int[], bool[], const int, const int, const int, const float);
void init_refills(bool[], int[], const int);
void spawn_refill(float[], float[], float[], int[], bool[], int&, const int, float, float);
void update_refills(float[], float[], float[], int[], bool[], const int, const float, const int, const float, const float, float, float, int, int, Sound&, int);
void display_refills(RenderWindow&, Sprite&, Texture&, float[], float[], bool[], const int, const int, const int, const float, const float, const int);
void draw_level(RenderWindow&, Sprite&, Texture[], int);

float find_floor(int screen, float left, float right, float old_bottom, float new_bottom) {
    int s = screen - 1;
    float best = -1.0f;

    for (int i = 0; i < piece_count[s]; i++) {
        int type = (int)level_data[s][i][0];
        if (type != 1 && type != 6) {
            continue;
        }

        float x0 = level_data[s][i][1] * TILE;
        float x1 = x0 + level_data[s][i][3] * TILE;
        float top = level_data[s][i][2] * TILE;

        bool over_it = (right > x0 + 2.0f && left < x1 - 2.0f);
        if (over_it && old_bottom <= top + 24.0f && new_bottom >= top - 2.0f) {
            if (best < 0.0f || top < best) {
                best = top;
            }
        }
    }
    return best;
}

float find_ceiling(int screen, float left, float right, float old_top, float new_top) {
    int s = screen - 1;
    float best = -1.0f;

    for (int i = 0; i < piece_count[s]; i++) {
        int type = (int)level_data[s][i][0];
        if (type != 1) {
            continue;
        }

        float x0 = level_data[s][i][1] * TILE;
        float x1 = x0 + level_data[s][i][3] * TILE;
        float bottom = (level_data[s][i][2] + level_data[s][i][4]) * TILE;

        bool under_it = (right > x0 + 4.0f && left < x1 - 4.0f);
        if (under_it && old_top >= bottom - 16.0f && new_top <= bottom) {
            if (best < 0.0f || bottom > best) {
                best = bottom;
            }
        }
    }
    return best;
}

bool find_ladder(int, float, float&, float&, float&);
float random_girder_x(int, int);

int main() {
    srand((unsigned int)time(0));

    RenderWindow window(VideoMode(screen_x, screen_y), "VICE: Project Doom - Stage 2-1", Style::Resize);
    window.setFramerateLimit(60);

    const int ground_y = 640;

    Texture bg_texture;
    bg_texture.loadFromFile("Data/bg.png");
    Sprite bg_sprite(bg_texture);
    Vector2u bg_size = bg_texture.getSize();
    if (bg_size.x > 0 && bg_size.y > 0) {
        bg_sprite.setScale((float)screen_x / bg_size.x, (float)screen_y / bg_size.y);
    }

    Texture piece_texture[8];
    const char* piece_file[8] = { "", "Data/lv_girder.png", "Data/lv_pillar.png", "Data/lv_post.png", "Data/lv_hazard.png", "Data/lv_brace.png", "Data/lv_ladder.png", "Data/lv_girder.png" };
    for (int i = 1; i < 8; i++) {
        piece_texture[i].loadFromFile(piece_file[i]);
        piece_texture[i].setRepeated(true);
    }
    Sprite level_sprite;

    const float respawn_x = 60.0f;

    Music level_music;
    level_music.openFromFile("Data/mus.ogg");
    level_music.setVolume(20);
    level_music.setLoop(true);
    level_music.play();

    SoundBuffer hit_buffer;
    hit_buffer.loadFromFile("Data/hit.wav");
    Sound hit_sound(hit_buffer);
    hit_sound.setVolume(50);

    SoundBuffer whip_buffer;
    whip_buffer.loadFromFile("Data/whip.wav");
    Sound whip_sound(whip_buffer);
    whip_sound.setVolume(50);

    SoundBuffer coin_buffer;
    coin_buffer.loadFromFile("Data/collect.wav");
    Sound coin_sound(coin_buffer);
    coin_sound.setVolume(60);

    const int stand_height = 108;
    const int kneel_height = 75;
    const int player_width = 54;
    int player_height = stand_height;
    int score = 28100;
    int current_screen = 1;
    const int max_screens = 4;
    bool is_on_ladder = false;

    float player_x = 90.0f;
    float player_y = (float)(800 - stand_height);
    float velocity_y = 0.0f;

    const float gravity = 0.9f;
    const float walk_speed = 6.0f;
    const float jump_velocity = -19.0f;
    const float terminal_velocity = 20.0f;

    bool on_ground = true;
    bool is_kneeling = false;
    bool facing_right = true;
    bool is_moving = false;
    bool was_against_wall = false;

    bool is_whipping = false;
    bool was_q_pressed = false;
    int whip_frame = 0;
    int whip_timer = 0;
    const int whip_delay = 3;

    bool whip_hitbox_active = false;
    float whip_box_x = 0.0f;
    float whip_box_y = 0.0f;
    float whip_box_w = 0.0f;
    float whip_box_h = 0.0f;

    Texture run_texture, stand_whip_texture, run_whip_texture, crouch_whip_texture;
    run_texture.loadFromFile("Data/run.png");
    stand_whip_texture.loadFromFile("Data/stand_whip.png");
    run_whip_texture.loadFromFile("Data/run_whip.png");
    crouch_whip_texture.loadFromFile("Data/crouch_whip.png");

    int run_step = 0;
    int anim_timer = 0;
    const int anim_delay = 5;
    int frame_width = 18;
    int frame_height = 42;
    int foot_height = 42;

    Sprite player_sprite(run_texture);
    player_sprite.setTextureRect(IntRect(1, 0, frame_width, frame_height));
    player_sprite.setScale(3.0f, 3.0f);

    Texture crawler_texture;
    crawler_texture.loadFromFile("Data/spiked_crawler.png");
    int raw_crawler_width = (int)crawler_texture.getSize().x;
    int raw_crawler_height = (int)crawler_texture.getSize().y;
    if (raw_crawler_width <= 0) raw_crawler_width = 26;
    if (raw_crawler_height <= 0) raw_crawler_height = 24;

    const float crawler_scale = ((float)stand_height / (float)raw_crawler_height) * 0.825f;
    const int crawler_width = (int)(raw_crawler_width * crawler_scale + 0.5f);
    const int crawler_height = (int)(raw_crawler_height * crawler_scale + 0.5f);
    Sprite crawler_sprite(crawler_texture);

    const int max_crawlers = 4;
    float crawler_x[4], crawler_y[4], crawler_vx[4], crawler_vy[4];
    bool crawler_alive[4], crawler_facing_right[4], crawler_on_ground[4];
    int crawler_spawned_count = 0;
    int crawler_spawn_timer = 30;
    init_crawlers(crawler_alive, max_crawlers);

    Texture blue_stand_texture, blue_jump_texture;
    blue_stand_texture.loadFromFile("Data/blue_jumping_enemy_1.png");
    blue_jump_texture.loadFromFile("Data/blue_jumping_enemy_2.png");
    int raw_blue_width = (int)blue_stand_texture.getSize().x;
    int raw_blue_height = (int)blue_stand_texture.getSize().y;
    if (raw_blue_width <= 0) raw_blue_width = 18;
    if (raw_blue_height <= 0) raw_blue_height = 32;

    const float blue_scale = (float)stand_height / (float)raw_blue_height;
    const int blue_width = (int)(raw_blue_width * blue_scale + 0.5f);
    const int blue_height = (int)(raw_blue_height * blue_scale + 0.5f);
    Sprite blue_sprite(blue_stand_texture);

    const int max_blue_enemies = 5;
    float blue_x[5], blue_y[5], blue_vx[5], blue_vy[5];
    int blue_jump_timer[5];
    bool blue_alive[5], blue_facing_right[5], blue_on_ground[5];
    int blue_spawned_count = 0;
    int blue_spawn_timer = 45;
    init_blue_enemies(blue_alive, max_blue_enemies);

    Texture insect_texture;
    insect_texture.loadFromFile("Data/flying_insect.png");
    int raw_insect_width = (int)insect_texture.getSize().x;
    int raw_insect_height = (int)insect_texture.getSize().y;
    if (raw_insect_width <= 0) raw_insect_width = 24;
    if (raw_insect_height <= 0) raw_insect_height = 18;

    const float insect_scale = ((float)stand_height / (float)raw_insect_height) * 0.55f;
    const int insect_width = (int)(raw_insect_width * insect_scale + 0.5f);
    const int insect_height = (int)(raw_insect_height * insect_scale + 0.5f);
    Sprite insect_sprite(insect_texture);

    const int max_insects = 6;
    float insect_x[6], insect_y[6], insect_base_y[6], insect_vx[6], insect_wave_angle[6];
    bool insect_alive[6], insect_facing_right[6];
    int insect_spawned_count = 0;
    int insect_spawn_timer = 60;
    init_flying_insects(insect_alive, max_insects);

    Texture coin_texture;
    coin_texture.loadFromFile("Data/coin_flip_96x16.png");
    int raw_coin_sheet_w = (int)coin_texture.getSize().x;
    int raw_coin_sheet_h = (int)coin_texture.getSize().y;
    if (raw_coin_sheet_w <= 0) raw_coin_sheet_w = 96;
    if (raw_coin_sheet_h <= 0) raw_coin_sheet_h = 16;

    const int coin_frame_w = raw_coin_sheet_w / 6;
    const int coin_frame_h = raw_coin_sheet_h;
    const float coin_scale = 40.0f / (float)coin_frame_h;
    const float coin_radius = (coin_frame_h * coin_scale) * 0.5f;
    Sprite coin_sprite;
    coin_sprite.setTexture(coin_texture, true);

    const int max_coins = 32;
    float coin_x[32], coin_y[32], coin_vy[32];
    int coin_frame[32], coin_anim_timer[32], coin_pickup_delay[32];
    bool coin_active[32];
    int coin_next_slot = 0;
    init_coins(coin_active, coin_frame, coin_anim_timer, coin_pickup_delay, max_coins);

    Texture refill_texture;
    if (!refill_texture.loadFromFile("Data/refill_2.png")) {
        refill_texture.loadFromFile("Data/refill.png");
    }
    int raw_refill_w = (int)refill_texture.getSize().x;
    int raw_refill_h = (int)refill_texture.getSize().y;
    if (raw_refill_w <= 0) raw_refill_w = 24;
    if (raw_refill_h <= 0) raw_refill_h = 16;

    const float refill_scale = 40.0f / (float)raw_refill_h;
    const float refill_half_w = (raw_refill_w * refill_scale) * 0.5f;
    const float refill_half_h = (raw_refill_h * refill_scale) * 0.5f;
    Sprite refill_sprite;
    refill_sprite.setTexture(refill_texture, true);

    const int max_refills = 16;
    float refill_x[16], refill_y[16], refill_vy[16];
    int refill_pickup_delay[16];
    bool refill_active[16];
    int refill_next_slot = 0;
    init_refills(refill_active, refill_pickup_delay, max_refills);

    Font font;
    bool font_loaded = font.loadFromFile("Data/font.ttf") || font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf");
    Text hud_text;
    if (font_loaded) {
        hud_text.setFont(font);
        hud_text.setCharacterSize(22);
        hud_text.setFillColor(Color::White);
        hud_text.setPosition(20.0f, 20.0f);
    }

    Event event;
    while (window.isOpen()) {
        int prev_screen = current_screen;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Escape)) {
            window.close();
        }

        player_kneel(player_y, player_height, stand_height, kneel_height, is_kneeling, on_ground, player_x, player_width, current_screen, is_on_ladder);
        player_movement(player_x, player_y, (float)ground_y, player_height, velocity_y, on_ground, is_kneeling, facing_right, is_moving, was_against_wall, hit_sound, walk_speed, jump_velocity, player_width, current_screen, max_screens, is_on_ladder);
        
        if (current_screen != prev_screen) {
            crawler_spawned_count = 0;
            crawler_spawn_timer = 15;
            init_crawlers(crawler_alive, max_crawlers);

            blue_spawned_count = 0;
            blue_spawn_timer = 20;
            init_blue_enemies(blue_alive, max_blue_enemies);

            insect_spawned_count = 0;
            insect_spawn_timer = 30;
            init_flying_insects(insect_alive, max_insects);

            init_coins(coin_active, coin_frame, coin_anim_timer, coin_pickup_delay, max_coins);
            init_refills(refill_active, refill_pickup_delay, max_refills);
        }

        player_gravity(player_y, velocity_y, on_ground, hit_sound, gravity, terminal_velocity, player_height, player_x, player_width, current_screen, is_on_ladder, stand_height);

        if (player_y > screen_y + 40) {
            player_x = respawn_x;
            float safe_floor = find_floor(current_screen, respawn_x, respawn_x + player_width, 0.0f, (float)screen_y);
            if (safe_floor < 0.0f) safe_floor = 400.0f;
            player_y = safe_floor - player_height;
            velocity_y = 0.0f;
            on_ground = true;
            is_on_ladder = false;
            play_hit_sound(hit_sound);
        }

        update_player_whip(is_whipping, was_q_pressed, whip_frame, whip_timer, whip_delay, whip_sound);
        update_whip_hitbox(is_whipping, is_kneeling, on_ground, facing_right, whip_frame, player_x, player_y, player_width, player_height, whip_hitbox_active, whip_box_x, whip_box_y, whip_box_w, whip_box_h);
        
        check_whip_hits(whip_hitbox_active, whip_box_x, whip_box_y, whip_box_w, whip_box_h,
                        crawler_x, crawler_y, crawler_alive, max_crawlers, crawler_width, crawler_height,
                        blue_x, blue_y, blue_alive, max_blue_enemies, blue_width, blue_height,
                        insect_x, insect_y, insect_alive, max_insects, insect_width, insect_height,
                        coin_x, coin_y, coin_vy, coin_frame, coin_anim_timer, coin_pickup_delay, coin_active, coin_next_slot, max_coins,
                        refill_x, refill_y, refill_vy, refill_pickup_delay, refill_active, refill_next_slot, max_refills, hit_sound);

        spawn_next_crawler(crawler_spawned_count, crawler_spawn_timer, crawler_x, crawler_y, crawler_vx, crawler_vy, crawler_alive, crawler_facing_right, crawler_on_ground, max_crawlers, crawler_width, crawler_height, current_screen);
        update_crawlers(crawler_x, crawler_y, crawler_vx, crawler_vy, crawler_alive, crawler_facing_right, crawler_on_ground, max_crawlers, crawler_width, crawler_height, gravity, terminal_velocity, current_screen);

        spawn_next_blue_enemy(blue_spawned_count, blue_spawn_timer, player_x, blue_x, blue_y, blue_vx, blue_vy, blue_jump_timer, blue_alive, blue_facing_right, blue_on_ground, max_blue_enemies, blue_width, blue_height, current_screen);
        update_blue_enemies(player_x, blue_x, blue_y, blue_vx, blue_vy, blue_jump_timer, blue_alive, blue_facing_right, blue_on_ground, max_blue_enemies, blue_width, blue_height, gravity, terminal_velocity, current_screen);

        spawn_next_flying_insect(insect_spawned_count, insect_spawn_timer, insect_x, insect_y, insect_base_y, insect_vx, insect_wave_angle, insect_alive, insect_facing_right, max_insects, insect_width, insect_height, ground_y);
        update_flying_insects(insect_x, insect_y, insect_base_y, insect_vx, insect_wave_angle, insect_alive, insect_facing_right, max_insects, insect_width, insect_height, ground_y);

        update_coins(coin_x, coin_y, coin_vy, coin_frame, coin_anim_timer, coin_pickup_delay, coin_active, max_coins, gravity * 0.55f, current_screen, coin_radius, player_x, player_y, player_width, player_height, score, coin_sound, stand_height);
        update_refills(refill_x, refill_y, refill_vy, refill_pickup_delay, refill_active, max_refills, gravity * 0.55f, current_screen, refill_half_w, refill_half_h, player_x, player_y, player_width, player_height, coin_sound, stand_height);

        update_player_sprite(player_sprite, run_texture, stand_whip_texture, run_whip_texture, crouch_whip_texture, is_moving, on_ground, is_kneeling, is_whipping, whip_frame, run_step, anim_timer, anim_delay, frame_width, frame_height, foot_height);

        float scale_x = facing_right ? 3.0f : -3.0f;
        player_sprite.setScale(scale_x, 3.0f);

        float visual_width = frame_width * 3.0f;
        float draw_x = facing_right
            ? player_x + (player_width - visual_width) / 2.0f
            : player_x + (player_width + visual_width) / 2.0f;

        float draw_y = player_y + player_height - foot_height * 3.0f;
        player_sprite.setPosition(draw_x, draw_y);

        window.clear();
        display_background(window, bg_sprite);
        draw_level(window, level_sprite, piece_texture, current_screen);
        display_crawlers(window, crawler_sprite, crawler_texture, crawler_x, crawler_y, crawler_alive, crawler_facing_right, max_crawlers, crawler_width, crawler_scale);
        display_blue_enemies(window, blue_sprite, blue_stand_texture, blue_jump_texture, blue_x, blue_y, blue_alive, blue_facing_right, blue_on_ground, max_blue_enemies, blue_width, blue_scale);
        display_flying_insects(window, insect_sprite, insect_texture, insect_x, insect_y, insect_alive, insect_facing_right, max_insects, insect_width, insect_scale);
        display_coins(window, coin_sprite, coin_texture, coin_x, coin_y, coin_frame, coin_active, max_coins, coin_frame_w, coin_frame_h, coin_scale);
        display_refills(window, refill_sprite, refill_texture, refill_x, refill_y, refill_active, max_refills, raw_refill_w, raw_refill_h, refill_scale, refill_scale, raw_refill_w);
        window.draw(player_sprite);

        if (font_loaded) {
            hud_text.setString("SCORE " + to_string(score) + "   P34   TIME 238   P2   O19\nSTAGE 2-1");
            window.draw(hud_text);
        }

        window.display();
    }

    coin_sound.stop();
    whip_sound.stop();
    hit_sound.stop();
    level_music.stop();
    return 0;
}

void display_background(RenderWindow& window, Sprite& bg_sprite) {
    window.draw(bg_sprite);
}

void draw_level(RenderWindow& window, Sprite& sprite, Texture piece_texture[], int current_screen) {
    int s = current_screen - 1;
    int draw_order[7] = { 5, 7, 3, 4, 1, 2, 6 };

    for (int o = 0; o < 7; o++) {
        int wanted = draw_order[o];
        for (int i = 0; i < piece_count[s]; i++) {
            if ((int)level_data[s][i][0] != wanted) {
                continue;
            }

            float x = level_data[s][i][1] * TILE;
            float y = level_data[s][i][2] * TILE;
            float w = level_data[s][i][3] * TILE;
            float h = level_data[s][i][4] * TILE;

            sprite.setTexture(piece_texture[wanted], true);
            if (wanted == 3 || wanted == 5) {
                Vector2u size = piece_texture[wanted].getSize();
                if (size.x == 0 || size.y == 0) continue;
                sprite.setScale(w / size.x, h / size.y);
            } else {
                sprite.setTextureRect(IntRect(0, 0, (int)w, (int)h));
                sprite.setScale(1.0f, 1.0f);
            }

            if (wanted == 7) sprite.setColor(Color(120, 90, 90));
            else sprite.setColor(Color::White);

            sprite.setPosition(x, y);
            window.draw(sprite);
        }
    }
}

bool find_ladder(int screen, float x, float& top_y, float& bottom_y, float& middle_x) {
    int s = screen - 1;

    for (int i = 0; i < piece_count[s]; i++) {
        if ((int)level_data[s][i][0] != 6) {
            continue;
        }

        float x0 = level_data[s][i][1] * TILE;
        float x1 = x0 + level_data[s][i][3] * TILE;
        if (x >= x0 && x <= x1) {
            top_y = level_data[s][i][2] * TILE;
            bottom_y = top_y + level_data[s][i][4] * TILE;
            middle_x = (x0 + x1) / 2.0f;
            return true;
        }
    }
    return false;
}

float random_girder_x(int screen, int width) {
    int s = screen - 1;
    int list[MAX_PIECES];
    int count = 0;

    for (int i = 0; i < piece_count[s]; i++) {
        if ((int)level_data[s][i][0] == 1 && level_data[s][i][3] * TILE >= width + 40) {
            list[count] = i;
            count++;
        }
    }
    if (count == 0) {
        return 100.0f;
    }

    int pick = list[rand() % count];
    float x0 = level_data[s][pick][1] * TILE;
    float x1 = x0 + level_data[s][pick][3] * TILE;
    if (x1 > screen_x) x1 = (float)screen_x;

    int room = (int)(x1 - x0) - width - 20;
    if (room < 1) room = 1;
    return x0 + 10.0f + (float)(rand() % room);
}

void play_hit_sound(Sound& hit_sound) {
    hit_sound.play();
    hit_sound.setPlayingOffset(milliseconds(80));
}

void player_kneel(float& player_y, int& player_height, const int stand_height, const int kneel_height, bool& is_kneeling, bool on_ground, float player_x, int player_width, int current_screen, bool is_on_ladder) {
    bool kneel_pressed = Keyboard::isKeyPressed(Keyboard::Down);

    float ladder_top = 0.0f, ladder_bottom = 0.0f, ladder_middle = 0.0f;
    if (find_ladder(current_screen, player_x + player_width / 2.0f, ladder_top, ladder_bottom, ladder_middle)) {
        float feet_y = player_y + player_height;
        if (feet_y >= ladder_top - 6.0f && feet_y <= ladder_top + 6.0f) {
            kneel_pressed = false;
        }
    }
    if (is_on_ladder) {
        kneel_pressed = false;
    }

    if (kneel_pressed && on_ground && !is_kneeling) {
        is_kneeling = true;
        player_y += (stand_height - kneel_height);
        player_height = kneel_height;
    } else if (!kneel_pressed && is_kneeling) {
        float standing_y = player_y - (stand_height - kneel_height);
        if (standing_y >= 0.0f) {
            player_y = standing_y;
            player_height = stand_height;
            is_kneeling = false;
        }
    }
}

void player_movement(float& player_x, float& player_y, float ground_y, const int player_height, float& velocity_y, bool& on_ground, bool is_kneeling, bool& facing_right, bool& is_moving, bool& was_against_wall, Sound& hit_sound, const float walk_speed, const float jump_velocity, const int player_width, int& current_screen, const int max_screens, bool& is_on_ladder) {
    float current_speed = is_kneeling ? walk_speed * 0.5f : walk_speed;
    float move_x = 0.0f;

    if (Keyboard::isKeyPressed(Keyboard::Left)) {
        move_x -= current_speed;
        if (is_on_ladder) { is_on_ladder = false; velocity_y = 0.0f; }
    }
    if (Keyboard::isKeyPressed(Keyboard::Right)) {
        move_x += current_speed;
        if (is_on_ladder) { is_on_ladder = false; velocity_y = 0.0f; }
    }

    bool up_pressed = Keyboard::isKeyPressed(Keyboard::Up);
    bool down_pressed = Keyboard::isKeyPressed(Keyboard::Down);

    float ladder_top = 0.0f, ladder_bottom = 0.0f, ladder_middle = 0.0f;
    bool near_ladder = find_ladder(current_screen, player_x + player_width / 2.0f, ladder_top, ladder_bottom, ladder_middle);
    float feet_y = player_y + player_height;

    if (!near_ladder) {
        is_on_ladder = false;
    }

    if (near_ladder && !is_on_ladder) {
        if (up_pressed && feet_y > ladder_top + 6.0f && feet_y <= ladder_bottom + 6.0f) {
            is_on_ladder = true;
        } else if (down_pressed && feet_y >= ladder_top - 6.0f && feet_y <= ladder_top + 6.0f) {
            is_on_ladder = true;
        }
        if (is_on_ladder) {
            player_x = ladder_middle - player_width / 2.0f;
            velocity_y = 0.0f;
            on_ground = false;
        }
    }

    if (is_on_ladder) {
        velocity_y = 0.0f;
        if (up_pressed) velocity_y = -4.0f;
        else if (down_pressed) velocity_y = 4.0f;

        float next_feet = feet_y + velocity_y;
        if (velocity_y < 0.0f && next_feet <= ladder_top) {
            player_y = ladder_top - player_height;
            velocity_y = 0.0f;
            is_on_ladder = false;
            on_ground = true;
        } else if (velocity_y > 0.0f && next_feet >= ladder_bottom) {
            player_y = ladder_bottom - player_height;
            velocity_y = 0.0f;
            is_on_ladder = false;
            on_ground = true;
        }
    }

    if (move_x < 0.0f) {
        facing_right = false;
        is_moving = true;
    } else if (move_x > 0.0f) {
        facing_right = true;
        is_moving = true;
    } else {
        is_moving = false;
    }

    bool hit_edge = false;
    if (move_x != 0.0f) {
        float next_x = player_x + move_x;
        if (!check_solid_obstacle(current_screen, next_x, next_x + player_width, player_y + 4.0f, player_y + player_height - 4.0f)) {
            player_x = next_x;
        } else {
            hit_edge = true;
        }
    }

    if (player_x < 0.0f) {
        if (current_screen > 1) {
            current_screen--;
            player_x = (float)(screen_x - player_width - 10.0f);
        } else {
            player_x = 0.0f;
            hit_edge = true;
        }
    } else if (player_x + player_width > screen_x) {
        if (current_screen < max_screens) {
            current_screen++;
            player_x = 20.0f;
        } else {
            player_x = (float)(screen_x - player_width);
            hit_edge = true;
        }
    }

    if (hit_edge && !was_against_wall) {
        play_hit_sound(hit_sound);
    }
    was_against_wall = hit_edge;

    static bool jump_key_released = true;
    if (!up_pressed) {
        jump_key_released = true;
    }

    if (up_pressed && jump_key_released && on_ground && !is_kneeling && !is_on_ladder) {
        velocity_y = jump_velocity;
        on_ground = false;
        jump_key_released = false;
    }
}

void player_gravity(float& player_y, float& velocity_y, bool& on_ground, Sound& hit_sound, const float gravity, const float terminal_velocity, const int player_height, float player_x, int player_width, int current_screen, bool& is_on_ladder, const int stand_height) {
    if (is_on_ladder) {
        player_y += velocity_y;
        return;
    }

    bool was_on_ground = on_ground;
    float next_y = player_y + velocity_y;

    if (velocity_y < 0.0f) {
        float ceiling_bottom = find_ceiling(current_screen, player_x, player_x + player_width, player_y, next_y);
        if (ceiling_bottom >= 0.0f || check_solid_obstacle(current_screen, player_x, player_x + player_width, next_y, next_y + player_height)) {
            if (ceiling_bottom >= 0.0f) player_y = ceiling_bottom;
            velocity_y = 0.0f;
        } else {
            player_y = next_y;
            on_ground = false;
            velocity_y += gravity;
        }
    } else {
        float landing_top = find_floor(current_screen, player_x, player_x + player_width, player_y + player_height, next_y + player_height);
        if (landing_top >= 0.0f) {
            on_ground = true;
            player_y = landing_top - player_height;
            velocity_y = 0.0f;
            if (!was_on_ground) {
                play_hit_sound(hit_sound);
            }
        } else if (check_solid_obstacle(current_screen, player_x, player_x + player_width, next_y, next_y + player_height)) {
            velocity_y = 0.0f;
        } else {
            player_y = next_y;
            on_ground = false;
            velocity_y += gravity;
            if (velocity_y > terminal_velocity) {
                velocity_y = terminal_velocity;
            }
        }
    }
}

void update_player_whip(bool& is_whipping, bool& was_q_pressed, int& whip_frame, int& whip_timer, const int whip_delay, Sound& whip_sound) {
    bool q_pressed = Keyboard::isKeyPressed(Keyboard::Q);

    if (q_pressed && !was_q_pressed && !is_whipping) {
        is_whipping = true;
        whip_frame = 0;
        whip_timer = 0;
        whip_sound.play();
    }
    was_q_pressed = q_pressed;

    if (is_whipping) {
        whip_timer++;
        if (whip_timer >= whip_delay) {
            whip_timer = 0;
            whip_frame++;
            if (whip_frame >= 5) {
                is_whipping = false;
                whip_frame = 0;
            }
        }
    }
}

void update_player_sprite(Sprite& player_sprite, Texture& run_texture, Texture& stand_whip_texture, Texture& run_whip_texture, Texture& crouch_whip_texture, bool is_moving, bool on_ground, bool is_kneeling, bool is_whipping, int whip_frame, int& run_step, int& anim_timer, const int anim_delay, int& frame_width, int& frame_height, int& foot_height) {
    if (is_whipping) {
        frame_width = 49;
        frame_height = 48;
        foot_height = 44;

        if (is_kneeling && on_ground) {
            player_sprite.setTexture(crouch_whip_texture);
        } else if (on_ground && is_moving) {
            player_sprite.setTexture(run_whip_texture);
        } else {
            player_sprite.setTexture(stand_whip_texture);
        }

        player_sprite.setTextureRect(IntRect(whip_frame * frame_width, 0, frame_width, frame_height));
        return;
    }

    player_sprite.setTexture(run_texture);
    frame_height = 42;
    foot_height = 42;

    if (is_kneeling && on_ground) {
        run_step = 0;
        anim_timer = 0;
        frame_width = 20;
        player_sprite.setTextureRect(IntRect(195, 0, frame_width, frame_height));
    } else if (on_ground && is_moving) {
        anim_timer++;
        if (anim_timer >= anim_delay) {
            anim_timer = 0;
            run_step = (run_step + 1) % 4;
        }

        int run_x[4] = {36, 75, 100, 136};
        int run_w[4] = {25, 15, 26, 15};
        frame_width = run_w[run_step];
        player_sprite.setTextureRect(IntRect(run_x[run_step], 0, frame_width, frame_height));
    } else {
        run_step = 0;
        anim_timer = 0;
        frame_width = 18;
        player_sprite.setTextureRect(IntRect(1, 0, frame_width, frame_height));
    }
}

void update_whip_hitbox(bool is_whipping, bool is_kneeling, bool on_ground, bool facing_right, int whip_frame, float player_x, float player_y, int player_width, int player_height, bool& whip_hitbox_active, float& whip_box_x, float& whip_box_y, float& whip_box_w, float& whip_box_h) {
    if (!is_whipping || (whip_frame != 3 && whip_frame != 4)) {
        whip_hitbox_active = false;
        whip_box_w = 0.0f;
        whip_box_h = 0.0f;
        return;
    }

    int local_x = 0, local_y = 0, local_w = 0, local_h = 0;
    if (is_kneeling && on_ground) {
        if (whip_frame == 3) { local_x = 25; local_y = 9;  local_w = 20; local_h = 23; }
        else                 { local_x = 31; local_y = 35; local_w = 18; local_h = 13; }
    } else {
        if (whip_frame == 3) { local_x = 25; local_y = 0;  local_w = 20; local_h = 25; }
        else                 { local_x = 31; local_y = 26; local_w = 17; local_h = 16; }
    }

    const float visual_width = 49.0f * 3.0f;
    const float sprite_top_y = player_y + player_height - 44.0f * 3.0f;

    whip_hitbox_active = true;
    whip_box_w = local_w * 3.0f;
    whip_box_h = local_h * 3.0f;
    whip_box_y = sprite_top_y + local_y * 3.0f;

    if (facing_right) {
        float sprite_left_x = player_x + (player_width - visual_width) / 2.0f;
        whip_box_x = sprite_left_x + local_x * 3.0f;
    } else {
        float sprite_right_x = player_x + (player_width + visual_width) / 2.0f;
        whip_box_x = sprite_right_x - (local_x + local_w) * 3.0f;
    }
}

void check_whip_hits(bool whip_hitbox_active, float whip_box_x, float whip_box_y, float whip_box_w, float whip_box_h,
                     float crawler_x[], float crawler_y[], bool crawler_alive[], const int max_crawlers, int crawler_width, int crawler_height,
                     float blue_x[], float blue_y[], bool blue_alive[], const int max_blue_enemies, int blue_width, int blue_height,
                     float insect_x[], float insect_y[], bool insect_alive[], const int max_insects, int insect_width, int insect_height,
                     float coin_x[], float coin_y[], float coin_vy[], int coin_frame[], int coin_anim_timer[], int coin_pickup_delay[], bool coin_active[], int& coin_next_slot, const int max_coins,
                     float refill_x[], float refill_y[], float refill_vy[], int refill_pickup_delay[], bool refill_active[], int& refill_next_slot, const int max_refills, Sound& hit_sound) {
    if (!whip_hitbox_active) return;

    for (int i = 0; i < max_crawlers; i++) {
        if (!crawler_alive[i]) continue;
        if (whip_box_x < crawler_x[i] + crawler_width &&
            whip_box_x + whip_box_w > crawler_x[i] &&
            whip_box_y < crawler_y[i] + crawler_height &&
            whip_box_y + whip_box_h > crawler_y[i]) {
            crawler_alive[i] = false;
            spawn_refill(refill_x, refill_y, refill_vy, refill_pickup_delay, refill_active, refill_next_slot, max_refills,
                         crawler_x[i] + crawler_width * 0.5f, crawler_y[i] + crawler_height * 0.5f);
            play_hit_sound(hit_sound);
        }
    }

    for (int i = 0; i < max_blue_enemies; i++) {
        if (!blue_alive[i]) continue;
        if (whip_box_x < blue_x[i] + blue_width &&
            whip_box_x + whip_box_w > blue_x[i] &&
            whip_box_y < blue_y[i] + blue_height &&
            whip_box_y + whip_box_h > blue_y[i]) {
            blue_alive[i] = false;
            spawn_coin(coin_x, coin_y, coin_vy, coin_frame, coin_anim_timer, coin_pickup_delay, coin_active, coin_next_slot, max_coins,
                       blue_x[i] + blue_width * 0.5f, blue_y[i] + blue_height * 0.5f);
            play_hit_sound(hit_sound);
        }
    }

    for (int i = 0; i < max_insects; i++) {
        if (!insect_alive[i]) continue;
        if (whip_box_x < insect_x[i] + insect_width &&
            whip_box_x + whip_box_w > insect_x[i] &&
            whip_box_y < insect_y[i] + insect_height &&
            whip_box_y + whip_box_h > insect_y[i]) {
            insect_alive[i] = false;
            spawn_coin(coin_x, coin_y, coin_vy, coin_frame, coin_anim_timer, coin_pickup_delay, coin_active, coin_next_slot, max_coins,
                       insect_x[i] + insect_width * 0.5f, insect_y[i] + insect_height * 0.5f);
            play_hit_sound(hit_sound);
        }
    }
}

void init_crawlers(bool crawler_alive[], const int max_crawlers) {
    for (int i = 0; i < max_crawlers; i++) crawler_alive[i] = false;
}

void spawn_next_crawler(int& spawned_count, int& spawn_timer, float crawler_x[], float crawler_y[], float crawler_vx[], float crawler_vy[], bool crawler_alive[], bool crawler_facing_right[], bool crawler_on_ground[], const int max_crawlers, const int crawler_width, const int crawler_height, const int current_screen) {
    if (spawned_count >= max_crawlers) return;
    if (spawn_timer > 0) { spawn_timer--; return; }

    int i = spawned_count;
    crawler_alive[i] = true;
    crawler_x[i] = random_girder_x(current_screen, crawler_width);
    crawler_y[i] = (float)(-crawler_height);
    crawler_vy[i] = 0.0f;
    crawler_on_ground[i] = false;

    if (rand() % 2 == 0) { crawler_vx[i] = 2.4f; crawler_facing_right[i] = true; }
    else                 { crawler_vx[i] = -2.4f; crawler_facing_right[i] = false; }

    spawned_count++;
    spawn_timer = 60 + rand() % 120;
}

void update_crawlers(float crawler_x[], float crawler_y[], float crawler_vx[], float crawler_vy[], bool crawler_alive[], bool crawler_facing_right[], bool crawler_on_ground[], const int max_crawlers, const int crawler_width, const int crawler_height, const float gravity, const float terminal_velocity, const int current_screen) {
    for (int i = 0; i < max_crawlers; i++) {
        if (!crawler_alive[i]) continue;

        if (!crawler_on_ground[i]) {
            float old_bottom = crawler_y[i] + crawler_height;
            crawler_y[i] += crawler_vy[i];
            crawler_vy[i] += gravity;
            if (crawler_vy[i] > terminal_velocity) crawler_vy[i] = terminal_velocity;

            float floor_y = find_floor(current_screen, crawler_x[i], crawler_x[i] + crawler_width, old_bottom, crawler_y[i] + crawler_height);
            if (floor_y >= 0.0f) {
                crawler_y[i] = floor_y - crawler_height;
                crawler_vy[i] = 0.0f;
                crawler_on_ground[i] = true;
            } else if (crawler_y[i] > screen_y) {
                crawler_alive[i] = false;
            }
        } else {
            crawler_x[i] += crawler_vx[i];
            float front_x = (crawler_vx[i] > 0.0f) ? crawler_x[i] + crawler_width : crawler_x[i];
            float feet_y = crawler_y[i] + crawler_height;
            if (find_floor(current_screen, front_x - 2.0f, front_x + 2.0f, feet_y - 1.0f, feet_y + 1.0f) < 0.0f) {
                crawler_x[i] -= crawler_vx[i];
                crawler_vx[i] = -crawler_vx[i];
                crawler_facing_right[i] = (crawler_vx[i] > 0.0f);
            }

            if (crawler_x[i] <= 0.0f) {
                crawler_x[i] = 0.0f;
                crawler_vx[i] = -crawler_vx[i];
                crawler_facing_right[i] = true;
            } else if (crawler_x[i] + crawler_width >= screen_x) {
                crawler_x[i] = (float)(screen_x - crawler_width);
                crawler_vx[i] = -crawler_vx[i];
                crawler_facing_right[i] = false;
            }
        }
    }
}

void display_crawlers(RenderWindow& window, Sprite& crawler_sprite, Texture& crawler_texture, float crawler_x[], float crawler_y[], bool crawler_alive[], bool crawler_facing_right[], const int max_crawlers, const int crawler_width, const float crawler_scale) {
    crawler_sprite.setTexture(crawler_texture, true);
    for (int i = 0; i < max_crawlers; i++) {
        if (!crawler_alive[i]) continue;
        float scale_x = crawler_facing_right[i] ? crawler_scale : -crawler_scale;
        float draw_x = crawler_facing_right[i] ? crawler_x[i] : crawler_x[i] + crawler_width;
        crawler_sprite.setScale(scale_x, crawler_scale);
        crawler_sprite.setPosition(draw_x, crawler_y[i]);
        window.draw(crawler_sprite);
    }
}

void init_blue_enemies(bool blue_alive[], const int max_blue_enemies) {
    for (int i = 0; i < max_blue_enemies; i++) blue_alive[i] = false;
}

void spawn_next_blue_enemy(int& spawned_count, int& spawn_timer, float player_x, float blue_x[], float blue_y[], float blue_vx[], float blue_vy[], int blue_jump_timer[], bool blue_alive[], bool blue_facing_right[], bool blue_on_ground[], const int max_blue_enemies, const int blue_width, const int blue_height, const int current_screen) {
    if (spawned_count >= max_blue_enemies) return;
    if (spawn_timer > 0) { spawn_timer--; return; }

    int i = spawned_count;
    blue_alive[i] = true;
    blue_x[i] = random_girder_x(current_screen, blue_width);
    blue_y[i] = (float)(-blue_height);
    blue_vx[i] = 0.0f;
    blue_vy[i] = 0.0f;
    blue_on_ground[i] = false;
    blue_facing_right[i] = (player_x > blue_x[i]);
    blue_jump_timer[i] = 20 + rand() % 40;

    spawned_count++;
    spawn_timer = 60 + rand() % 120;
}

void update_blue_enemies(float player_x, float blue_x[], float blue_y[], float blue_vx[], float blue_vy[], int blue_jump_timer[], bool blue_alive[], bool blue_facing_right[], bool blue_on_ground[], const int max_blue_enemies, const int blue_width, const int blue_height, const float gravity, const float terminal_velocity, const int current_screen) {
    for (int i = 0; i < max_blue_enemies; i++) {
        if (!blue_alive[i]) continue;

        if (blue_on_ground[i]) {
            blue_facing_right[i] = (player_x > blue_x[i]);
            if (blue_jump_timer[i] > 0) {
                blue_jump_timer[i]--;
            } else {
                float jump_speed_x = 2.2f + (float)(rand() % 13) / 10.0f;
                blue_vy[i] = -9.5f - (float)(rand() % 4);
                blue_vx[i] = blue_facing_right[i] ? jump_speed_x : -jump_speed_x;
                blue_on_ground[i] = false;
            }
        } else {
            blue_x[i] += blue_vx[i];
            if (blue_x[i] < 0.0f) blue_x[i] = 0.0f;
            else if (blue_x[i] + blue_width > screen_x) blue_x[i] = (float)(screen_x - blue_width);

            float old_bottom = blue_y[i] + blue_height;
            blue_y[i] += blue_vy[i];
            blue_vy[i] += gravity;
            if (blue_vy[i] > terminal_velocity) blue_vy[i] = terminal_velocity;

            float floor_y = -1.0f;
            if (blue_vy[i] >= 0.0f) {
                floor_y = find_floor(current_screen, blue_x[i], blue_x[i] + blue_width, old_bottom, blue_y[i] + blue_height);
            }
            if (floor_y >= 0.0f) {
                blue_y[i] = floor_y - blue_height;
                blue_vx[i] = 0.0f;
                blue_vy[i] = 0.0f;
                blue_on_ground[i] = true;
                blue_jump_timer[i] = 25 + rand() % 45;
            } else if (blue_y[i] > screen_y) {
                blue_alive[i] = false;
            }
        }
    }
}

void display_blue_enemies(RenderWindow& window, Sprite& blue_sprite, Texture& blue_stand_texture, Texture& blue_jump_texture, float blue_x[], float blue_y[], bool blue_alive[], bool blue_facing_right[], bool blue_on_ground[], const int max_blue_enemies, const int blue_width, const float blue_scale) {
    for (int i = 0; i < max_blue_enemies; i++) {
        if (!blue_alive[i]) continue;
        if (blue_on_ground[i]) blue_sprite.setTexture(blue_stand_texture, true);
        else blue_sprite.setTexture(blue_jump_texture, true);

        float scale_x = blue_facing_right[i] ? blue_scale : -blue_scale;
        float draw_x = blue_facing_right[i] ? blue_x[i] : blue_x[i] + blue_width;
        blue_sprite.setScale(scale_x, blue_scale);
        blue_sprite.setPosition(draw_x, blue_y[i]);
        window.draw(blue_sprite);
    }
}

void init_flying_insects(bool insect_alive[], const int max_insects) {
    for (int i = 0; i < max_insects; i++) insect_alive[i] = false;
}

void spawn_next_flying_insect(int& spawned_count, int& spawn_timer, float insect_x[], float insect_y[], float insect_base_y[], float insect_vx[], float insect_wave_angle[], bool insect_alive[], bool insect_facing_right[], const int max_insects, const int insect_width, const int insect_height, const int ground_y) {
    if (spawned_count >= max_insects) return;
    if (spawn_timer > 0) { spawn_timer--; return; }

    int i = spawned_count;
    int min_fly_y = 120;
    int max_fly_y = ground_y - insect_height - 90;
    if (max_fly_y <= min_fly_y) max_fly_y = min_fly_y + 1;

    insect_alive[i] = true;
    insect_x[i] = (float)(screen_x - insect_width);
    insect_base_y[i] = (float)(min_fly_y + rand() % (max_fly_y - min_fly_y));
    insect_wave_angle[i] = (float)(rand() % 628) / 100.0f;
    insect_y[i] = insect_base_y[i] + sin(insect_wave_angle[i]) * 45.0f;
    insect_vx[i] = -3.2f;
    insect_facing_right[i] = false;

    spawned_count++;
    spawn_timer = 75 + rand() % 90;
}

void update_flying_insects(float insect_x[], float insect_y[], float insect_base_y[], float insect_vx[], float insect_wave_angle[], bool insect_alive[], bool insect_facing_right[], const int max_insects, const int insect_width, const int insect_height, const int ground_y) {
    for (int i = 0; i < max_insects; i++) {
        if (!insect_alive[i]) continue;
        insect_x[i] += insect_vx[i];
        if (insect_x[i] <= 0.0f) {
            insect_x[i] = 0.0f;
            insect_vx[i] = -insect_vx[i];
            insect_facing_right[i] = true;
        } else if (insect_x[i] + insect_width >= screen_x) {
            insect_x[i] = (float)(screen_x - insect_width);
            insect_vx[i] = -insect_vx[i];
            insect_facing_right[i] = false;
        }

        insect_wave_angle[i] += 0.07f;
        insect_y[i] = insect_base_y[i] + sin(insect_wave_angle[i]) * 45.0f;
        if (insect_y[i] < 0.0f) insect_y[i] = 0.0f;
        else if (insect_y[i] + insect_height > ground_y) insect_y[i] = (float)(ground_y - insect_height);
    }
}

void display_flying_insects(RenderWindow& window, Sprite& insect_sprite, Texture& insect_texture, float insect_x[], float insect_y[], bool insect_alive[], bool insect_facing_right[], const int max_insects, const int insect_width, const float insect_scale) {
    insect_sprite.setTexture(insect_texture, true);
    for (int i = 0; i < max_insects; i++) {
        if (!insect_alive[i]) continue;
        float scale_x = insect_facing_right[i] ? insect_scale : -insect_scale;
        float draw_x = insect_facing_right[i] ? insect_x[i] : insect_x[i] + insect_width;
        insect_sprite.setScale(scale_x, insect_scale);
        insect_sprite.setPosition(draw_x, insect_y[i]);
        window.draw(insect_sprite);
    }
}

void init_coins(bool coin_active[], int coin_frame[], int coin_anim_timer[], int coin_pickup_delay[], const int max_coins) {
    for (int i = 0; i < max_coins; i++) {
        coin_active[i] = false; coin_frame[i] = 0; coin_anim_timer[i] = 0; coin_pickup_delay[i] = 0;
    }
}

void spawn_coin(float coin_x[], float coin_y[], float coin_vy[], int coin_frame[], int coin_anim_timer[], int coin_pickup_delay[], bool coin_active[], int& coin_next_slot, const int max_coins, float spawn_x, float spawn_y) {
    int slot = -1;
    for (int i = 0; i < max_coins; i++) { if (!coin_active[i]) { slot = i; break; } }
    if (slot == -1) { slot = coin_next_slot; coin_next_slot = (coin_next_slot + 1) % max_coins; }

    coin_active[slot] = true; coin_x[slot] = spawn_x; coin_y[slot] = spawn_y;
    coin_vy[slot] = -12.0f; coin_frame[slot] = 0; coin_anim_timer[slot] = 0; coin_pickup_delay[slot] = 20;
}

void update_coins(float coin_x[], float coin_y[], float coin_vy[], int coin_frame[], int coin_anim_timer[], int coin_pickup_delay[], bool coin_active[], const int max_coins, const float coin_gravity, const int current_screen, const float coin_radius, float player_x, float player_y, int player_width, int player_height, int& score, Sound& coin_sound, const int stand_height) {
    for (int i = 0; i < max_coins; i++) {
        if (!coin_active[i]) continue;
        if (coin_pickup_delay[i] > 0) coin_pickup_delay[i]--;

        coin_anim_timer[i]++;
        if (coin_anim_timer[i] >= 4) { coin_anim_timer[i] = 0; coin_frame[i] = (coin_frame[i] + 1) % 6; }

        float next_y = coin_y[i] + coin_vy[i];
        float coin_floor = -1.0f;
        if (coin_vy[i] >= 0.0f) {
            coin_floor = find_floor(current_screen, coin_x[i] - coin_radius, coin_x[i] + coin_radius, coin_y[i] + coin_radius, next_y + coin_radius);
        }
        if (coin_floor >= 0.0f) {
            coin_y[i] = coin_floor - coin_radius;
            coin_vy[i] = 0.0f;
        } else {
            coin_y[i] = next_y;
            coin_vy[i] += coin_gravity;
            if (coin_y[i] - coin_radius > screen_y) { coin_active[i] = false; continue; }
        }

        if (coin_pickup_delay[i] <= 0) {
            float coin_left = coin_x[i] - coin_radius;
            float coin_top = coin_y[i] - coin_radius;
            float coin_size = coin_radius * 2.0f;
            if (player_x < coin_left + coin_size && player_x + player_width > coin_left &&
                player_y < coin_top + coin_size && player_y + player_height > coin_top) {
                coin_active[i] = false;
                score += 100;
                coin_sound.play();
            }
        }
    }
}

void display_coins(RenderWindow& window, Sprite& coin_sprite, Texture& coin_texture, float coin_x[], float coin_y[], int coin_frame[], bool coin_active[], const int max_coins, const int coin_frame_w, const int coin_frame_h, const float coin_scale) {
    coin_sprite.setTexture(coin_texture, false);
    coin_sprite.setOrigin(coin_frame_w * 0.5f, coin_frame_h * 0.5f);
    coin_sprite.setScale(coin_scale, coin_scale);
    for (int i = 0; i < max_coins; i++) {
        if (!coin_active[i]) continue;
        coin_sprite.setTextureRect(IntRect(coin_frame[i] * coin_frame_w, 0, coin_frame_w, coin_frame_h));
        coin_sprite.setPosition(coin_x[i], coin_y[i]);
        window.draw(coin_sprite);
    }
}

void init_refills(bool refill_active[], int refill_pickup_delay[], const int max_refills) {
    for (int i = 0; i < max_refills; i++) { refill_active[i] = false; refill_pickup_delay[i] = 0; }
}

void spawn_refill(float refill_x[], float refill_y[], float refill_vy[], int refill_pickup_delay[], bool refill_active[], int& refill_next_slot, const int max_refills, float spawn_x, float spawn_y) {
    int slot = -1;
    for (int i = 0; i < max_refills; i++) { if (!refill_active[i]) { slot = i; break; } }
    if (slot == -1) { slot = refill_next_slot; refill_next_slot = (refill_next_slot + 1) % max_refills; }

    refill_active[slot] = true; refill_x[slot] = spawn_x; refill_y[slot] = spawn_y;
    refill_vy[slot] = -12.0f; refill_pickup_delay[slot] = 20;
}

void update_refills(float refill_x[], float refill_y[], float refill_vy[], int refill_pickup_delay[], bool refill_active[], const int max_refills, const float refill_gravity, const int current_screen, const float refill_half_w, const float refill_half_h, float player_x, float player_y, int player_width, int player_height, Sound& collect_sound, const int stand_height) {
    for (int i = 0; i < max_refills; i++) {
        if (!refill_active[i]) continue;
        if (refill_pickup_delay[i] > 0) refill_pickup_delay[i]--;

        float next_y = refill_y[i] + refill_vy[i];
        float refill_floor = -1.0f;
        if (refill_vy[i] >= 0.0f) {
            refill_floor = find_floor(current_screen, refill_x[i] - refill_half_w, refill_x[i] + refill_half_w, refill_y[i] + refill_half_h, next_y + refill_half_h);
        }
        if (refill_floor >= 0.0f) {
            refill_y[i] = refill_floor - refill_half_h;
            refill_vy[i] = 0.0f;
        } else {
            refill_y[i] = next_y;
            refill_vy[i] += refill_gravity;
            if (refill_y[i] - refill_half_h > screen_y) { refill_active[i] = false; continue; }
        }

        if (refill_pickup_delay[i] <= 0) {
            float refill_left = refill_x[i] - refill_half_w;
            float refill_top = refill_y[i] - refill_half_h;
            float refill_w = refill_half_w * 2.0f;
            float refill_h = refill_half_h * 2.0f;
            if (player_x < refill_left + refill_w && player_x + player_width > refill_left &&
                player_y < refill_top + refill_h && player_y + player_height > refill_top) {
                refill_active[i] = false;
                collect_sound.play();
            }
        }
    }
}

void display_refills(RenderWindow& window, Sprite& refill_sprite, Texture& refill_texture, float refill_x[], float refill_y[], bool refill_active[], const int max_refills, const int refill_w, const int refill_h, const float refill_scale_x, const float refill_scale_y, const int raw_refill_w) {
    refill_sprite.setTexture(refill_texture, true);
    refill_sprite.setOrigin((float)raw_refill_w * 0.5f, (float)refill_h * 0.5f);
    refill_sprite.setScale(refill_scale_x, refill_scale_y);
    for (int i = 0; i < max_refills; i++) {
        if (!refill_active[i]) continue;
        refill_sprite.setPosition(refill_x[i], refill_y[i]);
        window.draw(refill_sprite);
    }
}
