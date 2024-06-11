#pragma once
#include"ray.h"
#include"rtweekend.h"
#include"aabb.h"

class material;

class hit_record
{
public:
    point3 p;      // 碰撞点
    vec3 normal;   // 碰撞点处的法线向量
    double t;      // 光线参数 t，表示光线上的点 P(t) = A + tB
    bool front_face; //法线的朝向
    std::shared_ptr<material>mat;//材质的列表 
    double u;   //纹理横坐标
    double v;   //纹理纵坐标


    void set_face_normal(const ray& r, const vec3& outward_normal)
    {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable
{
public:
	virtual ~hittable() = default;
    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
    virtual aabb bounding_box() const = 0;
};

class translate : public hittable
{
public:
    translate(std::shared_ptr<hittable> object, const vec3& offset)
        : object(object), offset(offset)
    {
        bbox = object->bounding_box() + offset;
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec)const override
    {
        // 将光线原点减去移动偏移量
        ray offset_r(r.origin() - offset, r.direction(), r.time());


        // 确定沿着偏移射线是否存在交点（如果存在，则在哪里）
        if (!object->hit(offset_r, ray_t, rec))
            return false;

        // 将交点加上移动偏移量
        rec.p += offset;

        return true;
    }

    aabb bounding_box()const override { return bbox; }
private:
    std::shared_ptr<hittable>object;
    vec3 offset;
    aabb bbox;
};

class rotate_y :public hittable
{
public:
    rotate_y(std::shared_ptr<hittable> object, double angle) : object(object) 
    {
        auto radians = degrees_to_radians(angle);
        sin_theta = sin(radians);
        cos_theta = cos(radians);
        bbox = object->bounding_box();

        point3 min(infinity, infinity, infinity);
        point3 max(-infinity, -infinity, -infinity);

        for (int i = 0; i < 2; i++) 
        {
            for (int j = 0; j < 2; j++) 
            {
                for (int k = 0; k < 2; k++) 
                {
                    auto x = i * bbox.x.max + (1 - i) * bbox.x.min;
                    auto y = j * bbox.y.max + (1 - j) * bbox.y.min;
                    auto z = k * bbox.z.max + (1 - k) * bbox.z.min;

                    auto newx = cos_theta * x + sin_theta * z;
                    auto newz = -sin_theta * x + cos_theta * z;

                    vec3 tester(newx, y, newz);

                    for (int c = 0; c < 3; c++) 
                    {
                        min[c] = fmin(min[c], tester[c]);
                        max[c] = fmax(max[c], tester[c]);
                    }
                }
            }
        }

        bbox = aabb(min, max);
    }

    aabb bounding_box() const override { return bbox; }
private:
    std::shared_ptr<hittable> object;
    double sin_theta;
    double cos_theta;
    aabb bbox;

public:
    bool hit(const ray& r, interval ray_t, hit_record& rec)const override
    {
        // 将光线从世界空间更改为对象空间
        auto origin = r.origin();
        auto direction = r.direction();

        origin[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[2];
        origin[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];

        direction[0] = cos_theta * r.direction()[0] - sin_theta * r.direction()[2];
        direction[2] = sin_theta * r.direction()[0] + cos_theta * r.direction()[2];

        ray rotated_r(origin, direction, r.time());
        // 确定对象空间中是否存在交点（如果存在，则在哪里)
        if (!object->hit(rotated_r, ray_t, rec))
            return false;

        //将交点从对象空间更改为世界空间
        auto p = rec.p;
        p[0] = cos_theta * rec.p[0] + sin_theta * rec.p[2];
        p[2] = -sin_theta * rec.p[0] + cos_theta * rec.p[2];

        // 将法线从物体空间更改为世界空间
        auto normal = rec.normal;
        normal[0] = cos_theta * rec.normal[0] + sin_theta * rec.normal[2];
        normal[2] = -sin_theta * rec.normal[0] + cos_theta * rec.normal[2];

        rec.p = p;
        rec.normal = normal;

        return true;

    }
};