#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include <string.h>
#include <unistd.h>
#include<fstream>

#define MAX_SITUATION 0xffffffff
#define INITIAL_SCORE 20.00
#define MINIMAL_SCORE 0.0001
#define STEP_FACTOR 0.95

#define WALL -1
#define WHITE 0
#define BLACK 1
#define EMPTY 2

#define WIDTH 8

#define VERBOSE 0
#define OMIT_OPEN 1

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
u64 total_list=0;

int main() {

	printf("Last modified: 2018-May29\n");
	printf("MAX_SITUATION: %u\n",MAX_SITUATION);
	printf("MAX_GAME_NUMBER: %ld\n",MAX_GAME_NUMBER);
	printf("OMIT_OPEN: %d\n",OMIT_OPEN);

	printf("INITIAL_SCORE: %f\n",INITIAL_SCORE);
	printf("STEP_FACTOR: %f\n",STEP_FACTOR);
	printf("STORE_BATCH: %d\n",STORE_BATCH);

	printf("VERBOSE: %d\n",VERBOSE);
	printf("DEBUG: %d\n",DEBUG);

	//global variables
	srand((unsigned)time(NULL));

	for(int i=0; i<WIDTH; i++) {
		for(int j=0; j<WIDTH; j++) {
			for(int t=0; t<2; t++) {
				usleep(10000);
				//			srand(((unsigned)time(NULL)+(i<<15)+(j<<7)+t));
				u64 t_v_l=(u64)rand()|(u64)(rand()<<16)|(u64)(rand()<<32)|(u64)(rand()<<48);

				random_table[i][j][t]=t_v_l;
			}
		}
	}
	g_s game_table = (g_s)malloc(sizeof(game_situation)*MAX_SITUATION);

	for(long i=0; i<MAX_SITUATION; i++) {
		game_table[i].hash_code=0;

		game_table[i].score[WHITE]=INITIAL_SCORE;
		game_table[i].score[BLACK]=INITIAL_SCORE;
//		game_table[i].access_number[WHITE]=0;
//		game_table[i].access_number[BLACK]=0;
		game_table[i].total_access=0;
	}

	if(DEBUG==1) {
		printf("print the random board number \n");
		printf("**********************************\n");
		for(int i=0; i<WIDTH; i++) {
			for(int j=0; j<WIDTH; j++) {
				for(int t=0; t<2; t++) {
					printf("random_table[%d][%d][%d]: %lu\t",i,j,t,random_table[i][j][t]);
				}
			}
			printf("\n");
		}
		printf("**********************************\n");

	}
	// every one for-loop means one whole game


	//write into bit file
	FILE *f_z;
	f_z=fopen("Zobrist_code.dat","wb");
	for(int i=0; i<WIDTH; i++) {
		for(int j=0; j<WIDTH; j++) {
			for(int t=0; t<2; t++) {
				fwrite(&random_table[i][j][t],sizeof(u64),1,f_z);
			}
		}
		printf("\n");
	}
	fclose(f_z);
	// read back to see its performance
//	f_z=fopen("Zobrist_code.dat","rb");
//
//	printf("print the random board number \n");
//	printf("**********************************\n");
//	for(int i=0; i<WIDTH; i++) {
//		for(int j=0; j<WIDTH; j++) {
//			for(int t=0; t<2; t++) {
//				u64 temp;
//
//				fread(&temp,sizeof(u64),1,f_z);
//				if(temp!=random_table[i][j][t])
//					printf("Error!");
//			}
//		}
//		printf("\n");
//	}
//	printf("**********************************\n");

	for(long current_game_number=0; current_game_number<MAX_GAME_NUMBER; current_game_number++) {

		if(current_game_number%100000==0) {
			printf("current_game_number: %ld\n",current_game_number);
			if(DEBUG==1) {
				for(long traverse_game=0; traverse_game<MAX_SITUATION; traverse_game++) {
					if(game_table[traverse_game].hash_code!=0) {

						// this has stored information
						printf("***start****\n");
						printf("Hash Code: %ld\n",game_table[traverse_game].hash_code);
						printf("Score WHITE %f\n",game_table[traverse_game].score[WHITE]);
						printf("Score BLACK %f\n",game_table[traverse_game].score[BLACK]);
						printf("***end****\n");

					}
				}
			}
		}

		if(current_game_number!=0 && current_game_number%STORE_BATCH==0) {
			printf("Store file when current_game_number is %ld, current situation_number is %ld\n",current_game_number,total_list);
			FILE *f_table;
			f_table=fopen("Situation_evaluation.dat","wb");
			for(long traverse_game=0; traverse_game<MAX_SITUATION; traverse_game++) {
				if(game_table[traverse_game].hash_code!=0) {

					// this has stored information
//					if(game_table[traverse_game].access_number[0]==0 && game_table[traverse_game].access_number[1]==0)
//						printf("Fatal Error! Access number should not be two zeros\n");
					fwrite(&game_table[traverse_game],sizeof(struct game_situation),1,f_table);

				}
			}
			fclose(f_table);
		}
		if(DEBUG==1) {
			printf("DEBUG STATION 1\n");
		}
		struct step_list_record game_list[MAX_STEP]; //maximum steps of 60, used to store all game_situation happened at single game

		memset(game_list,0,sizeof(game_list));

		node_number[BLACK]=4;
		node_number[WHITE]=1;
		int current_turn=WHITE; //before taking stpes;
		int global_step=0;      //without the inital 4+1(5,6), how many steps have "already" been taken
		u64 z_hash=0;


		//initiate the board
		for(int i=0; i<WIDTH+2; i++) {
			for(int j=0; j<WIDTH+2; j++) {
				if (i==0 || i==WIDTH+1 || j==0 || j==WIDTH+1)
					board[i][j]=WALL;
				else
					board[i][j]=EMPTY;

			}
		}


		board[5][5]=BLACK;
		board[5][4]=BLACK;
		board[4][5]=BLACK;
		board[4][4]=WHITE;
		board[5][6]=BLACK;
		// initial 4 and first step at (5,6)

		struct step candidate[MAX_STEP];

		//this while-loop is for one single game
		bool end=false;
		if(DEBUG==1) {
			printf("DEBUG STATION 2\n");
		}
		while(true) {
			memset(candidate,0,sizeof(candidate));

			int candidate_number=0;
			int flip_step=(current_turn==WHITE)?BLACK:WHITE;

			//find the candidate for next step
			for(int i=1; i<WIDTH+1; i++) {
				for(int j=1; j<WIDTH+1; j++) {

					if(board[i][j]==flip_step) { //if this is the opposite one, search around it
						if(board[i-1][j]==EMPTY && check_down_hit(i,j,flip_step,current_turn)==true) {
							// want to step at up side
							candidate[candidate_number].x=i-1;
							candidate[candidate_number].y=j;
							candidate_number++;
						}
						if(board[i+1][j]==EMPTY && check_up_hit(i,j,flip_step,current_turn)==true) {
							// want to step at down side
							candidate[candidate_number].x=i+1;
							candidate[candidate_number].y=j;
							candidate_number++;

						}

						if(board[i][j+1]==EMPTY && check_left_hit(i,j,flip_step,current_turn)==true) {
							// want to step at right side
							candidate[candidate_number].x=i;
							candidate[candidate_number].y=j+1;
							candidate_number++;

						}

						if(board[i][j-1]==EMPTY && check_right_hit(i,j,flip_step,current_turn)==true) {
							// want to step at left side
							candidate[candidate_number].x=i;
							candidate[candidate_number].y=j-1;
							candidate_number++;

						}

						if(board[i+1][j+1]==EMPTY && check_left_up_hit(i,j,flip_step,current_turn)==true) {
							// want to step at right_down side
							candidate[candidate_number].x=i+1;
							candidate[candidate_number].y=j+1;
							candidate_number++;

						}
						if(board[i+1][j-1]==EMPTY && check_right_up_hit(i,j,flip_step,current_turn)==true) {
							// want to step at left_down side
							candidate[candidate_number].x=i+1;
							candidate[candidate_number].y=j-1;
							candidate_number++;

						}

						if(board[i-1][j+1]==EMPTY && check_left_down_hit(i,j,flip_step,current_turn)==true) {
							// want to step at right_up side
							candidate[candidate_number].x=i-1;
							candidate[candidate_number].y=j+1;
							candidate_number++;

						}

						if(board[i-1][j-1]==EMPTY && check_right_down_hit(i,j,flip_step,current_turn)==true) {
							// want to step at left_up side
							candidate[candidate_number].x=i-1;
							candidate[candidate_number].y=j-1;
							candidate_number++;

						}

					}
				}
			}

			if(candidate_number==0 && end==false) {
				// has no place to go, skip current
				current_turn=(current_turn==WHITE)?BLACK:WHITE;
				end=true;
			} else if(candidate_number==0 && end==true)
				//game over
				break;
			else {
				// candidate_number!=0
				end=false;
				int select=rand()%candidate_number;//now its randomly select one position

				// choose position is candidate[select];
				int X=candidate[select].x;
				int Y=candidate[select].y;
				if(DEBUG==1 && VERBOSE==1) {
					printf("DEBUG STATION 2.1 candidate_number: %d select:%d current_turn: %d, X: %d Y: %d\n,",candidate_number,select,current_turn,X,Y);
				}
				board[X][Y]=current_turn;
				z_hash^=random_table[X-1][Y-1][current_turn];
				game_list[global_step].hash_code=z_hash;
				game_list[global_step].who_finish_this=current_turn;
				global_step++;
				node_number[current_turn]+=1;//add this one before flipping the other board
				int temp_flip=(current_turn==WHITE)?BLACK:WHITE;
				/**************************************/
				if(board[X-1][Y-1]==temp_flip) {
					//X-1, Y-1 side is flipable
					if(check_left_up_hit(X-1,Y-1,temp_flip,current_turn)==true) {
						//can flip position
						int delete_number=0;
						int t_x=X-1;
						int t_y=Y-1;
						while(board[t_x][t_y]!=current_turn) {

							board[t_x][t_y]=current_turn;//flip this
							delete_number++;
							t_x--;
							t_y--;

						}
						node_number[temp_flip]-=delete_number;
						node_number[current_turn]+=delete_number;
					}

				}
				/**************************************/
				/**************************************/
				if(board[X-1][Y]==temp_flip) { //change in here
					//X-1, Y-1 side is flipable
					if(check_up_hit(X-1,Y,temp_flip,current_turn)==true) { //change in here
						//can flip position
						int delete_number=0;
						int t_x=X-1; //change in here
						int t_y=Y; //change in here
						while(board[t_x][t_y]!=current_turn) {

							board[t_x][t_y]=current_turn;//flip this
							delete_number++;
							t_x--; //change in here

						}
						node_number[temp_flip]-=delete_number;
						node_number[current_turn]+=delete_number;

					}

				}
				/**************************************/

				/**************************************/
				if(board[X-1][Y+1]==temp_flip) { //change in here
					//X-1, Y-1 side is flipable
					if(check_right_up_hit(X-1,Y+1,temp_flip,current_turn)==true) { //change in here
						//can flip position
						int delete_number=0;
						int t_x=X-1; //change in here
						int t_y=Y+1; //change in here
						while(board[t_x][t_y]!=current_turn) {

							board[t_x][t_y]=current_turn;//flip this
							delete_number++;
							t_x--; //change in here
							t_y++;

						}
						node_number[temp_flip]-=delete_number;
						node_number[current_turn]+=delete_number;

					}

				}
				/**************************************/
				/**************************************/
				if(board[X][Y-1]==temp_flip) { //change in here
					//X-1, Y-1 side is flipable
					if(check_left_hit(X,Y-1,temp_flip,current_turn)==true) { //change in here
						//can flip position
						int delete_number=0;
						int t_x=X; //change in here
						int t_y=Y-1; //change in here
						while(board[t_x][t_y]!=current_turn) {

							board[t_x][t_y]=current_turn;//flip this
							delete_number++;
							t_y--;

						}
						node_number[temp_flip]-=delete_number;
						node_number[current_turn]+=delete_number;

					}

				}
				/**************************************/
				/**************************************/
				if(board[X][Y+1]==temp_flip) { //change in here
					//X-1, Y-1 side is flipable
					if(check_right_hit(X,Y+1,temp_flip,current_turn)==true) { //change in here
						//can flip position
						int delete_number=0;
						int t_x=X; //change in here
						int t_y=Y+1; //change in here
						while(board[t_x][t_y]!=current_turn) {

							board[t_x][t_y]=current_turn;//flip this
							delete_number++;
							t_y++;

						}
						node_number[temp_flip]-=delete_number;
						node_number[current_turn]+=delete_number;

					}

				}
				/**************************************/
				/**************************************/
				if(board[X+1][Y-1]==temp_flip) { //change in here
					//X-1, Y-1 side is flipable
					if(check_left_down_hit(X+1,Y-1,temp_flip,current_turn)==true) { //change in here
						//can flip position
						int delete_number=0;
						int t_x=X+1; //change in here
						int t_y=Y-1; //change in here
						while(board[t_x][t_y]!=current_turn) {

							board[t_x][t_y]=current_turn;//flip this
							delete_number++;
							t_x++;
							t_y--;

						}
						node_number[temp_flip]-=delete_number;
						node_number[current_turn]+=delete_number;

					}

				}
				/**************************************/

				/**************************************/
				if(board[X+1][Y]==temp_flip) { //change in here
					//X-1, Y-1 side is flipable
					if(check_down_hit(X+1,Y,temp_flip,current_turn)==true) { //change in here
						//can flip position
						int delete_number=0;
						int t_x=X+1; //change in here
						int t_y=Y; //change in here
						while(board[t_x][t_y]!=current_turn) {

							board[t_x][t_y]=current_turn;//flip this
							delete_number++;
							t_x++;

						}
						node_number[temp_flip]-=delete_number;
						node_number[current_turn]+=delete_number;

					}

				}
				/**************************************/
				if(board[X+1][Y+1]==temp_flip) { //change in here
					//X-1, Y-1 side is flipable
					if(check_right_down_hit(X+1,Y+1,temp_flip,current_turn)==true) { //change in here
						//can flip position
						int delete_number=0;
						int t_x=X+1; //change in here
						int t_y=Y+1; //change in here
						while(board[t_x][t_y]!=current_turn) {

							board[t_x][t_y]=current_turn;//flip this
							delete_number++;
							t_x++;
							t_y++;

						}
						node_number[temp_flip]-=delete_number;
						node_number[current_turn]+=delete_number;

					}

				}
				/**************************************/
				//finish game updating
				current_turn=temp_flip;
			}

		}
		if(DEBUG==1) {
			printf("DEBUG STATION 3\n");
		}

		int Winner=(node_number[BLACK]>node_number[WHITE])?BLACK:WHITE;
		if(node_number[BLACK]==node_number[WHITE]) {
			if(DEBUG==1) {
				printf("Even result!");
			}
			continue;
		}
		// when the game finishes

		//black wins
		//feedback
		double current_step_value=1.0;
		if(DEBUG==1 && VERBOSE==1) {
			printf("DEBUG STATION 3.1 global_step: %d\n",global_step);
			for(int i=1; i<WIDTH+1; i++) {
				for(int j=1; j<WIDTH+1; j++) {
					printf("%3d  ",board[i][j]);
				}
				printf("\n");
			}
		}

		for(int i=global_step-1; i>=0; i--) {
			if(DEBUG==1) {
				printf("current i in global_step loop: %d\n",i);
			}
			if((u32)(game_list[i].hash_code)>=MAX_SITUATION) {
				current_step_value*=STEP_FACTOR;

				if(OMIT_OPEN==1)
					printf("Omit a unknown tag\n");
				continue;
			}
			if(game_table[(u32)(game_list[i].hash_code)].hash_code==game_list[i].hash_code) {
				// meet with previous exact one
				int finish=game_list[i].who_finish_this;
				if(finish!=WHITE && finish!=BLACK)
					printf("Fatal Error! Unexpected Finished\n");
				game_table[(u32)(game_list[i].hash_code)].total_access++;

				if(finish==Winner) {
					//winner
					game_table[(u32)(game_list[i].hash_code)].score[finish]+=current_step_value;

				} else {
					//loser
					game_table[(u32)(game_list[i].hash_code)].score[finish]=(game_table[(u32)(game_list[i].hash_code)].score[finish]-current_step_value>MINIMAL_SCORE)?
					        (game_table[(u32)(game_list[i].hash_code)].score[finish]-current_step_value):MINIMAL_SCORE;

				}
			} else if(game_table[(u32)(game_list[i].hash_code)].hash_code==0) {
				//meet with a new one
				total_list++;
				int finish=game_list[i].who_finish_this;
				if(finish!=WHITE && finish!=BLACK)
					printf("Fatal Error! Unexpected Finished\n");
				game_table[(u32)(game_list[i].hash_code)].total_access++;
				game_table[(u32)(game_list[i].hash_code)].hash_code=game_list[i].hash_code;
				if(finish==Winner) {
					//winner
					game_table[(u32)(game_list[i].hash_code)].score[finish]+=current_step_value;

				} else {
					//loser
					game_table[(u32)(game_list[i].hash_code)].score[finish]=(game_table[(u32)(game_list[i].hash_code)].score[finish]-current_step_value>MINIMAL_SCORE)?
					        (game_table[(u32)(game_list[i].hash_code)].score[finish]-current_step_value):MINIMAL_SCORE;
				}

			} else {
				printf("Error! repetive insertion into the same slot\n");
			}
			if(DEBUG==1) {
				printf("DEBUG STATION 3.2 current_step_value: %lf\n",current_step_value);
			}
			current_step_value*=STEP_FACTOR;

			if(DEBUG==1) {
				printf("DEBUG STATION 3.3 current_step_value: %lf\n",current_step_value);
			}
		}


		if(DEBUG==1) {
			printf("DEBUG STATION 4\n");
		}
	}
}

bool check_right_hit(int i0,int j0,int keep, int target) {
	// to see if we can go from (i0,j0) to right([i0,j0] not included), and find a place equals target
	// with variable keep means that if we meet keep, we don't stop but extends towards right

	int i=i0;
	int j=j0;
	while(board[i][j]!=WALL) {
		j++;
		if(board[i][j]==target)
			return true;
		if(board[i][j]!=keep)
			return false;
	}
	return false;
}


bool check_left_hit(int i0,int j0,int keep, int target) {
	// to see if we can go from (i0,j0) to left([i0,j0] not included), and find a place equals target
	// with variable keep means that if we meet keep, we don't stop but extends towards right

	int i=i0;
	int j=j0;
	while(board[i][j]!=WALL) {
		j--;
		if(board[i][j]==target)
			return true;
		if(board[i][j]!=keep)
			return false;
	}
	return false;
}


bool check_up_hit(int i0,int j0,int keep, int target) {
	// to see if we can go from (i0,j0) to up([i0,j0] not included), and find a place equals target
	// with variable keep means that if we meet keep, we don't stop but extends towards right

	int i=i0;
	int j=j0;
	while(board[i][j]!=WALL) {
		i--;
		if(board[i][j]==target)
			return true;
		if(board[i][j]!=keep)
			return false;
	}
	return false;
}

bool check_down_hit(int i0,int j0,int keep, int target) {
	// to see if we can go from (i0,j0) to up([i0,j0] not included), and find a place equals target
	// with variable keep means that if we meet keep, we don't stop but extends towards right

	int i=i0;
	int j=j0;
	while(board[i][j]!=WALL) {
		i++;
		if(board[i][j]==target)
			return true;
		if(board[i][j]!=keep)
			return false;
	}
	return false;
}

bool check_left_up_hit(int i0,int j0,int keep, int target) {
	// to see if we can go from (i0,j0) to left_up([i0,j0] not included), and find a place equals target
	// with variable keep means that if we meet keep, we don't stop but extends towards right

	int i=i0;
	int j=j0;
	while(board[i][j]!=WALL) {
		i--;
		j--;
		if(board[i][j]==target)
			return true;
		if(board[i][j]!=keep)
			return false;
	}
	return false;
}

bool check_right_up_hit(int i0,int j0,int keep, int target) {
	// to see if we can go from (i0,j0) to right_up([i0,j0] not included), and find a place equals target
	// with variable keep means that if we meet keep, we don't stop but extends towards right

	int i=i0;
	int j=j0;
	while(board[i][j]!=WALL) {
		i--;
		j++;
		if(board[i][j]==target)
			return true;
		if(board[i][j]!=keep)
			return false;
	}
	return false;
}

bool check_right_down_hit(int i0,int j0,int keep, int target) {
	// to see if we can go from (i0,j0) to right_down([i0,j0] not included), and find a place equals target
	// with variable keep means that if we meet keep, we don't stop but extends towards right

	int i=i0;
	int j=j0;
	while(board[i][j]!=WALL) {
		i++;
		j++;
		if(board[i][j]==target)
			return true;
		if(board[i][j]!=keep)
			return false;
	}
	return false;
}

bool check_left_down_hit(int i0,int j0,int keep, int target) {
	// to see if we can go from (i0,j0) to left_down([i0,j0] not included), and find a place equals target
	// with variable keep means that if we meet keep, we don't stop but extends towards right

	int i=i0;
	int j=j0;
	while(board[i][j]!=WALL) {
		i++;
		j--;
		if(board[i][j]==target)
			return true;
		if(board[i][j]!=keep)
			return false;
	}
	return false;
}
