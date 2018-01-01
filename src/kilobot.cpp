#pragma once
#include "kilolib.h"

#define ROTATIONPERTICK 0.05
#define DISTANCEPERTICK 0.5
#define RADPERINT 0.02472
#define BEACON_NUM 3

class mykilobot : public kilobot
{
	unsigned char dist;
	float theta;
	message_t out_message;
	long int motion_timer = 0;
	int phase = 0; // phase change occurs when see new robot whose id denotes the new phase
	// 0 phase is the initial state
	long int phase_start[BEACON_NUM + 1]; // array to record start time of each phase
	int phase_interval[BEACON_NUM + 1];
	int motion_flag = 0; // 0 means stop, nonzero means move at that phase
	int prev_motion = 0; // record previous motion phase

	//main loop
	void loop()
	{
		set_color(RGB(1,1,1));

		if (id == 0){
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

			spinup_motors();
			set_motors(50,0);

			float phase_weight = phase_interval[phase] / (phase_interval[1] + phase_interval[2] + phase_interval[3] + 1.0);
			int thrust_freq = 2 + (int)(6 * (1 - phase_weight));
			if (motion_timer % thrust_freq == 0 )
			{
				set_motors(-50, -50);
			}
			motion_timer++;
		} else {
			out_message.type=NORMAL;
			out_message.data[0] = id;
			out_message.crc=message_crc(&out_message);
		}
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
		if (id == 0 && ((theta > 6.2 && theta < 6.4) || (theta < 0.1 && theta > -0.1))) {
			int next_phase = message->data[0];
			if (phase != next_phase)
			{
				phase_start[next_phase] = motion_timer;
				int interval = phase_start[next_phase] - phase_start[phase];
				if (interval > 3) {
					phase_interval[phase] = interval;
					// printf("phase: %d, angle: %d\n", phase, phase_interval[phase]);
					phase = next_phase;
				}
			}
		}
	}
};
