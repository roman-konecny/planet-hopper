extends Area2D

@export var health: int = 50
@export var damage: int = 10
@export var speed: float = 100.0
var direction: Vector2 = Vector2.ZERO  # Initialize direction as a zero vector

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	# Move in the assigned direction
	if direction != Vector2.ZERO:
		global_position += direction * speed * delta

# Method to set the direction of the enemy
func set_direction(new_direction: Vector2) -> void:
	direction = new_direction

func _on_tower_hit(area: Area2D) -> void:
	if area.name == "TowerScene":  # Ensure you check for the correct tower name
		set_direction(Vector2.ZERO)
		add_to_group("damaging_tower")  # Add enemy to a group for damaging the tower
