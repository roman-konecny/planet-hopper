extends Node

# Player data variables
var player_essence: int = 0
var player_gold: int = 0
var player_diamonds: int = 0
var unlocked_weapons: Array = []
var tower_upgrades: Dictionary = {}

# Path to the save file
var save_path: String = "user://savegame.cfg"

# Called when the node enters the scene tree for the first time
func _ready() -> void:
	load_data()

# Function to save player data to a file
func save_data() -> void:
	var config = ConfigFile.new()
	config.set_value("Player", "essence", player_essence)
	config.set_value("Player", "gold", player_gold)
	config.set_value("Player", "diamonds", player_diamonds)
	config.set_value("Player", "unlocked_weapons", unlocked_weapons)
	config.set_value("Player", "tower_upgrades", tower_upgrades)
	
	var err = config.save(save_path)
	if err != OK:
		print("Error saving data: ", err)
	else:
		print("Game data saved successfully")

# Function to load player data from a file
func load_data() -> void:
	var config = ConfigFile.new()
	if config.load(save_path) == OK:
		player_essence = config.get_value("Player", "essence", 100)
		player_gold = config.get_value("Player", "gold", 0)
		player_diamonds = config.get_value("Player", "diamonds", 0)
		unlocked_weapons = config.get_value("Player", "unlocked_weapons", [])
		tower_upgrades = config.get_value("Player", "tower_upgrades", {})
		print("Game data loaded successfully")
	else:
		print("No save file found, starting with default values")

# Function to reset player data (e.g., for a new game)
func reset_data() -> void:
	player_essence = 100
	player_gold = 0
	player_diamonds = 0
	unlocked_weapons.clear()
	tower_upgrades.clear()
	save_data()  # Save the reset data
