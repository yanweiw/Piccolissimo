#define BEACON_NUM 8
#define ROBOT_COUNT 10
#define COLS 3
#define ROWS 3
#define HOR_SEPAR 200
#define VER_SEPAR HOR_SEPAR / 100 * 173 / 2
#define ARENA_WIDTH COLS * HOR_SEPAR
#define ARENA_HEIGHT (ROWS - 1) * VER_SEPAR

#define LIGHT_CENTER_X 1
#define LIGHT_CENTER_Y 1
#define SIMULATION_TIME 180 //in seconds



int iteration = 0;

// function to check if randomly generated positions are in collision
bool collision(float x, float y, float pos_list[ROBOT_COUNT][4], int limit) {
	for (int i = 0; i < limit; i++) {
		float existing_x = pos_list[i][0];
		float existing_y = pos_list[i][1];
		float dist = sqrt(pow(x - existing_x, 2) + pow(y - existing_y, 2));
		if (dist <= radius * 2) {
			return true;
		}
	}
	return false;
}

void setup_positions(float robot_pos[ROBOT_COUNT][4])
{

//assign each robot a random position, centered around light source
	// int robot_num;
	int x= ARENA_WIDTH / 2;
	int y= ARENA_HEIGHT / 2;
	printf("width: %d; height: %d; ver_sep: %d\n", ARENA_WIDTH, ARENA_HEIGHT, VER_SEPAR);
	// beacon setup
	for (int i = 0; i < COLS; i++) {
		robot_pos[i][0] = HOR_SEPAR / 2 + HOR_SEPAR * i;
		robot_pos[i][1] = 0;
		robot_pos[i][2] = 0;
		robot_pos[i][3] = i;
	}
	for (int i = COLS; i < BEACON_NUM - COLS - 1 + (ROWS%2); i++){
		int c = (i-COLS) % 2;
		int r = (i-COLS) / 2;
		robot_pos[i][0] = (r%2) * HOR_SEPAR / 2 + c * (ARENA_WIDTH - (r%2) * HOR_SEPAR);
		robot_pos[i][1] = (r+1) * VER_SEPAR;
		robot_pos[i][2] = 0;
		robot_pos[i][3] = i;
	}
	for (int i = BEACON_NUM - COLS + (ROWS%2) -1; i < BEACON_NUM; i++){
		int c = i - (BEACON_NUM - COLS + (ROWS%2) -1);
		robot_pos[i][0] = (ROWS%2) * HOR_SEPAR / 2 + c * HOR_SEPAR;
		robot_pos[i][1] = ARENA_HEIGHT;
		robot_pos[i][2] = 0;
		robot_pos[i][3] = i;
	}

//triangular positions
	// robot_pos[8][0] = HOR_SEPAR;
	// robot_pos[8][1] = VER_SEPAR;
	// robot_pos[8][2] = 0;
	// robot_pos[8][3] = 8;
	// robot_pos[9][0] = 2 * HOR_SEPAR;
	// robot_pos[9][1] = VER_SEPAR;
	// robot_pos[9][2] = 0;
	// robot_pos[9][3] = 9;
	// robot_pos[2][0] = x - 200;
	// robot_pos[2][1] = y - 100;
	// robot_pos[2][2] = 0;
	// robot_pos[2][3] = 3;
	// robot_pos[3][0] = x + 200;
	// robot_pos[3][1] = y - 100;
	// robot_pos[3][2] = 0;
	// robot_pos[3][3] = 2;
	for (int i = BEACON_NUM; i < ROBOT_COUNT; i++) {
		float x0;
		float y0;
		float t0;
		do
		{
			x0 = (float) rand() * (ARENA_WIDTH-2*radius) / RAND_MAX + radius;
			y0 = (float) rand() * (ARENA_HEIGHT-2*radius) / RAND_MAX + radius;
			t0 = rand() * 2 * PI / RAND_MAX;
		} while (collision(x0, y0, robot_pos, i));
		robot_pos[i][0] = x0;
		robot_pos[i][1] = y0;
		robot_pos[i][2] = t0;
		robot_pos[i][3] = i;
	}

	// for (int i = 0; i < ROBOT_COUNT; i++) {
	// 	printf("robot: %f, x: %f, y: %f\n", robot_pos[i][3], robot_pos[i][0], robot_pos[i][1]);
	// }
}
