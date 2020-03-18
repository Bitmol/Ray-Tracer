#ifndef BVH_H
#define BVH_H

#include "hitable.h"

class bvh_node : public hitable {
public:
	bvh_node() {}
	bvh_node(hitable **l, int n, float time0, float time1);
	virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const;
	hitable *left;
	hitable *right;
	aabb box;
};

int box_x_compare(const void* a, const void* b) {
	aabb box_left, box_right;
	hitable *ah = *(hitable**)a;
	hitable *bh = *(hitable**)b;

	if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
		std::cerr << "no bounding box in bvh_node constructor\n";

	// comparing BB of 2 objects on X-axis
	if (box_left.min().x() - box_right.min.x() < 0.0)
		return -1;
	else
		return 1;
}

int box_y_compare(const void* a, const void* b) {
	aabb box_left, box_right;
	hitable *ah = *(hitable**)a;
	hitable *bh = *(hitable**)b;

	if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
		std::cerr << "no bounding box in bvh_node constructor\n";

	// comparing BB of 2 objects on Y-axis
	if (box_left.min().y() - box_right.min.y() < 0.0)
		return -1;
	else
		return 1;
}

int box_z_compare(const void* a, const void* b) {
	aabb box_left, box_right;
	hitable *ah = *(hitable**)a;
	hitable *bh = *(hitable**)b;

	if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
		std::cerr << "no bounding box in bvh_node constructor\n";

	// comparing BB of 2 objects on Z-axis
	if (box_left.min().z() - box_right.min.z() < 0.0)
		return -1;
	else
		return 1;
}


bool bvh_node::bounding_box(float t0, float t1, aabb& b) const {
	b = box;
	return true;
}

// bvh_node::hit(...) : check if box for the node is hit, if so, check the children nodes and sort
bool bvh_node::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {

	if (box.hit(r, t_min, t_max)) { // hits box
		hit_record left_rec, right_rec;
		bool hit_left = left->hit(r, t_min, t_max, left_rec);
		bool hit_right = right->hit(r, t_min, t_max, right_rec);

		// hit both children nodes
		if (hit_left && hit_right) {
			if (left_rec.t < right_rec.t)
				rec = left_rec;
			else
				rec = right_rec;
			return true;
		}
		else if (hit_left) {	// hit left child
			rec = left_rec;
			return true;
		}
		else if (hit_right) {	// hit right child
			rec = right_rec;
			return true;
		}
		else // hit nothing
			return false;
	}
	else // doesnt hit box
		return false;
}

bvh_node::bvh_node(hitable **l, int n, float time0, float time1) {
	
	int axis = int(3 * rand() / (RAND_MAX + 1.0));
	//Randomly choose axis and sort hitable list
	if (axis == 0)
		qsort(l, n, sizeof(hitable *), box_x_compare);
	else if (axis == 1)
		qsort(l, n, sizeof(hitable *), box_y_compare);
	else
		qsort(l, n, sizeof(hitable *), box_z_compare);

	// if 1 element in list, duplicate element in left and right subtree
	if (n == 1) 
		left = right = l[0];
	else if (n == 2) { // 2 elements -> subdivide into left & right
		left = l[0];
		right = l[1];
	}
	else { // > 3 elements? subdivide evenly in left and right subtree
		left = new bvh_node(l, n / 2, time0, time1);
		right = new bvh_node(l + n / 2, n - n / 2, time0, time1);
	}

	aabb box_left, box_right;
	if (!left->bounding_box(time0, time1, box_left) || !right->bounding_box(time0, time1, box_right))
		std::cerr << "no bounding box in bvh_node constructor\n";

	box = surrounding_box(box_left, box_right);


}

#endif // BVH_H