#pragma once

#include <stdio.h>
#include <math.h>
#include <vector>

struct TimeValue
{
	float time;
	float value;
};


class Keytimes
{
private:
	std::vector<struct TimeValue*> tvs;

public:
	Keytimes();
	~Keytimes();
	void	AddTimeValue(float, float);
	float	GetFirstTime();
	float	GetLastTime();
	int		GetNumKeytimes();
	float	GetValue(float);
	void	Init();
	void	PrintTimeValues();
};

double cubic_y(double t, double x1, double y1, double x2, double y2);
double y_value_at_time(double t, double bias, double range, double x1, double y1, double x2, double y2);
