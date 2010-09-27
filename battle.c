/*
 TODO:
 Scale player and enemy techs
 Items should restore percentage of health
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#define NUM_OF_PLAYERS 2
#define PLAYER_ID 0
#define ENEMY_ID 1
#define SLOWPRINT_INTERVAL 18000
#define SLOWPRINT_THINKING 700000
#define LEN_OF_NAME 40
#define LEN_OF_DESCRIPTION 256
#define NUM_OF_TECHS 4
#define DO_ATTACK_ENEMY_HP 1
#define DO_ATTACK_ENEMY_MP 2
#define DO_ATTACK_PLAYER_HP 3
#define DO_ATTACK_PLAYER_MP 4
#define NUM_OF_STATUS_AILMENTS 3
#define INVENTORY_SIZE 10

//ENEMIES
#define NUM_OF_RANDOM_ENEMIES 5
#define ENEMY_BRUIN 0
#define ENEMY_STANFORD 1
#define ENEMY_BEAR 2
#define ENEMY_DUCK 3
#define ENEMY_RA 4
#define ENEMY_REDEKOPP 256


//ITEMS
#define NUM_OF_ITEMS 9
#define ITEM_EVK_FOOD 1
#define ITEM_PARKSIDE_FOOD 2
#define ITEM_PANDA_FOOD 3
#define ITEM_COFFEE 4
#define ITEM_STARBUCKS 5
#define ITEM_SHIELD 6
#define ITEM_BOOK 7
#define ITEM_SUSPICIOUS_PACKAGE 8
#define ITEM_ANTIDOTE 9

//TECHS
#define TECH_PROTRACTOR_ATTACK 1
#define TECH_REINFORCE 2
#define TECH_BREATHE 3
#define TECH_READING 4
#define TECH_GROW 5
#define TECH_FALL_OVER 6
#define TECH_HIBERNATE 7
#define TECH_BITE 8
#define TECH_QUACK 9
#define TECH_WADDLE 10
#define TECH_ICEBREAKER 11
#define TECH_WRITE_UP 12
#define TECH_CONFLICT_RESOLUTION 13
#define TECH_QUIET_HOURS 14
#define TECH_PROGRAMMING_ASSIGNMENT 15
#define TECH_LOGICAL_PUZZLE 16
#define TECH_LECTURE 17
#define TECH_MAGIC_MISSILE 18
#define TECH_THORN 19

//STATUSES
#define STATUS_HOMEWORK 1
#define STATUS_SLEEPING 2
#define STATUS_POISON 3

struct wpn {
	char name[LEN_OF_NAME];
	int modAttack;
	int modTech;
};

struct plyr {
	char name[LEN_OF_NAME];
	int baseHp; //base values are derived from the level in the math functions
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
	int tech[NUM_OF_TECHS]; //techs refer to special attacks pulled from their own menu
	
	//Cumulative data
	
	int cumAttack; //cumulative data is the sum of the base value and weapon/armor values
	int cumDefense;
	int cumTech;
	int expToNextLevel;
	
	//Live data
	
	int curHp; //current or remaining Hp
	int curMp;
	int moveHp; //moveHp and moveMp are temporary values used by techs and items
	int moveMp;
	int status[NUM_OF_STATUS_AILMENTS];
	int inventory[INVENTORY_SIZE];
	int canAttack; //checks for statuses like sleep
	
	//------- unimplemented ----------
	//Circle of weaknesses
	
	int typeEngineering; 
	int typeArt;
	int typeBusiness;
	
	//Eng beats business, business beats art, art beats engineering.  mainly just because.
	//--------------------------------
};

void loadEnemy(int, int, int);
void slowPrint(char *, int);
int rollD20();
int doAttack(int, int, int, int);
void updateDefinedEnemyData(int);
void updatePlayerData(int);
void describe(int);
void initPlayer(int);
int printMenu(int, int);
int doItem (int, int, int);
void getItemName (int, char *);
int itemMenu (int);
int mathAttack(int);
int mathDefense(int);
int mathTech(int);
int mathHp(int);
int mathMp(int);
int mathExpCost(int);
int mathExpEnemyValue(int);
void gameOver();
int mainMenu();
void loadDefaults(int);
int doBattle(int, int, int);
int techMenu(int);
int doTech(int, int, int);
void computerAttack(int, int);
int addStatus(int, int);
int statusEffect(int);
void getStatusName(int, char *);
int arraySum(int *, int);
void bubbleSort (int *, int);
int addItem(int, int);
int askYesNoQuestion(char *);
int betweenBattleMenu();
int getItemDrop(int);

struct plyr player[2];

int main(int argc, char **argv)
{
	int choice;
	int i, j;
	int lost = 0;
	int enemyChoice;
	int droppedItem;
	char string[LEN_OF_DESCRIPTION];
	char itemName[LEN_OF_NAME];
	int itemSucceed;
	
	srand((unsigned)(time(0)));  //Seed the random function
	for (i = 0; i < NUM_OF_PLAYERS; i++) {  //Load default values into both player structs
		loadDefaults(i);
	}
	
	printf("\n\n\n");
	choice = mainMenu();
	printf("\n");
	if (choice == 1) { //All this is only for the first battle
		initPlayer(PLAYER_ID);  //Load
		choice = askYesNoQuestion("You are walking along and you notice EVK is open.  Do you get some?");
		if (choice){
			slowPrint("EVK Food has been added to your inventory\n", SLOWPRINT_INTERVAL);
			addItem(1, PLAYER_ID);
		}
		else {
			slowPrint("Yeah, parkside is better anyway.\n\n", SLOWPRINT_INTERVAL);
		}
		slowPrint("\nYou're walking along just minding your own business when...\n ", SLOWPRINT_INTERVAL);
		printf("Press enter to continue ");
		fflush(stdin);
		getchar();
		printf("\n");
		updatePlayerData(PLAYER_ID);
		j = doBattle(PLAYER_ID, ENEMY_ID, 0);
		if (!j) {
			gameOver();
		}
		slowPrint("You just finished your first battle, nice job!\n\n", SLOWPRINT_INTERVAL);
		printf("Now you have the choice to either fight another random enemy or take on the\nfinal boss.\n");
		printf("I would strongly reccomend you choose to fight another random enemy for now.\n");
		while (!lost){
			choice = betweenBattleMenu();
			if (choice == 1) {
				enemyChoice = rand() % NUM_OF_RANDOM_ENEMIES;
				loadDefaults(ENEMY_ID);
				updateDefinedEnemyData(PLAYER_ID);
				j = doBattle(PLAYER_ID, ENEMY_ID, enemyChoice);
				droppedItem = getItemDrop(ENEMY_ID);
				printf("DROPPED  ITEM RETURN: %d\n", droppedItem);
				if ((droppedItem != 0) && j){
					getItemName(droppedItem, itemName);
					sprintf(string, "%s dropped %s, would you like to take it?", player[ENEMY_ID].name, itemName);
					choice = askYesNoQuestion(string);
					if (choice) {
						itemSucceed = addItem(droppedItem, PLAYER_ID);
						if (itemSucceed == 0) {
							printf("You don't have enough room in your inventory.  Discarding %s\n", itemName);
						}
						else {
							printf("%s has been added to your inventory.\n", itemName);
						}
					}
					else {
						printf("You discard %s\n", itemName);
					}
				}
				if (j == 0) {
					gameOver();
				}
			}
			else if (choice == 2){
				loadDefaults(ENEMY_ID);
				updateDefinedEnemyData(PLAYER_ID);
				j = doBattle(PLAYER_ID, ENEMY_ID, ENEMY_REDEKOPP);
				if (j == 0){
					slowPrint("You lost to Redekopp.\nPerhaps next time you should prepare for his awesomeness\n\n", SLOWPRINT_INTERVAL);
					exit(0);
				}
				else {
					slowPrint("You have beaten Redekopp.\nYou are truly king of the campus.\n", SLOWPRINT_INTERVAL);
					slowPrint("Congradulations, you have won!\n", SLOWPRINT_INTERVAL);
					exit(0);
				}

			}
		}
	}
	
	else if (choice == 2){
		printf("This isn't implemented yet, yay!!!\n");
	}
	return 0;
}

int getItemDrop(enemyID)
{
	int i;
	int numberOfItems = 0;
	int techChoice = 0;
	
	bubbleSort(player[enemyID].inventory, INVENTORY_SIZE);
	for (i = 0; i < INVENTORY_SIZE; i++) {
		if (player[enemyID].inventory[i] != 0) {
			numberOfItems++;
		}
	}
	if (numberOfItems == 0) {
		return 0;
	}
	else {
		techChoice = rand() % numberOfItems;
		return player[enemyID].inventory[techChoice];
	}

}

int betweenBattleMenu() //menu in between battles due to lack of story implementation
{
	int choice;
	
	while (1){
		printf("Between battle menuing system:\n\n");
		printf("1.  Continue to fight random enemies\n");
		printf("2.  Fight the final boss\n");
		printf("\nYour choice:  ");
		scanf("%d", &choice);
		if (choice == 1) {
			return 1;
		}
		else if (choice == 2) {
			return 2;
		}
		else {
			printf("That is not a valid option.\n\n");
		}
	}

}

void loadDefaults(int id)
{
	int i;
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
	player[id].canAttack = 1;
	for (i = 0;i < NUM_OF_STATUS_AILMENTS;i++){
		player[id].status[i] = 0;
	}
	for (i = 0; i < INVENTORY_SIZE; i++) {
		player[id].inventory[i] = 0;
	}
	for (i = 0;i < NUM_OF_TECHS; i++){
		player[id].tech[i] = 0;
	}
	player[id].typeEngineering = 0;
	player[id].typeArt = 0;
	player[id].typeBusiness = 0;
}

void loadEnemy(int globalID, int enemyID, int playerID) //loads an enemy into the global player struct
{
	switch (enemyID) {
		case ENEMY_BRUIN: //ENEMY: BRUIN
			strcpy(player[globalID].name, "Bruin");
			strcpy(player[globalID].armorName, "UCLA Sweatshirt");
			player[globalID].armorModDefense = 1;
			strcpy(player[globalID].weapon.name, "Fork");
			player[globalID].weapon.modAttack = 0;
			player[globalID].weapon.modTech = 0;
			if (player[playerID].level > 1) {
				player[globalID].level = player[playerID].level - 1;
			}
			else {
				player[globalID].level = 1;
			}
			player[globalID].expValue = mathExpEnemyValue(player[globalID].level);
			player[globalID].tech[0] = TECH_BREATHE;
			player[globalID].inventory[0] = ITEM_EVK_FOOD;
			break;
		case ENEMY_STANFORD:
			strcpy(player[globalID].name, "Stanford the Tree");
			strcpy(player[globalID].armorName, "Bark");
			player[globalID].armorModDefense = 2;
			strcpy(player[globalID].weapon.name, "Branch");
			player[globalID].weapon.modAttack = 0;
			player[globalID].weapon.modTech = 1;
			if (player[playerID].level > 1) {
				player[globalID].level = player[playerID].level - 1;
			}
			else {
				player[globalID].level = 1;
			}
			player[globalID].expValue = mathExpEnemyValue(player[globalID].level);
			player[globalID].tech[0] = TECH_GROW;
			player[globalID].tech[1] = TECH_FALL_OVER;
			player[globalID].tech[2] = TECH_THORN;
			addItem(ITEM_SHIELD, globalID);
			addItem(ITEM_ANTIDOTE, globalID);
			break;
		case ENEMY_BEAR:
			strcpy(player[globalID].name, "Golden Bear");
			strcpy(player[globalID].armorName, "Nothing but Fur");
			player[globalID].armorModDefense = 1;
			strcpy(player[globalID].weapon.name, "Claw");
			player[globalID].weapon.modAttack = 2;
			player[globalID].weapon.modTech = 0;
			if (player[playerID].level > 1) {
				player[globalID].level = player[playerID].level - 1;
			}
			else {
				player[globalID].level = 1;
			}
			player[globalID].expValue = mathExpEnemyValue(player[globalID].level);
			player[globalID].tech[0] = TECH_HIBERNATE;
			player[globalID].tech[1] = TECH_BITE;
			addItem(ITEM_BOOK, globalID);
			addItem(ITEM_PANDA_FOOD, globalID);
			player[globalID].tech[2] = TECH_REINFORCE;
			break;
		case ENEMY_DUCK:
			strcpy(player[globalID].name, "Oregon Duck");
			strcpy(player[globalID].armorName, "Useless Feathers");
			player[globalID].armorModDefense = 1;
			strcpy(player[globalID].weapon.name, "Duck Bill");
			player[globalID].weapon.modAttack = 1;
			player[globalID].weapon.modTech = 2;
			if (player[playerID].level > 1) {
				player[globalID].level = player[playerID].level - 1;
			}
			else {
				player[globalID].level = 1;
			}
			player[globalID].expValue = mathExpEnemyValue(player[globalID].level);
			player[globalID].tech[0] = TECH_QUACK;
			player[globalID].tech[1] = TECH_WADDLE;
			addItem(ITEM_COFFEE, globalID);
			addItem(ITEM_BOOK, globalID);
			player[globalID].tech[2] = TECH_BITE;
			break;
		case ENEMY_RA:
			strcpy(player[globalID].name, "RA");
			strcpy(player[globalID].armorName, "Stern Disapproval");
			player[globalID].armorModDefense = 1;
			strcpy(player[globalID].weapon.name, "Condom Bucket");
			player[globalID].weapon.modAttack = 1;
			player[globalID].weapon.modTech = 1;
			if (player[playerID].level > 1) {
				player[globalID].level = player[playerID].level - 1;
			}
			else {
				player[globalID].level = 1;
			}
			player[globalID].expValue = mathExpEnemyValue(player[globalID].level);
			player[globalID].tech[0] = TECH_ICEBREAKER;
			player[globalID].tech[1] = TECH_WRITE_UP;
			player[globalID].tech[2] = TECH_CONFLICT_RESOLUTION;
			player[globalID].tech[3] = TECH_QUIET_HOURS;
			break;
			addItem(ITEM_SUSPICIOUS_PACKAGE, globalID);
			addItem(ITEM_STARBUCKS, globalID);
			addItem(ITEM_ANTIDOTE, globalID);
		case ENEMY_REDEKOPP:
			strcpy(player[globalID].name, "Redekopp");
			strcpy(player[globalID].armorName, "PhD");
			player[globalID].armorModDefense = 5;
			strcpy(player[globalID].weapon.name, "Compiler");
			player[globalID].weapon.modAttack = 4;
			player[globalID].weapon.modTech = 9;
			player[globalID].level = 15;
			player[globalID].expValue = mathExpEnemyValue(player[globalID].level);
			player[globalID].tech[0] = TECH_PROGRAMMING_ASSIGNMENT;
			player[globalID].tech[1] = TECH_LOGICAL_PUZZLE;
			player[globalID].tech[2] = TECH_LECTURE;
			player[globalID].tech[3] = TECH_MAGIC_MISSILE;
			break;

		default:
			break;
	}
	updatePlayerData(globalID);
}

void updateDefinedEnemyData(int globalID) //Sets the cumulative attack, defense and tech without overriding the defined base values
{
	player[globalID].cumAttack = player[globalID].baseAttack + player[globalID].weapon.modAttack;
	player[globalID].cumDefense = player[globalID].baseDefense + player[globalID].armorModDefense;
	player[globalID].cumTech = player[globalID].baseTech + player[globalID].weapon.modTech;
	player[globalID].curHp = player[globalID].baseHp;  //resets the enemy's live HP to max
	player[globalID].curMp = player[globalID].baseMp;
}

void updatePlayerData(int globalID) //updates data for player progression as they level up
{
	player[globalID].baseAttack = mathAttack(player[globalID].level);
	player[globalID].baseTech = mathTech(player[globalID].level);
	player[globalID].baseDefense = mathDefense(player[globalID].level);
	player[globalID].baseHp = mathHp(player[globalID].level);
	player[globalID].baseMp = mathMp(player[globalID].level);
	player[globalID].expToNextLevel = mathExpCost(player[globalID].level);
	player[globalID].cumAttack = player[globalID].baseAttack + player[globalID].weapon.modAttack;
	player[globalID].cumDefense = player[globalID].baseDefense + player[globalID].armorModDefense;
	player[globalID].cumTech = player[globalID].baseTech + player[globalID].weapon.modTech;
	player[globalID].curHp = player[globalID].baseHp;
	player[globalID].curMp = player[globalID].baseMp;
}

int rollD20()  //Randomly roll a value between 1 and 20
{
	int result;
	result = (rand() % 20) + 1;
	return result;
}

int doAttack(int valueInput, int sourceID, int targetID, int effectType) //using this function as the modifier/randomize function, consolidating attack and tech
{
	int roll;
	double modifier;
	int returnValue;
	
	roll = rollD20();
	if ((roll >= 5) && (roll <= 16)) {
		//printf("A decent hit!\n"); ----------- these displays were used to describe the randomized values for attacks but didn't fit in for the techs
		modifier = 0.9 + (((double)roll / 110.0) * 2);
	}
	else if ((roll >= 16) && (roll <= 19)){
		//printf("A good hit!\n");
		modifier = 1.3;
	}
	else if ((roll >= 2) && (roll <= 4)){
		//printf("A weak hit!\n");
		modifier = 0.7;
	}
	else if (roll == 20){
		//slowPrint("*****  SMAAAASH!!  *****\n", SLOWPRINT_INTERVAL);
		//printf("A critical hit!\n");
		modifier = 2.0;
	}
	else if (roll == 1){
		//printf("A miss!\n");
		modifier = 0;
	} //error check with else
	
	if (effectType == 1) { //ifs apply the values based on whether the tech/attack did damage, heal, restored mp or reduced mp
	    returnValue = (int)(((double)valueInput * modifier) * ((double)(100 - player[targetID].cumDefense)/100.0));
	    player[targetID].curHp -= returnValue;
	    if (player[targetID].curHp < 0) {
			player[targetID].curHp = 0;
	    }
	}
	else if (effectType == 2) {
	    returnValue = (int)(((double)valueInput * modifier) * ((double)(100 - player[targetID].cumDefense)/100.0));
	    player[targetID].curMp -= returnValue;
	    if (player[targetID].curMp < 0) {
			player[targetID].curMp = 0;
	    }
	}
	else if (effectType == 3) { //randomized healing and mp restore were not used later because the call to this function didn't work with player[sourceID]
	    returnValue = (int)(((double)valueInput * modifier) * ((double)(100 - player[targetID].cumDefense)/100.0));
	    player[sourceID].curHp += returnValue;
	    if (player[sourceID].curHp < 0) {
			player[sourceID].curHp = 0;
	    }
	}
	else{
	    returnValue = (int)(((double)valueInput * modifier) * ((double)(100 - player[targetID].cumDefense)/100.0));
	    player[sourceID].curMp += returnValue;
	    if (player[sourceID].curHp < 0) {
			player[sourceID].curHp = 0;
	    }
		
	}
	return returnValue;
}

int statusEffect(int globalID)
{
	int i, dispel;
	char string[255];
	int returnValue = 1;
	//first 'for' loop checks for statuses and cehcks for status expiration
	for (i = 0; i < NUM_OF_STATUS_AILMENTS; i++) {
		dispel = rand()%10; //randomized status expiration
		switch(player[globalID].status[i]) {
			case 0:
				continue;
			case STATUS_HOMEWORK:
				if (dispel < 3) {
					player[globalID].status[i] =0;
					sprintf(string, "%s is no longer doing homework!\n", player[globalID].name);
					slowPrint(string, SLOWPRINT_INTERVAL);
					continue;
				}
			case STATUS_SLEEPING:
				if (dispel < 4) {
					player[globalID].status[i] =0;
					player[globalID].canAttack = 1;
					sprintf(string, "%s woke up!\n", player[globalID].name);
					slowPrint(string, SLOWPRINT_INTERVAL);
					continue;
				}
			case STATUS_POISON:
				if (dispel < 2) {
					player[globalID].status[i] =0;
					sprintf(string, "%s is no longer poisoned!\n", player[globalID].name);
					slowPrint(string, SLOWPRINT_INTERVAL);
					continue;
				}
			default:
				continue;
		}
	}
    for(i = 0; i < NUM_OF_STATUS_AILMENTS; i++) { //second 'for' loop actually applies the effects
		switch (player[globalID].status[i]) {
			case 0:
				continue;
			case STATUS_HOMEWORK:
				sprintf(string, "%s is doing homework!\n%s is losing motivation!\n\n", player[globalID].name, player[globalID].name);
				slowPrint(string, SLOWPRINT_INTERVAL);
				player[globalID].curMp --;
				continue;
			case STATUS_SLEEPING:
				sprintf(string, "%s is asleep!\n%s cannot move!!\n\n", player[globalID].name, player[globalID].name);
				slowPrint(string, SLOWPRINT_INTERVAL);
				returnValue = 0;
				continue;
			case STATUS_POISON:
				sprintf(string, "%s is poisoned!\n%s is losing hp!\n\n", player[globalID].name, player[globalID].name);
				slowPrint(string, SLOWPRINT_INTERVAL);
				player[globalID].curHp --;
				continue;
		}
	}
	return returnValue;
}


void getStatusName(int statusId, char * string) //fetches name of status without hardcoding the statuses
{
	switch (statusId) {
		case 0:
			break;
		case STATUS_HOMEWORK:
			strcpy(string, "homework");
			break;
		case STATUS_SLEEPING:
			strcpy(string, "sleep");
			break;
		case STATUS_POISON:
			strcpy(string, "posion");
			break;
		default:
			break;
	}
}

void slowPrint(char *characters, int interval) //prints text at a slower rate for dramatic effect
{
	int i;
	
	for (i = 0; i < strlen(characters); i++) {
		printf("%c", characters[i]);
		fflush(stdout);
		usleep(interval);
	}
}

void describe(enemyID) //displays basic enemy data
{
	char stringOut[255];
	sprintf(stringOut, "You are fighting a %s.  It appears to be wearing a %s and wielding a %s.\n",
			player[enemyID].name,
			player[enemyID].armorName,
			player[enemyID].weapon.name);
	slowPrint(stringOut, SLOWPRINT_INTERVAL);
	printf("\n\n");
}

int doBattle(playerID, globalEnemyID, enemyID)
{
	int choice;
	int techChoice, itemChoice;
	int damage, damageMp;
	//int recoveredMp, heal;
	//int i;
	char string[LEN_OF_DESCRIPTION];
	
	loadEnemy(globalEnemyID, enemyID, playerID);
	sprintf(string, "A wild %s appeared!!\n\n", player[globalEnemyID].name);
	slowPrint(string, SLOWPRINT_INTERVAL);
	while (1) {
		player[playerID].moveMp = 0;
		player[playerID].moveHp = 0;
		player[globalEnemyID].moveHp = 0;
		player[globalEnemyID].moveMp = 0;
		if (player[playerID].canAttack){
			choice = printMenu(playerID, globalEnemyID);
			if ((choice > 4) || (choice < 1)) {
				printf("That is not a valid option.\n\n");
			}
			switch (choice) {
				case 1:
					//ATTACK
					damage = doAttack(player[playerID].cumAttack,playerID, globalEnemyID, 1);
					sprintf(string, "You did %d damage!!\n\n", damage);
					slowPrint(string, SLOWPRINT_INTERVAL);
					break;
				case 2:
					techChoice = techMenu(playerID);  //Implement error checking!!!!
					if (techChoice == -1) {
						continue;
					}
					else if (techChoice == 0){
						slowPrint("No tech selected\n", SLOWPRINT_INTERVAL);
						continue;
					}
					else {
						if(doTech(techChoice, playerID, globalEnemyID) == 0) {
							slowPrint("Not enough motivation!\n\n", SLOWPRINT_INTERVAL);
							continue;
						}
						else {
							
							if (player[playerID].moveHp == 0) {
								break;
							}
							else {
								damage = doAttack(player[playerID].moveHp, playerID, globalEnemyID, 1); //calculate damage done to enemy
								sprintf(string, "You did %d damage!!\n", damage);
								slowPrint(string, SLOWPRINT_INTERVAL);
							}
							
							if (player[playerID].moveMp == 0) {
								break;
							}
							else {
								damageMp = doAttack(player[playerID].moveMp, playerID, globalEnemyID, 2);
								sprintf(string, "%s lost %d mp!\n",player[globalEnemyID].name, damageMp);
								slowPrint(string, SLOWPRINT_INTERVAL);
							}
						}	
					}
					break;
				case 3:
					itemChoice = itemMenu(playerID);  //Implement error checking!!!!
					if (itemChoice == -1) {
						continue;
					}
					else if (itemChoice == 0){
						slowPrint("No item selected\n\n", SLOWPRINT_INTERVAL);
						continue;
					}
					else {
						doItem(itemChoice - 1, playerID, globalEnemyID);
						
					}
					break;
				case 4:
					//DESCRIBE
					describe(globalEnemyID);
					continue;
				default:
					slowPrint("That is not a valid option\n\n", SLOWPRINT_INTERVAL);
					continue;
					//break;
			}
		}
		if (player[globalEnemyID].curHp == 0) {
			sprintf(string, "You beat %s!!\n", player[globalEnemyID].name);
			slowPrint(string, SLOWPRINT_INTERVAL);
			player[PLAYER_ID].exp += player[ENEMY_ID].expValue;
			while (player[PLAYER_ID].exp >= player[PLAYER_ID].expToNextLevel) {
				player[PLAYER_ID].level++;
				sprintf(string, "You leveled up!!!\nYou are now level %d!!!\n\n", player[PLAYER_ID].level);
				slowPrint(string, SLOWPRINT_INTERVAL);

				printf("Your attack went up by %d!\n", mathAttack(player[PLAYER_ID].level) - mathAttack(player[PLAYER_ID].level - 1));
				printf("Your defense went up by %d!\n", mathDefense(player[PLAYER_ID].level) - mathDefense(player[PLAYER_ID].level - 1));
				printf("Your tech went up by %d!\n", mathTech(player[PLAYER_ID].level) - mathTech(player[PLAYER_ID].level - 1));
				printf("Your HP went up by %d!\n", mathHp(player[PLAYER_ID].level) - mathHp(player[PLAYER_ID].level - 1));
				printf("Your MP went up by %d!\n", mathMp(player[PLAYER_ID].level) - mathMp(player[PLAYER_ID].level - 1));

				player[PLAYER_ID].exp -= player[PLAYER_ID].expToNextLevel;
				updatePlayerData(PLAYER_ID);
			}
			return 1;
		}
		statusEffect(playerID);
		if (player[globalEnemyID].canAttack == 1){
			computerAttack(globalEnemyID, playerID);
			printf("\n");
		}
		if (player[playerID].curHp == 0) {
			sprintf(string, "You lost to %s!!\n", player[globalEnemyID].name);
			slowPrint(string, SLOWPRINT_INTERVAL);
			return 0;
		}
		statusEffect(globalEnemyID);
	}//THIS IS AN ERROR!!!
}

void initPlayer(int id) //initializes basic player data and prints initial story text
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
		printf("\nUPRCLSMAN:  Your name is %s, is that correct?  [y/n] ", player[id].name);
		getchar();
		scanf("%c", &yesNo);
		if (yesNo == 'y'){
			nameRight = 1;
		}
		else {
			printf("Oops, let's try that again\n");
		}
	}
	printf("\nUPRCLSMAN:  Great, here are your keys to %s.  Enjoy your first day!\n\n", room);
	
	//LOAD UP DEFAULT.  WILL CHANGE WITH STORY LATER!!!
	//
	//
	
	player[id].level = 1;
	strcpy(player[id].armorName, "USC Hoodie");
	player[id].armorModDefense = 1;
	strcpy(player[id].weapon.name, "Fight on Fingers");
	player[id].weapon.modAttack = 1;
	player[id].weapon.modTech = 1;
	player[id].typeEngineering = 0;
	player[id].typeBusiness = 0;
	player[id].typeArt = 0;
	player[id].exp = 0;
	updatePlayerData(PLAYER_ID);
	player[id].curHp = player[id].baseHp;
	player[id].curMp = player[id].baseMp;
	player[id].tech[0] = TECH_PROTRACTOR_ATTACK;
	player[id].tech[1] = TECH_REINFORCE;
}

int printMenu(int pid, int eid) //prints battle menu
{
	int choice;
	//char stringOut[LEN_OF_DESCRIPTION];
	char trash[LEN_OF_DESCRIPTION];
	int error;
	printf("%s's HP: %d/%d\t\t%s's HP: %d/%d\n", player[pid].name, player[pid].curHp, player[pid].baseHp, //displays curent values of player and enemy
			player[eid].name, player[eid].curHp, player[eid].baseHp);
	printf("%s's MP: %d/%d\t\t%s's MP: %d/%d\n", player[pid].name, player[pid].curMp, player[pid].baseMp,
			player[eid].name, player[eid].curMp, player[eid].baseMp);
	printf("\nSelect an option:\n"); //then reads player's action
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

void gameOver() //prints game over screen when player loses
{
	int choice;
	slowPrint("You've lost, nice going.\n\n", SLOWPRINT_INTERVAL);
	printf("1. Quit\n");
	printf("2. Transfer to UCLA\n");
	scanf("%d", &choice);
	if (choice == 1) {
		printf("Good luck on your future endeavors\n");
		exit(0);
	}
	else if (choice == 2) {
		printf("You failed even harder\n>:( RAWR\n");
		exit(0);
	}
	else {
		printf("You can't even work a menu prompt");
		exit(0);
	}
	
}

int doTech(techID, sourceID, targetID) //loads up tech values to temporary variables to be applied in doAttack, this function also defines specifically what each tech does
{
	char string[LEN_OF_DESCRIPTION];
	switch (techID) {
		case 0:
			printf("You broke it\n\n");
			return 0;
		case TECH_PROTRACTOR_ATTACK:
			if (player[sourceID].curMp >= 6) { //checks if player has adequate mp left
				sprintf(string, "%s used Protractor Attack!\n", player[sourceID].name);
		        slowPrint(string, SLOWPRINT_INTERVAL); //displays the effects
				player[sourceID].moveHp = player[sourceID].cumTech * 0.9;  //loads temp values
				player[sourceID].moveMp = player[sourceID].cumTech * 0.2;
				player[sourceID].curMp -= 6; //applies mp cost
				return 1; //the if statement checks if the player has enough mp to use the tech and returns 1 for success
			}
			else {
				return 0;
			}
			
		case TECH_REINFORCE:
			if(player[sourceID].curMp > 2) {
		        sprintf(string, "%s used Reinforce!\n", player[sourceID].name);
				slowPrint(string, SLOWPRINT_INTERVAL);
				player[sourceID].cumDefense += player[sourceID].cumTech * 0.1;
				sprintf(string, "%s increased defense!\n", player[sourceID].name);
				slowPrint(string, SLOWPRINT_INTERVAL);
				player[sourceID].curMp -= 2;
				return 1;
			}
			else {
				return 0;
			}
			
		case TECH_BREATHE:
			if (player[sourceID].curMp >= 29) {
				player[sourceID].curMp -= 29;
				sprintf(string, "%s used Poison!\n", player[sourceID].name);
				slowPrint(string, SLOWPRINT_INTERVAL);
				if((rand() % 10) < 8) {
					sprintf(string, " %s was poisoned!\n", player[targetID].name);
					slowPrint(string, SLOWPRINT_INTERVAL);
					addStatus(3, targetID);
				}
				else {
					slowPrint(" It wasn't very effective!\n", SLOWPRINT_INTERVAL);
				}
				
				return 1;
			}
			else {
				return 0;
			}
		case TECH_READING:
			if (player[sourceID].curMp >= 45) {
				sprintf(string, "%s gave Reading Assignment!\n%s is asleep!\n", player[sourceID].name, player[targetID].name);
				slowPrint(string, SLOWPRINT_INTERVAL);
				addStatus(STATUS_SLEEPING, targetID);
				player[targetID].canAttack = 0;
				player[sourceID].curMp -= 45;
				return 1;
			}
			else {
				return 0;
			}
		case TECH_GROW:
			if (player[sourceID].curMp >= 2) {
				sprintf(string, "%s used Grow!\n", player[sourceID].name);
				slowPrint(string, SLOWPRINT_INTERVAL);
				player[sourceID].cumDefense += player[sourceID].cumTech * 0.1;
				sprintf(string, "%s increased defense!\n", player[sourceID].name);
				slowPrint(string, SLOWPRINT_INTERVAL);
				player[sourceID].curMp -= 2;
				return 1;
			}
			else {
				return 0;
			}
		case TECH_THORN:
			if (player[sourceID].curMp >= 20) {
				player[sourceID].curMp -= 20;
				sprintf(string, "%s used Thorn!\n", player[sourceID].name);
				slowPrint(string, SLOWPRINT_INTERVAL);
				if((rand() % 10) < 8) {
					sprintf(string, " %s was poisoned!!\n", player[targetID].name);
					slowPrint(string, SLOWPRINT_INTERVAL);
					addStatus(3, targetID);
				}
				else {
					slowPrint(" It wasn't very effective!\n", SLOWPRINT_INTERVAL);
				}
			}
		case TECH_FALL_OVER:
			if (player[sourceID].curMp >= 63) {
				sprintf(string, "%s used Fall Over!\n", player[sourceID].name);
				slowPrint(string, SLOWPRINT_INTERVAL);
				player[sourceID].moveHp = player[sourceID].cumTech * 3;
				sprintf(string, "%s fell over!\n", player[sourceID].name);
				slowPrint(string, SLOWPRINT_INTERVAL);
				player[sourceID].curHp = 0;
				player[sourceID].curMp -= 63;
				return 1;
			}
			else {
				return 0;
			}
		case TECH_HIBERNATE:
			if (player[sourceID].curMp >= 37) {
				sprintf(string, "%s used Hibernate!\n%s regained health and became motivated!\n%s is asleep!!\n", player[sourceID].name,
						player[sourceID].name, player[sourceID].name);
				slowPrint(string, SLOWPRINT_INTERVAL);
				addStatus(STATUS_SLEEPING, sourceID);
				player[sourceID].canAttack = 0;
				player[sourceID].curHp += player[sourceID].cumTech * 2;
				if (player[sourceID].curHp > player[sourceID].baseHp) {
					player[sourceID].curHp = player[sourceID].baseHp;
				}
				player[sourceID].curMp += player[sourceID].cumTech;
				if (player[sourceID].curMp > player[sourceID].baseMp) {
					player[sourceID].curMp = player[sourceID].baseMp;
				}
				return 1;
			}
			else {
				return 0;
			}
		case TECH_BITE:
			if (player[sourceID].curMp >= 16) {
				sprintf(string, "%s used Bite!\n%s regained health!\n", player[sourceID].name, player[sourceID].name);
				slowPrint(string, SLOWPRINT_INTERVAL);
				player[sourceID].moveHp = player[sourceID].cumTech * 0.7;
				player[sourceID].curHp += player[sourceID].cumTech * 0.4;
				if (player[sourceID].curHp > player[sourceID].baseHp) {
					player[sourceID].curHp = player[sourceID].baseHp;
				}
				player[sourceID].curMp -= 16;
				return 1;
			}
			else {
				return 0;
			}
		case TECH_QUACK:
			if (player[sourceID].curMp >= 2) {
				sprintf(string, "%s used Quack!\n", player[sourceID].name);
				slowPrint(string, SLOWPRINT_INTERVAL);
				player[sourceID].moveHp = player[sourceID].cumTech * 0.6;
				player[sourceID].moveMp = player[sourceID].cumTech * 0.2;
				player[sourceID].curMp -= 2;
				return 1;
			}
			else {
				return 0;
			}
		case TECH_WADDLE:
			sprintf(string, "%s used Waddle!\n", player[sourceID].name);
			slowPrint(string, SLOWPRINT_INTERVAL);
			usleep(SLOWPRINT_THINKING);
			slowPrint("It did nothing!\n", SLOWPRINT_INTERVAL);
			return 1;
		case TECH_ICEBREAKER:
			if (player[sourceID].curMp >= 2) {
				sprintf(string, "%s used Icebreaker!\n", player[sourceID].name);
				slowPrint(string, SLOWPRINT_INTERVAL);
				player[sourceID].moveHp = player[sourceID].cumTech * 0.3;
				player[sourceID].moveMp = player[sourceID].cumTech;
				player[sourceID].curMp -= 2;
				return 1;
			}
			else {
				return 0;
			}
		case TECH_WRITE_UP:
			if (player[sourceID].curMp >= 22) {
				sprintf(string, "%s used Write Up!\nIt reduced %s's defence!\n", player[sourceID].name, player[targetID].name);
				slowPrint(string, SLOWPRINT_INTERVAL);
				player[targetID].cumDefense -= player[sourceID].cumTech * 0.1;
				player[sourceID].moveHp = player[sourceID].cumTech * 2;
				player[sourceID].curMp -= 22;
				return 1;
			}
			else {
				return 0;
			}
		case TECH_CONFLICT_RESOLUTION:
			if (player[sourceID].curMp >= 45) {
				sprintf(string, "%s used Conflict Resolution!\n%s increased stats!\n", player[sourceID].name, player[sourceID].name);
				slowPrint(string, SLOWPRINT_INTERVAL);
				player[sourceID].cumDefense += player[sourceID].cumTech * 0.1;
				player[sourceID].cumAttack += player[sourceID].cumTech * 0.1;
				player[sourceID].cumTech += player[sourceID].cumTech * 0.1;
				player[sourceID].moveMp = player[sourceID].cumTech * 10;
				player[sourceID].curMp -= 45;
				return 1;
			}
			else {
				return 0;
			}
		case TECH_QUIET_HOURS:
			if (player[sourceID].curMp >= 16) {
				sprintf(string, "%s enforced Quiet Hours!\n%s is asleep!\n", player[sourceID].name, player[targetID].name);
				slowPrint(string, SLOWPRINT_INTERVAL);
				addStatus(STATUS_SLEEPING, targetID);
				player[targetID].canAttack = 0;
				player[sourceID].moveHp = player[sourceID].cumTech * 0.4;
				player[sourceID].moveMp = player[sourceID].cumTech * 0.4;
				player[sourceID].curMp -= 16;
				return 1;
			}
			else {
				return 0;
			}
		case TECH_PROGRAMMING_ASSIGNMENT:
			if (player[sourceID].curMp >= 7) {
				sprintf(string, "%s gave Programming Assignment!!\n", player[sourceID].name);
				slowPrint(string, SLOWPRINT_INTERVAL);
				addStatus(STATUS_HOMEWORK, targetID);
				player[sourceID].moveHp = player[sourceID].cumTech * 0.4;
				player[sourceID].moveMp = player[sourceID].cumTech * 0.6;
				player[sourceID].curMp -= 7;
				return 1;
			}
			else {
				return 0;
			}
		case TECH_LOGICAL_PUZZLE:
			if (player[sourceID].curMp >= 0) {
				sprintf(string, "%s used Logic Puzzle!!\n%s regained motivation!!\n", player[sourceID].name, player[sourceID].name);
				slowPrint(string, SLOWPRINT_INTERVAL);
				player[sourceID].moveHp = player[sourceID].cumTech * 0.4;
				player[sourceID].curMp += 30;
				if (player[sourceID].curMp > player[sourceID].baseHp)	{
					player[sourceID].curMp = player[sourceID].baseHp;
				}
				return 1;
			}
			else {
				return 0;
			}
		case TECH_LECTURE:
			if (player[sourceID].curMp >= 18) {
				sprintf(string, "%s used Lecture!!\n", player[sourceID].name);
				slowPrint(string, SLOWPRINT_INTERVAL);
				addStatus(STATUS_SLEEPING, targetID);
				player[targetID].canAttack = 0;
				player[sourceID].moveMp = player[sourceID].cumTech * 0.4;
				player[sourceID].curMp -= 12;
				return 1;
			}
			else {
				return 0;
			}
		case TECH_MAGIC_MISSILE:
			if (player[sourceID].curMp >= 50) {
				sprintf(string, "%s casts Magic Missile!!\n\n!!!MASSIVE DAMAGE!!!!!\n", player[sourceID].name);
				slowPrint(string, 1.5 * SLOWPRINT_INTERVAL);
				player[targetID].curHp = 1 + (rand() % player[targetID].level);
				player[sourceID].curMp -= 50;
				return 1;
			}
			else {
				return 0;
			}
			
			break;
			
		default:
			return 0;
	}
}

void getTechName(int techId, char *string) //fetches name of techs without hardcoding the techs
{
	switch (techId) {
		case 0:
			strcpy(string, "No Tech");
			return;
		case TECH_PROTRACTOR_ATTACK:
			strcpy(string, "Protractor Attack");
			return;
		case TECH_REINFORCE:
			strcpy(string, "Reinforce");
			return;
		case TECH_BREATHE:
			strcpy(string, "Poison");
			break;
		case TECH_READING:
			strcpy(string, "Reading Assignment");
			break;
			
	}
}

int mainMenu() //prints beginning text and options
{
	int choice;
	printf("THE BATTLE FOR THE CAMPUS!!!\n\n");
	printf("1.  Start Game\n");
	printf("2.  Custom Battle\n\n");
	printf("Your choice:  ");
	scanf("%d", &choice);
	return choice;
}

int customBattleMenu() //custom battles are still unimplemented
{
	//stuff
	return 0;
}

int techMenu(int id) //displays a modular menu of techs that retrieves the names of the techs from getTechName
{
	int i;
	char string[255];
	int choice;
	
	for (i = 0;i < NUM_OF_TECHS; i++){
		getTechName(player[id].tech[i], string);
		printf("%d:\t%s\n", i + 1, string);
	}
	printf("5:\tGo back\n");
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

void computerAttack(int computerID, int targetID)
{
	double randValue;
	double chanceOfTech = 0.3;
	double chanceOfItem = 0.1;
	//double chanceOfAttack = 0.6;
	int damage;
	char string[LEN_OF_DESCRIPTION];
	int i;
	int numOfTechs = 0;
	int numOfItems = 0;
	int choice;
	int techSucceed;
	int itemSucceed;
	
	randValue = (double)rand() / (double)RAND_MAX;
	sprintf(string, "\n%s is thinking", player[computerID].name);
	slowPrint(string, SLOWPRINT_INTERVAL);
	for (i = 0; i < 3; i++) {
		slowPrint(".", SLOWPRINT_THINKING);
	}
	printf("\n");
	
	if (randValue <= chanceOfTech){
		//do a tech
		for (i = 0; i < NUM_OF_TECHS; i++) {
			if (player[computerID].tech[i] != 0) {
				numOfTechs++;
			}
		}
		if (numOfTechs > 0) {
			choice = rand() % numOfTechs;
			techSucceed = doTech(player[computerID].tech[choice], computerID, targetID);
		}
		if ((numOfTechs <= 0) || (!techSucceed)) {
			printf("%s is attacking!!!\n", player[computerID].name);
			damage = doAttack(player[computerID].cumAttack, computerID, targetID, DO_ATTACK_ENEMY_HP);
			printf("%s did %d damage!!!\n", player[computerID].name, damage);
		}
	}
	else if (randValue <= (chanceOfTech + chanceOfItem)){
		bubbleSort(player[computerID].inventory, INVENTORY_SIZE);
		for (i = 0; i < INVENTORY_SIZE; i++) {
			if (player[computerID].inventory[i] != 0) {
				numOfItems++;
			}
		}
		if (numOfItems > 0) {
			choice = rand() % numOfItems;
			itemSucceed = doItem(player[computerID].inventory[choice], computerID, targetID);
		}
		if ((numOfItems <= 0) || (!itemSucceed)) {
			printf("%s is attacking!!!\n", player[computerID].name);
			damage = doAttack(player[computerID].cumAttack, computerID, targetID, DO_ATTACK_ENEMY_HP);
			printf("%s did %d damage!!!\n", player[computerID].name, damage);
		}
	}
	
	else {
		//do an attack
		printf("%s is attacking!!!\n", player[computerID].name);
		damage = doAttack(player[computerID].cumAttack, computerID, targetID, DO_ATTACK_ENEMY_HP);
		printf("%s did %d damage!!!\n", player[computerID].name, damage);
	}
	
}

int addStatus(int statusID, int playerID) //applies statuses onto a player while checking that the status is not already there and there are not too many statuses
{
	int i;
	
	for (i = 0; i < NUM_OF_STATUS_AILMENTS; i++) {
		if (player[playerID].status[i] == statusID) {
			return -1;
		}
	}
	for (i = 0; i < NUM_OF_STATUS_AILMENTS; i++) {
		if (player[playerID].status[i] == 0) {
			player[playerID].status[i] = statusID;
			return 0;
		}
	}
	return 1; //Returns a zero for success, a 1 if the status array is full, and a -1 if the status is already there
}

int addItem(int itemID, int playerID) //sorts inventory then adds item to next empty slot
{
	int i;
	bubbleSort(player[playerID].inventory, INVENTORY_SIZE);
	for (i = 0; i < INVENTORY_SIZE; i++) {
		if (player[playerID].inventory[i] == 0) {
			player[playerID].inventory[i] = itemID;
			return 1;
		}
	}
	return 0;
}

int itemMenu(int id) //displays a modular menu of items that retrieves the names of the items from getTechName
{
	int i;
	char string[255];
	int choice;
	
	bubbleSort(player[id].inventory, INVENTORY_SIZE);
	
	for (i = 0; i < INVENTORY_SIZE; i++) {
		getItemName(player[id].inventory[i], string);
		printf("%d:\t%s\n", i + 1, string);
	}
	printf("11:\tGo back\n");
	printf("Your choice: ");
	scanf("%d", &choice);
	printf("\n");
	if (choice == 11){
		return -1;
	}
	else if ((choice < 1) || (choice > 11) || (player[id].inventory[choice - 1] == 0)){
		return 0;
	}
	return choice;
}


void getItemName(int itemID, char *name) //fetches name of items without hardcoding the statuses
{
	switch (itemID) {
		case 0:
			strcpy(name, "No Item");
			return;
		case ITEM_EVK_FOOD:
			strcpy(name, "EVK Food");
			return;
		case ITEM_PARKSIDE_FOOD:
			strcpy(name, "Parkside Food");
			return;
		case ITEM_PANDA_FOOD:
			strcpy(name, "Panda Food");
			return;
		case ITEM_COFFEE:
			strcpy(name, "Coffee");
			return;
		case ITEM_STARBUCKS:
			strcpy(name, "Starbucks");
			return;
		case ITEM_SHIELD:
			strcpy(name, "Shield");
			return;
		case ITEM_BOOK:
			strcpy(name, "Book");
			return;
		case ITEM_SUSPICIOUS_PACKAGE:
			strcpy(name, "Suspicious Package");
			return;
		case ITEM_ANTIDOTE:
			strcpy(name, "Antidote");
			return;
		default:
			break;
	}
}

int doItem(int choice, int sourceID, int targetID) //loads item stats into temp variables and then applies effects
{
	char string[LEN_OF_DESCRIPTION];
	int effectType[4];
	int itemID = player[sourceID].inventory[choice];
	int	 i;
	for (i = 0; i < 4; i++) {
		effectType[i] = 0; //effectType allows one item to have multiple effects and still print results effeciently
	}
	switch (itemID) { //prints message and loads into temp variables
		case 0:
			printf("You broke it\n\n");
			return 0;
		case ITEM_EVK_FOOD:
			sprintf(string, "%s ate EVK food!\n", player[sourceID].name);
			slowPrint(string, SLOWPRINT_INTERVAL);
			
			if (rollD20() < 2) {
				addStatus(3, sourceID);
				sprintf(string, "%s was poisoned!\n", player[sourceID].name);
				slowPrint(string, SLOWPRINT_INTERVAL);
				break;
			}
			else {
				player[sourceID].moveHp = ((double)(player[sourceID].baseHp) * 0.25);
				effectType[2] = 1;
			}
			break;
		case ITEM_PARKSIDE_FOOD:
			sprintf(string, "%s ate Parkside food!\n", player[sourceID].name);
			slowPrint(string, SLOWPRINT_INTERVAL);
			player[sourceID].moveHp = ((double)(player[sourceID].baseHp) * 0.5);
			effectType[2] = 1;
			break;
		case ITEM_PANDA_FOOD:
			sprintf(string, "%s ate Panda food!\n", player[sourceID].name);
			slowPrint(string, SLOWPRINT_INTERVAL);
			player[sourceID].moveHp = ((double)(player[sourceID].baseHp) * 0.75);
			effectType[2] = 1;
			break;
		case ITEM_COFFEE:
			sprintf(string, "%s drank coffee!\n", player[sourceID].name);
			slowPrint(string, SLOWPRINT_INTERVAL);
			player[sourceID].moveMp = ((double)(player[sourceID].baseMp) * 0.25);
			effectType[3] = 1;
			break;
		case ITEM_STARBUCKS:
			sprintf(string, "%s drank Starbucks!\n", player[sourceID].name);
			slowPrint(string, SLOWPRINT_INTERVAL);
			player[sourceID].moveMp = ((double)(player[sourceID].baseMp) * 0.5);
			effectType[3] = 1;
			player[sourceID].moveHp = ((double)(player[sourceID].baseHp) * 0.5);
			effectType[2] = 1;
			break;
		case ITEM_SHIELD:
			sprintf(string, "%s used a shield!\n", player[sourceID].name);
			slowPrint(string, SLOWPRINT_INTERVAL);
			sprintf(string, "%s's defense has been increased!\n", player[sourceID].name);
			slowPrint(string, SLOWPRINT_INTERVAL);
			player[sourceID].cumDefense += 15;
			break;
		case ITEM_BOOK:
			sprintf(string, "%s threw a book at %s!\n", player[sourceID].name, player[targetID].name);
			slowPrint(string, SLOWPRINT_INTERVAL);
			slowPrint("..... it really didn't do much!\n", SLOWPRINT_INTERVAL);
			break;
		case ITEM_SUSPICIOUS_PACKAGE:
			sprintf(string, "%s used Suspicious Package!\n", player[sourceID].name);
			slowPrint(string, SLOWPRINT_INTERVAL);
			slowPrint("..... it was just a magazine with a blinking light...\n", SLOWPRINT_INTERVAL);
			break;
		case ITEM_ANTIDOTE:
			sprintf(string, "%s used Antidote!\n", player[sourceID].name);
			slowPrint(string, SLOWPRINT_INTERVAL);
			for (i = 0; i < NUM_OF_STATUS_AILMENTS; i++) {
				if (player[sourceID].status[i] == STATUS_POISON) {
					player[sourceID].status[i] = 0;
					printf("%s was cured of his/her poisoning!\n", player[sourceID].name);
				}
			}
			break;

		default:
			return 0;
	}
	//applies effects and prints results
	if (effectType[0] == 1) { //damage enemy
		player[targetID].curHp -= player[targetID].moveHp;
		printf("%s lost %d hp!\n", player[targetID].name, player[targetID].moveHp);
		if (player[targetID].curHp < 0) {
			player[targetID].curHp = 0;
		}
	}
	if (effectType[1] == 1) { //damage enemy mp
		player[targetID].curMp -= player[targetID].moveMp;
		printf("%s lost %d mp!\n", player[targetID].name, player[targetID].moveMp);
		if (player[targetID].curMp < 0) {
			player[targetID].curMp = 0;
		}
	}
	if (effectType[2] == 1) { //heal
		player[sourceID].curHp += player[sourceID].moveHp;
		printf("%s gained %d hp!\n", player[sourceID].name, player[sourceID].moveHp);
		if (player[sourceID].curHp > player[sourceID].baseHp) {
			player[sourceID].curHp = player[sourceID].baseHp;
		}
	}
	if (effectType[3] == 1){ //restore mp
		player[sourceID].curMp += player[sourceID].moveMp;
		printf("%s gained %d mp!\n", player[sourceID].name, player[sourceID].moveMp);
		if (player[sourceID].curMp > player[sourceID].baseMp) {
			player[sourceID].curMp = player[sourceID].baseMp;
		}
	}
	if (arraySum(effectType, 4) == 0) {
		printf("No effect!\n");
	}
	player[sourceID].inventory[choice] = 0;
	return 1;
}

void bubbleSort(int *a, int n) //sorts inventory from largest to smallest so all the blank slots are at the end
{
	int i;
	int change = 1;
	int temp;
	
	while (change){
		change = 0;
		for (i = 0; i < (n - 1); i++) {
			if (a[i] < a[i + 1]) {
				change = 1;
				temp = a[i];
				a[i] = a[i + 1];
				a[i + 1] = temp;
			}
		}
	}
}

int askYesNoQuestion(char *question) //function to check for choice in the story
{
	char choice;
	int error;
	
	while (1){
		printf("%s", question);
		printf("\n");
		printf("Your choice [y/n]:  ");
		fflush(stdin);
		error = scanf("%c", &choice);
		if (choice == 'y'){
			return 1;
		}
		else if (choice == 'n'){
			return 0;
		}
		else {
			printf("That is not a valid answer.  Please type either y or n.\n\n");
		}
	}
}

	
//functions used to caluclate base values based on level
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
	return 15 * pow(1.2, x) * pow(x, 0.6);
}

int mathExpEnemyValue(int x)
{
	return 15 * pow(1.2, x);
}

int arraySum(int *array, int arrayLength)
{
	int i, sum;
	for (i = 0; i < arrayLength; i++) {
		sum += array[i];
	}
	return sum;
}
