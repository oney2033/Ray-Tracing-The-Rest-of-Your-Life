#pragma once
#include<iostream>
#include"vec3.h"
#include"interval.h"

using color = vec3;

inline double linear_to_gamma(double linear_compoent)
{
    if (linear_compoent > 0)
        return std::sqrt(linear_compoent);
    return 0;
}

void write_color(std::ostream& out, const color& pixel_color)
{
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Apply a linear to gamma transform for gamma 2
    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);


    // 将 [0,1] 的值转换为范围 [0,255]。
    static const interval intensity(0.000, 0.999);
    int rbyte = int(256 * intensity.clamp(r));
    int gbyte = int(256 * intensity.clamp(g));
    int bbyte = int(256 * intensity.clamp(b));

    // 写出像素颜色的分量
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}