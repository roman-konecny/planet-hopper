#include <stdlib.h>  // For random number generation

#define MAX_ENTITY_COUNT 1024
#define MAX_PLANETS 33
#define PI 3.14159265358979323846
#define MAX_SPEED 300.0

typedef struct Sprite {
	Gfx_Image* image;
	Vector2 size;
} Sprite;

typedef enum SpriteID {
	SPRITE_nil,
	SPRITE_player,
	SPRITE_planet,
	SPRITE_planet2,
	SPRITE_planet3,
	SPRITE_planet4,
	SPRITE_antena,
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
	arch_planet = 1,
	arch_player = 2,
	arch_shoes = 3,
	arch_antena = 4	
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

void setup_player(Entity* en) {
	en->arch = arch_player;
	en->sprite_id = SPRITE_player;
	en->pos.x = 300.0;
	en->pos.y = 300.0;
	en->rotation = 0;
}

void setup_planet(Entity* en, SpriteID sprite, float pos_x, float pos_y) {
	en->arch = arch_planet;
	en->sprite_id = sprite;
	en->pos.x = pos_x;
	en->pos.y = pos_y;
}

// Helper function to get a random float between min and max
float get_random_float(float min, float max) {
    return min + (rand() / (float)RAND_MAX) * (max - min);
}

int entry(int argc, char **argv) {
	
	// Window Setup
	window.title = STR("planet-hopper");
	window.scaled_width = 1280; // We need to set the scaled size if we want to handle system scaling (DPI)
	window.scaled_height = 720; 
	window.x = 200;
	window.y = 200;
	window.clear_color = hex_to_rgba(0x000000ff);
	float zoom = 0.5;

	// Game Objects Creation
	world = alloc(get_heap_allocator(), sizeof(World));
	memset(world, 0, sizeof(World));

	sprites[SPRITE_planet] = (Sprite){ .image=load_image_from_disk(STR("pics/planet.png"), get_heap_allocator()), .size=v2(250.0, 250.0) };
	sprites[SPRITE_planet2] = (Sprite){ .image=load_image_from_disk(STR("pics/planet2.png"), get_heap_allocator()), .size=v2(250.0, 250.0) };
	sprites[SPRITE_planet3] = (Sprite){ .image=load_image_from_disk(STR("pics/planet3.png"), get_heap_allocator()), .size=v2(250.0, 250.0) };
	sprites[SPRITE_planet4] = (Sprite){ .image=load_image_from_disk(STR("pics/planet4.png"), get_heap_allocator()), .size=v2(250.0, 250.0) };
	sprites[SPRITE_player] = (Sprite){ .image=load_image_from_disk(STR("pics/player.png"), get_heap_allocator()), .size=v2(18.0, 37.0) };

	// Initialize random number generator
	srand((unsigned int)os_get_current_time_in_seconds());

	// Create planets
	Entity* planets[MAX_PLANETS];
	SpriteID planet_sprites[] = { SPRITE_planet, SPRITE_planet2, SPRITE_planet3, SPRITE_planet4 };
	int num_sprites = sizeof(planet_sprites) / sizeof(SpriteID);

	 // Define play area boundaries and section size
    float center_x = 0.0;
    float center_y = 0.0;
    float distance_increment = 500.0;  // Distance between layers
    float angle_increment = PI / 4.0;  // 45 degrees for diagonal placement

	// first planet in the center
	planets[0] = entity_create();
	SpriteID random_sprite = planet_sprites[rand() % num_sprites];
	setup_planet(planets[0], random_sprite, center_x, center_y);

    int planets_created = 1;
    int layer = 0;

    while (planets_created < MAX_PLANETS) {
        int num_in_layer = 8 * layer; // Number of planets to place in the current layer

        for (int i = 0; i < num_in_layer && planets_created < MAX_PLANETS; ++i) {
            float angle = i * angle_increment;
            float distance = distance_increment * layer;

            float pos_x = center_x + distance * cos(angle);
            float pos_y = center_y + distance * sin(angle);

            planets[planets_created] = entity_create();
            SpriteID random_sprite = planet_sprites[rand() % num_sprites];

            setup_planet(planets[planets_created], random_sprite, pos_x, pos_y);
            planets_created++;
        }

        layer++;  // Move to the next layer
    }

	Entity* player_en = entity_create();
	setup_player(player_en);

	// Game Loop
	float64 last_time = os_get_current_time_in_seconds();
	while (!window.should_close) {

		// Setups
		reset_temporary_storage();
		draw_frame.projection = m4_make_orthographic_projection(window.width * -0.5, window.width * 0.5, window.height * -0.5, window.height * 0.5, -1, 10);

		// Camera
		{
			if (is_key_down('-')) {
				zoom -= 0.5;
			} else if (is_key_down('+')) {
				zoom -= 0.5;
			}
			draw_frame.view = m4_make_scale(v3(1.0, 1.0, 1.0));
			draw_frame.view = m4_mul(draw_frame.view, m4_make_translation(v3(player_en->pos.x, player_en->pos.y, 0)));
			draw_frame.view = m4_mul(draw_frame.view, m4_make_scale(v3(1.0 / zoom, 1.0 / zoom, 1.0)));
		}

		// FPS
		float64 now = os_get_current_time_in_seconds();
		if ((int)now != (int)last_time) {
			log("%.2f FPS\n%.2fms", 1.0/(now-last_time), (now-last_time)*1000);
			log("Player position: (%.2f,%.2f), velocity: (%.2f, %.2f), direction: %.2f", player_en->pos.x, player_en->pos.y, player_en->velocity.x , player_en->velocity.y, player_en->rotation);
		}
		float64 delta_t = now - last_time;
		last_time = now;

		// :render
		for (int i = 0; i < MAX_ENTITY_COUNT; i++) {
			Entity* en = &world->entities[i];
			if (en->is_valid) {

				switch (en->arch) {
					case (arch_player):
					Sprite* sprite = get_sprite(en->sprite_id);
					Matrix4 xform = m4_scalar(1.0);
					xform         = m4_translate(xform, v3(en->pos.x, en->pos.y, 0));
					xform		  = m4_rotate_z(xform, en->rotation);
					draw_image_xform(sprite->image, xform, sprite->size, COLOR_WHITE);
					break;

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

		
		// Game controll logic
		if (is_key_just_pressed(KEY_ESCAPE)) {
			window.should_close = true;
		}

		Vector2 input_axis = v2(0, 0);
		if (is_key_down('A')) {
			input_axis.x -= 1.0;
		}
		if (is_key_down('D')) {
			input_axis.x += 1.0;
		}
		if (is_key_down('S')) {
			input_axis.y -= 1.0;
		}
		if (is_key_down('W')) {
			input_axis.y += 1.0;
		}
		input_axis = v2_normalize(input_axis);

		// Player acceleration and movement
		float acceleration = 200.0; // Acceleration value
		player_en->velocity = v2_add(player_en->velocity, v2_mulf(input_axis, acceleration * delta_t));

		// Limit speed
		float current_speed = v2_length(player_en->velocity);
		if (current_speed > MAX_SPEED) {
			player_en->velocity = v2_mulf(v2_normalize(player_en->velocity), MAX_SPEED);
		}

		// Gravity
		#define GRAVITY_CONSTANT 10000.0  // Adjusted for stronger gravity effect

		// Apply gravity from all planets
		for (int i = 0; i < MAX_PLANETS; ++i) {
			Entity* planet_en = planets[i];

			// Calculate radii based on sprite sizes
			float planet_radius = get_sprite(planet_en->sprite_id)->size.x / 2.0;
			float player_radius = get_sprite(SPRITE_player)->size.x / 2.0;

			// Calculate centers of the planet and player
			Vector2 planet_center = {
				planet_en->pos.x + planet_radius,
				planet_en->pos.y + planet_radius
			};
			Vector2 player_center = {
				player_en->pos.x + player_radius,
				player_en->pos.y + player_radius
			};

			// Vector from player center to planet center
			Vector2 to_planet = {
				planet_center.x - player_center.x,
				planet_center.y - player_center.y
			};

			// Calculate distance between centers
			float distance_between_centers = sqrt(to_planet.x * to_planet.x + to_planet.y * to_planet.y);

			// Calculate effective distance considering the surfaces of the sprites
			float effective_distance = distance_between_centers - (planet_radius + player_radius);

			// Normalize the direction vector for force application
			if (distance_between_centers > 0) {
				Vector2 direction = {
					to_planet.x / distance_between_centers,
					to_planet.y / distance_between_centers
				};

				// Apply gravitational force only if the player is outside the effective radius
				if (effective_distance > 0) {
					float force_magnitude = (GRAVITY_CONSTANT / (distance_between_centers * distance_between_centers)) * 250;
					Vector2 gravity_acceleration = {
						direction.x * force_magnitude * delta_t,
						direction.y * force_magnitude * delta_t
					};
					player_en->velocity.x += gravity_acceleration.x;
					player_en->velocity.y += gravity_acceleration.y;
				} else {
					// Prevent the player from entering the planet's radius
					float overlap = (planet_radius + player_radius) - distance_between_centers;
					if (overlap > 0) {
						player_en->pos.x -= direction.x * overlap;
						player_en->pos.y -= direction.y * overlap;
						player_en->velocity.x *= 0.9999;
						player_en->velocity.y *= 0.9999;
					}
				}
			}
		}

		player_en->pos = v2_add(player_en->pos, v2_mulf(player_en->velocity, delta_t));

		// Update player rotation to face movement direction
		if (current_speed > 0.0) {
			// Adjust by PI/2 (90 degrees) for correct sprite alignment
			player_en->rotation = atan2(player_en->velocity.x, player_en->velocity.y);
    
		}


		// Debugging information
		// printf("Planet center: (%.2f, %.2f)\n", planet_center.x, planet_center.y);
		// printf("Player center: (%.2f, %.2f)\n", player_center.x, player_center.y);
		// printf("Distance between centers: %.2f\n", distance_between_centers);
		// printf("Effective distance: %.2f\n", effective_distance);
		// printf("Player pos: (%.2f, %.2f)\n", player_en->pos.x, player_en->pos.y);


		// Updates		
		os_update(); 
		gfx_update();
	}

	return 0;
}
