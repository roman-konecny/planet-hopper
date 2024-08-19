extends Node2D

@export var projectile_scene: PackedScene  # The projectile scene this weapon will shoot
@export var attack_speed: float = 1.0  # Time between shots
@export var num_projectiles: int = 1  # Number of projectiles shot at once
@export var projectile_speed: float = 300.0  # Speed of the projectiles
@export var damage: int = 10  # Damage of the projectiles

var target: Area2D = null  # Current target

func _ready() -> void:
	$FireRate.wait_time = attack_speed
	$FireRate.start()

# Function to start shooting at the target
func shoot(target: Area2D) -> void:
	if target != null and is_instance_valid(target):
		for i in range(num_projectiles):
			var projectile = projectile_scene.instantiate()
			projectile.global_position = global_position
			projectile.set_direction((target.global_position - global_position).normalized())
			projectile.speed = projectile_speed
			projectile.damage = damage
			get_tree().root.add_child(projectile)

func _on_fire_rate_timeout() -> void:
	if target != null and is_instance_valid(target):
		shoot(target)
