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

double f(const vec3& d)
{
	auto cos_theta = d.z();
	return cos_theta * cos_theta * cos_theta;
}

double pdf(const vec3& d)
{
	return d.z() / pi;
}

int main() 
{
	int N = 1000000;
	auto sum = 0.0;
	for (int i = 0; i < N; i++)
	{
		vec3 d = random_cosine_direction();
		sum += f(d) / pdf(d);
	}
	std::cout << std::fixed << std::setprecision(12);
	std::cout << "PI/2 = " << pi / 2.0 << '\n';
	std::cout << "Estimate = " << sum / N << '\n';
}