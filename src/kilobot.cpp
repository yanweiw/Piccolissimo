#pragma once
#include "kilolib.h"

// calibration
#define BEARING_TO_RAD M_PI/(4*0.785398)
#define R1 120
#define R2 48
#define R3 96

class mykilobot : public kilobot
{

	unsigned char dist;
	float theta;
	message_t out_message;
	int newcycle = 0;
	long int motion_timer = 0;
	float motion_vec_x = 0;
	float motion_vec_y = 0;
	float repul_const = 0.2;


	//main loop
	void loop()
	{
		if (newcycle) {
			motion_timer = kilo_ticks;
			motion_vec_x = sin(angle_to_light);
			motion_vec_y = cos(angle_to_light);
			newcycle = 0;
		} else {
			// convert back to polar coords
			float bearing = atan2(motion_vec_x, motion_vec_y);
			float disttogo = distance(0, 0, motion_vec_x, motion_vec_y);
			int tickstorotate = (int)(abs(bearing/0.05));
			int tickstomove = (int)(disttogo/0.5);
			printf("bearing: %f, dist: %f\n",angle_to_light, disttogo);

			if (kilo_ticks <= motion_timer + 20) {
				// listen for incoming messages
			} else if (kilo_ticks <= motion_timer + 20 + tickstorotate) {
				// turn phase
				spinup_motors();
				if (bearing >= 0) {
					set_motors(0,kilo_turn_right);
				} else {
					set_motors(kilo_turn_left,0);
				}
			} else if (kilo_ticks <= motion_timer + tickstorotate + tickstomove)  {
				// move forward phase
				spinup_motors();
				// printf("disttogo: %f\n", disttogo);
				// if (disttogo > 0) {
					set_motors(kilo_straight_left, kilo_straight_right);
				// }
			} else {
					newcycle = 1;
			}
		}




		// send out message
		out_message.type = NORMAL;
		out_message.data[0] = id;
		out_message.crc = message_crc(&out_message);

		switch (id) {
			// case 0: set_color(RGB(3,0,0)); break;
			// case 1: set_color(RGB(0,3,0)); break;
			// case 3: set_color(RGB(0,0,3)); break;
		}
	}

	//executed once at start
	void setup()
	{
		if (id == 1) {
			set_color(RGB(3,0,0));
		} else {
			set_color(RGB(0,3,0));
		}
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
		if (!(count % 20))
		{
			return &out_message;
		}
		return NULL;
	}

	//receives message
	void message_rx(message_t *message, distance_measurement_t *distance_measurement,float t)
	{
		// if (kilo_ticks <= motion_timer + 20) {
			dist = estimate_distance(distance_measurement);
			theta=t;
			// printf("receives message, dist: %d\n", dist);
			if (dist < 2*R1) {
				float x_repel = repul_const * (-sin(theta) * (2*R1 - dist));
				float y_repel = repul_const * (-cos(theta) * (2*R1 - dist));
				// printf("x_repel: %f, y_repel: %f, dist: %d\n", x_repel, y_repel, dist);
				motion_vec_x += x_repel;
				motion_vec_y += y_repel;
			}
		// }
	}
};
