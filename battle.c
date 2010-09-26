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
#define DO_ATTACK_ENEMY_HP 1
#define DO_ATTACK_ENEMY_MP 2
#define DO_ATTACK_PLAYER_HP 3
#define DO_ATTACK_PLAYER_MP 4
#define NUM_OF_STATUS_AILMENTS 3
#define INVENTORY_SIZE 10

struct wpn {
	char name[LEN_OF_NAME];
	int modAttack;
	int modTech;
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
	int moveHp;
	int moveMp;
	int status[NUM_OF_STATUS_AILMENTS];
	int inventory[INVENTORY_SIZE];
	
	//Circle of weaknesses
	
	int typeEngineering;
	int typeArt;
	int typeBusiness;
	
	//Eng beats business, business beats art, art beats engineering.  mainly just because.
};

void loadEnemy(int, int);
void slowPrint(char *, int);
int rollD20();
int doAttack(int, int, int, int);
void updateDefinedEnemyData(int);
void updatePlayerData(int);
void describe(int);
void initPlayer(int);
int printMenu(int, int);
void doItem (int, int, int);
void getItemName (int, char *);
int itemMenu (int);
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
int doBattle(int, int, int);
int techMenu(int);
int doTech(int, int, int);
void computerAttack(int, int);
int addStatus(int, int);
int statusEffect(int);
void getStatusName(int, char *);
int arraySum(int *, int);

struct plyr player[2];

int main(int argc, char **argv)
{
	int choice;
	int i, j;
	//char string[LEN_OF_DESCRIPTION];
	
	srand((unsigned)(time(0)));  //Seed the random function
	for (i = 0; i < NUM_OF_PLAYERS; i++) {  //Load default values into both player structs
		loadDefaults(i);
	}
	choice = mainMenu();
	if (choice == 1) {
		initPlayer(PLAYER_ID);  //Load
		//TEMP
		for (i = 0;i < 5;i++){
			player[PLAYER_ID].inventory[i] = i + 1;
		}
		player[PLAYER_ID].inventory[5] = 5;
		//TEMP
		updatePlayerData(PLAYER_ID);
		j = doBattle(PLAYER_ID, ENEMY_ID, 0);
		if (j == 0) {
			gameOver();
			return 0;
		}
		else {
			gameOver();
		}
		
	}
	
	else if (choice == 2){
		printf("This isn't implemented yet, yay!!!\n");
	}
	return 0;
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

void loadEnemy(int globalID, int enemyID)
{
	switch (enemyID) {
		case 0: //ENEMY: BRUIN
			strcpy(player[globalID].name, "Bruin");
			player[globalID].baseHp = 15;
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
			player[globalID].expValue = 18;
			player[globalID].tech[0] = 3;
			break;
		case 1:
			//do another
			break;
		default:
			break;
	}
	updateDefinedEnemyData(globalID);  //Sets the cumulative attack, defense and tech to the base plus the weapon and armor mods
}

void updateDefinedEnemyData(int globalID)
{
	player[globalID].cumAttack = player[globalID].baseAttack + player[globalID].weapon.modAttack;
	player[globalID].cumDefense = player[globalID].baseDefense + player[globalID].armorModDefense;
	player[globalID].cumTech = player[globalID].baseTech + player[globalID].weapon.modTech;
	player[globalID].curHp = player[globalID].baseHp;  //Sets the enemy's live HP to max
	player[globalID].curMp = player[globalID].baseMp;
}

void updatePlayerData(int globalID)
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
		//printf("A decent hit!\n");
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
	
	if (effectType == 1) {
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
	else if (effectType == 3) {
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
	
	for (i = 0; i < NUM_OF_STATUS_AILMENTS; i++) {
		dispel = rand()%10;
		switch(player[globalID].status[i]) {
			case 0:
				continue;
			case 1:
				if (dispel < 3) {
					player[globalID].status[i] =0;
					sprintf(string, "%s is no longer doing homework!\n", player[globalID].name);
					slowPrint(string, SLOWPRINT_INTERVAL);
					continue;
				}
			case 2:
				if (dispel < 4) {
					player[globalID].status[i] =0;
					sprintf(string, "%s woke up!\n", player[globalID].name);
					slowPrint(string, SLOWPRINT_INTERVAL);
					continue;
				}
			case 3:
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
    for(i = 0; i < NUM_OF_STATUS_AILMENTS; i++) {
		switch (player[globalID].status[i]) {
			case 0:
				continue;
			case 1:
				sprintf(string, "%s is doing homework!\n%s is losing motivation!\n\n", player[globalID].name, player[globalID].name);
				slowPrint(string, SLOWPRINT_INTERVAL);
				player[globalID].curMp --;
				continue;
			case 2:
				sprintf(string, "%s is asleep!\n%s cannot attack!!\n\n", player[globalID].name, player[globalID].name);
				slowPrint(string, SLOWPRINT_INTERVAL);
				returnValue = 0;
				continue;
			case 3:
				sprintf(string, "%s is poisoned!\n%s is losing hp!\n\n", player[globalID].name, player[globalID].name);
				slowPrint(string, SLOWPRINT_INTERVAL);
				player[globalID].curHp --;
				continue;
		}
	}
	return returnValue;
}


void getStatusName(int statusId, char * string)
{
	switch (statusId) {
		case 0:
			break;
		case 1:
			strcpy(string, "homework");
			break;
		case 2:
			strcpy(string, "sleep");
			break;
		case 3:
			strcpy(string, "posion");
			break;
		default:
			break;
	}
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

int doBattle(playerID, globalEnemyID, enemyID)
{
	int choice;
	int techChoice, itemChoice;
	int damage, damageMp;
	int canAttack;
	//int recoveredMp, heal;
	//int i;
	char string[LEN_OF_DESCRIPTION];
	
	loadEnemy(globalEnemyID, enemyID);
	printf("A wild %s appeared!!\n\n", player[globalEnemyID].name);
	while (1) {
		choice = printMenu(playerID, globalEnemyID);
		player[playerID].moveMp = 0;
		player[playerID].moveHp = 0;
		player[globalEnemyID].moveHp = 0;
		player[globalEnemyID].moveMp = 0;
		if ((choice > 4) || (choice < 1)) {
			printf("That is not a valid option.\n\n");
		}
		canAttack = statusEffect(playerID);
		if (canAttack){
			switch (choice) {
				case 1:
					//ATTACK
					damage = doAttack(player[playerID].cumAttack,playerID, globalEnemyID, 1);
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
						if(doTech(techChoice, playerID, globalEnemyID) == 0) {
							printf("Not enough motivation!\n\n");
							continue;
						}
						else {
							
							if (player[playerID].moveHp == 0) {
								break;
							}
							else {
								damage = doAttack(player[playerID].moveHp, playerID, globalEnemyID, 1); //calculate damage done to enemy
								printf("You did %d damage!!\n", damage);
							}
							
							if (player[playerID].moveMp == 0) {
								break;
							}
							else {
								damageMp = doAttack(player[playerID].moveMp, playerID, globalEnemyID, 2);
								printf("%s lost %d mp!\n",player[globalEnemyID].name, damageMp);
							}
							
							//if (player[playerID].moveHp == 0) {
							// break;
							//}
							//else {
							// heal = doAttack(player[playerID].moveHp, playerID, globalEnemyID, 3);
							// printf("You healed %d hp!!\n", heal);
							//}
							
							//if (player[playerID].moveMp == 0) {
							// break;
							//}
							//else {
							//recoveredMp = doAttack(player[playerID].moveMp, playerID, globalEnemyID, 4);
							//printf("You recovered %d mp!!\n", recoveredMp);
							//}
						}	
					}
					break;
				case 3:
					itemChoice = itemMenu(playerID);  //Implement error checking!!!!
					if (itemChoice == -1) {
						continue;
					}
					else if (itemChoice == 0){
						printf("No item selected\n\n");
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
					printf("That is not a valid option\n\n");
					continue;
					//break;
			}
		}
		if (player[globalEnemyID].curHp == 0) {
			printf("You beat %s!!\n", player[globalEnemyID].name);
			player[PLAYER_ID].exp += player[ENEMY_ID].expValue;
			while (player[PLAYER_ID].exp >= player[PLAYER_ID].expToNextLevel) {
				player[PLAYER_ID].level++;
				sprintf(string, "You leveled up!!!\nYou are now level %d!!!\n", player[PLAYER_ID].level);
				slowPrint(string, SLOWPRINT_INTERVAL);
				player[PLAYER_ID].exp -= player[PLAYER_ID].expToNextLevel;
				updatePlayerData(PLAYER_ID);
			}
			return 1;
		}
		canAttack = statusEffect(globalEnemyID);
		if (canAttack){
			computerAttack(globalEnemyID, playerID);
		}
		if (player[playerID].curHp == 0) {
			printf("You lost to %s!!\n", player[globalEnemyID].name);
			return 0;
		}
	}
	return -1;  //THIS IS AN ERROR!!!
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
	sprintf(stringOut, "%s's HP: %d/%d   \t\t%s's HP: %d/%d\n", player[pid].name, player[pid].curHp, player[pid].baseHp,
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

int doTech(techID, sourceID, targetID)
{
	char string[LEN_OF_DESCRIPTION];
	switch (techID) {
		case 0:
			printf("You broke it\n\n");
			return 0;
		case 1:
			if (player[sourceID].curMp >= 5) {
				sprintf(string, "%s used protractor attack!\n", player[sourceID].name);
		        slowPrint(string, SLOWPRINT_INTERVAL);
				player[sourceID].moveHp = 5;
				player[sourceID].moveMp = 5;
				player[sourceID].curMp -= 5;
				return 1;
			}
			else {
				return 0;
			}
			
		case 2:
			if(player[sourceID].curMp > 5) {
		        sprintf(string, "%s used reinforce!\n", player[sourceID].name);
				slowPrint(string, SLOWPRINT_INTERVAL);
				player[sourceID].curMp -= 5;
				return 1;
			}
			else {
				return 0;
			}
			
		case 3:
			if (player[sourceID].curMp >= 2) {
				player[sourceID].curMp -= 2;
				sprintf(string, "%s used breathe", player[sourceID].name);
				slowPrint(string, SLOWPRINT_INTERVAL);
				slowPrint("...", SLOWPRINT_THINKING);
				if((rand() % 10) < 8) {
					sprintf(string, " %s was poisoned!!\n", player[targetID].name);
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
			
		default:
			return 0;
	}
}

void getTechName(int techId, char *string)
{
	switch (techId) {
		case 0:
			strcpy(string, "No Tech");
			return;
		case 1:
			strcpy(string, "Protractor Attack");
			return;
		case 2:
			strcpy(string, "Reinforce");
			return;
		case 3:
			strcpy(string, "Breathe");
			break;

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
	int choice;
	int techSucceed;
	
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
			sprintf(string, "%s tried to use a tech", player[computerID].name);
			slowPrint(string, SLOWPRINT_INTERVAL);
			slowPrint("...", SLOWPRINT_THINKING);
			slowPrint(" But couldn't!\n\n", SLOWPRINT_INTERVAL);
		}
	}
	else if (randValue <= (chanceOfTech + chanceOfItem)){
		//use an item
		printf("Item unimplemented\n");
	}
	else {
		//do an attack
		printf("%s is attacking!!!\n", player[computerID].name);
		damage = doAttack(player[computerID].cumAttack, computerID, targetID, DO_ATTACK_ENEMY_HP);
		printf("%s did %d damage!!!\n", player[computerID].name, damage);
	}
	
}

int addStatus(int statusID, int playerID)  //Returns a zero for success, a 1 if the status array is full, and a -1 if the status is already there
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
	return 1;
}

int itemMenu(int id)
{
	int i;
	char string[255];
	int choice;
	
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


void getItemName(int itemID, char *name)
{
	switch (itemID) {
		case 0:
			strcpy(name, "No Item");
			return;
		case 1:
			strcpy(name, "EVK Food");
			return;
		case 2:
			strcpy(name, "Parkside Food");
			return;
		case 3:
			strcpy(name, "Panda Food");
			return;
		case 4:
			strcpy(name, "Coffee");
			return;
		case 5:
			strcpy(name, "Starbucks");
			return;
		default:
			break;
	}
}

void doItem(int choice, int sourceID, int targetID)
{
	char string[LEN_OF_DESCRIPTION];
	int effectType[4];
	int itemID = player[sourceID].inventory[choice];
	int	 i;
	for (i = 0; i < 4; i++) {
		effectType[i] = 0;
	}
	switch (itemID) {
		case 0:
			printf("You broke it\n\n");
			return;
		case 1:
			sprintf(string, "%s ate EVK food!\n", player[sourceID].name);
			slowPrint(string, SLOWPRINT_INTERVAL);
			
			if (rollD20() < 2) {
				addStatus(3, sourceID);
				sprintf(string, "%s was poisoned!\n", player[sourceID].name);
				slowPrint(string, SLOWPRINT_INTERVAL);
				return;
			}
			else {
				player[sourceID].moveHp = 5;
				effectType[2] = 1;
			}
			break;
		case 2:
			sprintf(string, "%s ate Parkside food!\n", player[sourceID].name);
			slowPrint(string, SLOWPRINT_INTERVAL);
			player[sourceID].moveHp = 10;
			effectType[2] = 1;
			break;
		case 3:
			sprintf(string, "%s ate Panda food!\n", player[sourceID].name);
			slowPrint(string, SLOWPRINT_INTERVAL);
			player[sourceID].moveHp = 20;
			effectType[2] = 1;
			break;
		case 4:
			sprintf(string, "%s drank coffee!\n", player[sourceID].name);
			slowPrint(string, SLOWPRINT_INTERVAL);
			player[sourceID].moveMp = 5;
			effectType[3] = 1;
			break;
		case 5:
			sprintf(string, "%s drank Starbucks!\n", player[sourceID].name);
			slowPrint(string, SLOWPRINT_INTERVAL);
			player[sourceID].moveMp = 10;
			effectType[3] = 1;
			player[sourceID].moveHp = 5;
			effectType[2] = 1;
			break;
		default:
			break;
	}
	
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
