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
void player_movement(float&, float&, bool&, bool, bool&, bool&, bool&, Sound&, const float, const float, const int);
void player_gravity(float&, float&, bool&, Sound&, const float, const float, const int, const int);
void update_player_whip(bool&, bool&, int&, int&, const int, Sound&);
void update_player_sprite(Sprite&, Texture&, Texture&, Texture&, Texture&, bool, bool, bool, bool, int, int&, int&, const int, int&, int&, int&);
void update_whip_hitbox(bool, bool, bool, bool, int, float, float, int, int, bool&, float&, float&, float&, float&);
void check_whip_hits(bool, float, float, float, float, float[], float[], bool[], const int, int, int, float[], float[], bool[], const int, int, int, float[], float[], bool[], const int, int, int, Sound&);
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

int main() {
    srand((unsigned int)time(0));

    RenderWindow window(VideoMode(screen_x, screen_y), "VICE: Project Doom", Style::Resize);
    window.setFramerateLimit(60);

    const int ground_y = 832;

    Texture bg_texture;
    bg_texture.loadFromFile("Data/bg.png");
    Sprite bg_sprite(bg_texture);

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

    // Player setup
    const int stand_height = 108;  // 36px frame * 3 scale
    const int kneel_height = 75;   // 25px of kneeling art * 3 scale
    const int player_width = 54;
    int player_height = stand_height;

    float player_x = 120.0f;
    float player_y = (float)(ground_y - stand_height);
    float velocity_y = 0.0f;

    const float gravity = 1.0f;
    const float walk_speed = 6.0f;
    const float jump_velocity = -18.0f;
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

    // run.png is 221x49:
    // sprite 0 (x=1, w=18): stand / jump
    // sprites 1-4 (x=36,75,100,136; w=25,15,26,15): run cycle
    // sprite 6 (x=195, w=20): kneel
    // stand_whip.png, run_whip.png, crouch_whip.png are 245x48 (5 frames of 49x48)
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

    // Spiked Crawler setup (4 total)
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

    // Blue Jumping Enemy setup (5 total)
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

    // Flying Insect setup (6 total, sine-wave flight)
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

    Event event;
    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Escape)) {
            window.close();
        }

        player_kneel(player_y, player_height, stand_height, kneel_height, is_kneeling, on_ground);
        player_movement(player_x, velocity_y, on_ground, is_kneeling, facing_right, is_moving, was_against_wall, hit_sound, walk_speed, jump_velocity, player_width);
        player_gravity(player_y, velocity_y, on_ground, hit_sound, gravity, terminal_velocity, ground_y, player_height);
        update_player_whip(is_whipping, was_q_pressed, whip_frame, whip_timer, whip_delay, whip_sound);

        update_whip_hitbox(is_whipping, is_kneeling, on_ground, facing_right, whip_frame, player_x, player_y, player_width, player_height, whip_hitbox_active, whip_box_x, whip_box_y, whip_box_w, whip_box_h);
        check_whip_hits(whip_hitbox_active, whip_box_x, whip_box_y, whip_box_w, whip_box_h, crawler_x, crawler_y, crawler_alive, max_crawlers, crawler_width, crawler_height, blue_x, blue_y, blue_alive, max_blue_enemies, blue_width, blue_height, insect_x, insect_y, insect_alive, max_insects, insect_width, insect_height, hit_sound);

        spawn_next_crawler(crawler_spawned_count, crawler_spawn_timer, crawler_x, crawler_y, crawler_vx, crawler_vy, crawler_alive, crawler_facing_right, crawler_on_ground, max_crawlers, crawler_width, crawler_height);
        update_crawlers(crawler_x, crawler_y, crawler_vx, crawler_vy, crawler_alive, crawler_facing_right, crawler_on_ground, max_crawlers, crawler_width, crawler_height, gravity, terminal_velocity, ground_y);

        spawn_next_blue_enemy(blue_spawned_count, blue_spawn_timer, player_x, blue_x, blue_y, blue_vx, blue_vy, blue_jump_timer, blue_alive, blue_facing_right, blue_on_ground, max_blue_enemies, blue_width, blue_height);
        update_blue_enemies(player_x, blue_x, blue_y, blue_vx, blue_vy, blue_jump_timer, blue_alive, blue_facing_right, blue_on_ground, max_blue_enemies, blue_width, blue_height, gravity, terminal_velocity, ground_y);

        spawn_next_flying_insect(insect_spawned_count, insect_spawn_timer, insect_x, insect_y, insect_base_y, insect_vx, insect_wave_angle, insect_alive, insect_facing_right, max_insects, insect_width, insect_height, ground_y);
        update_flying_insects(insect_x, insect_y, insect_base_y, insect_vx, insect_wave_angle, insect_alive, insect_facing_right, max_insects, insect_width, insect_height, ground_y);

        update_player_sprite(player_sprite, run_texture, stand_whip_texture, run_whip_texture, crouch_whip_texture, is_moving, on_ground, is_kneeling, is_whipping, whip_frame, run_step, anim_timer, anim_delay, frame_width, frame_height, foot_height);

        float scale_x = facing_right ? 3.0f : -3.0f;
        player_sprite.setScale(scale_x, 3.0f);

        float visual_width = frame_width * 3.0f;
        float draw_x = facing_right
            ? player_x + (player_width - visual_width) / 2.0f
            : player_x + (player_width + visual_width) / 2.0f;

        // Align the foot baseline of the sprite frame with the bottom of the hitbox,
        // so standing, running, kneeling, and whipping poses all rest flush on the ground.
        float draw_y = player_y + player_height - foot_height * 3.0f;
        player_sprite.setPosition(draw_x, draw_y);

        window.clear();
        display_background(window, bg_sprite);
        display_crawlers(window, crawler_sprite, crawler_texture, crawler_x, crawler_y, crawler_alive, crawler_facing_right, max_crawlers, crawler_width, crawler_scale);
        display_blue_enemies(window, blue_sprite, blue_stand_texture, blue_jump_texture, blue_x, blue_y, blue_alive, blue_facing_right, blue_on_ground, max_blue_enemies, blue_width, blue_scale);
        display_flying_insects(window, insect_sprite, insect_texture, insect_x, insect_y, insect_alive, insect_facing_right, max_insects, insect_width, insect_scale);
        window.draw(player_sprite);
        window.display();
    }

    whip_sound.stop();
    hit_sound.stop();
    level_music.stop();
    return 0;
}

void display_background(RenderWindow& window, Sprite& bg_sprite) {
    window.draw(bg_sprite);
}

void play_hit_sound(Sound& hit_sound) {
    hit_sound.play();
    hit_sound.setPlayingOffset(milliseconds(80));
}

// Hold Down arrow while on the ground to kneel; release to stand back up.
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

void player_movement(float& player_x, float& velocity_y, bool& on_ground, bool is_kneeling, bool& facing_right, bool& is_moving, bool& was_against_wall, Sound& hit_sound, const float walk_speed, const float jump_velocity, const int player_width) {
    float current_speed = is_kneeling ? walk_speed * 0.5f : walk_speed;
    float move_x = 0.0f;

    if (Keyboard::isKeyPressed(Keyboard::Left)) {
        move_x -= current_speed;
    }
    if (Keyboard::isKeyPressed(Keyboard::Right)) {
        move_x += current_speed;
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
    if (move_x < 0.0f) {
        if (player_x + move_x >= 0.0f) {
            player_x += move_x;
        } else {
            player_x = 0.0f;
            hit_edge = true;
        }
    } else if (move_x > 0.0f) {
        if (player_x + move_x + player_width <= screen_x) {
            player_x += move_x;
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
    if (jump_pressed && on_ground && !is_kneeling) {
        velocity_y = jump_velocity;
        on_ground = false;
    }
}

void player_gravity(float& player_y, float& velocity_y, bool& on_ground, Sound& hit_sound, const float gravity, const float terminal_velocity, const int ground_y, const int player_height) {
    bool was_on_ground = on_ground;
    float next_y = player_y + velocity_y;

    if (velocity_y < 0.0f) {
        if (next_y <= 0.0f) {
            player_y = 0.0f;
            velocity_y = 0.0f;
            play_hit_sound(hit_sound);
        } else {
            player_y = next_y;
        }
        on_ground = false;
    } else {
        if (next_y + player_height >= ground_y) {
            on_ground = true;
            player_y = (float)(ground_y - player_height);
            if (!was_on_ground) {
                play_hit_sound(hit_sound);
            }
        } else {
            player_y = next_y;
            on_ground = false;
        }
    }

    if (!on_ground) {
        velocity_y += gravity;
        if (velocity_y > terminal_velocity) {
            velocity_y = terminal_velocity;
        }
    } else {
        velocity_y = 0.0f;
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
        // Kneeling pose in run.png
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
        // Standing / upright jumping pose in run.png
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

void check_whip_hits(bool whip_hitbox_active, float whip_box_x, float whip_box_y, float whip_box_w, float whip_box_h, float crawler_x[], float crawler_y[], bool crawler_alive[], const int max_crawlers, int crawler_width, int crawler_height, float blue_x[], float blue_y[], bool blue_alive[], const int max_blue_enemies, int blue_width, int blue_height, float insect_x[], float insect_y[], bool insect_alive[], const int max_insects, int insect_width, int insect_height, Sound& hit_sound) {
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

    float random_x = (float)(rand() % max_spawn_x);
    for (int attempt = 0; attempt < 10; attempt++) {
        bool too_close = false;
        for (int j = 0; j < i; j++) {
            if (crawler_alive[j] && fabs(random_x - crawler_x[j]) < crawler_width * 1.5f) {
                too_close = true;
                break;
            }
        }
        if (!too_close) {
            break;
        }
        random_x = (float)(rand() % max_spawn_x);
    }

    crawler_alive[i] = true;
    crawler_x[i] = random_x;
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

    float random_x = (float)(rand() % max_spawn_x);
    for (int attempt = 0; attempt < 15; attempt++) {
        bool too_close = false;
        for (int j = 0; j < i; j++) {
            if (blue_alive[j] && fabs(random_x - blue_x[j]) < blue_width * 2.5f) {
                too_close = true;
                break;
            }
        }
        if (!too_close) {
            break;
        }
        random_x = (float)(rand() % max_spawn_x);
    }

    blue_alive[i] = true;
    blue_x[i] = random_x;
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
                float jump_speed_x = 3.0f + (float)(rand() % 18) / 10.0f;
                blue_vy[i] = -16.0f - (float)(rand() % 5);
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

    float random_base_y = (float)(min_fly_y + rand() % (max_fly_y - min_fly_y));
    for (int attempt = 0; attempt < 20; attempt++) {
        bool too_close = false;
        for (int j = 0; j < i; j++) {
            if (insect_alive[j] && fabs(random_base_y - insect_base_y[j]) < insect_height * 1.2f) {
                too_close = true;
                break;
            }
        }
        if (!too_close) {
            break;
        }
        random_base_y = (float)(min_fly_y + rand() % (max_fly_y - min_fly_y));
    }

    insect_alive[i] = true;
    insect_x[i] = (float)(screen_x - insect_width);
    insect_base_y[i] = random_base_y;
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