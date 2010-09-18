#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define PLAYER_ID 0
#define ENEMY_ID 1
#define SLOWPRINT_INTERVAL 20000

struct wpn {
	char name[40];
	int modAttack;
	int modTech;
};

struct invtry {
	int potions;
};

struct plyr {
	char name[40];
	int baseHp;
	int baseMp;
	int baseTech;
	int baseAttack;
	int baseDefense;
	char armorName[40];
	int armorModDefense;
	struct wpn weapon;
	int level;
	
	//Cumulative data
	
	int cumAttack;
	int cumDefense;
	int cumTech;
	
	//Live data
	
	int curHp;
	int curMp;
	struct invtry inventory;
	
	//Circle of weaknesses
	
	int typeEngineering;
	int typeArt;
	int typeBusiness;
	
	//Eng beats business, business beats art, art beats engineering.  mainly just because.
};

void loadEnemy(int, int);
void slowPrint(char*, int);
int rollD20();
int doAttack();
void updatePlayerData(int);
void describe(int);
void initPlayer(int);
void testFunc();
void battle(int, int, int);
int printMenu(int, int);

struct plyr player[2];

int main()
{
	srand((unsigned)(time(0)));
	testFunc();
	return 0;
}

void loadEnemy(int globalID, int enemyID){
	player[globalID].typeEngineering = 0;
	player[globalID].typeArt = 0;
	player[globalID].typeBusiness = 0;
	player[globalID].inventory.potions = 0;
	switch (enemyID) {
		case 0: //ENEMY: BRUIN
			strcpy(player[globalID].name, "Bruin");
			player[globalID].baseHp = 10;
			player[globalID].baseMp = 2;
			player[globalID].baseTech = 4;
			player[globalID].baseAttack = 3;
			player[globalID].baseDefense = 1;
			strcpy(player[globalID].armorName, "UCLA Sweatshirt");
			player[globalID].armorModDefense = 1;
			strcpy(player[globalID].weapon.name, "Defeat Bell");
			player[globalID].weapon.modAttack = 1;
			player[globalID].weapon.modTech = 1;
			player[globalID].level = 1;
			player[globalID].inventory.potions = 1;
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

int doAttack(int baseAttack)
{
	int roll;
	double attackModifier;
	
	roll = rollD20();
	if ((roll >= 5) && (roll <= 16)) {
		printf("A decent hit!\n");
		roll -= 5;
		attackModifier = 0.9 + (((double)roll / 110.0) * 2);
	}
	else if ((roll >= 16) && (roll <= 19)){
		printf("A good hit!\n");
		attackModifier = 1.5;
	}
	else if ((roll >= 2) && (roll <= 4)){
		printf("A weak hit!\n");
		attackModifier = 0.5;
	}
	else if (roll == 20){
		slowPrint("*****  SMAAAASH!!  *****\n", SLOWPRINT_INTERVAL);
		printf("A critical hit!\n");
		attackModifier = 3.0;
	}
	else if (roll == 1){
		printf("A miss!\n");
		attackModifier = 0;
	}
	return (int)((double)baseAttack * attackModifier);
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
	loadEnemy(globalEnemyID, enemyID);
	printf("A wild %s appeared!!\n\n", player[globalEnemyID].name);
	while (!(player[playerID].curHp <= 0) && !(player[globalEnemyID].curHp <= 0)) {
		choice = printMenu(playerID, globalEnemyID);
		switch (choice) {
			case 1:
				//ATTACK
				break;
			case 2:
				//TECH
				break;
			case 3:
				//INVENTORY
				break;
			case 4:
				//DESCRIBE
				describe(globalEnemyID);
				break;
			default:
				printf("That is not a valid option\n\n");
				continue;
				//break;
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
		scanf("%s", player[id].name);
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
	player[id].baseAttack = 7;
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
	updatePlayerData(PLAYER_ID);
	player[id].curHp = player[id].baseHp;
	player[id].curMp = player[id].baseMp;
}

void testFunc()
{
	//loadEnemy(ENEMY_ID, 0);
	//describe(ENEMY_ID);
	initPlayer(PLAYER_ID);
	doBattle(PLAYER_ID, ENEMY_ID, 0);
}

int printMenu(int pid, int eid)
{
	int choice;
	char stringOut[255];
	char trash[255];
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
	
	
