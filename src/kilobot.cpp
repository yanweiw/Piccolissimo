#pragma once
#include "kilolib.h"

#define ROTATIONPERTICK 0.05
#define DISTANCEPERTICK 0.5
#define ALIGN_CONST 1
#define REPUL_CONST 80
#define COHES_CONST 0.0009
#define MIGRA_CONST 0.2
#define RADPERINT 0.02472

class mykilobot : public kilobot
{
	unsigned char dist;
	float theta;
	message_t out_message;
	long int motion_timer = 0;
	int phase = 0; // phase change occurs when see new robot whose id denotes the new phase
	// 0 phase is the initial state
	long int phase_start[4]; // array to record start time of each phase
	int phase_interval[4];
	int motion_flag = 0; // 0 means stop, nonzero means move at that phase
	int prev_motion = 0; // record previous motion phase

	//main loop
	void loop()
	{
		// printf("%d\n", id);

		set_color(RGB(1,1,1));
		if (id == 0){
			// printf("phase 1: %d, phase 2: %d, phase 3: %d\n", phase_interval[1], phase_interval[2], phase_interval[3]);
			switch (phase)
			{
				case 1:
				{
					set_color(RGB(1,0,0));
					break;
				}
				case 2:
				{
					set_color(RGB(0,1,0));
					break;
				}
				case 3:
				{
					set_color(RGB(0,0,1));
					break;
				}
			}
		}
		// if (angle_to_light < 0.3 && angle_to_light > -0.3){
			// set_color(RGB(0,1,0));
		// }
		// out_message.type=NORMAL;
		// out_message.data[0]=id1;
		// out_message.data[1]=id2;
    //
		// spinup_motors();
		// motion_timer = kilo_ticks;
		// separ_vec_x = 0.5 * separ_vec_x;
		// separ_vec_y = 0.5 * separ_vec_y;
		// cohes_vec_x = 0.6 * cohes_vec_x;
		// cohes_vec_y = 0.6 * cohes_vec_y;
		// migra_vec_x = sin(angle_to_light) * MIGRA_CONST;
		// migra_vec_y = cos(angle_to_light) * MIGRA_CONST;
		// motion_vec_x = align_vec_x - migra_vec_x - separ_vec_x - cohes_vec_x;
		// motion_vec_y = align_vec_y - migra_vec_y - separ_vec_y - cohes_vec_y;
		// float bearing = atan2(motion_vec_x, motion_vec_y);
		// float disttogo = distance(0, 0, motion_vec_x, motion_vec_y);
		// int tickstorotate = (int)(abs(bearing/ROTATIONPERTICK)/5);
		// int tickstomove = (int) (disttogo/DISTANCEPERTICK);
		// if (kilo_ticks < motion_timer + tickstorotate) {
		// 	if (bearing <= 0) {
		// 		set_motors(0,kilo_turn_right);
		// 	} else {
		// 		set_motors(kilo_turn_left,0);
		// 	}
		// }
    //
		// out_message.crc=message_crc(&out_message);
		if (id == 0){
			spinup_motors();
			set_motors(50,0);
			if (motion_flag && motion_flag == phase)
			{
				prev_motion = motion_flag;
				if (motion_timer % 4 == 0) {
					set_motors(50,50);
				}
			}
			motion_timer++;
		} else {
			out_message.type=NORMAL;
			out_message.data[0] = id;
			out_message.crc=message_crc(&out_message);
		}


		// printf("%f\n", angle_to_light);
	}

	//executed once at start
	void setup()
	{
		// out_message.type = NORMAL;
		// id1 = rand() % 256;
		// id2 = rand() % 256;
		// out_message.data[0] = id1; // sender id
		// out_message.data[1] = id2;
		// out_message.data[2] = 255; // 255 signifies blank message
		// out_message.data[3] = 0;
		// out_message.data[4] = 0; // target recepient id , currently blank
		// out_message.crc = message_crc(&out_message);
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
		if (!(count % 1))
		{
			return &out_message;
		}
		return NULL;
	}

	//receives message
	void message_rx(message_t *message, distance_measurement_t *distance_measurement,float t)
	{
		dist = estimate_distance(distance_measurement);
		theta=t;
		// if (id == 0 && message->data[0] == 3)
		// {
			// printf("id: %d, theta: %f\n", message->data[0], theta);
		// }
		if (id == 0 && ((theta < 6.5 && theta > 6.1) || (theta < 0.2 && theta > -0.2))) {
			int next_phase = message->data[0];
			if (phase != next_phase)
			{
				phase_start[next_phase] = motion_timer;
				phase_interval[phase] = phase_start[next_phase] - phase_start[phase];
				if (motion_flag && prev_motion) {// prev_motion is nonzero only when the new motion que has started
					motion_flag = 0;
				} else if (phase == prev_motion) {
					int longest_phase = 1;
					for (int i = 2; i <= 3; i++) {
						if (phase_interval[i] > phase_interval[i-1]) {longest_phase = i;}
					}
					motion_flag = longest_phase % 3 + 1;
					prev_motion = 0; // signifies a new motion_flag set and to be executed
				}
				phase = next_phase;
			}
			// if(id==0){printf("%d\n",phase);}
		}
		// int angle_int = message->data[2];
		// if (angle_int != 255) { // 255 -> blank; 0-127 -> -PI-0; 127-254 -> 0-PI
		// 	if (message->data[3]==id1 && message->data[4]==id2){
		// 		float angletoturn = (theta - (angle_int - 127) * RADPERINT - PI);
		// 		align_vec_x = ALIGN_CONST * (-sin(angletoturn));
		// 		align_vec_y = ALIGN_CONST * (-cos(angletoturn));
		// 	}
		// }
		// out_message.data[2] = (int)((theta + PI) / RADPERINT);
		// out_message.data[3] = message->data[0];
		// out_message.data[4] = message->data[1];
    //
		// separ_vec_x += REPUL_CONST * (-sin(theta)) / dist;
		// separ_vec_y += REPUL_CONST * (-cos(theta)) / dist;
		// cohes_vec_x += COHES_CONST * (sin(theta) * dist);
		// cohes_vec_y += COHES_CONST * (cos(theta) * dist);
	}
};
