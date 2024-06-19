#pragma once
#include"rtweekend.h"
#include"color.h"
#include"ray.h"
#include"hittable.h"
#include"texture.h"
#include"onb.h"
#include"pdf.h"

using namespace std;

class scatter_record 
{
public:
    color attenuation;
    shared_ptr<pdf> pdf_ptr;
    bool skip_pdf;
    ray skip_pdf_ray;
};

class material
{
public:
    virtual ~material() = default;
    virtual bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const 
    {
        return false;
    }
    virtual color emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const
    {
        return color(0, 0, 0);
    }

    virtual double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered)const
    {
        return 0;
    }
};

class lambertian : public material
{
public:
    lambertian(const color& albedo) : tex(std::make_shared<solid_color>(albedo)) {}
    lambertian(std::shared_ptr<textrue> tex) : tex(tex) {}
    bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override
    {
        srec.attenuation = tex->value(rec.u, rec.v, rec.p);
        srec.pdf_ptr = make_shared<cosine_pdf>(rec.normal);
        srec.skip_pdf = false;
        return true;
    }

    double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const
    {
        auto cos_theta = dot(rec.normal, unit_vector(scattered.direction()));
        return cos_theta < 0 ? 0 : cos_theta / pi;
        //return 1 / (2 * pi);
    }

private:
    std::shared_ptr<textrue>tex;
};

class metal : public material
{
public:
    metal(const color& a, double fuzz) : albedo(a), fuzz(fuzz < 1 ? fuzz : 1) { }

    bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override
    {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        reflected = unit_vector(reflected + fuzz * random_unit_vector());

        srec.attenuation = albedo;
        srec.pdf_ptr = nullptr;
        srec.skip_pdf = true;
        srec.skip_pdf_ray = ray(rec.p, reflected, r_in.time());

        return true;
    }

private:
    color albedo;
    double fuzz;
};

class dielectric : public material
{
public:
    dielectric(double refraction_index) : refraction_index(refraction_index) {}

    bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override
     {
        srec.attenuation = color(1.0, 1.0, 1.0);
        srec.pdf_ptr = nullptr;
        srec.skip_pdf = true;

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

        srec.skip_pdf_ray = ray(rec.p, direction, r_in.time());
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

    bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override
    {
        return false;
    }

    color emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p)const override 
    {
        if (!rec.front_face)
            return color(0, 0, 0);
        return emit->value(u, v, p);
    }

private:
    std::shared_ptr<textrue> emit;
};

class isotropic : public material
{
public:
    isotropic(const color& albedo) : tex(make_shared<solid_color>(albedo)) {}
    isotropic(shared_ptr<textrue> tex) : tex(tex) {}

    bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override
    {
        srec.attenuation = tex->value(rec.u, rec.v, rec.p);
        srec.pdf_ptr = make_shared<sphere_pdf>();
        srec.skip_pdf = false;
        return true;
    }

    double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered)const override 
{
        return 1 / (4 * pi);
    }

private:
    shared_ptr<textrue> tex;
};