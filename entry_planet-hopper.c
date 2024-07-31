
typedef enum EntityArchetype {
	nil = 0,
	planet = 1,
	player = 2,
	shoes = 3,
	antena = 4	
}

typedef struct Entity {
	bool is_valid;
	EntityArchetype arch;
	Vector2 pos;

} Entity;


int entry(int argc, char **argv) {
	
	window.title = STR("planet-hopper");
	window.scaled_width = 1280; // We need to set the scaled size if we want to handle system scaling (DPI)
	window.scaled_height = 720; 
	window.x = 200;
	window.y = 90;
	window.clear_color = hex_to_rgba(0x000000ff);

	Gfx_Image* player = load_image_from_disk(fixed_string("pics/player.png"), get_heap_allocator());
	Gfx_Image* planet1 = load_image_from_disk(fixed_string("pics/planet1.png"), get_heap_allocator());
	assert(player, "fuckkk");

	float64 last_time = os_get_current_time_in_seconds();

	Vector2 player_pos = v2(0,0);


	while (!window.should_close) {
		reset_temporary_storage();
		draw_frame.projection = m4_make_orthographic_projection(window.width * -0.5, window.width * 0.5, window.height * -0.5, window.height * 0.5, -1, 10);

		float64 now = os_get_current_time_in_seconds();
		if ((int)now != (int)last_time) log("%.2f FPS\n%.2fms", 1.0/(now-last_time), (now-last_time)*1000);
		float64 delta_t = now - last_time;
		last_time = now;
		

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

		int speed = 75

		player_pos = v2_add(player_pos, v2_mulf(input_axis, speed * delta_t));

		Matrix4 xform_player = m4_scalar(1);
		Matrix4 xform_planet1 = m4_scalar(1);
		xform_planet1         = m4_translate(xform_planet1, v3(-0.5, -0.5, 0));
		draw_image_xform(planet1, xform_planet1, v2(250, 250), COLOR_WHITE);
		
		xform_player         = m4_translate(xform_player, v3(player_pos.x, player_pos.y, 0));
		draw_image_xform(player, xform_player, v2(32, 37), COLOR_WHITE);
		
		os_update(); 
		gfx_update();
	}

	return 0;
}