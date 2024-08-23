extends Node2D

@export var projectile_scene: PackedScene  # The projectile scene this weapon will shoot
@export var attack_speed: float = 1 / GameConfig.base_attack_speed # Time between shots
@export var num_projectiles: int = GameConfig.base_projectile_number  # Number of projectiles shot at once
@export var projectile_speed: float = 100  # Speed of the projectiles
@export var damage: int = GameConfig.base_damage  # Damage of the projectiles

var shooting: bool = false

var target: Area2D = null  # Current target

func _ready() -> void:
	$FireRate.wait_time = attack_speed

func _process(delta: float) -> void:
	$FireRate.wait_time = 1 / (GameConfig.base_attack_speed + GameConfig.bonus_attack_speed)
	damage = GameConfig.base_damage + GameConfig.bonus_damage
	num_projectiles = GameConfig.base_projectile_number + GameConfig.bonus_projectile_number

# Function to start shooting at the target
func shoot(target: Area2D) -> void:
	if target != null and is_instance_valid(target):
		for i in range(num_projectiles):
			var projectile = projectile_scene.instantiate()
			if (i % 2 == 0):
				projectile.global_position = Vector2(global_position.x + 3*i, global_position.y + 3*i)
			else:
				projectile.global_position = Vector2(global_position.x - 3*i, global_position.y - 3*i)
			var direction_to_target = (target.global_position - global_position).normalized()
			projectile.set_direction(direction_to_target)  # Set the movement direction
			projectile.rotation = direction_to_target.angle() + PI/2
			projectile.speed = projectile_speed
			projectile.damage = damage * crit_multiply()
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

func crit_multiply() -> float:
	if (GameConfig.bonus_crit_chance > 0):
		if (is_crit()):
			return GameConfig.base_crit_multiplier + GameConfig.bonus_crit_multiplier
	return GameConfig.base_crit_multiplier

func is_crit() -> bool:
	var roll = randi() % 100
	return roll < GameConfig.base_crit_chance + GameConfig.bonus_crit_chance
