#ifndef HITABLEH
#define HITABLEH

#include "ray.h"

inline float ffmin(float a, float b) { return a < b ? a : b; }
inline float ffmax(float a, float b) { return a > b ? a : b; }

class material;

struct hit_record {
    float t;
	float u;
	float v;
    vec3 p;
    vec3 normal;
    material *mat_ptr;
};

// Axis Aligned Bounding Box (AABB)
class aabb {
public:
	aabb() {}
	aabb(const vec3& a, const vec3& b) : _min(a), _max(b) {}

	vec3 min() const { return _min; }
	vec3 max() const { return _max; }

	bool hit(const ray& r, float tmin, float tmax) const {
		for (int i = 0; i < 3; ++i) {
			float t0 = ffmin((_min[i] - r.origin()[i]) / r.direction()[i],
				(_max[i] - r.origin()[i]) / r.direction()[i]);
			float t1 = ffmax((_min[i] - r.origin()[i]) / r.direction()[i],
				(_max[i] - r.origin()[i]) / r.direction()[i]);

			tmin = ffmax(t0, tmin);
			tmax = ffmin(t1, tmax);

			if (tmax <= tmin)
				return false;
		}
		return true;
	}

	/*
	// Optimized version of hit func
	inline bool hit(const ray& r, float tmin, float tmax) const {
		for(int i = 0; i < 3; ++i) {
		float invD = 1.0f / r.direction()[i];
		float t0 = (min()[i] - r.origin()[i]) * invD;
		float t1 = (max()[i] - r.origin()[i]) * invD;
		if (invD < 0.0f)
			std::swap(t0, t1);
		tmin = t0 > tmin ? t0 : tmin;
		tmax = t1 < tmax ? t1 : tmax;
		if (tmax <= tmin)
			return false;
		}
		return true;
	}
	*/

	vec3 _min;
	vec3 _max;
};

aabb surrounding_box(aabb box0, aabb box1) {
	vec3 small(
		fmin(box0.min().x(), box1.min().x()),
		fmin(box0.min().y(), box1.min().y()),
		fmin(box0.min().z(), box1.min().z())
	);

	vec3 big(
		fmax(box0.max().x(), box1.max().x()),
		fmax(box0.max().y(), box1.max().y()),
		fmax(box0.max().z(), box1.max().z())
	);

	return aabb(small, big);
}

class hitable {
public:
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
	virtual bool bounding_box(float t0, float t1, aabb& box) const = 0;
};



// Flips normals to the opposite direction
class flip_normals : public hitable {
public:
	flip_normals(hitable* p) : ptr(p) {}
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
		if (ptr->hit(r, t_min, t_max, rec)) {
			rec.normal = -rec.normal;
			return true;
		}
		else
			return false;
	}

	virtual bool bounding_box(float t0, float t1, aabb& box) const {
		return ptr->bounding_box(t0, t1, box);
	}

	hitable *ptr;
};


#endif // HITABLEH
