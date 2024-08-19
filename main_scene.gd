extends Node

@export var enemy_scene: PackedScene  # Assign the enemy scene in the inspector
@onready var tower = $TowerScene  # Reference to the TowerScene
@onready var hud = $HUD  # Reference to the HUD

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	# Connect HUD signals
	hud.connect("start_game", Callable(self, "_on_start_game"))
	hud.connect("restart_game", Callable(self, "_on_restart_game"))
	
	# Initially, stop the game logic
	set_process(false)
	
	# Set the initial position of the tower
	tower.position = $TowerPosition.position

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	# Update HUD with current health
	hud.update_health(tower.health)
	hud.update_essence(PlayerData.player_essence, PlayerData.esence_rate)

	# Check for game over condition
	if tower.health <= 1:
		_on_game_over()

# Function to handle the start of the game
func _on_start_game() -> void:
	# Start the game logic
	set_process(true)
	
	# Initialize or reset any game variables
	tower.health = 100
	hud.update_health(tower.health)
	
	# Start the enemy timer
	$StartTimer.start()

# Function to handle the restart of the game
func _on_restart_game() -> void:
	# Restart the game by resetting necessary variables
	tower.health = 100
	hud.update_health(tower.health)
	
	# Reset any other game elements (enemies, score, etc.)
	reset_game()
	
	# Start the game logic again
	set_process(true)
	tower.show()
	tower.set_process(true)
	# Restart the enemy timer
	$StartTimer.start()

# Start spawning enemies after the start timer times out
func _on_start_timer_timeout() -> void:
	$EnemyTimer.start()  # Start the enemy spawn timer
	$EssenceTimer.start()

# Spawn an enemy at random locations along a path
func _on_enemy_timer_timeout() -> void:
	if not tower:
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

# Function to handle game over logic
func _on_game_over() -> void:
	# Stop the game logic
	set_process(false)
	
	# Stop enemy spawning
	$EnemyTimer.stop()
	$EssenceTimer.stop()
	
	# Show the game over screen
	hud.show_game_over()
	PlayerData.save_data()

	# Optionally, remove or reset enemies
	reset_game()

# Function to reset the game state
func reset_game() -> void:
	# Clear or reset the enemies and any other game state elements
	get_tree().call_group("enemies", "queue_free")
	# Reset other necessary elements for a new game state


func _on_esence_timer_timeout() -> void:
	PlayerData.player_essence += PlayerData.tower_upgrades.get("essence_rate")
