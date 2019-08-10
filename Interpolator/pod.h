#pragma once
#include <math.h>

class pod
{
public:
	double pos[2] = { 0.5, 0.5 };
	double rot = 0.0;
	double speed[2] = { 0.0, 0.0 };
	double rot_speed = 0.0;

	void fast_fmod(double& value, double divider)
	{
		while (value < 0.0) value += divider;
		while (value >= divider) value -= divider;
	}
	
	void move(double thrust, double side_thrust_1, double side_thrust_2)
	{
		speed[0] = (speed[0] + thrust * cos(rot)) * 0.9;
		speed[1] = (speed[1] + thrust * sin(rot)) * 0.9;
		rot_speed = (rot_speed + side_thrust_1 - side_thrust_2) * 0.9;
		rot += rot_speed * 0.1;
		fast_fmod(rot, 6.283185307179586476925286766559);
		for (int i = 0; i < 2; i++)
		{
			pos[i] += speed[i] * 0.003;
			fast_fmod(pos[i], 1.0);
		}
	}
};

