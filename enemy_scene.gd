extends Area2D

@export var health: int = 50
@export var damage: int = 10
@export var speed: float = 100.0

var moving: bool = true

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	var tower = get_tree().root.get_node("/root/MainScene/TowerScene")
	if tower and moving:
		var direction = (tower.global_position - global_position).normalized()
		position += direction * speed * delta

func _on_tower_hit(area: Area2D) -> void:
	if area.name == "TowerScene":  # Ensure you check for the correct tower name
		print("collided")
		moving = false  # Stop enemy movement
		add_to_group("damaging_tower")  # Add enemy to a group for damaging the tower
