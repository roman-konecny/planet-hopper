extends CanvasLayer

# Signals to communicate with the main game scene
signal start_game
signal restart_game
signal next_wave

func _ready() -> void:
	# Initially hide the game over label and restart button
	$GameOverLabel.hide()
	$RestartButton.hide()
	$NextWave.hide()

func _on_start_button_pressed() -> void:
	# Emit signal to start the game
	emit_signal("start_game")
	
	# Hide the start button
	$StartButton.hide()

func _on_restart_button_pressed() -> void:
	# Emit signal to restart the game
	emit_signal("restart_game")
	
	# Hide the game over elements and restart button
	$GameOverLabel.hide()
	$RestartButton.hide()

# Function to show the game over screen
func show_game_over() -> void:
	$GameOverLabel.show()
	$RestartButton.show()

# Optional function to update the health or score display
func update_health(health: int) -> void:
	$HealthLabel.text = "Health: %d" % health

func update_essence(essence: int) -> void:
	$Essence.text = "Essence: %d" % essence

func update_wave_count() -> void:
	$WaveCount.text = "Wave: %d" % GameConfig.current_wave

func update_bonus_damage() -> void:
	$AddBonusDamage.text = "Damage: %d" % GameConfig.bonus_damage + " | +%dE" % GameConfig.current_price_damage

func update_bonus_crit_m() -> void:
	$AddBonusCritM.text = "Crit Multiplier: %.1f" % GameConfig.bonus_crit_multiplier + " | +%dE" % GameConfig.current_price_crit_multiplier

func update_bonus_crit_c() -> void:
	$AddBonusCritC.text = "Crit Chance: %d" % GameConfig.bonus_crit_chance + " | +%dE" % GameConfig.current_price_crit_chance

func update_bonus_defense_rad() -> void:
	$AddBonusDefRad.text = "Defense Radius: %.1f" % GameConfig.bonus_defence_radius + " | +%dE" % GameConfig.current_price_defence_radius

func update_bonus_projectile_num() -> void:
	$AddBonusProjNum.text = "Projectile count: %d" % GameConfig.bonus_projectile_number + " | +%dE" % GameConfig.current_price_projectile_number

func update_bonus_attack_s() -> void:
	$AddBonusAS.text = "Attack Speed: %.1f" % GameConfig.bonus_attack_speed + " | +%dE" % GameConfig.current_price_attack_speed

func update_ingame_stats_labels() -> void:
	update_bonus_attack_s()
	update_bonus_crit_c()
	update_bonus_crit_m()
	update_bonus_damage()
	update_bonus_defense_rad()
	update_bonus_projectile_num()

func _on_next_wave_pressed() -> void:
	emit_signal("next_wave")
	# Hide the button
	$NextWave.hide()

func show_next_wave() -> void:
	$NextWave.show()

func _on_add_bonus_as_pressed() -> void:
	GameConfig.upgrade_attack_speed()

func _on_add_bonus_proj_num_pressed() -> void:
	GameConfig.upgrade_projectile_number()

func _on_add_bonus_def_rad_pressed() -> void:
	GameConfig.upgrade_defence_radius()

func _on_add_bonus_crit_c_pressed() -> void:
	GameConfig.upgrade_crit_chance()

func _on_add_bonus_crit_m_pressed() -> void:
	GameConfig.upgrade_crit_multiplier()

func _on_add_bonus_damage_pressed() -> void:
	GameConfig.upgrade_damage()

func show_ingame_buttons() -> void:
	$AddBonusAS.show()
	$AddBonusCritC.show()
	$AddBonusCritM.show()
	$AddBonusDamage.show()
	$AddBonusDefRad.show()
	$AddBonusProjNum.show()

func hide_ingame_buttons() -> void:
	$AddBonusAS.hide()
	$AddBonusCritC.hide()
	$AddBonusCritM.hide()
	$AddBonusDamage.hide()
	$AddBonusDefRad.hide()
	$AddBonusProjNum.hide()

var auto_wave: bool = false

func _on_auto_wave_toggle_pressed() -> void:
	auto_wave = not auto_wave
	if (auto_wave):
		$AutoWaveToggle.text = "Auto: On"
	else:
		$AutoWaveToggle.text = "Auto: Off"
