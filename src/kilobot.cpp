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
#define RADPERINT 0.02472


class mykilobot : public kilobot
{
	unsigned char dist;
	float theta;
	message_t out_message;
	int newcycle = 0;
	long int motion_timer = 0;
	float motion_vec_x = 0;
	float motion_vec_y = 0;
	float angletoturn = 0;
	int alreadyset=0;
	int id1;
	int id2; // use id1 and id2, together 16 bits, as identity in communication
	float align_vec_x = 0;
	float align_vec_y = 0;


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

		out_message.type=NORMAL;
		out_message.data[0]=id1;
		out_message.data[1]=id2;

		spinup_motors();
		motion_timer = kilo_ticks;
		float bearing = atan2(align_vec_x, align_vec_y);
		int tickstorotate = (int)(abs(bearing/ROTATIONPERTICK))/10;
		printf("ticks to rotate: %d\n", tickstorotate);
		if (kilo_ticks < motion_timer + tickstorotate) {
			if (bearing <= 0) {
				set_motors(0,kilo_turn_right);
			} else {
				set_motors(kilo_turn_left,0);
			}
		}
		int tickstolight = (int)(abs(angle_to_light/10)/ROTATIONPERTICK);
		printf("ticks to light: %d\n", tickstolight);
		if (kilo_ticks < motion_timer + tickstorotate + tickstolight) {
			if (angle_to_light >= 0) {
				set_motors(0,kilo_turn_right);
			} else {
				set_motors(kilo_turn_left,0);
			}
		}
		// if (newcycle) {
		// 	motion_timer = kilo_ticks;
		// 	// motion_vec_x = sin(angle_to_light);
		// 	// motion_vec_y = cos(angle_to_light);
		//
		// 	newcycle = 0;
		// } else {
		// 	// convert back to polar coords
		// 	// float bearing = atan2(randomized_x, randomized_y);
		// 	// float disttogo = distance(0, 0, randomized_x, randomized_y);
		// 	// int tickstorotate = (int)(abs(angletoturn/ROTATIONPERTICK));
		// 	// int tickstomove = (int)(disttogo/DISTANCEPERTICK);
		//
		// 	if (kilo_ticks <= motion_timer + TICKSTOLISTEN) {
		// 		// listen for incoming messages
		// 	// } else if (kilo_ticks <= motion_timer + TICKSTOLISTEN + tickstorotate) {
		// 	// 	// turn phase
		// 	// 	spinup_motors();
		// 	// 	if (angletoturn >= 0) {
		// 	// 		set_motors(0,kilo_turn_right);
		// 	// 	} else {
		// 	// 		set_motors(kilo_turn_left,0);
		// 	// 	}
		// 	// } else if (kilo_ticks <= motion_timer + TICKSTOLISTEN + tickstorotate + tickstomove)  {
		// 	// 	// move forward phase
		// 	// 	spinup_motors();
		// 	// 	set_motors(kilo_straight_left, kilo_straight_right);
		// 	}
		// 	if (kilo_ticks > motion_timer + 50){
		// 			newcycle = 1;
		// 	}
		// }

		out_message.crc=message_crc(&out_message);
	}

	//executed once at start
	void setup()
	{
		motion_timer = kilo_ticks;
		newcycle = 1;
		out_message.type = NORMAL;
		id1 = rand() % 256;
		id2 = rand() % 256;
		out_message.data[0] = id1; // sender id
		out_message.data[1] = id2;
		out_message.data[2] = 255; // 255 signifies blank message
		out_message.data[3] = 0;
		out_message.data[4] = 0; // target recepient id , currently blank
		out_message.crc = message_crc(&out_message);
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
		if (!(count % 2))
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
		int angle_int = message->data[2];
		if (angle_int != 255) { // 255 -> blank; 0-127 -> -PI-0; 127-254 -> 0-PI
			if (message->data[3]==id1 && message->data[4]==id2){
				angletoturn = (theta - (angle_int - 127) * RADPERINT - PI);
				align_vec_x = (-sin(angletoturn/10));
				align_vec_y = (-cos(angletoturn/10));

				// if (angletoturn <= 0) {
				// 	angletoturn = (PI + angletoturn) / 2;
				// } else {
				// 	angletoturn = (-PI + angletoturn) / 2;
				// }

			}
		}
		out_message.data[2] = (int)((theta + PI) / RADPERINT);
		out_message.data[3] = message->data[0];
		out_message.data[4] = message->data[1];


			// if (dist < 2 * r) {
			// 	float x_repel = REPUL_CONST * (-sin(theta) * (2*r - dist));
			// 	float y_repel = REPUL_CONST * (-cos(theta) * (2*r - dist));
			// 	motion_vec_x += x_repel;
			// 	motion_vec_y += y_repel;
			// }
		// }
	}

};
