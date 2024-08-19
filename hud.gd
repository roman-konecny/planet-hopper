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
	$Essence.text = "E: %d" % essence

func update_wave_count() -> void:
	$WaveCount.text = "Wave number: %d" % GameConfig.current_wave

func _on_next_wave_pressed() -> void:
	emit_signal("next_wave")
	# Hide the button
	$NextWave.hide()

func show_next_wave() -> void:
	$NextWave.show()
