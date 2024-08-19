extends Node

@export var enemy_scene: PackedScene  # Assign the enemy scene in the inspector
@onready var tower = $TowerScene  # Reference to the TowerScene

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	$StartTimer.start()  # Start the game after a delay
	tower.position = $TowerPosition.position

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass

# Start spawning enemies after the start timer times out
func _on_start_timer_timeout() -> void:
	$EnemyTimer.start()  # Start the enemy spawn timer

# Spawn an enemy at random locations along a path
func _on_enemy_timer_timeout() -> void:
	if (not tower):
		return
	var enemy = enemy_scene.instantiate()  # Create a new enemy instance
	
	# Choose a random location on the EnemyPath
	var enemy_spawn_location = get_node("EnemyPath/EnemySpawnLocation")
	enemy_spawn_location.progress = randi()  # Randomize spawn location along the path
	
	# Set the enemy's position to the chosen spawn location
	enemy.position = enemy_spawn_location.position
	
	# Calculate the direction from the enemy to the tower
	var direction_to_tower = (tower.global_position - enemy.global_position).normalized()
	
	# Pass the direction to the enemy instance using the set_direction method
	enemy.set_direction(direction_to_tower)
	
	# Add the enemy to the scene
	add_child(enemy)
	
	# Optionally add the enemy to the "enemies" group for easy management
	enemy.add_to_group("enemies")
