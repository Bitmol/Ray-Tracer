#include <iostream>
#include <fstream>
#include <cfloat>

#include "camera.h"
#include "hitablelist.h"
#include "material.h"
#include "sphere.h"


hitable *random_scene() {
	int n = 50000;
	hitable **list = new hitable*[n + 1];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));
	int i = 1;
	
	for (int a = -10; a < 10; ++a) {
		for (int b = -10; b < 10; ++b) {
			float choose_mat = random_double();
			vec3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
			if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
				
				if (choose_mat < 0.8) { // Diffuse
					list[i++] = new moving_sphere(center, center + vec3(0, 0.5*random_double(), 0), 0., 1.0, 0.2, new lambertian(
						vec3(random_double() * random_double(), random_double() * random_double(), random_double() * random_double()))
					);
				}
				else if (choose_mat < 0.95) { // Metal
					list[i++] = new sphere(center, 0.2, new metal(
						vec3(0.5 * (1 + random_double()), 0.5 * (1 + random_double()), 0.5 * (1 + random_double())), 0.5 * random_double())
					);
				}
				else { // Glass
					list[i++] = new sphere(center, 0.2, new dielectric(1.5));
				}
			}
		}
	}

	list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
	list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
	list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

	return new hitable_list(list, i);
}

vec3 color(const ray& r, hitable *world, int depth) {

    hit_record rec;
    if(world->hit(r, 0.001, FLT_MAX, rec)) {
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            return attenuation * color(scattered, world, depth + 1);
        }
        else
            return vec3(0, 0, 0);
    }
    else {
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5 * (unit_direction.y() + 1.);
        return (1. - t) * vec3(1., 1., 1.) + t * vec3(0.5, 0.7, 1.);
    }
}

int main() {
    int nx = 200;
    int ny = 100;
    int ns = 10;
    std::ofstream outfile_ppm;
    outfile_ppm.open("C:\\Users\\anmol\\Desktop\\Scripts\\Cpp\\Projects\\RTT\\Ray-Tracer\\Ray-Tracer\\Ray-Tracer\\res\\Render.ppm");

    outfile_ppm << "P3\n" << nx << " " << ny << "\n255\n";

    hitable *world = random_scene();
    
	vec3 lookFrom(13, 2, 3);
	vec3 lookAt(0, 0, 0);
	float dist_to_focus = 10.0; //(lookFrom - lookAt).length();
	float aperture = 0.0;
	camera cam(lookFrom, lookAt, vec3(0, 1, 0), 20, float(nx)/float(ny), aperture, dist_to_focus, 0.0, 1.0);

    for(int j = ny - 1; j >= 0; --j) {
        for(int i = 0; i < nx; ++i) {
            vec3 col(0, 0, 0);
            for(int s = 0; s < ns; ++s) {
                 float u = float(i + random_double()) / float(nx);
                 float v = float(j + random_double()) / float(ny);
                 ray r = cam.get_ray(u, v);
                 col+= color(r, world, 0);
            }

            col /= float(ns);
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2])); // Gamma Correction term {gamma 2}
            int ir = int(255.99*col[0]);
            int ig = int(255.99*col[1]);
            int ib = int(255.99*col[2]);

            outfile_ppm << ir << " " << ig << " " << ib << "\n";
        }
    }
    outfile_ppm.close();
    std::cout << "Finished" << std::endl;
}
