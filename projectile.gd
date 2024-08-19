extends Area2D

@export var speed: float
@export var damage: int

var direction: Vector2 = Vector2.ZERO  # Direction of movement

func _ready() -> void:
	pass

# Set the direction of the projectile
func set_direction(new_direction: Vector2) -> void:
	direction = new_direction

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	position += direction * speed * delta


func _on_enemy_hit(area: Area2D) -> void:
	if area.is_in_group("enemies"):
		area.health -= damage  # Reduce enemy health by the projectile's damage
		queue_free()  # Destroy the projectile upon collision
