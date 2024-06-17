#include<iostream>
#include"color.h"
#include"vec3.h"
#include"ray.h"
#include "rtweekend.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include"camera.h"
#include"color.h"
#include"material.h"
#include"bvh.h"
#include"texture.h"
#include"quad.h"
#include"constant_medium.h"
#include <iomanip>
#include <math.h>
#include <stdlib.h>

using namespace std;

struct sample
{
	double x;
	double p_x;
};

bool compare_by_x(const sample& a, const sample& b)
{
	return a.x < b.x;
}

int main()
{
	int N = 10000;
	double sum = 0;
	std::vector<sample>samples;
	for (int i = 0; i < N; i++)
	{
		auto x = random_double(0, 2 * pi);
		auto sin_x = sin(x);
		auto p_x = exp(-x / (2 * pi)) * sin_x * sin_x;
		sum += p_x;
		sample this_sample = { x,p_x };
		samples.push_back(this_sample);
	}

	std::sort(samples.begin(), samples.end(), compare_by_x);
	double half_sum = sum / 2.0;
	double halfway_point = 0.0;
	double accum = 0.0;
	for (int i = 0; i < N; i++)
	{
		accum += samples[i].p_x;
		if (accum >= half_sum)
		{
			halfway_point = samples[i].x;
			break;
		}
	}
	std::cout << std::fixed << std::setprecision(12);
	std::cout << "Average = " << sum / N << '\n';
	std::cout << "Area under curve = " << 2 * pi * sum / N << '\n';
	std::cout << "Halfway = " << halfway_point << '\n';
}