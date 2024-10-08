extends Node

var current_wave: int = 1
var current_tier: int = 1
var current_difficulty: int = 1

var enemy_health_damage_multiplier: float = 1.0
var enemy_wave_duration: float = 10.0
var enemy_wave_size: int = 4
var base_reward_per_enemy: int = 1
var current_reward_per_enemy: int = 1

func get_enemy_spawn_rate() -> float:
	return enemy_wave_duration / enemy_wave_size

func get_tier_multiplier() -> float:
	if (current_tier == 1):
		return 1.0
	elif (current_tier == 2):
		return 3.5
	elif (current_tier == 3):
		return 14.0
	return 1.0

func get_wave_multiplier() -> float:
	return 0.8 * current_wave
	#if (current_wave < 10):
	#elif (current_wave >= 10 and current_wave < 20):
		#return 2.0
	#elif (current_wave >= 20 and current_wave < 30):
		#return 2.5
	#elif (current_wave >= 30 and current_wave < 40):
		#return 3.5
	#elif (current_wave >= 40 and current_wave <= 50):
		#return 5.0
	#return 1.0

func get_difficulty_multiplier() -> float:
	return 1.0

func get_reward_difficulty_multiplier() -> float:
	if (current_difficulty == 1):
		return 1
	elif (current_difficulty == 2):
		return 2
	elif (current_difficulty == 3):
		return 3
	return 1

func get_reward_tier_multiplier() -> float:
	if (current_tier == 1):
		return 1
	elif (current_tier == 2):
		return 4
	elif (current_tier == 3):
		return 8
	return 1
	
func get_reward_wave_multiplier() -> float:
	return current_wave * 1.1

func set_damage_multiplier() -> void:
	var t_m: float = get_tier_multiplier()
	var w_m: float = get_wave_multiplier()
	var d_m: float = get_difficulty_multiplier()
	enemy_health_damage_multiplier = t_m * w_m * d_m

func set_reward_count() -> void:
	current_reward_per_enemy = base_reward_per_enemy * get_reward_wave_multiplier() * get_reward_tier_multiplier() * get_reward_difficulty_multiplier() * 1000

func set_next_wave() -> void:
	if (current_wave < 50):
		current_wave += 1
	else:
		current_wave = 0
	
	set_damage_multiplier()
	set_reward_count()
	
	if (current_wave < 10):
		enemy_wave_duration = 15
		enemy_wave_size = current_wave * 2
		
	elif (current_wave >= 10 and current_wave < 20):
		enemy_wave_duration = 25
		enemy_wave_size = current_wave * 2
		
	elif (current_wave >= 20 and current_wave < 30):
		enemy_wave_duration = 30
		enemy_wave_size = current_wave * 3
		
	elif (current_wave >= 30 and current_wave < 40):
		enemy_wave_duration = 35
		enemy_wave_size = current_wave * 4
		
	elif (current_wave >= 40 and current_wave <= 50):
		enemy_wave_duration = 50
		enemy_wave_size = current_wave * 5

func increaseTier() -> void:
	if (current_tier < 3):
		current_tier += 1

func decreaseTier() -> void:
	if (current_tier > 1):
		current_tier -= 1

func increaseDifficulty() -> void:
	if (current_difficulty < 3):
		current_difficulty += 1

func decreaseDifficulty() -> void:
	if (current_difficulty > 1):
		current_difficulty -= 1

# UPGRADES

var base_damage: int = 1
var base_crit_multiplier: float = 1.0
var base_crit_chance: int = 0
var base_defence_radius: float = 100.0
var base_projectile_number: int = 1
var base_attack_speed: float = 1.00

var bonus_damage: int = 0
var bonus_crit_multiplier: float = 0.0
var bonus_crit_chance: int = 0
var bonus_defence_radius: float = 0.0
var bonus_projectile_number: int = 0
var bonus_attack_speed: float = 0.00

var base_price_damage: int = 1
var base_price_crit_multiplier: int = 5
var base_price_crit_chance: int = 3
var base_price_defence_radius: int = 8
var base_price_projectile_number: int = 200
var base_price_attack_speed: int = 1

var current_price_damage: int = 1
var current_price_crit_multiplier: int = 5
var current_price_crit_chance: int = 3
var current_price_defence_radius: int = 8
var current_price_projectile_number: int = 200
var current_price_attack_speed: int = 1

func can_upgrade(x: float) -> bool:
	return x <= PlayerData.player_essence

func upgrade_damage() -> void:
	if (!can_upgrade(current_price_damage)):
		return
	bonus_damage += 1
	PlayerData.player_essence -= current_price_damage
	current_price_damage = base_price_damage + bonus_damage * bonus_damage

func upgrade_crit_multiplier() -> void:
	if (!can_upgrade(current_price_crit_multiplier)):
		return
	bonus_crit_multiplier += 0.01
	PlayerData.player_essence -= current_price_crit_multiplier
	current_price_crit_multiplier = base_price_crit_multiplier + bonus_crit_multiplier * bonus_crit_multiplier * 250

func upgrade_crit_chance() -> void:
	if (!can_upgrade(current_price_crit_chance)):
		return
	bonus_crit_chance += 0.5
	PlayerData.player_essence -= current_price_crit_chance
	current_price_crit_chance = base_price_crit_chance + bonus_crit_chance * bonus_crit_chance * 20
	
func upgrade_defence_radius() -> void:
	if (!can_upgrade(current_price_defence_radius)):
		return
	bonus_defence_radius += 0.25
	PlayerData.player_essence -= current_price_defence_radius
	current_price_defence_radius = base_price_defence_radius + bonus_defence_radius * bonus_defence_radius * 30
	
func upgrade_projectile_number() -> void:
	if (!can_upgrade(current_price_projectile_number)):
		return
	bonus_projectile_number += 1
	PlayerData.player_essence -= current_price_projectile_number
	current_price_projectile_number = base_price_projectile_number + bonus_projectile_number * bonus_projectile_number * 1200
	
func upgrade_attack_speed() -> void:
	if (!can_upgrade(current_price_attack_speed)):
		return
	bonus_attack_speed += 0.025
	PlayerData.player_essence -= current_price_attack_speed
	current_price_attack_speed = base_price_attack_speed + bonus_attack_speed * bonus_attack_speed * 400
