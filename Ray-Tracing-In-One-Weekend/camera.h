#pragma once
#include"rtweekend.h"
#include"color.h"
#include"hittable.h"
#include<iostream>
#include"material.h"

class camera
{
public:
    double aspect_ratio = 1.0;  // ͼ������߶ȵı�ֵ
    int    image_width = 100;  // ��Ⱦͼ���ȣ���������Ϊ��λ
    int    samples_per_pixel = 10;//ÿ�����ص������������
    int    max_depth = 10;      //�����������е�����������
    color  background;          // �����ı�����ɫ
    
    double vfov = 90;           //��ֱ�ӽ�
    point3 lookfrom = point3(0, 0, 0); // ��������ӽ�
    point3 lookat = point3(0, 0, -1);  // ������Ŀ���
    vec3   vup = vec3(0, 1, 0);       // ������Ϸ���

    double defocus_angle = 0;   //��Ȧ���ڽǶ�
    double focus_dist = 10;     //����

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
    int    image_height;   // ��Ⱦͼ��߶�
    double pixel_samples_scale;  // ������ɫ�ı���
    point3 center;         // ���������
    point3 pixel00_loc;    // ���� 0, 0 ��λ��
    vec3   pixel_delta_u;  // �������ҵ�ƫ����
    vec3   pixel_delta_v;  // �������µ�ƫ����
    vec3   u, v, w;        // ���֡��������
    vec3   defocus_disk_u; //ɢ���̵�ˮƽ�뾶
    vec3   defocus_disk_v; //ɢ���̵Ĵ�ֱ�뾶

    void initialize() {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        pixel_samples_scale = 1.0 / samples_per_pixel;

        center = lookfrom;

        // ȷ���ӿڳߴ�
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta / 2);
        auto viewport_height = 2 * h * focus_dist;
        auto viewport_width = viewport_height * (double(image_width) / image_height);

        // �����������ϵ�� u,v,w ��λ������
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // ������ˮƽ�ʹ�ֱ�ӿڵĴ�С
        vec3 viewport_u = viewport_width * u;   
        vec3 viewport_v = viewport_height * -v; 

        // ��������ص����ص�ˮƽ�ʹ�ֱ��������
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // �������Ͻ����ص�λ��
        auto viewport_upper_left = center - (focus_dist * w) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);


        // �������ɢ���̻�����
        auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;

    }

    ray get_ray(int i, int j)const
    {
        //����һ������ɢ���̲�ָ������Ĺ���
        //��i��j����Χ
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
        //����[-0.5, 0-.5] - [+0.5, +0.5] ��λ�������е������
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    point3 defocus_disk_sample()const
    {
        //�������ɢ�����е������
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