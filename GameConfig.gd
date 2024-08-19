extends Node

var current_wave: int = 1
var current_tier: int = 1
var current_difficulty: int = 1

var enemy_health_damage_multiplier: float = 1.0
var enemy_wave_duration: float = 10.0
var enemy_wave_size: int = 10
var reward_per_enemy: int = 1

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
	if (current_wave < 10):
		return 1.0
	elif (current_wave >= 10 and current_wave < 20):
		return 2.0
	elif (current_wave >= 20 and current_wave < 30):
		return 2.5
	elif (current_wave >= 30 and current_wave < 40):
		return 3.5
	elif (current_wave >= 40 and current_wave <= 50):
		return 5.0
	return 1.0

func get_difficulty_multiplier() -> float:
	return 1.0

func get_reward_difficulty_multiplier() -> int:
	if (current_difficulty == 1):
		return 1
	elif (current_difficulty == 2):
		return 2
	elif (current_difficulty == 3):
		return 3
	return 1

func get_reward_tier_multiplier() -> int:
	if (current_tier == 1):
		return 1
	elif (current_tier == 2):
		return 4
	elif (current_tier == 3):
		return 8
	return 1

func set_damage_multiplier() -> void:
	var t_m: float = get_tier_multiplier()
	var w_m: float = get_wave_multiplier()
	var d_m: float = get_difficulty_multiplier()
	enemy_health_damage_multiplier = t_m * w_m * d_m

func set_reward_count() -> void:
	reward_per_enemy = get_reward_tier_multiplier() * get_reward_difficulty_multiplier()

func set_next_wave() -> void:
	if (current_wave < 50):
		current_wave += 1
	else:
		current_wave = 0
	
	set_damage_multiplier()
	set_reward_count()
	
	enemy_wave_size = current_wave * 10
	if (current_wave < 10):
		enemy_wave_duration = 10
	elif (current_wave >= 10 and current_wave < 20):
		enemy_wave_duration = 15
	elif (current_wave >= 20 and current_wave < 30):
		enemy_wave_duration = 20
	elif (current_wave >= 30 and current_wave < 40):
		enemy_wave_duration = 30
	elif (current_wave >= 40 and current_wave <= 50):
		enemy_wave_duration = 30

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
