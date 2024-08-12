#define MAX_ENTITY_COUNT 1024
#define PI 3.14159265358979323846
#define FONT_HEIGHT 48

// ENEMY SETUP
#define MAX_ENEMIES 100 // Make sure you dont go over MAX_ENTITY_COUNT
#define ENEMY_SPEED 0.01
#define ENEMY_SPAWN_RATE 1
#define SPAWN_SPEED 1000000000 // indirect effect - increase number to decrease speed etc.

typedef struct Sprite {
	Gfx_Image* image;
	Vector2 size;
} Sprite;

Gfx_Font *font;

typedef enum SpriteID {
	SPRITE_nil,
	SPRITE_tower,
	SPRITE_enemy,
	SPRITE_missile,
	SPRITE_MAX,
} SpriteID;

Sprite sprites[SPRITE_MAX];
Sprite* get_sprite(SpriteID id) {
	if (id >= 0 && id < SPRITE_MAX) {
		return &sprites[id];
	}
	return &sprites[0];
}

typedef enum EntityArchetype {
	arch_nil = 0,
	arch_tower = 1,
	arch_enemy = 2,
	arch_missile = 3,
} EntityArchetype;

typedef struct Entity {
	bool is_valid;
	EntityArchetype arch;
	Vector2 pos;
	float rotation;
	Vector2 velocity;
	bool render_sprite;
	SpriteID sprite_id;
} Entity;

typedef struct Tower {
	Entity* entity;
	Vector2 defence_size;
	Vector2 tower_center_pos;
	Vector2 outline_circle_pos;
	Vector2 inside_circle_pos;
	int health;
} Tower;

typedef struct Enemy {
	Entity* entity;
	int health;
	int dmg;
	int incoming_dmg;
} Enemy;

typedef struct Missile Missile;

typedef struct Weapon {
	Entity* entity;
	Tower* tower;
	float speed;
	int dmg;
	Missile** missiles;
	int missile_limit;
} Weapon;

typedef struct Missile {
	Entity* entity;
	Weapon* weapon;
	Enemy* target;
} Missile;

typedef struct World {
	Entity entities[MAX_ENTITY_COUNT];
} World;

World* world = 0;

Entity* entity_create() {
	Entity* entity_found = 0;
	for (int i = 0; i < MAX_ENTITY_COUNT; i++) {
		Entity* existing_entity = &world->entities[i];
		if (!existing_entity->is_valid) {
			entity_found = existing_entity;
			break;
		}
	}
	assert(entity_found, "No more free entities");
	entity_found->is_valid = true;
	return entity_found;
}

void entity_destroy(Entity* entity) {
	memset(entity, 0, sizeof(Entity));
}


// ENEMIES START
void enemy_destroy(Missile* missile) {
    missile->target->entity->render_sprite = false;
    missile->target->health = 0;
    missile->target->incoming_dmg = 0;
	missile->target = NULL;
}

Enemy* setup_enemy(Entity* en, SpriteID sprite, float pos_x, float pos_y, Allocator heap) {
	en->arch = arch_enemy;
	en->sprite_id = sprite;
	en->render_sprite = false;
	en->pos.x = pos_x;
	en->pos.y = pos_y;
	Enemy* enemy = alloc(heap, sizeof(Enemy));
	enemy->entity = en;
	enemy->health = 1;
	enemy->dmg = 1;
	return enemy;
}

void create_enemy_wave(Enemy** enemies, int wave_size, EntityArchetype type, Tower* tower, Allocator heap) {
	float spawn_distance = 1.5; // Multiplier for the distance from the center

	for (int i = 0; i < wave_size; i++) {
		Entity* en = entity_create();
		s64 ran = get_random_int_in_range(1, 42);
		Vector2 direction;
		float angle = (ran - 1) * (PI / 16); // Converts direction to radians (each quadrant is PI/4 rad apart)

		// Calculate spawn position based on the angle and distance
		direction.x = cos(angle) * tower->defence_size.x * spawn_distance;
		direction.y = sin(angle) * tower->defence_size.y * spawn_distance;

		Vector2 spawn_position = v2(
			tower->tower_center_pos.x + direction.x,
			tower->tower_center_pos.y + direction.y
		);

		enemies[i] = setup_enemy(en, SPRITE_enemy, spawn_position.x, spawn_position.y, heap);
	}
}

void enemy_movement(Enemy* enemy, Tower* t) {
    float step_size = ENEMY_SPEED;  // Define how fast the enemy moves towards the tower
	// Compute the vector from the enemy to the tower center
	Vector2 vector_to_tower;
	vector_to_tower.x = t->tower_center_pos.x - enemy->entity->pos.x;
	vector_to_tower.y = t->tower_center_pos.y - enemy->entity->pos.y;

	// Calculate the distance to the tower center
	float distance = sqrt(vector_to_tower.x * vector_to_tower.x + vector_to_tower.y * vector_to_tower.y);

	// Normalize the vector
	if (distance > 0) {
		vector_to_tower.x /= distance;
		vector_to_tower.y /= distance;
		// Move the enemy towards the tower
		enemy->entity->pos.x += vector_to_tower.x * step_size;
		enemy->entity->pos.y += vector_to_tower.y * step_size;
	} else {
		enemy->entity->pos.x = t->tower_center_pos.x;
		enemy->entity->pos.y = t->tower_center_pos.y;
	}
}


bool is_enemy_touching_tower(Enemy* enemy, Tower* tower) {
    // Calculate the horizontal and vertical distances between the center of the tower and the top-left corner of the enemy
    float horizontal_distance = fabs((tower->tower_center_pos.x - enemy->entity->pos.x) - sprites[SPRITE_enemy].size.x / 2);
    float vertical_distance = fabs((tower->tower_center_pos.y - enemy->entity->pos.y) - sprites[SPRITE_enemy].size.y / 2);

    // Check if the horizontal and vertical distances are less than the half widths/heights added together
    return horizontal_distance <= (sprites[SPRITE_tower].size.x / 2 + sprites[SPRITE_enemy].size.x / 2) &&
           vertical_distance <= (sprites[SPRITE_tower].size.y / 2 + sprites[SPRITE_enemy].size.y / 2);
}

void enemy_damage_tower(Enemy* enemy, Tower* t) {
	if(is_enemy_touching_tower(enemy, t)) {
		t->health -= enemy->dmg; // if damaged once, no more dmg;
		enemy->dmg = 0;
	}
}

void enemy_attack(Enemy** enemies, Tower* t) {
	for (int i = 0; i < MAX_ENEMIES; i++) {
		if (enemies[i] && enemies[i]->entity->is_valid) {
			if (enemies[i]->health > 0 && enemies[i]->entity->render_sprite) {
				enemy_movement(enemies[i], t);
				enemy_damage_tower(enemies[i], t);
			}
		}
	}
}
// ENEMIES END


// DEFENCE START
Tower* setup_tower(Allocator heap) {
    Entity* tower_en = entity_create();
	tower_en->arch = arch_tower;
	tower_en->sprite_id = SPRITE_tower;
	tower_en->pos.x = 0.0;
	tower_en->pos.y = 0.0;
	tower_en->rotation = 0.0;
	tower_en->render_sprite = false;
	Tower* tower = alloc(heap, sizeof(Tower));
	tower->entity = tower_en;
	tower->defence_size = v2_mulf(sprites[SPRITE_tower].size, 4.0);
	tower->tower_center_pos = v2((tower->entity->pos.x + sprites[SPRITE_tower].size.x) / 2, (tower->entity->pos.y + sprites[SPRITE_tower].size.y) / 2);
	tower->outline_circle_pos = v2(tower->tower_center_pos.x - (tower->defence_size.x / 2), tower->tower_center_pos.y - (tower->defence_size.y / 2));
	tower->inside_circle_pos = v2(tower->tower_center_pos.x - ((tower->defence_size.x - 2) / 2), tower->tower_center_pos.y - ((tower->defence_size.y - 2) / 2));
	tower->health = 100;
	return tower;
}

Missile* setup_missile(Weapon* weapon, Allocator heap) {
    Entity* missile_en = entity_create();
	Missile* missile = alloc(heap, sizeof(Missile));
	missile->entity = missile_en;
	missile->entity->render_sprite = false;
	missile->entity->arch = arch_missile;
	missile->entity->sprite_id = SPRITE_missile;
	missile->weapon = weapon;
	missile->target = NULL;
	return missile;
}

void load_weapon(Weapon* w, Allocator heap) {
	for (int i = 0; i < w->missile_limit; i++) {
		w->missiles[i] = setup_missile(w, heap);
	}
}

Weapon* setup_weapon(Tower* t, Allocator heap) {
    Entity* weapon_en = entity_create();
	Weapon* weapon = alloc(heap, sizeof(Weapon));
	weapon->dmg = 1;
	weapon->speed = 0.03;
	weapon->missile_limit = 3;
	weapon->entity = weapon_en;
	weapon->tower = t;
	weapon->missiles = alloc(heap, sizeof(Missile*) * weapon->missile_limit);
	load_weapon(weapon, heap);
	return weapon;
}


void missile_destroy(Missile* missile) {
	missile->entity->render_sprite = false;
	missile->entity->pos = missile->weapon->tower->tower_center_pos;
}

bool is_enemy_within_defence_range(Enemy* enemy, Tower* tower) {
    // // Calculate the center position of the enemy, assuming its position is the top-left corner
    float enemy_center_x = enemy->entity->pos.x + sprites[SPRITE_enemy].size.x / 2;
    float enemy_center_y = enemy->entity->pos.y + sprites[SPRITE_enemy].size.y / 2;

    // // Calculate the distance between the center of the tower and the center of the enemy
    float dx = tower->tower_center_pos.x - enemy_center_x;
    float dy = tower->tower_center_pos.y - enemy_center_y;
    float distance = sqrt(dx * dx + dy * dy);

    // Get the radius of the tower's defense circle
    float defense_radius = tower->defence_size.x / 2;  // Assuming x and y are equal for the radius

    // Check if the distance is less than or equal to the defense radius
    return distance <= defense_radius;
}

Enemy* pick_target_in_range(Enemy** enemies, Weapon* weapon) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i] != NULL && enemies[i]->entity->render_sprite) {
            if (is_enemy_within_defence_range(enemies[i], weapon->tower) && enemies[i]->incoming_dmg < enemies[i]->health) {
                return enemies[i];
            }
        }
    }
    return NULL;
}

void move_missile_and_destroy(Missile* missile) {
	// Calculate direction vector towards the target
    Vector2 target_center = {
        missile->target->entity->pos.x + sprites[missile->target->entity->sprite_id].size.x / 2,
        missile->target->entity->pos.y + sprites[missile->target->entity->sprite_id].size.y / 2
    };

    Vector2 direction = {
        target_center.x - missile->entity->pos.x,
        target_center.y - missile->entity->pos.y
    };

    // Normalize the direction vector
    float distance = sqrt(direction.x * direction.x + direction.y * direction.y);
    if (distance == 0) {
        distance = 0.1;
    }

    direction.x /= distance;
    direction.y /= distance;

    // Update missile position
    missile->entity->pos.x += direction.x * missile->weapon->speed;
    missile->entity->pos.y += direction.y * missile->weapon->speed;

    if (distance < missile->weapon->speed) {
        missile->target->health -= missile->weapon->dmg;
        missile->target->incoming_dmg -= missile->weapon->dmg;  // Decrement incoming damage as this missile hits

        if (missile->target->health <= 0) {
			missile_destroy(missile);
            enemy_destroy(missile);
        }

    }
}

void send_missile(Weapon* weapon, Enemy* target) {
	for (int i = 0; i < weapon->missile_limit; i++) {
		if (weapon->missiles[i]->target == NULL) {
			weapon->missiles[i]->target = target;
			weapon->missiles[i]->entity->render_sprite = true;
			target->incoming_dmg += weapon->dmg;
			break;
		}
	}
}

void update_missiles(Weapon* weapon) {
    for (int i = 0; i < weapon->missile_limit; i++) {
        Missile* missile = weapon->missiles[i];
        if (missile->target != NULL && missile->entity->render_sprite == true) {
            move_missile_and_destroy(missile);
        }
    }
}

void tower_attack(Enemy** enemies, Weapon* weapon) {
	Enemy* target = pick_target_in_range(enemies, weapon);
	if (target) {
		send_missile(weapon, target);
	}
	update_missiles(weapon);
}
// DEFENCE END


int entry(int argc, char **argv) {

	Allocator heap = get_heap_allocator();
	
	// Window Setup
	window.title = STR("Tower Defence");
	window.scaled_width = 1280; // We need to set the scaled size if we want to handle system scaling (DPI)
	window.scaled_height = 720; 
	window.x = 200;
	window.y = 200;
	window.clear_color = hex_to_rgba(0x000000ff);
	float zoom = 1.1;

	font = load_font_from_disk(STR("C:/windows/fonts/arial.ttf"), heap);
	assert(font, "Failed loading arial.ttf");

	// Game Objects Creation
	world = alloc(heap, sizeof(World));
	memset(world, 0, sizeof(World));

	sprites[SPRITE_tower] = (Sprite){ .image=load_image_from_disk(STR("pics/tower.png"), heap), .size=v2(100.0, 100.0) };
	sprites[SPRITE_enemy] = (Sprite){ .image=load_image_from_disk(STR("pics/Enemies/enemy1.png"), heap), .size=v2(16.0, 16.0) };
	sprites[SPRITE_missile] = (Sprite){ .image=load_image_from_disk(STR("pics/Missiles/rocket1.png"), heap), .size=v2(32.0, 32.0) };

	// Setup player
    Tower* tower = setup_tower(heap);
	Weapon* weapon = setup_weapon(tower, heap);

	// Create enemies
	Enemy** enemies = alloc(heap, sizeof(Enemy*) * MAX_ENEMIES);
	create_enemy_wave(enemies, MAX_ENEMIES, arch_enemy, tower, heap);

	// Game Loop
	float64 last_time = os_get_current_time_in_seconds();
	int cycle_divisor = SPAWN_SPEED;
	int start_cycle = 0;

	while (!window.should_close) {
		int current_cycle = (int)(rdtsc() / cycle_divisor);
		if (start_cycle == 0) {
			start_cycle = current_cycle;
		}
		
		// Setups
		reset_temporary_storage();
		draw_frame.projection = m4_make_orthographic_projection(window.width * -0.5, window.width * 0.5, window.height * -0.5, window.height * 0.5, -1, 10);

		// Camera
		{
			draw_frame.view = m4_make_scale(v3(1.0, 1.0, 1.0));
			draw_frame.view = m4_mul(draw_frame.view, m4_make_scale(v3(1.0 / zoom, 1.0 / zoom, 1.0)));
		}

		// FPS
		float64 now = os_get_current_time_in_seconds();
		if ((int)now != (int)last_time) {
			log("%.2f FPS\n%.2fms", 1.0/(now-last_time), (now-last_time)*1000);
		}
		float64 delta_t = now - last_time;
		last_time = now;

		// :render

		// health count
		draw_text(font, tprint("Tower Health: %d", tower->health), 12, v2(200, 200), v2(1, 1), COLOR_WHITE);
		// entities
		for (int i = 0; i < MAX_ENTITY_COUNT; i++) {
			Entity* en = &world->entities[i];
			if (en->is_valid) {

				switch (en->arch) {

					case (arch_tower):
					{
						Sprite* sprite = get_sprite(en->sprite_id);
						draw_circle(tower->outline_circle_pos, tower->defence_size, COLOR_WHITE);
						draw_circle(tower->inside_circle_pos, v2(tower->defence_size.x - 2, tower->defence_size.y - 2), COLOR_BLACK);
						Matrix4 xform = m4_scalar(1.0);
						xform         = m4_translate(xform, v3(en->pos.x, en->pos.y, 0));
						draw_image_xform(sprite->image, xform, sprite->size, COLOR_WHITE);
						en->render_sprite = true;
						break;
					}
					case (arch_enemy):
					{
						if (en->render_sprite) {
							Sprite* sprite = get_sprite(en->sprite_id);
							Matrix4 xform = m4_scalar(1.0);
							xform         = m4_translate(xform, v3(en->pos.x, en->pos.y, 0));
							draw_image_xform(sprite->image, xform, sprite->size, COLOR_WHITE);
						}
						
						int cycle_step = current_cycle - start_cycle; // o 4 mimo cca
						int spawn_rate = ENEMY_SPAWN_RATE; 
						if (cycle_step * spawn_rate < MAX_ENEMIES) {
							int start_index = max(cycle_step * spawn_rate - spawn_rate, 0);  // Compute the start index safely
							for (int i = start_index; i < min(start_index + spawn_rate, MAX_ENEMIES); i++) {
								if (enemies[i] != NULL) {
									enemies[i]->entity->render_sprite = true;  // Safely activate rendering only if pointer is valid
								}
							}
						}
						break;
					}

					case (arch_missile):
					{
						if (en->render_sprite) {
							Sprite* sprite = get_sprite(en->sprite_id);
							Matrix4 xform = m4_scalar(1.0);
							xform         = m4_translate(xform, v3(en->pos.x, en->pos.y, 0));
							xform		  = m4_rotate_z(xform, en->rotation);
							xform         = m4_translate(xform, v3(-sprite->size.x / 2, -sprite->size.y / 2, 0));
							draw_image_xform(sprite->image, xform, sprite->size, COLOR_WHITE);
						}
						break;
					}
					default:
					{
						Sprite* sprite = get_sprite(en->sprite_id);
						if (sprite != SPRITE_nil) {
							Matrix4 xform = m4_scalar(1.0);
							xform         = m4_translate(xform, v3(en->pos.x, en->pos.y, 0));
							draw_image_xform(sprite->image, xform, sprite->size, COLOR_WHITE);
						}
						break;
					}
				}

			}
		}

		enemy_attack(enemies, tower);
		tower_attack(enemies, weapon);
		
        // Game controll logic
		if (is_key_just_pressed(KEY_ESCAPE)) {
			window.should_close = true;
		}

		// Updates		
		os_update(); 
		gfx_update();
	}

	return 0;
}
