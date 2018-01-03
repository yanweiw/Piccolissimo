#pragma once
#include "kilolib.h"

#define BEACON_NUM 8
#define ROBOT_COUNT 10

class mykilobot : public kilobot
{
	unsigned char dist;
	float theta;
	message_t out_message;
	long int motion_timer = 0;
	int phase; // phase change occurs when see new robot whose id denotes the new phase
	long int phase_start[ROBOT_COUNT]; // array to record start time of each phase
	int phase_interval[ROBOT_COUNT];
	int robot_interval[ROBOT_COUNT];
	int too_close;

	//main loop
	void loop()
	{

		set_color(RGB(1,1,1));
		// printf("%d\n", id);

		if (id >= BEACON_NUM){
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
				case 4:
				{
					set_color(RGB(1,1,0));
					break;
				}
				case 5:
				{
					set_color(RGB(0,1,1));
					break;
				}
				case 0:
				{
					set_color(RGB(1,0,1));
					break;
				}
			}

			spinup_motors();
			set_motors(50,0);

			int thrust_freq = 2;
			if (phase_interval[phase] > 5 && phase_interval[phase] < 42) {
				thrust_freq += 1;
			}
			if (phase_interval[phase] > 10 && phase_interval[phase] < 31) {
				thrust_freq += 1;
			}
			if (phase_interval[phase] > 15 && phase_interval[phase] < 26) {
				thrust_freq += 1;
			}
			if (phase_interval[phase] > 19 && phase_interval[phase] < 23) {
				thrust_freq += 1;
			}
			// float phase_weight = phase_interval[phase] / 125.6;
			// thrust_freq = 2 + (int)(6 * (1 - phase_weight));
			// if (phase >= BEACON_NUM) {thrust_freq =5;}
			// if (too_close) {thrust_freq = 2;}
			if (motion_timer % thrust_freq == 0 )
			{
				set_motors(-50, -50);
			}
			motion_timer++;
		}
		out_message.type=NORMAL;
		out_message.data[0] = id;
		out_message.crc=message_crc(&out_message);
	}

	//executed once at start
	void setup()
	{
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
		if (dist < 50 && ((theta > 6.2 && theta < 6.4) || (theta < 0.1 && theta > -0.1))) {
			too_close = 1;
		} else {too_close = 0;}
		if (id >= BEACON_NUM && ((theta > 6.2 && theta < 6.4) || (theta < 0.1 && theta > -0.1))) {
			int next_phase = message->data[0];
			if (phase != next_phase)
			{
				int interval = motion_timer - phase_start[phase];
				// if (interval < 20) { // 14 corresponds to 40 degree
					// phase_interval[next_phase] = 0;
				// } else {
					// if (id == 9) {printf("robot: %d, theta: %f\n", next_phase, theta);}
					phase_interval[phase] = interval;
					if (id == 8) {printf("phase: %d, angle: %d\n", phase, phase_interval[phase]);}
					phase = next_phase;
					phase_start[phase] = motion_timer;
				// }
			}
		}
	}
};
