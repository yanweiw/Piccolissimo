#pragma once
#include "kilolib.h"

#define R1 60
#define R2 125
#define R3 160
#define TICKSTOLISTEN 10
#define ROTATIONPERTICK 0.05
#define DISTANCEPERTICK 0.5
#define RANDOM_CONST 0.02
#define REPUL_CONST 0.2


class mykilobot : public kilobot
{
	unsigned char dist;
	float theta;
	message_t out_message;
	int newcycle = 0;
	long int motion_timer = 0;
	float motion_vec_x = 0;
	float motion_vec_y = 0;

	//main loop
	void loop()
	{
		if (id == 0) {
			set_color(RGB(3,0,0));
		} else if (id == 1){
			set_color(RGB(0,3,0));
		} else {
			set_color(RGB(0,0,3));
		}

		if (newcycle) {
			motion_timer = kilo_ticks;
			motion_vec_x = sin(angle_to_light);
			motion_vec_y = cos(angle_to_light);
			newcycle = 0;
		} else {
			// add randomness
			float randomized_x = motion_vec_x + RANDOM_CONST*(rand()%10);
			float randomized_y = motion_vec_y + RANDOM_CONST*(rand()%10);
			// convert back to polar coords
			float bearing = atan2(randomized_x, randomized_y);
			float disttogo = distance(0, 0, randomized_x, randomized_y);
			int tickstorotate = (int)(abs(bearing/ROTATIONPERTICK));
			int tickstomove = (int)(disttogo/DISTANCEPERTICK);

			if (kilo_ticks <= motion_timer + TICKSTOLISTEN) {
				// listen for incoming messages
			} else if (kilo_ticks <= motion_timer + TICKSTOLISTEN + tickstorotate) {
				// turn phase
				spinup_motors();
				if (bearing >= 0) {
					set_motors(0,kilo_turn_right);
				} else {
					set_motors(kilo_turn_left,0);
				}
			} else if (kilo_ticks <= motion_timer + TICKSTOLISTEN + tickstorotate + tickstomove)  {
				// move forward phase
				spinup_motors();
				set_motors(kilo_straight_left, kilo_straight_right);
			} else {
					newcycle = 1;
			}
		}
	}

	//executed once at start
	void setup()
	{
		motion_timer = kilo_ticks;
		newcycle = 1;
	}

	//executed on successfull message send
	void message_tx_success()
	{

	}

	//sends message at fixed rate
	message_t *message_tx()
	{
		static int count = rand();
		count--;
		if (!(count % TICKSTOLISTEN))
		{
			return &out_message;
		}
		return NULL;
	}

	//receives message
	void message_rx(message_t *message, distance_measurement_t *distance_measurement,float t)
	{
		if (kilo_ticks <= motion_timer + TICKSTOLISTEN) {
			dist = estimate_distance(distance_measurement);
			theta=t;
			int r;
			switch (id) {
				case 0: r = R1; break;
				case 1: r = R2; break;
				case 2: r = R3; break;
			}
			if (dist < 2 * r) {
				float x_repel = REPUL_CONST * (-sin(theta) * (2*r - dist));
				float y_repel = REPUL_CONST * (-cos(theta) * (2*r - dist));
				motion_vec_x += x_repel;
				motion_vec_y += y_repel;
			}
		}
	}
};
