#pragma once
#include"rtweekend.h"
#include"color.h"
#include"ray.h"
#include"hittable.h"
#include"texture.h"


class material
{
public:
	virtual ~material() = default;
	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const = 0;
    virtual color emitted(double u, double v, const point3& p) const 
    {
        return color(0, 0, 0);
    }
};

class lambertian : public material 
{
public:
	lambertian(const color& albedo) : tex(std::make_shared<solid_color>(albedo)){}
	lambertian(std::shared_ptr<textrue> tex) : tex(tex) {}
	bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)const override 
	{

		auto scatter_direction = rec.normal + random_unit_vector();

		// 当向量在所有维度都非常接近0时
			if (scatter_direction.near_zero())
				scatter_direction = rec.normal;

		scattered = ray(rec.p, scatter_direction, r_in.time());
		attenuation = tex->value(rec.u, rec.v, rec.p);
		return true;
	}

private:
	std::shared_ptr<textrue>tex;
};

class metal : public material 
{
public:
	metal(const color& a, double fuzz) : albedo(a), fuzz(fuzz < 1 ? fuzz : 1) { }

	bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)const override 
	{
		vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
		reflected = unit_vector(reflected + fuzz * random_unit_vector());
		scattered = ray(rec.p, reflected,r_in.time());
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);
	}

private:
	color albedo;
	double fuzz;
};

class dielectric : public material 
{
public:
    dielectric(double refraction_index) : refraction_index(refraction_index) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
        const override {
        attenuation = color(1.0, 1.0, 1.0);
        double ri = rec.front_face ? (1.0 / refraction_index) : refraction_index;

        vec3 unit_direction = unit_vector(r_in.direction());
        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = ri * sin_theta > 1.0;
        vec3 direction;

        if (cannot_refract || reflectance(cos_theta, ri) > random_double())
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, ri);

        scattered = ray(rec.p, direction,r_in.time());
        return true;
    }

private:
    //折射率
    double refraction_index;

    static double reflectance(double cosine, double refraction_index) {
        // 使用 Schlick 近似计算反射率。
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};

class diffuse_light : public material 
{
public:
    diffuse_light(std::shared_ptr<textrue> a) : emit(a) {}
    diffuse_light(color c) : emit(std::make_shared<solid_color>(c)) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)const override 
    {
        return false;
    }

    color emitted(double u, double v, const point3& p) const override 
    {
        return emit->value(u, v, p);
    }

private:
    std::shared_ptr<textrue> emit;
};

class isotropic : public material 
{
public:
    isotropic(color c) : albedo(std::make_shared<solid_color>(c)) {}
    isotropic(std::shared_ptr<textrue> a) : albedo(a) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)const override 
    {
        scattered = ray(rec.p, random_unit_vector(), r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

private:
    std::shared_ptr<textrue> albedo;
};