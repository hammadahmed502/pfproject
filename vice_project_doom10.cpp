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

void display_background(RenderWindow&, Sprite&);
void play_hit_sound(Sound&);
void player_kneel(float&, int&, const int, const int, bool&, bool);
void player_movement(float&, float&, float, const int, float&, bool&, bool, bool&, bool&, bool&, Sound&, const float, const float, const int, int&, const int, bool&);
void player_gravity(float&, float&, bool&, Sound&, const float, const float, const int, const int, float, int, float[], float[], float[], float[], int, bool&);
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
void spawn_next_crawler(int&, int&, float[], float[], float[], float[], bool[], bool[], bool[], const int, const int, const int);
void update_crawlers(float[], float[], float[], float[], bool[], bool[], bool[], const int, const int, const int, const float, const float, const int);
void display_crawlers(RenderWindow&, Sprite&, Texture&, float[], float[], bool[], bool[], const int, const int, const float);
void init_blue_enemies(bool[], const int);
void spawn_next_blue_enemy(int&, int&, float, float[], float[], float[], float[], int[], bool[], bool[], bool[], const int, const int, const int);
void update_blue_enemies(float, float[], float[], float[], float[], int[], bool[], bool[], bool[], const int, const int, const int, const float, const float, const int);
void display_blue_enemies(RenderWindow&, Sprite&, Texture&, Texture&, float[], float[], bool[], bool[], bool[], const int, const int, const float);
void init_flying_insects(bool[], const int);
void spawn_next_flying_insect(int&, int&, float[], float[], float[], float[], float[], bool[], bool[], const int, const int, const int, const int);
void update_flying_insects(float[], float[], float[], float[], float[], bool[], bool[], const int, const int, const int, const int);
void display_flying_insects(RenderWindow&, Sprite&, Texture&, float[], float[], bool[], bool[], const int, const int, const float);
void init_coins(bool[], int[], int[], int[], const int);
void spawn_coin(float[], float[], float[], int[], int[], int[], bool[], int&, const int, float, float);
void update_coins(float[], float[], float[], int[], int[], int[], bool[], const int, const float, const int, const float, float, float, int, int, int&, Sound&);
void display_coins(RenderWindow&, Sprite&, Texture&, float[], float[], int[], bool[], const int, const int, const int, const float);
void init_refills(bool[], int[], const int);
void spawn_refill(float[], float[], float[], int[], bool[], int&, const int, float, float);
void update_refills(float[], float[], float[], int[], bool[], const int, const float, const int, const float, const float, float, float, int, int, Sound&);
void display_refills(RenderWindow&, Sprite&, Texture&, float[], float[], bool[], const int, const int, const int, const float, const float, const int);
void draw_platforms_and_ladders(RenderWindow&, Sprite&, Texture&, Sprite&, Texture&, int);

int main() {
    srand((unsigned int)time(0));

    RenderWindow window(VideoMode(screen_x, screen_y), "VICE: Project Doom - Custom Box & Ladder Textures", Style::Resize);
    window.setFramerateLimit(60);

    const int ground_y = 815;

    Texture bg_texture;
    bg_texture.loadFromFile("Data/bg.png");
    Sprite bg_sprite(bg_texture);
    Vector2u bg_size = bg_texture.getSize();
    if (bg_size.x > 0 && bg_size.y > 0) {
        bg_sprite.setScale((float)screen_x / bg_size.x, (float)screen_y / bg_size.y);
    }

    Texture block_texture;
    block_texture.loadFromFile("Data/block1.png");
    Sprite block_sprite(block_texture);

    Texture ladder_texture;
    if (!ladder_texture.loadFromFile("Data/ladder.png")) {
        ladder_texture.loadFromFile("Data/block1.png");
    }
    Sprite ladder_sprite(ladder_texture);

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
    int score = 0;
    int current_screen = 1;
    const int max_screens = 4;
    bool is_on_ladder = false;

    float player_x = 120.0f;
    float player_y = (float)(ground_y - stand_height);
    float velocity_y = 0.0f;

    const float gravity = 1.0f;
    const float walk_speed = 6.0f;
    const float jump_velocity = -17.5f;
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

    Texture run_texture;
    Texture stand_whip_texture;
    Texture run_whip_texture;
    Texture crouch_whip_texture;
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

    const float size_ratio = 0.825f;
    const float crawler_scale = ((float)stand_height / (float)raw_crawler_height) * size_ratio;
    const int crawler_width = (int)(raw_crawler_width * crawler_scale + 0.5f);
    const int crawler_height = (int)(raw_crawler_height * crawler_scale + 0.5f);

    Sprite crawler_sprite(crawler_texture);

    const int max_crawlers = 4;
    float crawler_x[max_crawlers];
    float crawler_y[max_crawlers];
    float crawler_vx[max_crawlers];
    float crawler_vy[max_crawlers];
    bool crawler_alive[max_crawlers];
    bool crawler_facing_right[max_crawlers];
    bool crawler_on_ground[max_crawlers];

    int crawler_spawned_count = 0;
    int crawler_spawn_timer = 30;

    init_crawlers(crawler_alive, max_crawlers);

    Texture blue_stand_texture;
    Texture blue_jump_texture;
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
    float blue_x[max_blue_enemies];
    float blue_y[max_blue_enemies];
    float blue_vx[max_blue_enemies];
    float blue_vy[max_blue_enemies];
    int blue_jump_timer[max_blue_enemies];
    bool blue_alive[max_blue_enemies];
    bool blue_facing_right[max_blue_enemies];
    bool blue_on_ground[max_blue_enemies];

    int blue_spawned_count = 0;
    int blue_spawn_timer = 45;

    init_blue_enemies(blue_alive, max_blue_enemies);

    Texture insect_texture;
    insect_texture.loadFromFile("Data/flying_insect.png");

    int raw_insect_width = (int)insect_texture.getSize().x;
    int raw_insect_height = (int)insect_texture.getSize().y;
    if (raw_insect_width <= 0) raw_insect_width = 24;
    if (raw_insect_height <= 0) raw_insect_height = 18;

    const float insect_size_ratio = 0.55f;
    const float insect_scale = ((float)stand_height / (float)raw_insect_height) * insect_size_ratio;
    const int insect_width = (int)(raw_insect_width * insect_scale + 0.5f);
    const int insect_height = (int)(raw_insect_height * insect_scale + 0.5f);

    Sprite insect_sprite(insect_texture);

    const int max_insects = 6;
    float insect_x[max_insects];
    float insect_y[max_insects];
    float insect_base_y[max_insects];
    float insect_vx[max_insects];
    float insect_wave_angle[max_insects];
    bool insect_alive[max_insects];
    bool insect_facing_right[max_insects];

    int insect_spawned_count = 0;
    int insect_spawn_timer = 60;

    init_flying_insects(insect_alive, max_insects);

    Texture coin_texture;
    coin_texture.loadFromFile("Data/coin_flip_96x16.png");

    const int coin_total_frames = 6;
    int raw_coin_sheet_w = (int)coin_texture.getSize().x;
    int raw_coin_sheet_h = (int)coin_texture.getSize().y;
    if (raw_coin_sheet_w <= 0) raw_coin_sheet_w = 96;
    if (raw_coin_sheet_h <= 0) raw_coin_sheet_h = 16;

    const int coin_frame_w = raw_coin_sheet_w / coin_total_frames;
    const int coin_frame_h = raw_coin_sheet_h;
    const float coin_scale = 40.0f / (float)coin_frame_h;
    const float coin_radius = (coin_frame_h * coin_scale) * 0.5f;

    Sprite coin_sprite;
    coin_sprite.setTexture(coin_texture, true);

    const int max_coins = 32;
    float coin_x[max_coins];
    float coin_y[max_coins];
    float coin_vy[max_coins];
    int coin_frame[max_coins];
    int coin_anim_timer[max_coins];
    int coin_pickup_delay[max_coins];
    bool coin_active[max_coins];
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
    float refill_x[max_refills];
    float refill_y[max_refills];
    float refill_vy[max_refills];
    int refill_pickup_delay[max_refills];
    bool refill_active[max_refills];
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

        player_kneel(player_y, player_height, stand_height, kneel_height, is_kneeling, on_ground);
        player_movement(player_x, player_y, ground_y, player_height, velocity_y, on_ground, is_kneeling, facing_right, is_moving, was_against_wall, hit_sound, walk_speed, jump_velocity, player_width, current_screen, max_screens, is_on_ladder);
        
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
        }

        float p_x[2], p_y[2], p_w[2], p_h[2];
        int p_count = 0;
        if (current_screen == 1) {
            p_count = 1;
            p_x[0] = 300.0f; p_y[0] = 620.0f; p_w[0] = 520.0f; p_h[0] = 24.0f;
        } else if (current_screen == 2) {
            p_count = 2;
            p_x[0] = 150.0f; p_y[0] = 640.0f; p_w[0] = 380.0f; p_h[0] = 24.0f;
            p_x[1] = 650.0f; p_y[1] = 550.0f; p_w[1] = 360.0f; p_h[1] = 24.0f;
        } else if (current_screen == 3) {
            p_count = 1;
            p_x[0] = 200.0f; p_y[0] = 600.0f; p_w[0] = 720.0f; p_h[0] = 24.0f;
        } else if (current_screen == 4) {
            p_count = 2;
            p_x[0] = 100.0f; p_y[0] = 640.0f; p_w[0] = 350.0f; p_h[0] = 24.0f;
            p_x[1] = 600.0f; p_y[1] = 530.0f; p_w[1] = 420.0f; p_h[1] = 24.0f;
        }

        player_gravity(player_y, velocity_y, on_ground, hit_sound, gravity, terminal_velocity, ground_y, player_height, player_x, player_width, p_x, p_y, p_w, p_h, p_count, is_on_ladder);
        update_player_whip(is_whipping, was_q_pressed, whip_frame, whip_timer, whip_delay, whip_sound);

        update_whip_hitbox(is_whipping, is_kneeling, on_ground, facing_right, whip_frame, player_x, player_y, player_width, player_height, whip_hitbox_active, whip_box_x, whip_box_y, whip_box_w, whip_box_h);
        
        check_whip_hits(whip_hitbox_active, whip_box_x, whip_box_y, whip_box_w, whip_box_h,
                        crawler_x, crawler_y, crawler_alive, max_crawlers, crawler_width, crawler_height,
                        blue_x, blue_y, blue_alive, max_blue_enemies, blue_width, blue_height,
                        insect_x, insect_y, insect_alive, max_insects, insect_width, insect_height,
                        coin_x, coin_y, coin_vy, coin_frame, coin_anim_timer, coin_pickup_delay, coin_active, coin_next_slot, max_coins,
                        refill_x, refill_y, refill_vy, refill_pickup_delay, refill_active, refill_next_slot, max_refills, hit_sound);

        spawn_next_crawler(crawler_spawned_count, crawler_spawn_timer, crawler_x, crawler_y, crawler_vx, crawler_vy, crawler_alive, crawler_facing_right, crawler_on_ground, max_crawlers, crawler_width, crawler_height);
        update_crawlers(crawler_x, crawler_y, crawler_vx, crawler_vy, crawler_alive, crawler_facing_right, crawler_on_ground, max_crawlers, crawler_width, crawler_height, gravity, terminal_velocity, ground_y);

        spawn_next_blue_enemy(blue_spawned_count, blue_spawn_timer, player_x, blue_x, blue_y, blue_vx, blue_vy, blue_jump_timer, blue_alive, blue_facing_right, blue_on_ground, max_blue_enemies, blue_width, blue_height);
        update_blue_enemies(player_x, blue_x, blue_y, blue_vx, blue_vy, blue_jump_timer, blue_alive, blue_facing_right, blue_on_ground, max_blue_enemies, blue_width, blue_height, gravity, terminal_velocity, ground_y);

        spawn_next_flying_insect(insect_spawned_count, insect_spawn_timer, insect_x, insect_y, insect_base_y, insect_vx, insect_wave_angle, insect_alive, insect_facing_right, max_insects, insect_width, insect_height, ground_y);
        update_flying_insects(insect_x, insect_y, insect_base_y, insect_vx, insect_wave_angle, insect_alive, insect_facing_right, max_insects, insect_width, insect_height, ground_y);

        update_coins(coin_x, coin_y, coin_vy, coin_frame, coin_anim_timer, coin_pickup_delay, coin_active, max_coins, gravity * 0.55f, ground_y, coin_radius, player_x, player_y, player_width, player_height, score, coin_sound);
        update_refills(refill_x, refill_y, refill_vy, refill_pickup_delay, refill_active, max_refills, gravity * 0.55f, ground_y, refill_half_w, refill_half_h, player_x, player_y, player_width, player_height, coin_sound);

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
        draw_platforms_and_ladders(window, block_sprite, block_texture, ladder_sprite, ladder_texture, current_screen);
        display_crawlers(window, crawler_sprite, crawler_texture, crawler_x, crawler_y, crawler_alive, crawler_facing_right, max_crawlers, crawler_width, crawler_scale);
        display_blue_enemies(window, blue_sprite, blue_stand_texture, blue_jump_texture, blue_x, blue_y, blue_alive, blue_facing_right, blue_on_ground, max_blue_enemies, blue_width, blue_scale);
        display_flying_insects(window, insect_sprite, insect_texture, insect_x, insect_y, insect_alive, insect_facing_right, max_insects, insect_width, insect_scale);
        display_coins(window, coin_sprite, coin_texture, coin_x, coin_y, coin_frame, coin_active, max_coins, coin_frame_w, coin_frame_h, coin_scale);
        display_refills(window, refill_sprite, refill_texture, refill_x, refill_y, refill_active, max_refills, raw_refill_w, raw_refill_h, refill_scale, refill_scale, raw_refill_w);
        window.draw(player_sprite);

        if (font_loaded) {
            hud_text.setString("Score: " + to_string(score) + "   Screen: " + to_string(current_screen) + " / " + to_string(max_screens));
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

void draw_platforms_and_ladders(RenderWindow& window, Sprite& block_sprite, Texture& block_texture, Sprite& ladder_sprite, Texture& ladder_texture, int current_screen) {
    Vector2u b_size = block_texture.getSize();
    float b_w = (b_size.x > 0) ? b_size.x : 24.0f;
    float b_h = (b_size.y > 0) ? b_size.y : 24.0f;

    auto draw_platform_line = [&](float start_x, float y, float total_w, float h) {
        int count = (int)(total_w / b_w) + 1;
        for (int i = 0; i < count; i++) {
            float cx = start_x + i * b_w;
            if (cx < start_x + total_w) {
                block_sprite.setPosition(cx, y);
                block_sprite.setScale(b_w > 0 ? b_w / b_w : 1.f, h / b_h);
                window.draw(block_sprite);
            }
        }
    };

    if (current_screen == 1) {
        draw_platform_line(300.f, 620.f, 520.f, 24.f);
        ladder_sprite.setPosition(540.f, 620.f);
        window.draw(ladder_sprite);
    } else if (current_screen == 2) {
        draw_platform_line(150.f, 640.f, 380.f, 24.f);
        draw_platform_line(650.f, 550.f, 360.f, 24.f);
        ladder_sprite.setPosition(300.f, 640.f);
        window.draw(ladder_sprite);
        ladder_sprite.setPosition(800.f, 550.f);
        window.draw(ladder_sprite);
    } else if (current_screen == 3) {
        draw_platform_line(200.f, 600.f, 720.f, 24.f);
        ladder_sprite.setPosition(500.f, 600.f);
        window.draw(ladder_sprite);
    } else if (current_screen == 4) {
        draw_platform_line(100.f, 640.f, 350.f, 24.f);
        draw_platform_line(600.f, 530.f, 420.f, 24.f);
        ladder_sprite.setPosition(240.f, 640.f);
        window.draw(ladder_sprite);
        ladder_sprite.setPosition(780.f, 530.f);
        window.draw(ladder_sprite);
    }
}

void play_hit_sound(Sound& hit_sound) {
    hit_sound.play();
    hit_sound.setPlayingOffset(milliseconds(80));
}

void player_kneel(float& player_y, int& player_height, const int stand_height, const int kneel_height, bool& is_kneeling, bool on_ground) {
    bool kneel_pressed = Keyboard::isKeyPressed(Keyboard::Down);

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
        is_on_ladder = false;
    }
    if (Keyboard::isKeyPressed(Keyboard::Right)) {
        move_x += current_speed;
        is_on_ladder = false;
    }

    float ladder_x = 0.f;
    if (current_screen == 1) ladder_x = 540.f;
    else if (current_screen == 2) ladder_x = 300.f;
    else if (current_screen == 3) ladder_x = 500.f;
    else if (current_screen == 4) ladder_x = 240.f;

    float platform_top_for_ladder = 620.f;
    if (current_screen == 1) platform_top_for_ladder = 620.f;
    else if (current_screen == 2) platform_top_for_ladder = 550.f; // bottom/top platforms
    else if (current_screen == 3) platform_top_for_ladder = 600.f;
    else if (current_screen == 4) platform_top_for_ladder = 530.f;

    // Only allow ladder climbing if player is at ground level or around platform Y, not trapped right under a block
    if (player_x + player_width >= ladder_x - 15.f && player_x <= ladder_x + 50.f) {
        if (Keyboard::isKeyPressed(Keyboard::Up)) {
            // Check if player is near ground or platform base
            if (player_y >= ground_y - player_height - 20.f || (player_y >= platform_top_for_ladder - 30.f && player_y <= platform_top_for_ladder + 40.f)) {
                velocity_y = -4.0f;
                is_on_ladder = true;
                on_ground = false;
            }
        } else if (Keyboard::isKeyPressed(Keyboard::Down)) {
            if (player_y >= ground_y - player_height - 20.f || player_y <= platform_top_for_ladder + 20.f) {
                velocity_y = 4.0f;
                is_on_ladder = true;
                on_ground = false;
            }
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
    player_x += move_x;

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

    bool jump_pressed = Keyboard::isKeyPressed(Keyboard::Up);
    if (jump_pressed && on_ground && !is_kneeling && !is_on_ladder) {
        velocity_y = jump_velocity;
        on_ground = false;
    }
}

void player_gravity(float& player_y, float& velocity_y, bool& on_ground, Sound& hit_sound, const float gravity, const float terminal_velocity, const int ground_y, const int player_height, float player_x, int player_width, float p_x[], float p_y[], float p_w[], float p_h[], int p_count, bool& is_on_ladder) {
    if (is_on_ladder) {
        player_y += velocity_y;
        return;
    }

    bool was_on_ground = on_ground;
    float next_y = player_y + velocity_y;

    bool landed = false;
    float landing_y = (float)(ground_y - player_height);

    if (next_y + player_height >= ground_y) {
        landing_y = (float)(ground_y - player_height);
        landed = true;
    }

    for (int i = 0; i < p_count; i++) {
        float plat_top = p_y[i];
        float plat_left = p_x[i];
        float plat_right = p_x[i] + p_w[i];
        bool over_platform = (player_x + player_width > plat_left + 4.0f && player_x < plat_right - 4.0f);
        if (over_platform && velocity_y >= 0.0f && player_y + player_height <= plat_top + 14.0f && next_y + player_height >= plat_top) {
            landed = true;
            landing_y = plat_top - player_height;
        }
    }

    if (landed && velocity_y >= 0.0f) {
        on_ground = true;
        player_y = landing_y;
        velocity_y = 0.0f;
        if (!was_on_ground) {
            play_hit_sound(hit_sound);
        }
    } else {
        player_y = next_y;
        on_ground = false;
        velocity_y += gravity;
        if (velocity_y > terminal_velocity) {
            velocity_y = terminal_velocity;
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

    int local_x = 0;
    int local_y = 0;
    int local_w = 0;
    int local_h = 0;

    if (is_kneeling && on_ground) {
        if (whip_frame == 3) {
            local_x = 25;
            local_y = 9;
            local_w = 20;
            local_h = 23;
        } else {
            local_x = 31;
            local_y = 35;
            local_w = 18;
            local_h = 13;
        }
    } else {
        if (whip_frame == 3) {
            local_x = 25;
            local_y = 0;
            local_w = 20;
            local_h = 25;
        } else {
            local_x = 31;
            local_y = 26;
            local_w = 17;
            local_h = 16;
        }
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
    if (!whip_hitbox_active) {
        return;
    }

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
    for (int i = 0; i < max_crawlers; i++) {
        crawler_alive[i] = false;
    }
}

void spawn_next_crawler(int& spawned_count, int& spawn_timer, float crawler_x[], float crawler_y[], float crawler_vx[], float crawler_vy[], bool crawler_alive[], bool crawler_facing_right[], bool crawler_on_ground[], const int max_crawlers, const int crawler_width, const int crawler_height) {
    if (spawned_count >= max_crawlers) {
        return;
    }

    if (spawn_timer > 0) {
        spawn_timer--;
        return;
    }

    int i = spawned_count;
    int max_spawn_x = screen_x - crawler_width;
    if (max_spawn_x < 1) max_spawn_x = 1;

    crawler_alive[i] = true;
    crawler_x[i] = (float)(rand() % max_spawn_x);
    crawler_y[i] = (float)(-crawler_height);
    crawler_vy[i] = 0.0f;
    crawler_on_ground[i] = false;

    if (rand() % 2 == 0) {
        crawler_vx[i] = 2.4f;
        crawler_facing_right[i] = true;
    } else {
        crawler_vx[i] = -2.4f;
        crawler_facing_right[i] = false;
    }

    spawned_count++;
    spawn_timer = 60 + rand() % 120;
}

void update_crawlers(float crawler_x[], float crawler_y[], float crawler_vx[], float crawler_vy[], bool crawler_alive[], bool crawler_facing_right[], bool crawler_on_ground[], const int max_crawlers, const int crawler_width, const int crawler_height, const float gravity, const float terminal_velocity, const int ground_y) {
    for (int i = 0; i < max_crawlers; i++) {
        if (!crawler_alive[i]) {
            continue;
        }

        if (!crawler_on_ground[i]) {
            crawler_y[i] += crawler_vy[i];
            crawler_vy[i] += gravity;

            if (crawler_vy[i] > terminal_velocity) {
                crawler_vy[i] = terminal_velocity;
            }

            if (crawler_y[i] + crawler_height >= ground_y) {
                crawler_y[i] = (float)(ground_y - crawler_height);
                crawler_vy[i] = 0.0f;
                crawler_on_ground[i] = true;
            }
        } else {
            crawler_x[i] += crawler_vx[i];

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
        if (!crawler_alive[i]) {
            continue;
        }

        float scale_x = crawler_facing_right[i] ? crawler_scale : -crawler_scale;
        float draw_x = crawler_facing_right[i] ? crawler_x[i] : crawler_x[i] + crawler_width;

        crawler_sprite.setScale(scale_x, crawler_scale);
        crawler_sprite.setPosition(draw_x, crawler_y[i]);
        window.draw(crawler_sprite);
    }
}

void init_blue_enemies(bool blue_alive[], const int max_blue_enemies) {
    for (int i = 0; i < max_blue_enemies; i++) {
        blue_alive[i] = false;
    }
}

void spawn_next_blue_enemy(int& spawned_count, int& spawn_timer, float player_x, float blue_x[], float blue_y[], float blue_vx[], float blue_vy[], int blue_jump_timer[], bool blue_alive[], bool blue_facing_right[], bool blue_on_ground[], const int max_blue_enemies, const int blue_width, const int blue_height) {
    if (spawned_count >= max_blue_enemies) {
        return;
    }

    if (spawn_timer > 0) {
        spawn_timer--;
        return;
    }

    int i = spawned_count;
    int max_spawn_x = screen_x - blue_width;
    if (max_spawn_x < 1) max_spawn_x = 1;

    blue_alive[i] = true;
    blue_x[i] = (float)(rand() % max_spawn_x);
    blue_y[i] = (float)(-blue_height);
    blue_vx[i] = 0.0f;
    blue_vy[i] = 0.0f;
    blue_on_ground[i] = false;
    blue_facing_right[i] = (player_x > blue_x[i]);
    blue_jump_timer[i] = 20 + rand() % 40;

    spawned_count++;
    spawn_timer = 60 + rand() % 120;
}

void update_blue_enemies(float player_x, float blue_x[], float blue_y[], float blue_vx[], float blue_vy[], int blue_jump_timer[], bool blue_alive[], bool blue_facing_right[], bool blue_on_ground[], const int max_blue_enemies, const int blue_width, const int blue_height, const float gravity, const float terminal_velocity, const int ground_y) {
    for (int i = 0; i < max_blue_enemies; i++) {
        if (!blue_alive[i]) {
            continue;
        }

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

            if (blue_x[i] < 0.0f) {
                blue_x[i] = 0.0f;
            } else if (blue_x[i] + blue_width > screen_x) {
                blue_x[i] = (float)(screen_x - blue_width);
            }

            blue_y[i] += blue_vy[i];
            blue_vy[i] += gravity;
            if (blue_vy[i] > terminal_velocity) {
                blue_vy[i] = terminal_velocity;
            }

            if (blue_y[i] + blue_height >= ground_y) {
                blue_y[i] = (float)(ground_y - blue_height);
                blue_vx[i] = 0.0f;
                blue_vy[i] = 0.0f;
                blue_on_ground[i] = true;
                blue_jump_timer[i] = 25 + rand() % 45;
            }
        }
    }
}

void display_blue_enemies(RenderWindow& window, Sprite& blue_sprite, Texture& blue_stand_texture, Texture& blue_jump_texture, float blue_x[], float blue_y[], bool blue_alive[], bool blue_facing_right[], bool blue_on_ground[], const int max_blue_enemies, const int blue_width, const float blue_scale) {
    for (int i = 0; i < max_blue_enemies; i++) {
        if (!blue_alive[i]) {
            continue;
        }

        if (blue_on_ground[i]) {
            blue_sprite.setTexture(blue_stand_texture, true);
        } else {
            blue_sprite.setTexture(blue_jump_texture, true);
        }

        float scale_x = blue_facing_right[i] ? blue_scale : -blue_scale;
        float draw_x = blue_facing_right[i] ? blue_x[i] : blue_x[i] + blue_width;

        blue_sprite.setScale(scale_x, blue_scale);
        blue_sprite.setPosition(draw_x, blue_y[i]);
        window.draw(blue_sprite);
    }
}

void init_flying_insects(bool insect_alive[], const int max_insects) {
    for (int i = 0; i < max_insects; i++) {
        insect_alive[i] = false;
    }
}

void spawn_next_flying_insect(int& spawned_count, int& spawn_timer, float insect_x[], float insect_y[], float insect_base_y[], float insect_vx[], float insect_wave_angle[], bool insect_alive[], bool insect_facing_right[], const int max_insects, const int insect_width, const int insect_height, const int ground_y) {
    if (spawned_count >= max_insects) {
        return;
    }

    if (spawn_timer > 0) {
        spawn_timer--;
        return;
    }

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
    const float wave_amplitude = 45.0f;
    const float wave_speed = 0.07f;

    for (int i = 0; i < max_insects; i++) {
        if (!insect_alive[i]) {
            continue;
        }

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

        insect_wave_angle[i] += wave_speed;
        insect_y[i] = insect_base_y[i] + sin(insect_wave_angle[i]) * wave_amplitude;

        if (insect_y[i] < 0.0f) {
            insect_y[i] = 0.0f;
        } else if (insect_y[i] + insect_height > ground_y) {
            insect_y[i] = (float)(ground_y - insect_height);
        }
    }
}

void display_flying_insects(RenderWindow& window, Sprite& insect_sprite, Texture& insect_texture, float insect_x[], float insect_y[], bool insect_alive[], bool insect_facing_right[], const int max_insects, const int insect_width, const float insect_scale) {
    insect_sprite.setTexture(insect_texture, true);

    for (int i = 0; i < max_insects; i++) {
        if (!insect_alive[i]) {
            continue;
        }

        float scale_x = insect_facing_right[i] ? insect_scale : -insect_scale;
        float draw_x = insect_facing_right[i] ? insect_x[i] : insect_x[i] + insect_width;

        insect_sprite.setScale(scale_x, insect_scale);
        insect_sprite.setPosition(draw_x, insect_y[i]);
        window.draw(insect_sprite);
    }
}

void init_coins(bool coin_active[], int coin_frame[], int coin_anim_timer[], int coin_pickup_delay[], const int max_coins) {
    for (int i = 0; i < max_coins; i++) {
        coin_active[i] = false;
        coin_frame[i] = 0;
        coin_anim_timer[i] = 0;
        coin_pickup_delay[i] = 0;
    }
}

void spawn_coin(float coin_x[], float coin_y[], float coin_vy[], int coin_frame[], int coin_anim_timer[], int coin_pickup_delay[], bool coin_active[], int& coin_next_slot, const int max_coins, float spawn_x, float spawn_y) {
    int slot = -1;
    for (int i = 0; i < max_coins; i++) {
        if (!coin_active[i]) {
            slot = i;
            break;
        }
    }

    if (slot == -1) {
        slot = coin_next_slot;
        coin_next_slot = (coin_next_slot + 1) % max_coins;
    }

    coin_active[slot] = true;
    coin_x[slot] = spawn_x;
    coin_y[slot] = spawn_y;
    coin_vy[slot] = -12.0f;
    coin_frame[slot] = 0;
    coin_anim_timer[slot] = 0;
    coin_pickup_delay[slot] = 20;
}

void update_coins(float coin_x[], float coin_y[], float coin_vy[], int coin_frame[], int coin_anim_timer[], int coin_pickup_delay[], bool coin_active[], const int max_coins, const float coin_gravity, const int ground_y, const float coin_radius, float player_x, float player_y, int player_width, int player_height, int& score, Sound& coin_sound) {
    const int total_coin_frames = 6;
    const int coin_anim_delay = 4;

    for (int i = 0; i < max_coins; i++) {
        if (!coin_active[i]) {
            continue;
        }

        if (coin_pickup_delay[i] > 0) {
            coin_pickup_delay[i]--;
        }

        coin_anim_timer[i]++;
        if (coin_anim_timer[i] >= coin_anim_delay) {
            coin_anim_timer[i] = 0;
            coin_frame[i] = (coin_frame[i] + 1) % total_coin_frames;
        }

        if (coin_y[i] + coin_radius < ground_y || coin_vy[i] < 0.0f) {
            coin_y[i] += coin_vy[i];
            coin_vy[i] += coin_gravity;
            if (coin_y[i] + coin_radius >= ground_y && coin_vy[i] >= 0.0f) {
                coin_y[i] = (float)ground_y - coin_radius;
                coin_vy[i] = 0.0f;
            }
        }

        if (coin_pickup_delay[i] <= 0) {
            float coin_left = coin_x[i] - coin_radius;
            float coin_top = coin_y[i] - coin_radius;
            float coin_size = coin_radius * 2.0f;

            if (player_x < coin_left + coin_size &&
                player_x + player_width > coin_left &&
                player_y < coin_top + coin_size &&
                player_y + player_height > coin_top) {
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
        if (!coin_active[i]) {
            continue;
        }

        coin_sprite.setTextureRect(IntRect(coin_frame[i] * coin_frame_w, 0, coin_frame_w, coin_frame_h));
        coin_sprite.setPosition(coin_x[i], coin_y[i]);
        window.draw(coin_sprite);
    }
}

void init_refills(bool refill_active[], int refill_pickup_delay[], const int max_refills) {
    for (int i = 0; i < max_refills; i++) {
        refill_active[i] = false;
        refill_pickup_delay[i] = 0;
    }
}

void spawn_refill(float refill_x[], float refill_y[], float refill_vy[], int refill_pickup_delay[], bool refill_active[], int& refill_next_slot, const int max_refills, float spawn_x, float spawn_y) {
    int slot = -1;
    for (int i = 0; i < max_refills; i++) {
        if (!refill_active[i]) {
            slot = i;
            break;
        }
    }

    if (slot == -1) {
        slot = refill_next_slot;
        refill_next_slot = (refill_next_slot + 1) % max_refills;
    }

    refill_active[slot] = true;
    refill_x[slot] = spawn_x;
    refill_y[slot] = spawn_y;
    refill_vy[slot] = -12.0f;
    refill_pickup_delay[slot] = 20;
}

void update_refills(float refill_x[], float refill_y[], float refill_vy[], int refill_pickup_delay[], bool refill_active[], const int max_refills, const float refill_gravity, const int ground_y, const float refill_half_w, const float refill_half_h, float player_x, float player_y, int player_width, int player_height, Sound& collect_sound) {
    for (int i = 0; i < max_refills; i++) {
        if (!refill_active[i]) {
            continue;
        }

        if (refill_pickup_delay[i] > 0) {
            refill_pickup_delay[i]--;
        }

        if (refill_y[i] + refill_half_h < ground_y || refill_vy[i] < 0.0f) {
            refill_y[i] += refill_vy[i];
            refill_vy[i] += refill_gravity;
            if (refill_y[i] + refill_half_h >= ground_y && refill_vy[i] >= 0.0f) {
                refill_y[i] = (float)ground_y - refill_half_h;
                refill_vy[i] = 0.0f;
            }
        }

        if (refill_pickup_delay[i] <= 0) {
            float refill_left = refill_x[i] - refill_half_w;
            float refill_top = refill_y[i] - refill_half_h;
            float refill_w = refill_half_w * 2.0f;
            float refill_h = refill_half_h * 2.0f;

            if (player_x < refill_left + refill_w &&
                player_x + player_width > refill_left &&
                player_y < refill_top + refill_h &&
                player_y + player_height > refill_top) {
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
        if (!refill_active[i]) {
            continue;
        }

        refill_sprite.setPosition(refill_x[i], refill_y[i]);
        window.draw(refill_sprite);
    }
}
