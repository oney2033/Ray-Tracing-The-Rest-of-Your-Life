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

int main() 
{
	int a = 0, b = 2;
	int N = 1000000;
	auto sum = 0.0;
	for (int i = 0; i < N; i++)
	{
		auto x = random_double(a, b);
		sum += x * x;
	}
	std::cout << std::fixed << std::setprecision(12);
	std::cout << "I = " << (b - a) * (sum / N) << "\n";
}