#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PLAYER_ID 0
#define ENEMY_ID 1

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
	int cumAttack;
	int cumDefense;
	int cumTech;
	int curHp;
	int curMp;
};

void loadEnemy(int);
void slowPrint(char*, int);
int rollD20();
int attackRandomizer();

struct plyr players[2];

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

void loadEnemy(int id){
	printf("h");
	//Load stuff into players[1] (enemy)
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

