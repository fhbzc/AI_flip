#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include <string.h>
#include <unistd.h>
#include<fstream>

#define MAX_SITUATION 0xffffffff
#define INITIAL_SCORE 50.0
#define STEP_FACTOR 0.94

#define WALL -1
#define WHITE 0
#define BLACK 1
#define EMPTY 2

#define WIDTH 8

#define VERBOSE 0
#define DEBUG 0
#define MAX_STEP 60
#define MAX_GAME_SITUATION 200
#define MAX_GAME_NUMBER 0x0fffffffffffffff
#define STORE_BATCH 800000
typedef struct game_situation * g_s;
typedef unsigned long u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

struct game_situation {
	u64 hash_code;
	float score[2];
	u32 total_access;
};
struct step {
	int x;
	int y;
};
struct step_list_record {
	int who_finish_this;
	long hash_code;
};
u64 random_table[WIDTH][WIDTH][2];
int board[WIDTH+2][WIDTH+2];
int node_number[2];//current bloack and white on board
u64 z_hash=0;

bool check_right_hit(int i0,int j0,int keep, int target);
bool check_left_hit(int i0,int j0,int keep, int target);
bool check_up_hit(int i0,int j0,int keep, int target);
bool check_down_hit(int i0,int j0,int keep, int target);
bool check_left_up_hit(int i0,int j0,int keep, int target);
bool check_right_up_hit(int i0,int j0,int keep, int target);
bool check_right_down_hit(int i0,int j0,int keep, int target);
bool check_left_down_hit(int i0,int j0,int keep, int target);
/* about the definition of coordinate
*   y y y y y y y  y increases from left to right
*  x
*  x
*  x
*  x
*  x increases from top to down
*/

int main() {



	// read back to see its performance
	FILE * f_z;
	f_z=fopen("Zobrist_code.dat","rb");

	printf("print the random board number \n");
	printf("**********************************\n");
	for(int i=0; i<WIDTH; i++) {
		for(int j=0; j<WIDTH; j++) {
			for(int t=0; t<2; t++) {
				u64 temp;
				fread(&random_table[i][j][t],sizeof(u64),1,f_z);

			}
		}
	}
	fclose(f_z);

	printf("**********************************\n");
	FILE * f_hash;

	f_hash= fopen("Situation_evaluation.dat","rb");
	while(!feof(f_hash)) {
		struct game_situation temp;
		fread(&temp,sizeof(struct game_situation),1,f_hash);
		printf("***start****\n");
		printf("Hash Code: %ld\n",temp.hash_code);
		printf("Score WHITE %f\n",temp.score[WHITE]);
		printf("Score BLACK %f\n",temp.score[BLACK]);
//		printf("ACCESS NUMBER WHITE %d\n",temp.access_number[WHITE]);
//		printf("ACCESS NUMBER BLACK %d\n",temp.access_number[BLACK]);
		printf("ACCESS TOTAL NUMBER %d\n",temp.total_access);
		printf("***end****\n");
	}
	fclose(f_hash);
	return 1;
}
