extends Area2D

@export var health: int = 100
@export var defence_radius: float = 100.0
@export var weapon_slots: Array = []
# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass

# Function to apply damage from enemies
func apply_damage():
	for enemy in get_tree().get_nodes_in_group("damaging_tower"):
		health -= enemy.damage  # Subtract enemy's damage from tower's health
		print(health)
		if health <= 0:
			die()  # Handle the tower's destruction when health reaches 0

# Handle tower's destruction
func die():
	queue_free()  # You can expand this with an animation or game over logic

func _on_damage_timer_timeout() -> void:
	apply_damage()  # Apply damage every time the timer triggers
