#ifndef HITABLEH
#define HITABLEH

#include "ray.h"

inline float ffmin(float a, float b) { return a < b ? a : b; }
inline float ffmax(float a, float b) { return a > b ? a : b; }

class material;

struct hit_record {
    float t;
    vec3 p;
    vec3 normal;
    material *mat_ptr;
};

class hitable {
public:
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
	virtual bool bounding_box(float t0, float t1, aabb& box) const = 0;
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
#endif // HITABLEH
