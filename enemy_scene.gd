extends Area2D

@export var health: int = 1 * GameConfig.enemy_health_damage_multiplier
@export var damage: int = 1 * GameConfig.enemy_health_damage_multiplier
@export var speed: float = 50.0
var direction: Vector2 = Vector2.ZERO  # Initialize direction as a zero vector

func _ready() -> void:
	$Health.text = "%d" % health
# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	# Move in the assigned direction
	if direction != Vector2.ZERO:
		global_position += direction * speed * delta
	if health <= 0:
		PlayerData.player_essence += GameConfig.current_reward_per_enemy
		queue_free()

# Method to set the direction of the enemy
func set_direction(new_direction: Vector2) -> void:
	direction = new_direction

func _on_tower_hit(area: Area2D) -> void:
	if area.name == "TowerScene":  # Ensure you check for the correct tower name
		set_direction(Vector2.ZERO)
		add_to_group("damaging_tower")  # Add enemy to a group for damaging the tower
