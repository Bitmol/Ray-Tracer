#ifndef TEXTURE_H
#define TEXTURE_H
#include "perlin.h"

class texture {
public:
	virtual vec3 value(float u, float v, const vec3& p) const = 0;
};

// Texture class for flat-color as a texture
class constant_texture : public texture {
public:
	constant_texture() {}
	constant_texture(vec3 c) : color(c) {}
	virtual vec3 value(float u, float v, const vec3& p) const {
		return color;
	}

	vec3 color;
};

// Texture class for a checker pattern
class checker_texture : public texture {
public:
	checker_texture() {}
	checker_texture(texture* t0, texture* t1) : even(t0), odd(t1) {}
	virtual vec3 value(float u, float v, const vec3& p) const {
		float sines = sin(10*p.x()) * sin(10 * p.y()) * sin(10 * p.z());
		if (sines < 0)
			return odd->value(u, v, p);
		else
			return even->value(u, v, p);
	}
	texture *odd;
	texture *even;

};

// Texture class for using noise as a solid-texture
class noise_texture : public texture {
public:
	noise_texture() {}
	noise_texture(float sc) : scale(sc) {}
	virtual vec3 value(float u, float v, const vec3& p) const {
		//return vec3(1, 1, 1) * noise.noise(scale * p); // Basic noise
		//return vec3(1, 1, 1) * 0.5 * (1 + noise.turb(scale * p)); // lightened turbulence pattern
		return vec3(1, 1, 1) * 0.5 * (1 + sin(scale * p.z() + 10 * noise.turb(p))); // pattern for marble-like surface
	} 

	perlin noise;
	float scale;
};

// Texture class for using image as a texture
class image_texture : public texture {
public:
	image_texture() {}
	image_texture(unsigned char* pixels, int m, int n) : data(pixels), nx(m), ny(n) {}
	virtual vec3 value(float u, float v, const vec3& p) const;
	unsigned char *data;
	int nx, ny;
};

vec3 image_texture::value(float u, float v, const vec3& p) const {
	int i = (u)* nx;
	int j = (1 - v) * ny - 0.001;
	
	// Boundary checks
	if (i < 0)
		i = 0;
	if (j < 0)
		j = 0;
	if (i > nx - 1)
		i = nx - 1;
	if (j > ny - 1)
		j = ny - 1;

	// unpack image
	float r = int(data[3 * i + 3 * nx * j]) / 255.0;
	float g = int(data[3 * i + 3 * nx * j + 1]) / 255.0;
	float b = int(data[3 * i + 3 * nx * j + 2]) / 255.0;
	return vec3(r, g, b);
}

#endif // TEXTURE_H