#pragma once
#include "kilolib.h"

#define ROBOT_COUNT 1
#define BEACON_NUM 6

class mykilobot : public kilobot
{
	unsigned char dist;
	float theta;
	message_t out_message;
	long int motion_timer = 0;
	int phase; // phase change occurs when see new robot whose id denotes the new phase
	long int phase_start[BEACON_NUM + ROBOT_COUNT]; // array to record start time of each phase
	int phase_interval[BEACON_NUM + ROBOT_COUNT];

	//main loop
	void loop()
	{
		set_color(RGB(1,1,1));
		printf("%d\n", id);

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

			float phase_weight = phase_interval[phase] / 125.6;
			int thrust_freq = 2 + (int)(12 * (1 - phase_weight));
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
		if (id >= BEACON_NUM && ((theta > 6.2 && theta < 6.4) || (theta < 0.1 && theta > -0.1))) {
			int next_phase = message->data[0];
			if (phase != next_phase)
			{
				phase_start[next_phase] = motion_timer;
				int interval = phase_start[next_phase] - phase_start[phase];
				if (interval <= 14) { // 14 corresponds to 40 degree
					phase_interval[phase] = 0;
				} else {
					phase_interval[phase] = interval;
					printf("phase: %d, angle: %d\n", phase, phase_interval[phase]);
					phase = next_phase;
				}
			}
		}
	}
};
