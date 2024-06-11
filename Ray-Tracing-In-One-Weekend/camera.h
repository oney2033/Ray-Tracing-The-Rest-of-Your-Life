#pragma once
#include"rtweekend.h"
#include"color.h"
#include"hittable.h"
#include<iostream>
#include"material.h"

class camera
{
public:
    double aspect_ratio = 1.0;  // 图像宽度与高度的比值
    int    image_width = 100;  // 渲染图像宽度（以像素数为单位
    int    samples_per_pixel = 10;//每个像素的随机样本总数
    int    max_depth = 10;      //反弹到场景中的最大光线数量
    color  background;          // 场景的背景颜色
    
    double vfov = 90;           //垂直视角
    point3 lookfrom = point3(0, 0, 0); // 点相机的视角
    point3 lookat = point3(0, 0, -1);  // 点相机的看向
    vec3   vup = vec3(0, 1, 0);       // 相机向上方向

    double defocus_angle = 0;   //光圈开口角度
    double focus_dist = 10;     //焦点

    void render(const hittable& world) 
    {
        initialize();

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; j++) 
        {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; i++) 
            {
                color pixel_color(0, 0, 0);
                for (int sample = 0; sample < samples_per_pixel; sample++)
                {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth,world);
                }
                write_color(std::cout, pixel_samples_scale * pixel_color);
            }
        }

        std::clog << "\rDone.                 \n";
    }

private:
    int    image_height;   // 渲染图像高度
    double pixel_samples_scale;  // 像素颜色的比例
    point3 center;         // 相机的中心
    point3 pixel00_loc;    // 像素 0, 0 的位置
    vec3   pixel_delta_u;  // 像素向右的偏移量
    vec3   pixel_delta_v;  // 像素向下的偏移量
    vec3   u, v, w;        // 相机帧基础向量
    vec3   defocus_disk_u; //散焦盘的水平半径
    vec3   defocus_disk_v; //散焦盘的垂直半径

    void initialize() {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        pixel_samples_scale = 1.0 / samples_per_pixel;

        center = lookfrom;

        // 确定视口尺寸
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta / 2);
        auto viewport_height = 2 * h * focus_dist;
        auto viewport_width = viewport_height * (double(image_width) / image_height);

        // 计算相机坐标系的 u,v,w 单位基向量
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // 计算沿水平和垂直视口的大小
        vec3 viewport_u = viewport_width * u;   
        vec3 viewport_v = viewport_height * -v; 

        // 计算从像素到像素的水平和垂直增量向量
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // 计算左上角像素的位置
        auto viewport_upper_left = center - (focus_dist * w) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);


        // 计算相机散焦盘基向量
        auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;

    }

    ray get_ray(int i, int j)const
    {
        //构造一条来自散焦盘并指向随机的光线
        //在i和j的周围
        auto offset = sample_square();
        auto pixel_sample = pixel00_loc
                             + ((i + offset.x()) * pixel_delta_u)
                             + ((j + offset.y()) * pixel_delta_v);

        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;
        auto ray_time = random_double();

        return ray(ray_origin, ray_direction,ray_time);
    }

    vec3 sample_square()const
    {
        //返回[-0.5, 0-.5] - [+0.5, +0.5] 单位正方形中的随机点
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    point3 defocus_disk_sample()const
    {
        //返回相机散焦盘中的随机点
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

	color ray_color(const ray& r, int depth, const hittable& world)const
	{
		hit_record rec;

        if (depth <= 0)
            return color(0, 0, 0);

        if (!world.hit(r, interval(0.001, infinity), rec))
            return background;

        ray scattered;
        color attenuation;
        color color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);

        if (!rec.mat->scatter(r, rec, attenuation, scattered))
            return color_from_emission;

        color color_from_scatter = attenuation * ray_color(scattered, depth - 1, world);

        return color_from_emission + color_from_scatter;
	}

};