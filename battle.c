#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PLAYER_ID 0
#define ENEMY_ID 1
#define SLOWPRINT_INTERVAL 20000

struct wpn {
	char name[40];
	int modAttack;
	int modTech;
};

struct invtry {
	int potions = 0;
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
	
	int typeEngineering = 0;
	int typeArt = 0;
	int typeBusiness = 0;
	
	//Eng beats business, business beats art, art beats engineering
};

void loadEnemy(int);
void slowPrint(char*, int);
int rollD20();
int attackRandomizer();

struct plyr player[2];

int main()
{
	while (1) {
		printf("You did %d damage!", attackRandomizer(50));
		getchar();
	}
	//Do something more useful!!!
	//And again...
	return 1;
}

void loadEnemy(int globalID, int enemyID){
	switch (enemyID) {
		case 0: //ENEMY: BRUIN
			player[globalID].name = "Bruin";
			player[globalID].baseHp = 10;
			player[globalID].baseMp = 2;
			player[globalID].baseTech = 4;
			player[globalID].baseAttack = 3;
			player[globalID].baseDefense = 1;
			player[globalID].armorName = "UCLA Sweatshirt";
			player[globalID].armorModDefense = 1;
			player[globalID].weapon.name = "Defeat Bell";
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

int attackRandomizer(int baseAttack)
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
		slowPrint("*****  SMAAAASH!!  *****\n", 20000);
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
