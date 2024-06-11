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

int main() 
{
    int inside_circle = 0;
    int inside_circle_stratified = 0;
    int sqrt_N = 1000;
    for (int i = 0; i < sqrt_N; i++) 
    {
        for (int j = 0; j < sqrt_N; j++) 
        {
            auto x = random_double(-1, 1);
            auto y = random_double(-1, 1);
            if (x * x + y * y < 1)
                inside_circle++;
            x = 2 * ((i + random_double()) / sqrt_N) - 1;
            y = 2 * ((j + random_double()) / sqrt_N) - 1;
            if (x * x + y * y < 1)
                inside_circle_stratified++;
        }
    }

    std::cout << std::fixed << std::setprecision(12);
    std::cout
        << "Regular    Estimate of Pi = "
        << (4.0 * inside_circle) / (sqrt_N * sqrt_N) << '\n'
        << "Stratified Estimate of Pi = "
        << (4.0 * inside_circle_stratified) / (sqrt_N * sqrt_N) << '\n';
}