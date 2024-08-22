extends Area2D

@export var health: int = 100
@export var defence_radius: float = 75.0
@export var weapon_slots: Array = []
@export var weapons_cnt: int = PlayerData.weapon_upgrades.get("weapon_slots_unlocked")
@export var weapon_scene: PackedScene

# Draw the defense radius for visualization
func _draw() -> void:
	draw_circle(Vector2.ZERO, defence_radius, Color(1, 0, 0, 0.4))  # Draw a semi-transparent red circle

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	for i in range(weapons_cnt):
		var new_weapon = weapon_scene.instantiate()
		if (i == 0):
			new_weapon.position = $WeaponSlot1.position
		elif (i == 1):
			new_weapon.position = $WeaponSlot2.position
		elif (i == 2):
			new_weapon.position = $WeaponSlot3.position
		elif (i == 3):
			new_weapon.position = $WeaponSlot4.position
		weapon_slots.append(new_weapon)
		add_child(new_weapon)

# Check for enemies within the defense radius
func check_enemies_in_range() -> Array:
	var enemies_in_range = []
	for enemy in get_tree().get_nodes_in_group("enemies"):
		if global_position.distance_to(enemy.global_position) <= defence_radius:
			enemies_in_range.append(enemy)
	return enemies_in_range

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	if (GameConfig.base_defence_radius + GameConfig.bonus_defence_radius != defence_radius):
		defence_radius = GameConfig.base_defence_radius + GameConfig.bonus_defence_radius
		queue_redraw()
	var enemies_in_range = check_enemies_in_range()
	for weapon in weapon_slots:
		if enemies_in_range.size() > 0:
			weapon.target = enemies_in_range[0]

# Function to apply damage from enemies
func apply_damage():
	for enemy in get_tree().get_nodes_in_group("damaging_tower"):
		health -= enemy.damage  # Subtract enemy's damage from tower's health
		if health <= 1:
			health = 1
			die()  # Handle the tower's destruction when health reaches 0

# Handle tower's destruction
func die():
	hide()
	set_process(false)

func _on_damage_timer_timeout() -> void:
	apply_damage()  # Apply damage every time the timer triggers
