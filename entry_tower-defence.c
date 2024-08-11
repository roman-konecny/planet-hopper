#define MAX_ENTITY_COUNT 1024
#define PI 3.14159265358979323846
#define FONT_HEIGHT 48

// ENEMY SETUP
#define MAX_ENEMIES 500 // Make sure you dont go over MAX_ENTITY_COUNT
#define ENEMY_SPEED 0.008
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
} Enemy;

typedef struct Weapon {
	Tower* tower;
	Entity* target;
	int speed;
	int dmg;
} Weapon;

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

Tower* setup_tower(Entity* en, Allocator heap) {
	en->arch = arch_tower;
	en->sprite_id = SPRITE_tower;
	en->pos.x = 0.0;
	en->pos.y = 0.0;
	en->rotation = 0.0;
	en->render_sprite = false;
	Tower* tower = alloc(heap, sizeof(Tower));
	tower->entity = en;
	tower->defence_size = v2_mulf(sprites[SPRITE_tower].size, 4.0);
	tower->tower_center_pos = v2((tower->entity->pos.x + sprites[SPRITE_tower].size.x) / 2, (tower->entity->pos.y + sprites[SPRITE_tower].size.y) / 2);
	tower->outline_circle_pos = v2(tower->tower_center_pos.x - (tower->defence_size.x / 2), tower->tower_center_pos.y - (tower->defence_size.y / 2));
	tower->inside_circle_pos = v2(tower->tower_center_pos.x - ((tower->defence_size.x - 2) / 2), tower->tower_center_pos.y - ((tower->defence_size.y - 2) / 2));
	tower->health = 100;
	return tower;
}

Weapon* setup_weapon(Tower* t, Allocator heap) {
	Weapon* weapon = alloc(heap, sizeof(Weapon));
}

void enemy_movement(Entity* enemy, Tower* t) {
    float step_size = ENEMY_SPEED;  // Define how fast the enemy moves towards the tower
	// Compute the vector from the enemy to the tower center
		Vector2 vector_to_tower;
		vector_to_tower.x = t->tower_center_pos.x - enemy->pos.x;
		vector_to_tower.y = t->tower_center_pos.y - enemy->pos.y;

		// Calculate the distance to the tower center
		float distance = sqrt(vector_to_tower.x * vector_to_tower.x + vector_to_tower.y * vector_to_tower.y);

		// Normalize the vector
		if (distance > 0) {
			vector_to_tower.x /= distance;
			vector_to_tower.y /= distance;
		}

		// Move the enemy towards the tower
		enemy->pos.x += vector_to_tower.x * step_size;
		enemy->pos.y += vector_to_tower.y * step_size;
}

bool is_enemy_touching_tower(Entity* enemy, Tower* tower) {
    // Calculate the horizontal and vertical distances between the center of the tower and the top-left corner of the enemy
    float horizontal_distance = fabs((tower->tower_center_pos.x - enemy->pos.x) - 5 / 2);
    float vertical_distance = fabs((tower->tower_center_pos.y - enemy->pos.y) - 5 / 2);

    // Check if the horizontal and vertical distances are less than the half widths/heights added together
    return horizontal_distance <= (sprites[SPRITE_tower].size.x / 2 + 5 / 2) &&
           vertical_distance <= (sprites[SPRITE_tower].size.y / 2 + 5 / 2);
}

void enemy_damage_tower(Entity* enemy, Tower* t) {
	if(is_enemy_touching_tower(enemy, t)) {
		enemy->render_sprite = false;
		t->health -= 1;
	}
}

void enemy_attack(Entity** enemies, Tower* t) {
	for (int i = 0; i < MAX_ENEMIES; i++) {
		if (enemies[i]->render_sprite) {
			enemy_movement(enemies[i], t);
			enemy_damage_tower(enemies[i], t);
		}
	}
}

void tower_attack() {

}

Enemy* setup_enemy(Entity* en, SpriteID sprite, float pos_x, float pos_y, Allocator heap) {
	en->arch = arch_enemy;
	en->sprite_id = sprite;
	en->pos.x = pos_x;
	en->pos.y = pos_y;
	Enemy* enemy = alloc(heap, sizeof(Enemy));
	enemy->entity = en;
	enemy->health = 1;
	enemy->dmg = 1;
}

int entry(int argc, char **argv) {

	Allocator heap = get_heap_allocator();
	
	// Window Setup
	window.title = STR("Tower Defence");
	window.scaled_width = 1280; // We need to set the scaled size if we want to handle system scaling (DPI)
	window.scaled_height = 720; 
	window.x = 200;
	window.y = 200;
	window.clear_color = hex_to_rgba(0x000000ff);
	float zoom = 1.5;

	font = load_font_from_disk(STR("C:/windows/fonts/arial.ttf"), heap);
	assert(font, "Failed loading arial.ttf");

	// Game Objects Creation
	world = alloc(heap, sizeof(World));
	memset(world, 0, sizeof(World));

	sprites[SPRITE_tower] = (Sprite){ .image=load_image_from_disk(STR("pics/tower.png"), heap), .size=v2(50.0, 50.0) };
    Entity* tower_en = entity_create();
    Tower* tower = setup_tower(tower_en, heap);

	// Create enemies
	Enemy* enemies[MAX_ENEMIES];
	float spawn_distance = 2.0; // Multiplier for the distance from the center

	for (int i = 0; i < MAX_ENEMIES; i++) {
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

		enemies[i] = setup_enemy(en, SPRITE_nil, spawn_position.x, spawn_position.y, heap);
	}

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
		draw_text(font, tprint("Tower Health: %d", tower->health), FONT_HEIGHT, v2(100, 100), v2(1, 1), COLOR_WHITE);
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
							draw_rect(en->pos, v2(5, 5), COLOR_WHITE);
						}
						
						int cycle_step = current_cycle - start_cycle;
						int spawn_rate = ENEMY_SPAWN_RATE;
						// Ensure we never exceed the bounds of the enemies array
						if (cycle_step * spawn_rate < MAX_ENEMIES) {
							// Activate rendering for the enemies that were spawned at this time step
							int start_index = max(cycle_step * spawn_rate - spawn_rate, 0);  // Compute the start index safely
							for (int i = start_index; i < min(start_index + spawn_rate, MAX_ENEMIES); i++) {
								if (enemies[i] != NULL) {
									enemies[i]->render_sprite = true;  // Safely activate rendering only if pointer is valid
								}
							}
						}
						break;
					}
					default:
					{
						Sprite* sprite = get_sprite(en->sprite_id);
						Matrix4 xform = m4_scalar(1.0);
						xform         = m4_translate(xform, v3(en->pos.x, en->pos.y, 0));
						draw_image_xform(sprite->image, xform, sprite->size, COLOR_WHITE);
						break;
					}
				}

			}
		}

		enemy_attack(enemies, tower);
		
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
