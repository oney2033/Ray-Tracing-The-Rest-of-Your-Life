class sphere : public hittable {
public:

    //��ֹ����
    sphere(point3 _center, double _radius,std::shared_ptr<material>_material)
        : center1(_center), radius(_radius),mat(_material),is_moving(false) 
    {
        auto rvec = vec3(radius, radius, radius);
        bbox = aabb(center1 - rvec, center1 + rvec);
    }

    aabb bounding_box()const override { return bbox; }

    //�˶�����
    sphere(point3 _center1,point3 _center2, double _radius,std::shared_ptr<material>_material) 
        : center1(_center1), radius(_radius),mat(_material),is_moving(true) 
    {
        auto rvec = vec3(radius, radius, radius);
        aabb box1(_center1 - rvec, _center1 + rvec);
        aabb box2(_center2 - rvec, _center2 + rvec);
        bbox = aabb(box1, box2);
        center_vec = _center2 - _center1;
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override
    {
        point3 center = is_moving ? sphere_center(r.time()) : center1;

        vec3 oc = r.origin() - center;
        auto a = r.direction().length_squared();
        auto half_b = dot(oc, r.direction());
        auto c = oc.length_squared() - radius * radius;

        auto discriminant = half_b * half_b - a * c;
        if (discriminant < 0) return false;
        auto sqrtd = sqrt(discriminant);

        // �ҵ�λ�ڿɽ��ܷ�Χ�ڵ�����ĸ�
        auto root = (-half_b - sqrtd) / a;
        if (!ray_t.surrounds(root)) 
        {
            root = (-half_b + sqrtd) / a;
            if (!ray_t.surrounds(root))
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);
        get_sphere_uv(outward_normal, rec.u, rec.v);
        rec.mat = mat;

        return true;
    }

private:
    point3 center1;
    double radius;
    std::shared_ptr<material> mat;
    bool is_moving;
    vec3 center_vec;
    aabb bbox;

    point3 sphere_center(double time)const
    {
        //����ʱ��� center1 �� center2 �����������ɣ����� t = 0 ���� center1��t=1 ���� center2��
        return center1 + time * center_vec;
    }

    static void get_sphere_uv(const point3& p, double& u, double& v)
    {
        // p:  һ���뾶Ϊ 1 ����������ԭ��Ϊ���ĵĸ����㡣
       //  u:  ���ش� X=-1 �� Y ��ĽǶ�ֵ [0,1]��
       //  v:  ���ش� Y=-1 �� Y=+1 �ĽǶ�ֵ [0,1]��
       //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
       //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
       //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>
        auto theta = acos(-p.y());
        auto phi = atan2(-p.z(), p.x()) + pi;
        u = phi / (2 * pi);
        v = theta / pi;
    }

};