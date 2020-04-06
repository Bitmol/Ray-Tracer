#include <iostream>
#include <fstream>
#include <cfloat>

#define STB_IMAGE_IMPLEMENTATION
#include "Utility/stb_image.h"
#include "Utility/camera.h"
#include "Utility/hitablelist.h"
#include "Utility/bvh.h"

#include "Texture/material.h"
#include "Texture/volume.h"

#include "Shape/sphere.h"
#include "Shape/rectangle.h"
#include "Shape/box.h"
#include "Shape/instance.h"


// GENERATE SCENES
hitable *random_scene() {
	int n = 50000;
	hitable **list = new hitable*[n + 1];
	texture *checker = new checker_texture(new constant_texture(vec3(0.2, 0.3, 0.1)), new constant_texture(vec3(0.9, 0.9, 0.9)));
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(checker));
	int i = 1;
	
	for (int a = -10; a < 10; ++a) {
		for (int b = -10; b < 10; ++b) {
			float choose_mat = random_double();
			vec3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
			if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
				
				if (choose_mat < 0.8) { // Diffuse
					list[i++] = new moving_sphere(center, center + vec3(0, 0.5*random_double(), 0), 0., 1.0, 0.2, new lambertian(
						new constant_texture(vec3(random_double() * random_double(), random_double() * random_double(), random_double() * random_double())))
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
	list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(new constant_texture(vec3(0.4, 0.2, 0.1))));
	list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

	return new hitable_list(list, i);
}

hitable *two_perlin_spheres() {
	texture *perlin_tex = new noise_texture(4);
	hitable **list = new hitable*[2];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(perlin_tex));
	list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(perlin_tex));
	return new hitable_list(list, 2);
}

hitable *image_texture_sphere() {
	// Test earth texture on sphere
	int nx, ny, nn;
	unsigned char *tex = stbi_load("C:\\Users\\anmol\\Desktop\\Scripts\\Cpp\\Projects\\RTT\\Ray-Tracer\\Ray-Tracer\\Ray-Tracer\\res\\earth.jpg", &nx, &ny, &nn, 0);
	material *mat = new lambertian(new image_texture(tex, nx, ny));

	return new sphere(vec3(0, 0, 0), 2, mat);
}

hitable *simple_light() {
	texture *perlin_tex = new noise_texture(4);
	hitable **list = new hitable*[4];
	
	// Default scene
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(perlin_tex));
	list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(perlin_tex));
	
	// Emitters
	list[2] = new sphere(vec3(0, 7, 0), 2, new diffuse_light(new constant_texture(vec3(4, 4, 4))));
	list[3] = new xy_rect(3, 5, 1, 3, -2, new diffuse_light(new constant_texture(vec3(4, 4, 4))));
	
	return new hitable_list(list, 4);
}

hitable *cornell_box() {
	hitable **list = new hitable*[6];
	int i = 0;

	material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
	material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
	material *light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));

	//Surrounding box
	list[i++] = (hitable*) new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
	list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
	list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
	list[i++] = (hitable*) new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
	list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
	list[i++] = (hitable*) new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
	
	// 2 smaller boxes inside box - NON INSTANCED
	/*list[i++] = new box(vec3(130, 0, 65), vec3(295, 165, 230), white);
	list[i++] = new box(vec3(265, 0, 295), vec3(430, 330, 460), white);
	*/

	// Rotated and translated boxes
	list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18), vec3(130, 0, 65));
	list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white), 15), vec3(265, 0, 295));

	return new hitable_list(list, i);
}

hitable *cornell_smoke() {
	hitable **list = new hitable*[8];
	int i = 0;
	
	material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
	material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
	material *light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));

	//Surrounding box
	list[i++] = (hitable*) new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
	list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
	list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
	list[i++] = (hitable*) new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
	list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
	list[i++] = (hitable*) new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));

	// Rotated and translated boxes
	hitable *box1 = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18), vec3(130, 0, 65));
	hitable *box2 = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white), 15), vec3(265, 0, 295));
	
	// Volumetric media (boxes as dark-colored smoke and light-colored fog)
	list[i++] = new constant_medium(box1, 0.01, new constant_texture(vec3(1.0, 1.0, 1.0)));
	list[i++] = new constant_medium(box2, 0.01, new constant_texture(vec3(0.0, 0.0, 0.0)));

	return new hitable_list(list, i);
}

hitable *final_scene_book2() {

	int nb = 20;
	hitable **list = new hitable*[30];
	hitable **boxlist = new hitable*[10000];
	hitable **boxlist2 = new hitable*[10000];
	
	material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *ground = new lambertian(new constant_texture(vec3(0.48, 0.83, 0.53)));

	int b = 0;

	for (int i = 0; i < nb; ++i) {
		for (int j = 0; j < nb; ++j) {
			float w = 100;
			float x0 = -1000 + i * w;
			float z0 = -1000 + j * w;
			float y0 = 0;
			float x1 = x0 + w;
			float y1 = 100 * (random_double() + 0.01);
			float z1 = z0 + w;
			boxlist[b++] = new box(vec3(x0, y0, z0), vec3(x1, y1, z1), ground);
		}
	}

	int l = 0;
	list[l++] = new bvh_node(boxlist, b, 0, 1);
	
	material *light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));
	list[l++] = new xz_rect(213, 343, 227, 332, 554, light);

	vec3 center(400, 400, 200);
	list[l++] = new moving_sphere(center, center + vec3(30, 0, 0), 0, 1, 50, new lambertian(new constant_texture(vec3(0.7, 0.3, 0.1))));
	list[l++] = new sphere(vec3(260, 150, 45), 50, new dielectric(1.5));
	list[l++] = new sphere(vec3(0, 150, 145), 50, new metal(vec3(0.8, 0.8, 0.9), 10.0));

	hitable *boundary = new sphere(vec3(360, 150, 145), 70, new dielectric(1.5));
	list[l++] = boundary;
	list[l++] = new constant_medium(boundary, 0.2, new constant_texture(vec3(0.2, 0.4, 0.9)));
	boundary = new sphere(vec3(0, 0, 0), 5000, new dielectric(1.5));
	list[l++] = new constant_medium(boundary, 0.0001, new constant_texture(vec3(1.0, 1.0, 1.0)));

	int nx, ny, nn;
	unsigned char *tex_data = stbi_load("C:\\Users\\anmol\\Desktop\\Scripts\\Cpp\\Projects\\RTT\\Ray-Tracer\\Ray-Tracer\\Ray-Tracer\\res\\earth.jpg", &nx, &ny, &nn, 0);
	material *emat = new lambertian(new image_texture(tex_data, nx, ny));
	list[l++] = new sphere(vec3(400, 200, 400), 100, emat);
	
	texture *perlin_text = new noise_texture(0.1);
	list[l++] = new sphere(vec3(220, 280, 300), 80, new lambertian(perlin_text));

	int ns = 1000;
	for (int j = 0; j < ns; ++j) {
		boxlist2[j] = new sphere(vec3(165 * doubleRand, 165 * doubleRand, 165 * doubleRand), 10, white);
	}

	list[l++] = new translate(new rotate_y(new bvh_node(boxlist2, ns, 0.0, 1.0), 15), vec3(-100, 270, 395));
	
	return new hitable_list(list, l);
}

// Main color
vec3 color(const ray& r, hitable *world, int depth) {

    hit_record rec;
    if(world->hit(r, 0.001, FLT_MAX, rec)) {
		// World scene color
        ray scattered;
        vec3 attenuation;
		vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return emitted + attenuation * color(scattered, world, depth + 1);
		}
		else
			//return vec3(0, 0, 0);
			return emitted;
    }
    else {
	   // Background color
       /* vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5 * (unit_direction.y() + 1.);
        return (1. - t) * vec3(1., 1., 1.) + t * vec3(0.5, 0.7, 1.);*/
		
		return vec3(0, 0, 0); // Return background as black
    }
}

int main() {
    int nx = 1200;
    int ny = 800;
    int ns = 500;
    std::ofstream outfile_ppm;
    outfile_ppm.open("C:\\Users\\anmol\\Desktop\\Scripts\\Cpp\\Projects\\RTT\\Ray-Tracer\\Ray-Tracer\\Ray-Tracer\\res\\Render.ppm");

    outfile_ppm << "P3\n" << nx << " " << ny << "\n255\n";

	hitable *world = final_scene_book2();

	vec3 lookFrom(478, 278, -600);
	vec3 lookAt(278, 278, 0);
	float dist_to_focus = 10.0; //(lookFrom - lookAt).length();
	float aperture = 0.0;
	float vfov = 40.0;

	camera cam(lookFrom, lookAt, vec3(0, 1, 0), vfov, float(nx)/float(ny), aperture, dist_to_focus, 0.0, 1.0);

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
