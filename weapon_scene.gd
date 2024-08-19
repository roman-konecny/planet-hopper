extends Node2D

@export var projectile_scene: PackedScene  # The projectile scene this weapon will shoot
@export var attack_speed: float = 0.5  # Time between shots
@export var num_projectiles: int = 2  # Number of projectiles shot at once
@export var projectile_speed: float = 200.0  # Speed of the projectiles
@export var damage: int = 5  # Damage of the projectiles

var shooting: bool = false

var target: Area2D = null  # Current target

func _ready() -> void:
	$FireRate.wait_time = attack_speed

# Function to start shooting at the target
func shoot(target: Area2D) -> void:
	if target != null and is_instance_valid(target):
		for i in range(num_projectiles):
			var projectile = projectile_scene.instantiate()
			projectile.global_position = Vector2(global_position.x + 10*i, global_position.y)
			var direction_to_target = (target.global_position - global_position).normalized()
			projectile.set_direction(direction_to_target)  # Set the movement direction
			projectile.rotation = direction_to_target.angle() + PI/2
			projectile.speed = projectile_speed
			projectile.damage = damage
			get_tree().root.add_child(projectile)

func _on_fire_rate_timeout() -> void:
	if shooting and target != null and is_instance_valid(target):
		shoot(target)
	else:
		shooting = false
		$FireRate.stop()
		$CheckForEnemy.start()


func _on_check_for_enemy_timeout() -> void:
	if not shooting and target != null and is_instance_valid(target):
		shoot(target)
		$FireRate.start()
		$CheckForEnemy.stop()
