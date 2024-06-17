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

double f(double d) {
    return 8.0 * pow(d, 1.0 / 3.0);
}

double pdf(double x) {
    return (3.0 / 8.0) * x * x;
}

int main() {
    int N = 1;
    auto sum = 0.0;
    for (int i = 0; i < N; i++) {
        auto x = f(random_double());
        sum += x * x / pdf(x);
    }
    std::cout << std::fixed << std::setprecision(12);
    std::cout << "I = " << sum / N << '\n';
}