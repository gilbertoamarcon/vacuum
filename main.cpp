#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <vector>

using namespace std;

// Robot actions
#define GO_FORWARD		0
#define TURN_LEFT		1
#define TURN_RIGHT		2
#define SUCK_DIRT		3
#define TURN_OFF		4

// File parameters
#define BUFFER_SIZE		255
#define PATH			"out.csv"

// World parameters
#define M				10		// World num rows
#define N				10		// World num cols
#define INNER_WALLS		0		// Inner walls

// Learning parameters	
#define EP_IT			10000	// Max episode iterations
#define NUM_EP			50		// Num of episodes

// Viewing actions and states
#define DEBUG			0

// Robot class definition
class Robot{

	public:

		int i;
		int j;
		int dir;

		bool left_mode = 0;
		bool turn_mode = 0;

		// Constructor
		Robot(int i=1,int j=1,int dir=3){
			this->set(i,j,dir);
		}

		// Set position
		void set(int i=1,int j=1,int dir=3){
			this->i		= i;
			this->j		= j;
			this->dir	= dir;
		}

		// ====================================
		// Agents
		// ====================================

		int reflex_robot(char world[M][N]){
			if(senseDirt(world)) return SUCK_DIRT;
			if(senseWall(world)) return TURN_RIGHT;
			return GO_FORWARD;
		}

		int random_reflex_robot(char world[M][N]){
			if(senseDirt(world)) return SUCK_DIRT;
			if(senseWall(world)){
				if(rand()%2)
					return TURN_LEFT;
				return TURN_RIGHT;
			}
			if(rand()%100 < 25){
				if(rand()%2)
					return TURN_LEFT;
				return TURN_RIGHT;
			}
			return GO_FORWARD;
		}

		int deterministic_model_based_reflex_robot(char world[M][N]){

			if(senseDirt(world)){
				left_mode = !left_mode;
				return SUCK_DIRT;
			}

			if(turn_mode){
				if(!senseWall(world))
					turn_mode = 0;
			}else{
				if(senseWall(world))
					turn_mode = 1;
			}

			if(turn_mode){
				if(left_mode)
					return TURN_LEFT;
				else
					return TURN_RIGHT;
			}

			return GO_FORWARD;
		}
		// ====================================
		// ====================================




		// ====================================
		// Sensors
		// ====================================

		// Wall sensing in front of robot
		bool senseWall(char world[M][N]){
			int aux_i = i;
			int aux_j = j;
			switch(dir){
				case 0:	// Left
					aux_j--;
					break;
				case 1:	// Up
					aux_i--;
					break;
				case 2:	// Right
					aux_j++;
					break;
				case 3:	// Down
					aux_i++;
					break;
			}
			return world[aux_i][aux_j] == '#';
		}

		// Dirt sensing in the robot position
		bool senseDirt(char world[M][N]){
			return world[i][j] == '.';
		}

		// Home sensing for the robot position
		bool senseHome(){
			return i==1 && j==1;
		}

		// ====================================
		// ====================================




		// ====================================
		// Action execution
		// ====================================
		void act(char world[M][N]){

			int action = random_reflex_robot(world);

			if(DEBUG)
				printf("ACTION: %d\n",action);

			switch(action){
				case GO_FORWARD:
					if(senseWall(world))
						break;
					switch(dir){
						case 0:	// Left
							j--;
							break;
						case 1:	// Up
							i--;
							break;
						case 2:	// Right
							j++;
							break;
						case 3:	// Down
							i++;
							break;
					}
					break;
				case TURN_LEFT:
					dir--;
					break;
				case TURN_RIGHT:
					dir++;
					break;
				case SUCK_DIRT:
					world[i][j] = ' ';
					break;
				case TURN_OFF:
					break;
			}

			// Direction bounds enforcement
			dir += 4;
			dir = dir%4;
		}
		// ====================================
		// ====================================
};

// Initialize world
void set_world(char world[M][N]);

// Assess amount of dirt
int count_dirt(char world[M][N]);

// Display world
void display(Robot robot, char world[M][N]);

// Vector average
float avg(vector<int> vec);

int main(int argc, char **argv){

	vector<int> original_dirt;
	vector<int> final_dirt;
	vector<int> iterations;

	char world[M][N];

	srand(clock());

	// Robot
	Robot robot;

	for(int ep = 0; ep < NUM_EP; ep++){

		set_world(world);
		original_dirt.push_back(count_dirt(world));

		robot.set();
		
		int it = 0;
		for(it = 0; it < EP_IT; it++){

			// Display
			if(DEBUG && ep == NUM_EP-1)
				display(robot,world);

			// Sensing and acting
			robot.act(world);

			if(count_dirt(world) == 0)
				break;

		}

		final_dirt.push_back(count_dirt(world));
		iterations.push_back(it);

		if(DEBUG)
			printf("original_dirt: %d, final_dirt: %d, iterations: %d\n",original_dirt.back(),final_dirt.back(),iterations.back());

	}

	printf("Averages:\n");
	printf("original_dirt: %.3f, final_dirt: %.3f, iterations: %.3f\n",avg(original_dirt),avg(final_dirt),avg(iterations));
	
}

float avg(vector<int> vec){
	int acc = 0;
	for(int i = 0; i < vec.size(); i++){
		acc += vec.at(i);
	}
	return ((float)acc)/vec.size();
}

int count_dirt(char world[M][N]){

	int count = 0;

	for(int i = 0; i < M; i++)
		for(int j = 0; j < N; j++)
			if(world[i][j] == '.')
				count++;

	return count;
}

void set_world(char world[M][N]){

	// Empty space
	for(int i = 0; i < M; i++)
		for(int j = 0; j < N; j++)
			world[i][j] = ' ';

	// Outer walls
	for(int i = 0; i < M; i++){
		world[i][N-N] = '#';
		world[i][N-1] = '#';
	}
	for(int j = 0; j < N; j++){
		world[M-M][j] = '#';
		world[M-1][j] = '#';
	}

	// Dirt
	world[1][1] = '.';
	world[1][2] = '.';
	world[1][4] = '.';
	world[1][6] = '.';
	world[1][7] = '.';
	world[1][8] = '.';
	world[2][2] = '.';
	world[2][4] = '.';
	world[2][6] = '.';
	world[2][8] = '.';
	world[3][8] = '.';
	world[4][1] = '.';
	world[4][2] = '.';
	world[4][4] = '.';
	world[4][6] = '.';
	world[4][8] = '.';
	world[6][1] = '.';
	world[6][3] = '.';
	world[6][4] = '.';
	world[6][6] = '.';
	world[6][7] = '.';
	world[6][8] = '.';
	world[7][2] = '.';
	world[7][3] = '.';
	world[7][4] = '.';
	world[7][5] = '.';
	world[8][1] = '.';
	world[8][3] = '.';
	world[8][4] = '.';
	world[8][6] = '.';
	world[8][7] = '.';
	world[8][8] = '.';

	if(INNER_WALLS){

		// Inner walls
		int wall_i = floor(M/2);
		int wall_j = floor(N/2);
		for(int i = 0; i < M; i++)
			world[i][wall_j] = '#';
		for(int j = 0; j < N; j++)
			world[wall_i][j] = '#';

		// Inner doors
		world[(int)floor(0.25*M)][wall_j] = ' ';
		world[(int)floor(0.75*M)][wall_j] = ' ';
		world[wall_i][(int)floor(0.25*N)] = ' ';
		world[wall_i][(int)floor(0.75*N)] = ' ';

	}
}

void display(Robot robot, char world[M][N]){

	// Column Number
	printf("--");
	for(int j = 0; j < N; j++)
		printf("-%d-",j);
	printf("\n");

	for(int i = 0; i < M; i++){

		// Row Number
		printf("%d|",i);

		// World
		for(int j = 0; j < N; j++){
			if(robot.i == i && robot.j == j){
				switch(robot.dir){
					case 0:
						printf(" \u2190 ");
						break;
					case 1:
						printf(" \u2191 ");
						break;
					case 2:
						printf(" \u2192 ");
						break;
					case 3:
						printf(" \u2193 ");
						break;
					default:
						printf(" A ");
						break;
				}
			}
			else
				printf(" %c ",world[i][j]);
		}

		printf("\n");
	}

}
