#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#define NUM_OF_PLAYERS 2
#define PLAYER_ID 0
#define ENEMY_ID 1
#define SLOWPRINT_INTERVAL 20000
#define SLOWPRINT_THINKING 700000
#define LEN_OF_NAME 40
#define LEN_OF_DESCRIPTION 256
#define NUM_OF_TECHS 4

struct wpn {
	char name[LEN_OF_NAME];
	int modAttack;
	int modTech;
};

struct itm {
	char name[LEN_OF_NAME];
	int number;
	char description[LEN_OF_DESCRIPTION];
};

struct invtry {
	struct itm evkFood;
	struct itm parksideFood;
	struct itm pandaFood;
};

struct flg {
	int homework;
	int sickness;
};

struct plyr {
	char name[LEN_OF_NAME];
	int baseHp;
	int baseMp;
	int baseTech;
	int baseAttack;
	int baseDefense;
	char armorName[LEN_OF_NAME];
	int armorModDefense;
	struct wpn weapon;
	int level;
	int exp;
	int expValue;
	int tech[NUM_OF_TECHS];
	
	//Cumulative data
	
	int cumAttack;
	int cumDefense;
	int cumTech;
	int expToNextLevel;
	
	//Live data
	
	int curHp;
	int curMp;
	struct invtry inventory;
	struct flg flags;
	
	//Circle of weaknesses
	
	int typeEngineering;
	int typeArt;
	int typeBusiness;
	
	//Eng beats business, business beats art, art beats engineering.  mainly just because.
};

void loadEnemy(int, int);
void slowPrint(char*, int);
int rollD20();
int doAttack(int, int);
void updatePlayerData(int);
void describe(int);
void initPlayer(int);
int printMenu(int, int);
int mathAttack(int);
int mathDefense(int);
int mathTech(int);
int mathHp(int);
int mathMp(int);
int mathExpCost(int);
int mathExpEnemyValue(int);
int gameOver();
int mainMenu();
void loadDefaults(int);
void doBattle(int, int, int);
int techMenu(int);
void doTech(int, int, int);

struct plyr player[2];

int main()
{
	int choice;
	int i;
	
	srand((unsigned)(time(0)));  //Seed the random function
	for (i = 0; i < NUM_OF_PLAYERS; i++) {  //Load default values into both player structs
		loadDefaults(i);
	}
	choice = mainMenu();
	if (choice == 1) {
		initPlayer(PLAYER_ID);  //Load 
		doBattle(PLAYER_ID, ENEMY_ID, 0);
	}
	
	else if (choice == 2){
		printf("This isn't implemented yet, yay!!!\n");
	}
	return 0;
}

void loadDefaults(int id)
{
	strcpy(player[id].name, "defaultName");
	player[id].baseHp = 1;
	player[id].baseMp = 1;
	player[id].baseTech = 1;
	player[id].baseAttack = 1;
	player[id].baseDefense = 1;
	strcpy(player[id].armorName, "defaultArmorName");
	player[id].armorModDefense = 1;
	strcpy(player[id].weapon.name, "defaultWeaponName");
	player[id].weapon.modAttack = 1;
	player[id].weapon.modTech = 1;
	player[id].level = 1;
	player[id].exp = 0;
	player[id].expValue = 0;
	player[id].cumAttack = 0;
	player[id].cumDefense = 0;
	player[id].cumTech = 0;
	player[id].expToNextLevel = 0;
	player[id].curHp = 1;
	player[id].curMp = 1;
	player[id].flags.homework = 0;
	player[id].flags.sickness = 0;
	
	//Inventory Initialization
	
	strcpy(player[id].inventory.evkFood.name, "EVK Food");
	player[id].inventory.evkFood.number = 0;
	strcpy(player[id].inventory.evkFood.description, "Decent food that will normally increase your health, but there are no guarantees.");
	
	strcpy(player[id].inventory.parksideFood.name, "Parkside Food");
	player[id].inventory.parksideFood.number = 0;
	strcpy(player[id].inventory.parksideFood.description, "Good food, but it's hella far from southside.");
	
	strcpy(player[id].inventory.pandaFood.name, "Panda Express");
	player[id].inventory.pandaFood.number = 0;
	strcpy(player[id].inventory.pandaFood.description, "Good food that restores a lot of health, but it's pretty expensive");
	
	player[id].typeEngineering = 0;
	player[id].typeArt = 0;
	player[id].typeBusiness = 0;
}

void loadEnemy(int globalID, int enemyID)
{
	switch (enemyID) {
		case 0: //ENEMY: BRUIN
			strcpy(player[globalID].name, "Bruin");
			player[globalID].baseHp = 100;
			player[globalID].baseMp = 2;
			player[globalID].baseTech = 3;
			player[globalID].baseAttack = 3;
			player[globalID].baseDefense = 1;
			strcpy(player[globalID].armorName, "UCLA Sweatshirt");
			player[globalID].armorModDefense = 1;
			strcpy(player[globalID].weapon.name, "fork");
			player[globalID].weapon.modAttack = 0;
			player[globalID].weapon.modTech = 0;
			player[globalID].level = 1;
			player[globalID].expValue = 16;
			break;
		case 1:
			//do another
			break;

		default:
			break;
	}
	player[globalID].curHp = player[globalID].baseHp;  //Sets the enemy's live HP to max
	player[globalID].curMp = player[globalID].baseMp;  //Sets the enemy's live MP to max
	updatePlayerData(globalID);  //Sets the cumulative attack, defense and tech to the base plus the weapon and armor mods
}

void updatePlayerData(int globalID)
{
	player[globalID].cumAttack = player[globalID].baseAttack + player[globalID].weapon.modAttack;
	player[globalID].cumDefense = player[globalID].baseDefense + player[globalID].armorModDefense;
	player[globalID].cumTech = player[globalID].baseTech + player[globalID].weapon.modTech;
}

int rollD20()  //Randomly roll a value between 1 and 20
{
	int result;
	result = (rand() % 20) + 1;
	return result;
}

int doAttack(int sourceID, int targetID)
{
	int roll;
	double attackModifier;
	int damage;
	
	roll = rollD20();
	if ((roll >= 5) && (roll <= 16)) {
		printf("A decent hit!\n");
		roll -= 5;
		attackModifier = 0.9 + (((double)roll / 110.0) * 2);
	}
	else if ((roll >= 16) && (roll <= 19)){
		printf("A good hit!\n");
		attackModifier = 1.3;
	}
	else if ((roll >= 2) && (roll <= 4)){
		printf("A weak hit!\n");
		attackModifier = 0.7;
	}
	else if (roll == 20){
		slowPrint("*****  SMAAAASH!!  *****\n", SLOWPRINT_INTERVAL);
		printf("A critical hit!\n");
		attackModifier = 2.0;
	}
	else if (roll == 1){
		printf("A miss!\n");
		attackModifier = 0;
	}
	damage = (int)(((double)player[sourceID].cumAttack * attackModifier) * ((double)(100 - player[targetID].cumDefense)/100.0));
	player[targetID].curHp -= damage;
	return damage;
}

void slowPrint(char *characters, int interval)
{
	int i;
	
	for (i = 0; i < strlen(characters); i++) {
		printf("%c", characters[i]);
		fflush(stdout);
		usleep(interval);
	}
}

void describe(enemyID)
{
	char stringOut[255];
	sprintf(stringOut, "You are fighting a %s.  It appears to be wearing a %s and wielding a %s.\n",
		   player[enemyID].name,
		   player[enemyID].armorName,
		   player[enemyID].weapon.name);
	slowPrint(stringOut, SLOWPRINT_INTERVAL);
	printf("\n\n");
}

void doBattle(playerID, globalEnemyID, enemyID)
{
	int choice;
	int techChoice;
	int damage;
	int i;
	char string[LEN_OF_DESCRIPTION];
	
	loadEnemy(globalEnemyID, enemyID);
	printf("A wild %s appeared!!\n\n", player[globalEnemyID].name);
	while (!(player[playerID].curHp <= 0) && !(player[globalEnemyID].curHp <= 0)) {
		choice = printMenu(playerID, globalEnemyID);
		switch (choice) {
			case 1:
				//ATTACK
				damage = doAttack(playerID, globalEnemyID);
				printf("You did %d damage!!\n\n", damage);
				break;
			case 2:
				techChoice = techMenu(playerID);  //Implement error checking!!!!
				if (techChoice == -1) {
					continue;
				}
				else if (techChoice == 0){
					printf("No tech selected\n");
					continue;
				}
				else {
					doTech(techChoice, playerID, globalEnemyID);
				}
				break;
			case 3:
				//INVENTORY
				break;
			case 4:
				//DESCRIBE
				describe(globalEnemyID);
				continue;
			default:
				printf("That is not a valid option\n\n");
				continue;
				//break;
		}
		if (!(player[playerID].curHp <= 0) && !(player[globalEnemyID].curHp <= 0)) {
			sprintf(string, "%s is thinking", player[globalEnemyID].name);
			slowPrint(string, SLOWPRINT_INTERVAL);
			for (i = 0; i < 3; i++) {
				slowPrint(".", SLOWPRINT_THINKING);
			}
			sprintf(string, "\n%s is attacking!\n\n", player[globalEnemyID].name);
			slowPrint(string, SLOWPRINT_INTERVAL);
			damage = doAttack(globalEnemyID, playerID);
			printf("%s did %d damage!!\n", player[globalEnemyID].name, damage);
		}
	}
}

void initPlayer(int id)
{
	int nameRight = 0;
	char yesNo;
	char room[30];
	if (rand() % 2){
		strcpy(room, "room 227 in Marks Hall");
	}
	else {
		strcpy(room, "room 718 in Pardee Tower");
	}

	while (!nameRight) {
		printf("UPRCLSMAN: Welcome to USC, new student, what is your first name?\n\tName (with no spaces): ");
		scanf("%40s", player[id].name);
		printf("UPRCLSMAN:  Your name is %s, is that correct?  [y/n] ", player[id].name);
		getchar();
		scanf("%c", &yesNo);
		if (yesNo == 'y'){
			nameRight = 1;
		}
		else {
			printf("Oops, let's try that again\n");
		}
	}
	printf("UPRCLSMAN:  Great, here are your keys to %s.  Enjoy your first day!\n\n", room);
	
	//LOAD UP DEFAULT.  WILL CHANGE WITH STORY LATER!!!
	//
	//
	
	player[id].baseHp = 20;
	player[id].baseMp = 5;
	player[id].baseTech = 5;
	player[id].baseAttack = 5;
	player[id].baseDefense = 4;
	strcpy(player[id].armorName, "USC Hoodie");
	player[id].armorModDefense = 1;
	strcpy(player[id].weapon.name, "Fight on Fingers");
	player[id].weapon.modAttack = 1;
	player[id].weapon.modTech = 1;
	player[id].typeEngineering = 0;
	player[id].typeBusiness = 0;
	player[id].typeArt = 0;
	player[id].level = 1;
	player[id].exp = 0;
	updatePlayerData(PLAYER_ID);
	player[id].curHp = player[id].baseHp;
	player[id].curMp = player[id].baseMp;
	player[id].tech[0] = 1;
	player[id].tech[1] = 2;
	player[id].tech[2] = 0;
	player[id].tech[3] = 0;
}

int printMenu(int pid, int eid)
{
	int choice;
	char stringOut[LEN_OF_DESCRIPTION];
	char trash[LEN_OF_DESCRIPTION];
	int error;
	sprintf(stringOut, "%s's HP: %d/%d\t\t%s's HP: %d/%d\n", player[pid].name, player[pid].curHp, player[pid].baseHp,
		   player[eid].name, player[eid].curHp, player[eid].baseHp);
	slowPrint(stringOut, SLOWPRINT_INTERVAL);
	sprintf(stringOut, "%s's MP: %d/%d   \t\t%s's MP: %d/%d\n", player[pid].name, player[pid].curMp, player[pid].baseMp,
		   player[eid].name, player[eid].curMp, player[eid].baseMp);
	slowPrint(stringOut, SLOWPRINT_INTERVAL);
	printf("\nSelect an option:\n");
	printf("1.  Attack\t\t3.  Inventory\n");
	printf("2.  Tech\t\t4.  Describe\n");
	printf("Your choice:  ");
	error = scanf("%d", &choice);
	if (error != 1) {
		scanf("%s", trash);
		choice = 0;
	}
	printf("\n\n");
	return choice;
}

int gameOver()
{
	int choice;
	slowPrint("You partied too hard, drank too much, and studied way too little. You fucked up college.\n\n", SLOWPRINT_INTERVAL);
	printf("1. Continue\n");
	printf("2. Transfer to UCLA\n");
	scanf("%d", &choice);
	if (choice == 1) {
		return 1;
	}
	else if (choice == 2) {
		return 2;
	}
	else {
		return 0;
	}

}

void doTech(techID, sourceID, targetID)
{
	char string[LEN_OF_DESCRIPTION];
	switch (techID) {
		case 0:
			printf("You broke it\n\n");
			break;
		case 1:
			sprintf(string, "%s used reinforce!\n", player[sourceID].name);
			slowPrint(string, SLOWPRINT_INTERVAL);
			//REINFORCE
			break;
		case 2:
			//PROTRACTOR ATTACK
			break;
		default:
			break;
	}
}

void getTechName(int techId, char *string)
{
	switch (techId) {
		case 0:
			strcpy(string, "No Tech");
			return;
		case 1:
			strcpy(string, "Reinforce");
			return;
		case 2:
			strcpy(string, "Protractor Attack");
			return;
	}
}

int mainMenu()
{
	int choice;
	printf("THE BATTLE FOR THE CAMPUS!!!\n\n");
	printf("1.  Start Game\n");
	printf("2.  Custom Battle\n\n");
	printf("Your choice:  ");
	scanf("%d", &choice);
	return choice;
}

int customBattleMenu()
{
	//stuff
	return 0;
}

int techMenu(int id)
{
	int i;
	char string[255];
	int choice;
	
	for (i = 0;i < NUM_OF_TECHS; i++){
		getTechName(player[id].tech[i], string);
		printf("%d:  %s\n", i + 1, string);
	}
	printf("5:  Go back\n");
	printf("Your choice:  ");
	scanf("%d", &choice);
	printf("\n");
	if (choice == 5){
		return -1;
	}
	else if ((choice < 1) || (choice > 5)){
		return 0;
	}
	return player[id].tech[choice - 1];
}

int mathAttack(int x)
{
	return 2 * (x) + 7;
}

int mathTech(int x)
{
	return pow(x, 1.75)/1.8 + 5;
}

int mathDefense(int x)
{
	return 2 * x;
}

int mathHp(int x)
{
	return 10 * (pow(x, 1.5));
}

int mathMp(int x)
{
	return 3 * (pow(x, 1.5));
}

int mathExpCost(int x)
{
	return 1.5 * pow(1.2, x) * pow(x, 0.6);
}

int mathExpEnemyValue(int x)
{
	return 1.5 * pow(1.2, x);
}