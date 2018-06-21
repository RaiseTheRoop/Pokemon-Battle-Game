/*
Pokemon - C++ Command Line
By:
Roopinder Singh

Features:
Load / Save Functionality
Three Selectable Starter Pokemon
ASCII Pokemon Sprites
Random Pokemon Battles with a Battle UI
Battle up to 10 different Pokemon Species of random difficulty.
Gain Experience and Level Up your Pokemon
Battle UI Includes:
2 Different Attack Types (Differing Movesets per Pokemon Species)
Inventory Bag (for using Items)
Pokemon Selection Window (for swapping Pokemon)
Flee Battle Mechanic
Completely Furnished Pokemon Mart with Elixir and Pokeballs
Pokemon Center to heal your Pokemon to full health
Trainer Statistics Panel
Save / Quit Game
*/

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <time.h>
#include <iomanip>

using namespace std;

// Global Variables
const int MOVES = 2;
const int PLAYER_MAX_POKEMON = 6;
const int POKEMON_IN_GAME = 10;
const int ITEMS_IN_GAME = 2;

// Global ENUMs
enum PokemonSpecies { BULBASAUR, CHARMANDER, SQUIRTLE, CATERPIE, PIDGEY, PIKACHU, EKANS, ODDISH, DIGLETT, PSYDUCK };
enum ItemNames { ELIXIR, POKEBALL };
enum Status { HIT, DEAD, REVIVE, CAUGHT, FAILED, MISSED, SUCCESS, NORMAL, SPECIAL, PLAYER, COMPUTER, LEVELUP, BATTLE_END, BATTLE_CONTINUE };
enum MenuLocation { ATTACK, BAG, SELECTION, OVERVIEW };

// Global Strings
string DefaultSpeciesNames[] = { "Bulbasaur", "Charmander", "Squirtle", "Caterpie", "Pidgey", "Pikachu", "Ekans", "Oddish", "Diglett", "Psyduck" };

// Pokemon Data Struct (Contains Species Information)
struct PokemonSpeciesData
{
	string Name;
	int iconBegin;
	int iconEnd;
	string moveSet[MOVES];

	vector<string> icon;

	void saveIcon()
	{
		// Open File
		ifstream file("pokemon.txt");
		string line;

		// Incrementer
		int i = 0;

		// For Each Line of the File
		while (getline(file, line))
		{
			// If the line is between the iconBegin and iconEnd
			if (i >= iconBegin && i < iconEnd)
			{
				// Push Line onto Vector
				icon.push_back(line);
			}

			//Increment Line Counter
			i++;
		}

		// Close File
		file.close();
	}

	void printIcon()
	{
		// Read Everyline from Vector and Print It.
		for (int i = 0; i < icon.size(); i++)
		{
			cout << icon.at(i) << endl;
		}
	}
};

// Item Data (Contains Information about Items)
struct PokemonItem
{
	string name;
	string description;
	int price;
};

// Player Pokemon Struct (Contains Information about Pokemon in Player's Possession)
struct PokemonData
{
	string name;
	int health = 25;
	int level = 5;
	int exp = 0;
	PokemonSpecies species;
	bool isDead = false;
	int maxHealth = 5 * level;
	int nextLevelUp = 25 * level;

	Status takeDamage(int damage)
	{
		if (damage == 0)
		{
			return MISSED;
		}

		if (health < damage)
		{
			isDead = true;
			health = 0;
			return DEAD;
		}
		else
		{
			health -= damage;

			if (health == 0)
			{
				isDead = true;
				return DEAD;
			}
			else
			{
				return HIT;
			}
		}
	}

	Status addExp(int incomingEXP)
	{
		exp += incomingEXP;

		if (exp > nextLevelUp)
		{
			exp -= nextLevelUp;

			level++;

			nextLevelUp = level * 25;
			health = level * 5;
			maxHealth = level * 5;

			return LEVELUP;
		}

		return SUCCESS;
	}

	Status giveHealth(int incomingHealth)
	{
		int maxHealthToGive = maxHealth - health;

		if (incomingHealth >= maxHealthToGive)
		{
			health = maxHealth;
		}
		else
		{
			health += incomingHealth;
		}

		return SUCCESS;
	}
};

// Global List of Items
PokemonItem itemData[ITEMS_IN_GAME];

// Player Data Struct (Contains Information about the Player)
struct PlayerData
{
	string name;
	string rivalName;
	int money = 5000;
	int pokemonOwned = 0;
	int itemsOwned[ITEMS_IN_GAME] = {};
	PokemonData pokemon[PLAYER_MAX_POKEMON] = {};

	Status removeMoney(int amount)
	{
		if ((money - amount) < 0)
		{
			return FAILED;
		}
		else
		{
			money -= amount;
			return SUCCESS;
		}
	}

	Status addPokemon(PokemonData newPokemon)
	{
		if (pokemonOwned != PLAYER_MAX_POKEMON)
		{
			pokemon[pokemonOwned] = newPokemon;
			pokemonOwned++;
			return SUCCESS;
		}
		else
		{
			return FAILED;
		}
	}

	Status addItem(ItemNames item)
	{
		if (money < itemData[item].price)
		{
			return FAILED;
		}
		else
		{
			money -= itemData[item].price;
			itemsOwned[item]++;
			return SUCCESS;
		}
	}

	Status removeItem(ItemNames item)
	{
		if (itemsOwned[item] > 0)
		{
			itemsOwned[item]--;
			return SUCCESS;
		}
		else
		{
			return FAILED;
		}
	}

	bool hasItems()
	{
		int items = 0;

		for (int i = 0; i < ITEMS_IN_GAME; i++)
		{
			items += itemsOwned[i];
		}

		return (items > 0) ? true : false;
	}

	int alivePokemon()
	{
		int count = 0;

		for (int i = 0; i < pokemonOwned; i++)
		{
			if (pokemon[i].isDead == false)
			{
				count++;
			}
		}

		return count;
	}
};

// Global List of Species Data
PokemonSpeciesData speciesData[POKEMON_IN_GAME];

// Function Prototypes for Debug Purposes
void displayData(PlayerData player);

// Function Prototypes for Helper Functions
void   clear();
void   pressEnterToContinue();
int    getMenuSelection();
void   drawLines(int lines);
string multipleStrings(vector<string> statement);

// Function Prototypes for Initilization Functions
void initGame();
void initSpeciesData(PokemonSpeciesData data[POKEMON_IN_GAME]);
void initItemData(PokemonItem data[ITEMS_IN_GAME]);

// Function Prototypes for Files
bool gameExists();
void saveGame(PlayerData player);
void loadGame(PlayerData &player);

// Function Prototypes for UI Systems
void getPokemonIcon(PokemonSpecies species);

void drawHealthUI(int hp, int max);
void drawBattleUIHeader(PokemonData &attackingPokemon);
void drawBattleUIFooter(MenuLocation location, PlayerData &trainer);
void drawBattleUIStatus(PlayerData &trainer, PokemonData &attackingPokemon, string text);
void drawBattleUI(PlayerData &trainer, PokemonData &attackingPokemon, MenuLocation location, Status &battleStatus);

void battleUIController(PlayerData &trainer, PokemonData &attackingPokemon, MenuLocation &location, int menuSelection, Status &battleStatus);
void battleUIAttack(PlayerData &trainer, PokemonData &attackingPokemon, Status attackType, Status &battleStatus);
void battleUIFlee(PlayerData &trainer, PokemonData &attackingPokemon, Status &battleStatus);

// Function Prototypes for Menu Systems
Status confirmStarterSelection(PlayerData &trainer, int selection);
void   selectStarterPokemon(PlayerData &trainer);
void   newGame(PlayerData &trainer);
void   newGame(PlayerData &trainer);

// Function Prototypes for Shop Systems
void   pokemonCenter(PlayerData &trainer);
int    pokemonCenterCost(int health, int maxHealth);
Status pokemonCenterHeal(PlayerData &trainer, int pokemonToHeal);
void   pokemonMart(PlayerData &trainer);
Status pokemonMartItem(PlayerData &trainer, int item);

// Function Prototypes for Combat Systems
Status computerAttack(PlayerData &trainer, PokemonData &attackingPokemon, Status &lastAttack);
void   playerAttack(PlayerData &trainer, PokemonData &attackingPokemon, Status &lastAttack);
Status throwPokeball(PlayerData &trainer, PokemonData &attackingPokemon, Status &battleStatus);
Status swapPokemon(PlayerData &trainer, int pokemonToSwap);
void   deadPickNew(PlayerData &trainer);
void   playerWin(PlayerData &trainer, PokemonData &attackingPokemon);
void   computerWin(PlayerData &trainer, PokemonData &attackingPokemon);
void   pokemonBattleSetup(PlayerData &trainer);

// Function Prototypes for Main Game Loops
void mainBattleLoop(PlayerData &trainer, PokemonData &attackingPokemon, int whoGoes);
void mainGameLoop(PlayerData &trainer);
void mainMenu(PlayerData &trainer);

// *******************************************
//           main
//    COMPLETE STARTING POINT
//********************************************
int main()
{
	// Must Be Called On Initial Load
	initGame();

	// Create a PlayerData object
	PlayerData newPlayer;

	// Start Game
	mainMenu(newPlayer);

	return 0;
}
// *******************************************
//           initGame
//    Sets EVERYTHING UP
//********************************************
void initGame()
{
	// Seed the Pseduo Random Number Generator
	srand(time(NULL));

	// Populate Species Data Array
	initSpeciesData(speciesData);

	// Populate Item Data Array
	initItemData(itemData);
}
// *******************************************
//           initSpeciesData
//    This function initializes the species
//		information for each specific pokemon
//		including it's icon line number location
//		as well as it's special attacks.
//********************************************
void initSpeciesData(PokemonSpeciesData data[POKEMON_IN_GAME])
{
	// Initiate Pokemon Data
	data[0] = { "Bulbasaur",   0,  28,{ "Tackle", "Growl" } };
	data[1] = { "Charmander", 28,  62,{ "Scratch", "Growl" } };
	data[2] = { "Squirtle",   62,  95,{ "Tackle", "Tail Whip" } };
	data[3] = { "Caterpie",   95, 125,{ "Tackle", "String Shot" } };
	data[4] = { "Pidgey",    125, 159,{ "Tackle", "Sand Attack" } };
	data[5] = { "Pikachu",   159, 202,{ "Thunder Shock", "Tail Whip" } };
	data[6] = { "Ekans",     202, 240,{ "Poison Sting", "Bite" } };
	data[7] = { "Oddish",    240, 274,{ "Absorb", "Acid" } };
	data[8] = { "Diglett",   274, 299,{ "Scratch", "Sand Attack" } };
	data[9] = { "Psyduck",   299, 335,{ "Scratch", "Water Gun" } };

	// Save Icon Information
	for (int i = 0; i < POKEMON_IN_GAME; i++)
	{
		data[i].saveIcon();
	}
}
// *******************************************
//           initItemData
//    This function initializes the item
//		information for each specific item
//		including it's name, description and
//		as price
//********************************************
void initItemData(PokemonItem data[ITEMS_IN_GAME])
{
	data[0] = { "Elixir", "Restores 20 HP to Current Pokemon", 500 };
	data[1] = { "Pokeball", "Used to attempt the capture of a wild Pokemon", 2000 };
}
// *******************************************
//           getPokemonIcon
//    This function retrieves the pokemon
//		sprite data and prints it out to the
//    console.
//********************************************
void getPokemonIcon(PokemonSpecies species)
{
	PokemonSpeciesData selectedPokemon = speciesData[species];
	selectedPokemon.printIcon();
}
// *******************************************
//           displayData
//    Displays Player Data to Console (Debug)
//********************************************
void displayData(PlayerData player)
{
	cout << player.name << endl;
	cout << player.rivalName << endl;
	cout << player.money << endl;
	cout << player.pokemonOwned << endl << endl;

	// Item Information
	for (int i = 0; i < ITEMS_IN_GAME; i++)
	{
		cout << itemData[i].name << " : " << player.itemsOwned[i] << endl;
	}

	cout << endl;

	// Pokemon Information
	for (int i = 0; i < player.pokemonOwned; i++)
	{
		cout << player.pokemon[i].name << endl;
		cout << player.pokemon[i].health << endl;
		cout << player.pokemon[i].level << endl;
		cout << player.pokemon[i].exp << endl;
		cout << player.pokemon[i].species << endl;
		cout << player.pokemon[i].isDead << endl;
		cout << player.pokemon[i].maxHealth << endl;
		getPokemonIcon(player.pokemon[i].species);
		cout << endl;
	}
}
// *******************************************
//           saveGame
//    Writes Player Data to File
//********************************************
void saveGame(PlayerData player)
{
	// Open Save File
	ofstream saveFile;
	saveFile.open("save.txt", ios::out);

	// Save Trainer Data
	saveFile << player.name << endl;
	saveFile << player.rivalName << endl;
	saveFile << player.money << endl;
	saveFile << player.pokemonOwned << endl;

	// Save Owned Item Amount Information
	for (int i = 0; i < ITEMS_IN_GAME; i++)
	{
		saveFile << player.itemsOwned[i];

		// Do we put a space in the line?
		if (i < ITEMS_IN_GAME - 1)
		{
			saveFile << " ";
		}
	}

	// New Line (To Seperate)
	saveFile << endl;

	// Save Owned Pokemon Data
	for (int i = 0; i < player.pokemonOwned; i++)
	{
		saveFile << player.pokemon[i].name << endl;
		saveFile << player.pokemon[i].health << endl;
		saveFile << player.pokemon[i].level << endl;
		saveFile << player.pokemon[i].exp << endl;
		saveFile << player.pokemon[i].species << endl;
		saveFile << player.pokemon[i].isDead << endl;
		saveFile << player.pokemon[i].maxHealth << endl;
	}

	// Close Save File
	saveFile.close();
}
// *******************************************
//           loadGame
//    Read Player Data from File
//********************************************
void loadGame(PlayerData &player)
{
	// Constant Values
	const int PLAYER_LINES = 5;
	const int POKEMON_LINES = 7;

	// Open Save File
	ifstream saveFile;
	saveFile.open("save.txt");

	// Container for Line Information
	string line;

	// Line Incrementers
	int lineNumber = 0;
	int pokemonRead = 0;
	int pokemonLineInc = 0;

	// For Each Line
	while (getline(saveFile, line))
	{
		// Trainer Data
		if (lineNumber < PLAYER_LINES)
		{
			switch (lineNumber)
			{
			case 0:
				// Get Player Name
				player.name = line;
				break;
			case 1:
				// Get Rival Name
				player.rivalName = line;
				break;
			case 2:
				// Get Money
				player.money = stoi(line);
				break;
			case 3:
				// Get Number of Pokemon Owned
				player.pokemonOwned = stoi(line);
				break;
			case 4:
				// Get Item Quantities Owned

				// Create StringStream with Buffer (This splits the line by spaces)
				string buffer;
				stringstream ss(line);

				// Incrementer
				int i = 0;

				// For Each Value
				while (ss >> buffer)
				{
					// Get Item Quantity
					int itemQuantity = stoi(buffer);

					/// Add Item (i = ItemNames) to Player * Quantity
					for (int q = 0; q < itemQuantity; q++)
					{
						player.itemsOwned[i] = itemQuantity;
					}

					// Increment to Next Item
					i++;
				}
				break;
			}
		}

		// Pokemon Data
		if (player.pokemonOwned > 0 && lineNumber > 4)
		{
			switch (pokemonLineInc)
			{
			case 0:
				// Get Pokemon Name
				player.pokemon[pokemonRead].name = line;
				break;
			case 1:
				// Get Pokemon Health
				player.pokemon[pokemonRead].health = stoi(line);
				break;
			case 2:
				// Get Pokemon Level
				player.pokemon[pokemonRead].level = stoi(line);
				break;
			case 3:
				// Get Pokemon Experience Points
				player.pokemon[pokemonRead].exp = stoi(line);
				break;
			case 4:
				// Get Pokemon Species ENUM
				player.pokemon[pokemonRead].species = static_cast<PokemonSpecies>(stoi(line));
				break;
			case 5:
				// Get Pokemon Dead Status
				player.pokemon[pokemonRead].isDead = static_cast<bool>(stoi(line));
				break;
			case 6:
				// Get Pokemon Max Health Value
				player.pokemon[pokemonRead].maxHealth = stoi(line);
			}

			// Increment Individual Pokemon Data Counter
			++pokemonLineInc;

			// If the current pokemon line is at the end point, reset line counter and read in the next pokemon.
			if (pokemonLineInc == POKEMON_LINES)
			{
				pokemonLineInc = 0;
				pokemonRead++;
			}
		}

		// Increment Line Counter
		lineNumber++;
	}

	// Close Save File
	saveFile.close();
}
// *******************************************
//           drawHealthUI
//    Draws Health as | and *'s
//********************************************
void drawHealthUI(int hp, int max)
{
	// Draw Current HP Bars
	for (int i = 0; i < hp; i++)
	{
		cout << "|";
	}

	int starsToDraw = max - hp;

	// Draw HP Lost Stars
	for (int i = 0; i < starsToDraw; i++)
	{
		cout << "*";
	}
}
// *******************************************
//           drawBattleUIHeader
//    Draws Top Portion of Battle UI System
//    This displays the name and health of the
//    opponent.
//********************************************
void drawBattleUIHeader(PokemonData &attackingPokemon)
{
	// Draw First Line (60 Characters)
	drawLines(60);

	// Draw Attacking Pokemon Information
	cout << "= Target Name: " << attackingPokemon.name << endl;
	cout << "= Target Level: " << attackingPokemon.level << endl;

	cout << "= Target HP: ";
	drawHealthUI(attackingPokemon.health, attackingPokemon.maxHealth);
	cout << " (" << attackingPokemon.health << " HP / " << attackingPokemon.maxHealth << " HP) " << endl;

	// Draw Last Line (60 Characters)
	drawLines(60);
}
// *******************************************
//           drawBattleUIFooter
//    Draws Bottom Portion of Battle UI System
//    Several Menu Systems are handled here.
//    Allows the user to select to attack, bag
//    swap out their pokemon, and attempt to
//    flee
//********************************************
void drawBattleUIFooter(MenuLocation location, PlayerData &trainer)
{
	// Draw First Line (60 Characters)
	drawLines(60);

	if (location == ATTACK)
	{
		// Get Species Information of currently active trainer Pokemon
		PokemonData trainersPokemon = trainer.pokemon[0];
		PokemonSpecies trainerPokemonSpecies = trainersPokemon.species;
		PokemonSpeciesData pokemonSpecies = speciesData[trainerPokemonSpecies];

		// Assemble Menu Items
		string attack1 = "1. ";
		attack1.append(pokemonSpecies.moveSet[0]);

		string attack2 = "2. ";
		attack2.append(pokemonSpecies.moveSet[1]);

		string back = "3. Previous Menu";

		// Assemble Right Hand Side Stats about currently active trainer Pokemon
		string trainerPokemonName = "= Name:  ";
		trainerPokemonName.append(trainersPokemon.name);

		string trainerPokemonLevel = "= Level: ";
		trainerPokemonLevel.append(to_string(trainersPokemon.level));

		string trainerPokemonHP = "= HP:    ";
		trainerPokemonHP.append(to_string(trainersPokemon.health));
		trainerPokemonHP.append(" HP / ");
		trainerPokemonHP.append(to_string(trainersPokemon.maxHealth));
		trainerPokemonHP.append(" HP");

		// Output Information to Screen
		cout << left;
		cout << setw(20) << "Select Attack: " << setw(20) << "= Player Pokemon Stats:" << endl;
		cout << setw(20) << attack1 << setw(20) << "=" << endl;
		cout << setw(20) << attack2 << setw(20) << trainerPokemonName << endl;
		cout << right << setfill(' ') << setw(30) << trainerPokemonLevel << endl;
		cout << left << setw(20) << back << setw(20) << trainerPokemonHP << endl;
	}
	else if (location == BAG)
	{
		// Print Trainer's Name
		cout << trainer.name << "'s Bag:" << endl << endl;

		// If the user has a ELIXIR in their inventory, show this as a selectable option.
		if (trainer.itemsOwned[ELIXIR] > 0)
		{
			cout << "1. Elixir   (Quantity: " << trainer.itemsOwned[ELIXIR] << ") " << endl;
		}

		// If the user has a POKEBALL in their inventory, show this as a selectable option.
		if (trainer.itemsOwned[POKEBALL] > 0)
		{
			cout << "2. Pokeball (Quantity: " << trainer.itemsOwned[POKEBALL] << ") " << endl;
		}

		// Print End of Menu
		cout << endl;
		cout << "3. Previous Menu" << endl;
	}
	else if (location == SELECTION)
	{
		// Print Trainer's Name
		cout << trainer.name << "'s Pokemon: " << endl;
		cout << endl;

		// Display All Pokemon in Trainer's Inventory
		for (int i = 0; i < trainer.pokemonOwned; i++)
		{
			cout << i + 1 << ". " << left << setfill(' ') << setw(15) << trainer.pokemon[i].name;
			cout << " LV: " << trainer.pokemon[i].level;
			cout << " HP: " << trainer.pokemon[i].health;
			cout << " HP / " << trainer.pokemon[i].maxHealth << " HP" << endl;
		}

		// Finish End of Menu
		cout << endl;
		cout << "7. Previous Menu" << endl;
	}
	else
	{
		// Display Main Menu Screen
		cout << "=== 1. Attack == == 2. Bag == == 3. Pokemon == == 4. Flee ==" << endl;
	}

	// Draw End Line
	drawLines(60);
}
// *******************************************
//           drawBattleUI
//    Draws the entire Battle UI
//********************************************
void drawBattleUI(PlayerData &trainer, PokemonData &attackingPokemon, MenuLocation location, Status &battleStatus)
{
	// Clear Screen
	clear();

	// Draw Battle Header
	drawBattleUIHeader(attackingPokemon);

	// Display Opponent Pokemon
	getPokemonIcon(attackingPokemon.species);

	// Draw Battle Footer
	drawBattleUIFooter(location, trainer);

	// Send Command to Battle UI Controller
	battleUIController(trainer, attackingPokemon, location, getMenuSelection(), battleStatus);
}
// *******************************************
//           clear
//    Cross-Platform Console Clearing
//********************************************
void clear()
{
	system("cls");
}
// *******************************************
//           pressEntertoContinue
//    Pause until User Presses Enter
//********************************************
void pressEnterToContinue()
{
	// Ignore Previous Enter
	cin.ignore();

	// New Line
	cout << endl;

	// Tell User
	cout << "Press Enter to Continue";

	// Ignore Enter and Continue Program Execution
	cin.ignore();
}
// *******************************************
//           drawLines
//    Draws X number of = (For UI Systems)
//********************************************
void drawLines(int lines)
{
	for (int i = 0; i < lines; i++)
	{
		cout << "=";
	}
	cout << endl;
}
// *******************************************
//           confirmStarterSelection
//    Allows user to confirm their starter
//		pokemon selection.
//********************************************
Status confirmStarterSelection(PlayerData &trainer, int selection)
{
	// Clear the Screen
	clear();

	// Print Pokemon by casting the selection into the a PokemonSpecies ENUM
	getPokemonIcon(static_cast<PokemonSpecies>(selection - 1));

	// Print Menu Output
	cout << "You have selected " << DefaultSpeciesNames[selection - 1] << "! Are you sure?" << endl << endl;

	cout << "1. Accept" << endl;
	cout << "2. Go Back" << endl << endl;

	// Get User Choice
	int choice = getMenuSelection();

	if (choice == 1)
	{
		// User has Accepted the Pokemon, Lets Create It
		PokemonData starterPokemon;
		starterPokemon.name = DefaultSpeciesNames[selection - 1];
		starterPokemon.species = static_cast<PokemonSpecies>(selection - 1);

		// Add that Pokemon to the Trainer's Inventory
		trainer.addPokemon(starterPokemon);
		return SUCCESS;
	}
	else
	{
		// They didn't want this one, so return back to the loop.
		return FAILED;
	}
}
// *******************************************
//           selectStarterPokemon
//    Allows user to select their starter
//		pokemon selection.
//********************************************
void selectStarterPokemon(PlayerData &trainer)
{
	// Storage Variable for the Menu Selection
	int selection;

	// Storage Variable for the Added Pokemon Status ENUM
	Status addedPokemon = FAILED;

	do
	{
		// Clear the Screen
		clear();

		// Print Menu
		cout << "Pick your Starter Pokemon: " << endl;
		cout << "1. Bulbasaur" << endl;
		cout << "2. Charmander" << endl;
		cout << "3. Squirtle" << endl;

		// Get Selection
		selection = getMenuSelection();

		// Confirm Starter Pokemon
		addedPokemon = confirmStarterSelection(trainer, selection);
	} while (addedPokemon == FAILED);
}
// *******************************************
//           newGame
//    Prompts user to input their name and
//		their rival's name.
//********************************************
void newGame(PlayerData &trainer)
{
	// Clear the Screen
	clear();

	// Storage Variable for User Input
	string input;

	// Get Trainer's Name and append to Trainer Object
	cout << "Enter your name: ";
	getline(cin, input);
	trainer.name = input;

	// Clear the Screen
	clear();

	// Get Rival's Name and append to Trainer Object
	cout << "Enter your rival's name: ";
	getline(cin, input);
	trainer.rivalName = input;

	// Move to Select Starter Pokemon
	selectStarterPokemon(trainer);
}
// *******************************************
//           printStats
//    Displays all trainer information as well
//		as information about their pokemon.
//********************************************
void printStats(PlayerData &trainer)
{
	// Clear the Screen
	clear();

	// Draw 60 =
	drawLines(60);

	// Print Trainer and Rival Name
	cout << "Trainer Name: " << trainer.name << endl;
	cout << "Rival's Name: " << trainer.rivalName << endl;
	cout << endl;

	// Print Money on Hand and Number of Pokemon
	cout << "Money:   " << trainer.money << endl;
	cout << "Pokemon: " << trainer.pokemonOwned << endl;
	cout << endl;

	// If the Trainer has Items, display them
	if (trainer.hasItems())
	{
		cout << "Bag: " << endl;
		if (trainer.itemsOwned[ELIXIR] > 0)
		{
			cout << "Elixir   (Quantity: " << trainer.itemsOwned[ELIXIR] << ") " << endl;
		}

		if (trainer.itemsOwned[POKEBALL] > 0)
		{
			cout << "Pokeball (Quantity: " << trainer.itemsOwned[POKEBALL] << ") " << endl;
		}

		cout << endl;
	}

	// Draw 60 =
	drawLines(60);

	// Pokemon Information
	for (int i = 0; i < trainer.pokemonOwned; i++)
	{
		// Get Current Pokemon for i
		PokemonData currentPokemon = trainer.pokemon[i];

		// Are they Fainted or Ready to Battle?
		string status = (currentPokemon.isDead == true ? "Fainted" : "Ready for Combat");

		// Print Status
		cout << "Name:  " << currentPokemon.name << endl;
		cout << "Level: " << currentPokemon.level << endl;
		cout << "EXP:   " << currentPokemon.exp << endl;
		cout << "HP:    " << currentPokemon.health << " HP / " << currentPokemon.maxHealth << " HP" << endl;
		cout << endl;
		cout << "Status: " << status << endl;
		cout << endl;

		// Print Icon
		getPokemonIcon(currentPokemon.species);

		// Draw 60 =
		drawLines(60);
	}

	// Press Enter to Continue
	pressEnterToContinue();
}
// *******************************************
//           pokemonCenterCost
//    Calculates how many credits it costs to
//		bring a pokemon back to full health
//    depending on what it's current health is.
//********************************************
int pokemonCenterCost(int health, int maxHealth)
{
	int healthToRestore = maxHealth - health;

	// 25 Credits per HP Restored
	return healthToRestore * 25;
}
// *******************************************
//           pokemonCenterHeal
//    Attempts to heal selected pokemon.
//********************************************
Status pokemonCenterHeal(PlayerData &trainer, int pokemonToHeal)
{
	// What Pokemon are we healing (modified to fit 0 based array.)
	int pokemon = pokemonToHeal - 1;

	// Get Pokemon Data for Pokemon Selected
	PokemonData current = trainer.pokemon[pokemon];

	// Determine how much it will cost
	int cost = pokemonCenterCost(current.health, current.maxHealth);

	// Clear the Screen
	clear();

	// Display Pokemon Information
	cout << "Pokemon Name: " << current.name << endl;
	cout << "Current HP: " << current.health << " HP" << endl << endl;

	// Display Menu to User to inquire about healing this Pokemon
	cout << "Would you like to restore \"" << current.name << "\" to full health? (" << current.maxHealth << " HP)" << endl;
	cout << "It will cost " << cost << " to restore them to full health." << endl << endl;

	cout << "1. Accept" << endl;
	cout << "2. Decline" << endl;

	// Get User Selection
	int selection = getMenuSelection();

	// Clear the Screen
	clear();

	if (selection == 1)
	{
		// Attempt to Remove Money from Trainer's Wallet
		Status removeMoney = trainer.removeMoney(cost);

		// If SUCCESS then they had enough money
		if (removeMoney == SUCCESS)
		{
			// Give 999 Health to Pokemon and set it to Alive (No Pokemon will have more than 999 health)
			trainer.pokemon[pokemon].giveHealth(999);
			trainer.pokemon[pokemon].isDead = false;

			// Print Success Message
			cout << "Success! You have healed " << current.name << " to full health!" << endl;
		}
		else
		{
			// Player did not have enough money.
			cout << "You do not have enough money to heal " << current.name << ". Come back when you have the money." << endl;
		}

		// Press Enter to Continue
		pressEnterToContinue();

		return SUCCESS;
	}
	else
	{
		return FAILED;
	}
}
// *******************************************
//           pokemonCenter
//    Displays the Pokemon Center Menu
//********************************************
void pokemonCenter(PlayerData &trainer)
{
	// Container for Selection
	int selection = 0;

	do
	{
		// Clear the Screen
		clear();

		// Display Menu
		cout << "Pokemon Center (Select Pokemon to Heal): " << endl << endl;

		// Create Menu Entry for each Pokemon in Trainer's Inventory
		for (int i = 0; i < trainer.pokemonOwned; i++)
		{
			// Get Pokemon Data for i
			PokemonData current = trainer.pokemon[i];

			// Print Pokemon Data on Menu
			cout << i + 1 << ". " << current.name << " ( " << current.health << " HP / " << current.maxHealth << " HP )" << endl;
		}

		// Spacing
		cout << endl;

		cout << "7. Return to Menu" << endl;

		// Get User Input
		selection = getMenuSelection();

		// If the Selction is not 7 (return to menu) AND (Selection is Greater than ZERO AND Selection is less than or equal to pokemonOwned)
		if (selection != 7 && (selection > 0 && selection <= trainer.pokemonOwned))
		{
			// Send them to get Healed
			pokemonCenterHeal(trainer, selection);
		}
	} while (selection != 7);
}
// *******************************************
//           pokemonMartItem
//    Attempts the purchase of an item from
//    the Pokemon Mart.
//********************************************
Status pokemonMartItem(PlayerData &trainer, int item)
{
	// Clear the Screen
	clear();

	// Get Item Data for Selected Item (adjusts for zero based array)
	PokemonItem selectedItem = itemData[item - 1];

	// Print Item Information
	cout << "Item Name:  " << selectedItem.name << endl;
	cout << "Item Price: " << selectedItem.price << endl;
	cout << "Item Description: " << selectedItem.description << endl;

	// Spacing
	cout << endl;

	// Inquire about purchasing the item
	cout << "Would you like to buy this item?" << endl;

	// Spacing
	cout << endl;

	// Menu
	cout << "1. Purchase" << endl;
	cout << "2. Decline" << endl;

	// Get User Input
	int selection = getMenuSelection();

	// Clear the Screen
	clear();

	if (selection == 1)
	{
		// Attempt to add the item to Trainer's Inventory
		Status addItem = trainer.addItem(static_cast<ItemNames>(item - 1));

		if (addItem == SUCCESS)
		{
			// Player had enough money
			cout << "You have successfully purchased a " << selectedItem.name << "." << endl;
		}
		else
		{
			// Player didn't have enough money
			cout << "You do not have enough money to purchase a " << selectedItem.name << "." << endl;
		}
	}
	else
	{
		return FAILED;
	}

	// Press Enter to Continue
	pressEnterToContinue();

	return SUCCESS;
}
// *******************************************
//           pokemonMart
//    Displays the Pokemon Mart Menu
//********************************************
void pokemonMart(PlayerData &trainer)
{
	// Selection Container
	int selection = 0;

	do
	{
		// Clear the Screen
		clear();

		// Menu
		cout << "Pokemon Mart (Select Item to Buy): " << endl;
		cout << endl;

		// Items
		cout << "1. Elixir   (Cost: 500)" << endl;
		cout << "2. Pokeball (Cost: 2000)" << endl;
		cout << endl;

		cout << "3. Return to Menu" << endl;

		// Get User Input
		selection = getMenuSelection();

		// If (Selection is NOT 3 and (Selection is greater than ZERO AND Selection is less than or equal to the number of Items in Game))
		if (selection != 3 && (selection > 0 && selection <= ITEMS_IN_GAME))
		{
			// Attempt to buy the item.
			pokemonMartItem(trainer, selection);
		}
	} while (selection != 3);
}
// *******************************************
//           drawBattleUIStatus
//    Draws Battle UI with Health and Pokemon
//		Image, but with a passthrough message
// 		along with waiting for an enter key to
//    be pressed.
//********************************************
void drawBattleUIStatus(PlayerData &trainer, PokemonData &attackingPokemon, string text)
{
	// Clear the Screen
	clear();

	// Show Attacking Pokemon's Name, Level, and HP
	drawBattleUIHeader(attackingPokemon);

	// Draw Attacking Pokemon
	getPokemonIcon(attackingPokemon.species);

	// Draw 60 =
	drawLines(60);

	// Output Message
	cout << text << endl;

	// Draw 60 =
	drawLines(60);

	// Press Enter to Continue
	pressEnterToContinue();
}
// *******************************************
//           computerAttack
//    Processes Computer Attack
//********************************************
Status computerAttack(PlayerData &trainer, PokemonData &attackingPokemon, Status &lastAttack)
{
	// Variable
	int attackUsed;
	int attackToUse = rand() % 10;
	int attackPower = 0;

	// 10% Chance of Special Attack
	if (attackToUse >= 8)
	{
		// Special
		attackUsed = 1;

		// Number between 0 and 9 * (level * .25)
		attackPower = ((rand() % 9) * (attackingPokemon.level * 0.25));
	}
	else
	{
		// Normal
		attackUsed = 0;

		// Number between 0 and 5 + 3 * (level * .25);
		attackPower = ((((rand() % 5) + 3)) * (attackingPokemon.level * 0.25));
	}

	// Hit Player
	Status hitPokemon = trainer.pokemon[0].takeDamage(attackPower);

	// Create Status Message
	vector <string> statusMessage = { "Wild ", attackingPokemon.name, " used ", speciesData[attackingPokemon.species].moveSet[attackUsed], "! " };

	// Based on the result of hitting the player
	switch (hitPokemon)
	{
	case HIT:
		statusMessage.insert(statusMessage.end(), { trainer.pokemon[0].name, " took ", to_string(attackPower), " damage!" });
		break;
	case MISSED:
		statusMessage.insert(statusMessage.end(), " It missed!");
		break;
	case DEAD:
		statusMessage.insert(statusMessage.end(), { trainer.pokemon[0].name, " has fainted!" });
		break;
	}

	// Draw Status Message
	drawBattleUIStatus(trainer, attackingPokemon, multipleStrings(statusMessage));

	// Set lastAttack
	lastAttack = hitPokemon;

	return SUCCESS;
}
// *******************************************
//           playerAttack
//    Processes Player Attack
//********************************************
void playerAttack(PlayerData &trainer, PokemonData &attackingPokemon, Status &lastAttack)
{
	// Show Battle UI Overview
	drawBattleUI(trainer, attackingPokemon, OVERVIEW, lastAttack);
}
// *******************************************
//           swapPokemon
//    Takes selected pokemon and moves it to
//    the front of the Pokemon array for the
//    trainer and moves the previously default
//    pokemon to the place of the selected
//    pokemon.
//********************************************
Status swapPokemon(PlayerData &trainer, int pokemonToSwap)
{
	// Get Currently Equipped Pokemon Data
	PokemonData currentPokemon = trainer.pokemon[0];

	// Get Pokemon to be Swapped Data
	PokemonData swapPokemon = trainer.pokemon[pokemonToSwap - 1];

	// Create Summon Message
	vector<string> message = { currentPokemon.name, " come back! Go! ", swapPokemon.name, "!" };

	// If the Pokemon we are trying to swap to is not dead
	if (swapPokemon.isDead != true)
	{
		// Set the Zero Index (first Pokemon) to the Swap Pokemon
		trainer.pokemon[0] = swapPokemon;

		// Set the Currently Equipped Pokemon to the Location of the Swap Pokemon
		trainer.pokemon[pokemonToSwap - 1] = currentPokemon;

		// Draw Battle UI with Status Message
		drawBattleUIStatus(trainer, swapPokemon, multipleStrings(message));

		return SUCCESS;
	}
	else
	{
		// Draw Battle UI with Status Message
		drawBattleUIStatus(trainer, swapPokemon, "This POKEMON is not fit for battle! Cannot swap!");

		return FAILED;
	}
}
// *******************************************
//           deadPickNew
//    Prompts user to select a new pokemon to
//    replace the currently dead one. This
//    message will only show up in the event
//    that there are other pokemon in the
//    trainer's inventory that aren't dead.
//********************************************
void deadPickNew(PlayerData &trainer)
{
	// Container for Status
	Status swapSuccess;

	do
	{
		// Clear the Screen
		clear();

		// Tell User to pick new Pokemon
		cout << "Call out a new POKEMON! " << endl;

		// Print All Pokemon in Trainer's Inventory
		for (int i = 0; i < trainer.pokemonOwned; i++)
		{
			// Only Print Pokemon that are not Dead
			if (trainer.pokemon[i].isDead == false)
			{
				// Print Pokemon Stats
				cout << i + 1 << ". " << left << setfill(' ') << setw(15) << trainer.pokemon[i].name;
				cout << " LV: " << trainer.pokemon[i].level;
				cout << " HP: " << trainer.pokemon[i].health;
				cout << " HP / " << trainer.pokemon[i].maxHealth << " HP" << endl;
			}
		}

		// Get User Selection
		int selection = getMenuSelection();

		// Swap the Pokemon
		swapSuccess = swapPokemon(trainer, selection);
	} while (swapSuccess == FAILED);
}
// *******************************************
//           multipleStrings
//    Takes a vector of strings and creates
//    one string out of it.
//********************************************
string multipleStrings(vector<string> statement)
{
	// Creates Result Container
	string result;

	// For each item in the incoming vector
	for (int i = 0; i < statement.size(); i++)
	{
		// Append that Item to the Result String
		result.append(statement.at(i));
	}

	// Return Result String
	return result;
}
// *******************************************
//           playerWin
//    Tells player they've won the battle and
//    gives out money and experience.
//********************************************
void playerWin(PlayerData &trainer, PokemonData &attackingPokemon)
{
	// Determine how much EXP and Money to Give Player
	int expEarned = attackingPokemon.level * 15;
	int moneyEarned = attackingPokemon.level * 200;

	// Create Victory Message
	vector<string> messages = { trainer.name, " has defeated ", attackingPokemon.name, "! ",
		trainer.pokemon[0].name, " has earned ", to_string(expEarned),
		" EXP! \n", trainer.name, " has earned ", to_string(moneyEarned),
		" credits!" };

	// Add Money to Trainer's Wallet
	trainer.money += moneyEarned;

	// Give EXP and check if the Pokemon Leveled Up
	if (trainer.pokemon[0].addExp(expEarned) == LEVELUP)
	{
		// Create Level UP Message
		vector<string> levelUP = { trainer.pokemon[0].name, " has leveled up to Level ", to_string(trainer.pokemon[0].level), "!" };

		// Display Battle UI with Status Message
		drawBattleUIStatus(trainer, trainer.pokemon[0], multipleStrings(levelUP));
	}

	// Display Battle UI with Status Message
	drawBattleUIStatus(trainer, attackingPokemon, multipleStrings(messages));
}
// *******************************************
//           computerWin
//    Calculates how much money the player has
//    lost and tells them.
//********************************************
void computerWin(PlayerData &trainer, PokemonData &attackingPokemon)
{
	// Determine how much money will be taken from the Player
	int moneyLost = attackingPokemon.level * 25;

	// Create Loss Message
	vector<string> messages = { trainer.name, " has been defeated by ", attackingPokemon.name, "! ",
		trainer.name, " has lost ", to_string(moneyLost), " credits." };

	// Attempt to remove money from the Player
	if (trainer.removeMoney(moneyLost) == FAILED)
	{
		// The trainer doesn't have enough money to lose, so set it to zero.
		trainer.money = 0;
	}

	// Draw Battle UI with Status Message
	drawBattleUIStatus(trainer, attackingPokemon, multipleStrings(messages));
}
// *******************************************
//           mainBattleLoop
//    Battle Loop for Pokemon Battle System
//********************************************
void mainBattleLoop(PlayerData &trainer, PokemonData &attackingPokemon, int whoGoes)
{
	// Are We Battling?
	bool isBattling = true;

	// Status Variable
	Status lastAttack;

	// Determine Loop Order
	if (whoGoes == 0)
	{
		// Computer Goes First
		while (isBattling)
		{
			// Computer Attacks
			computerAttack(trainer, attackingPokemon, lastAttack);

			// If the Computer killed the trainer's current Pokemon
			if (lastAttack == DEAD)
			{
				// Do we have any replacement pokemon?
				if (trainer.alivePokemon() != 0)
				{
					// Tell Trainer to Pick a New One
					deadPickNew(trainer);
				}
				else
				{
					// Computer Win
					computerWin(trainer, attackingPokemon);

					// End Battle
					isBattling = false;
					break;
				}
			}

			// Player Attacks
			playerAttack(trainer, attackingPokemon, lastAttack);

			// If the Player kills the Computer
			if (lastAttack == DEAD)
			{
				// Player Win
				playerWin(trainer, attackingPokemon);

				// End Battle
				isBattling = false;
				break;
			}
			// If Player Ended Battle (Fled / Pokeball)
			else if (lastAttack == BATTLE_END)
			{
				// End Battle
				isBattling = false;
				break;
			}
		}
	}
	else
	{
		// Player Goes First
		while (isBattling)
		{
			// Player Attack
			playerAttack(trainer, attackingPokemon, lastAttack);

			// If Player killed Computer
			if (lastAttack == DEAD)
			{
				// Player Win
				playerWin(trainer, attackingPokemon);

				// End Battle
				isBattling = false;
				break;
			}
			// If Player Ended Battle (Fled / Pokeball)
			else if (lastAttack == BATTLE_END)
			{
				// End Battle
				isBattling = false;
				break;
			}

			// Computer Attack
			computerAttack(trainer, attackingPokemon, lastAttack);

			// If Computer killed Player
			if (lastAttack == DEAD)
			{
				// Do they have other pokemon?
				if (trainer.alivePokemon() != 0)
				{
					// Tell Them to Swap
					deadPickNew(trainer);
					continue;
				}
				else
				{
					// Computer Win
					computerWin(trainer, attackingPokemon);

					// End Battle
					isBattling = false;
					break;
				}
			}
		}
	}
}
// *******************************************
//           pokemonBattleSetup
//    Creates Opponent for the Pokemon Battle
//********************************************
void pokemonBattleSetup(PlayerData &trainer)
{
	// Clear Screen
	clear();

	// Get Data about Primary Pokemon in Trainer's Possession
	PokemonData trainerPrimary = trainer.pokemon[0];

	// Get Random Species 0 - 9
	int opponentSpecies = rand() % 10;

	// Determine the Highest and Lowest Levels possible
	int lowestLevel = trainerPrimary.level - 3;
	int highestLevel = trainerPrimary.level + 4;

	// Generate Level
	int opponentLevel = (rand() % (highestLevel - lowestLevel)) + lowestLevel;

	// Make sure level isn't less than 1
	if (opponentLevel < 1) opponentLevel = 1;

	// Create Opponent
	PokemonData attackingPokemon;
	attackingPokemon.name = DefaultSpeciesNames[opponentSpecies];
	attackingPokemon.level = opponentLevel;
	attackingPokemon.health = opponentLevel * 5;
	attackingPokemon.maxHealth = attackingPokemon.health;
	attackingPokemon.species = static_cast<PokemonSpecies>(opponentSpecies);

	// Determine Who Attacks First
	int whoGoes = rand() % 2;

	// Create Status Message (A wild POKEMON_NAME appeared! GO! PRIMARY_NAME!)
	vector<string> statusMessage = { "A wild ", attackingPokemon.name, " appeared! GO! ", trainerPrimary.name, "!" };

	// Draw UI
	drawBattleUIStatus(trainer, attackingPokemon, multipleStrings(statusMessage));

	// Begin Loop
	mainBattleLoop(trainer, attackingPokemon, whoGoes);
}
// *******************************************
//           mainGameLoop
//    Main Game Loop for Entire Game
//********************************************
void mainGameLoop(PlayerData &trainer)
{
	// Are we Playing
	bool playing = true;

	// Container for Input
	int input;

	// Main Loop
	while (playing)
	{
		// Clear Screen
		clear();

		// Print Menu
		cout << "===================================================================" << endl;
		cout << "== 1. Battle == 2. Shop == 3. Heal == 4. Stats == 5. Save / Quit ==" << endl;
		cout << "===================================================================" << endl;

		// Get Input
		input = getMenuSelection();

		switch (input)
		{
		case 1:
			if (trainer.alivePokemon() != 0)
			{
				// Trainer has live Pokemon, lets battle!
				pokemonBattleSetup(trainer);

				// After Battle, Auto Save Game
				saveGame(trainer);
			}
			else
			{
				// Clear the Screen
				clear();

				// Tell User they have no Pokemon fit for Battle
				cout << "None of your Pokemon are fit for battle. You need to heal one before you can fight again." << endl;

				// Press Enter to Continue
				pressEnterToContinue();
			}
			break;
		case 2:
			// Go to Pokemon Mart
			pokemonMart(trainer);

			// Auto Save Game
			saveGame(trainer);
			break;
		case 3:
			// Go to Pokemon Center
			pokemonCenter(trainer);

			// Auto Save Game
			saveGame(trainer);
			break;
		case 4:
			// Display Trainer Info
			printStats(trainer);
			break;
		case 5:
			// Save Game
			saveGame(trainer);

			// Stop Playing
			playing = false;
			break;
		}
	}
}
// *******************************************
//           mainMenu
//    Main Menu shown on the Program Start
//********************************************
void mainMenu(PlayerData &trainer)
{
	// Clear the Screen
	clear();

	// Display the Menu
	cout << "Pokemon - Main Menu" << endl;

	// If there is a Save File
	if (gameExists())
	{
		cout << "1. Continue Game" << endl;
	}

	cout << "2. New Game" << endl;
	cout << "3. Exit Game" << endl;

	// Get Menu Selection
	int menuSelection = getMenuSelection();

	// Ignore the Enter
	cin.ignore();

	switch (menuSelection)
	{
	case 1:
		// If there isn't a Save File (Quit)
		if (!gameExists())
		{
			clear();
			cout << "No Game to Load. You shouldn't be here. Exiting." << endl;
			exit(0);
		}

		// Load Save File
		loadGame(trainer);

		// Enter Main Game Loop
		mainGameLoop(trainer);
		break;
	case 2:
		// Create New Trainer
		newGame(trainer);

		// Save Game
		saveGame(trainer);

		// Enter Main Game Loop
		mainGameLoop(trainer);
		break;
	case 3:
		// Exit
		exit(0);
		break;
	}
}
// *******************************************
//           battleUIAttack
//    Attacks opponent. This is invoked by the
//		user's battle UI.
//********************************************
void battleUIAttack(PlayerData &trainer, PokemonData &attackingPokemon, Status attackType, Status &battleStatus)
{
	// Containers for Attack Power and Attack Used
	int attackPower;
	int attackUsed;

	if (attackType == NORMAL)
	{
		// Normal Attack
		attackUsed = 0;

		// Number between 0 and 5 + 3 * (level * .25)
		attackPower = ((((rand() % 5) + 3)) * (trainer.pokemon[0].level * 0.25));
	}
	else
	{
		// Special Attack
		attackUsed = 1;

		// Number between 0 and 9 * (level * .25)
		attackPower = ((rand() % 9) * (trainer.pokemon[0].level * 0.25));
	}

	// Hit Attacking Pokemon
	Status doDamage = attackingPokemon.takeDamage(attackPower);

	// Create Use Message
	vector<string> statusMessage = { trainer.pokemon[0].name, " used ", speciesData[trainer.pokemon[0].species].moveSet[attackUsed], "! " };

	switch (doDamage)
	{
	case HIT:
		// Create Hit Message
		statusMessage.insert(statusMessage.end(), { "Wild ", attackingPokemon.name, " took ", to_string(attackPower), " damage!" });
		break;
	case MISSED:
		// Create Miss Message
		statusMessage.insert(statusMessage.end(), " It missed!");
		break;
	case DEAD:
		// Create Dead Message
		statusMessage.insert(statusMessage.end(), { "Wild ", attackingPokemon.name, " has fainted!" });
		break;
	}

	// Set battleStatus to doDamage (for other systems)
	battleStatus = doDamage;

	// Draw Battle UI with Status Message
	drawBattleUIStatus(trainer, attackingPokemon, multipleStrings(statusMessage));
}
// *******************************************
//           battleUIFlee
//    Attempts to flee from battle. This is
//		invoked by the user's battle UI.
//********************************************
void battleUIFlee(PlayerData &trainer, PokemonData &attackingPokemon, Status &battleStatus)
{
	// 50/50 Chance of Success
	int chance = rand() % 2;

	if (chance == 0)
	{
		// Fled
		drawBattleUIStatus(trainer, attackingPokemon, "Got away safely!");
		battleStatus = BATTLE_END;
	}
	else
	{
		// Failed
		drawBattleUIStatus(trainer, attackingPokemon, "Can't Escape!");
		battleStatus = BATTLE_CONTINUE;
	}
}
// *******************************************
//           throwPokeball
//    Attempts to throw a pokeball and catch
//    the opponent. This is invoked by the
//		user's battle UI.
//********************************************
Status throwPokeball(PlayerData &trainer, PokemonData &attackingPokemon, Status &battleStatus)
{
	// 1 / 10 (10% Chance)
	int chance = rand() % 10;

	if (chance == 0)
	{
		// Attempt to Catch Pokemon
		Status addAttempt = trainer.addPokemon(attackingPokemon);

		if (addAttempt == SUCCESS)
		{
			// End Battle
			battleStatus = BATTLE_END;

			return CAUGHT;
		}
		else
		{
			battleStatus = BATTLE_CONTINUE;
			return FAILED;
		}
	}
	else
	{
		battleStatus = BATTLE_CONTINUE;
		return FAILED;
	}
}
// *******************************************
//           battleUIController
//    Controller for the Battle UI System.
//    Based on where the User is in the UI
//		this function redirects them to a new
//		location.
//********************************************
void battleUIController(PlayerData &trainer, PokemonData &attackingPokemon, MenuLocation &location, int menuSelection, Status &battleStatus)
{
	// Containers for Message and Catch Attempt
	string message;
	Status catchAttempt;

	switch (location)
	{
	case OVERVIEW:
		switch (menuSelection)
		{
		case 1:
			drawBattleUI(trainer, attackingPokemon, ATTACK, battleStatus);
			break;
		case 2:
			drawBattleUI(trainer, attackingPokemon, BAG, battleStatus);
			break;
		case 3:
			drawBattleUI(trainer, attackingPokemon, SELECTION, battleStatus);
			break;
		case 4:
			battleUIFlee(trainer, attackingPokemon, battleStatus);
			break;
		}
		break;
	case ATTACK:
		switch (menuSelection)
		{
		case 1:
			battleUIAttack(trainer, attackingPokemon, NORMAL, battleStatus);
			break;
		case 2:
			battleUIAttack(trainer, attackingPokemon, SPECIAL, battleStatus);
			break;
		case 3:
			drawBattleUI(trainer, attackingPokemon, OVERVIEW, battleStatus);
			break;
		}
		break;
	case BAG:
		switch (menuSelection)
		{
		case 1:
			if (trainer.removeItem(ELIXIR) != FAILED)
			{
				// Give Current Pokemon 20 HP
				trainer.pokemon[0].giveHealth(20);

				// Create Message
				vector<string> message = { "Added 20 HP to ", trainer.pokemon[0].name, "!" };

				// Draw Battle UI with Message
				drawBattleUIStatus(trainer, attackingPokemon, multipleStrings(message));
			}
			else
			{
				// Player doesn't have any Elixir
				drawBattleUIStatus(trainer, attackingPokemon, "You do not have any of that item.");
			}

			battleStatus = BATTLE_CONTINUE;

			break;
		case 2:
			if (trainer.removeItem(POKEBALL) != FAILED)
			{
				// Create Message
				vector<string> message = { trainer.name, " used a POKEBALL! " };

				// Attempt to Capture Pokemon
				catchAttempt = throwPokeball(trainer, attackingPokemon, battleStatus);

				if (catchAttempt == CAUGHT)
				{
					// Caught Pokemon
					message.insert(message.end(), { "GOTCHA! Wild ", attackingPokemon.name, " was caught!" });
				}
				else
				{
					// Failed to Capture Pokemon or Trainer already has 6 Pokemon
					message.insert(message.end(), "Oh, no! The POKEMON broke free!");
				}

				// Draw Battle UI with Status Message
				drawBattleUIStatus(trainer, attackingPokemon, multipleStrings(message));
			}
			else
			{
				// Player doesn't have any Pokeballs
				drawBattleUIStatus(trainer, attackingPokemon, "You do not have any of that item.");
			}

			battleStatus = BATTLE_CONTINUE;
			break;
		case 3:
			drawBattleUI(trainer, attackingPokemon, OVERVIEW, battleStatus);
			break;
		}
		break;
	case SELECTION:
		if (menuSelection == 7)
		{
			drawBattleUI(trainer, attackingPokemon, OVERVIEW, battleStatus);
		}
		else
		{
			swapPokemon(trainer, menuSelection);
		}
		battleStatus = BATTLE_CONTINUE;
		break;
	}
}
// *******************************************
//           getMenuSelection
//    Helper Function for getting input from
//    menu system.
//********************************************
int getMenuSelection()
{
	// Storage
	int value;

	// Spacing
	cout << endl;

	// Get User Input
	cout << "Enter Selection: ";
	cin >> value;

	// Return Value
	return value;
}
// *******************************************
//           gameExists
//    Checks if Save File Exists
//********************************************
bool gameExists()
{
	ifstream saveFile("save.txt");

	return static_cast<bool>(saveFile);
}
