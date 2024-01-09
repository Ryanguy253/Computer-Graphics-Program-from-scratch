#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include<iostream>

//issues
// solved the discriminant being always 0 -> floating point inaccuracy, need to use epsilon
/* second issue : 
			no copy construtcor
			closest_sphere.center.x = _spheres[i].center.x;
			closest_sphere.center.y = _spheres[i].center.y;
			closest_sphere.center.z = _spheres[i].center.z; //but you have to assign lke this

			closest_sphere.radius = _spheres[i].radius;
			closest_sphere.color = _spheres[i].color;
			//cout << "Cloest_t : " << closest_t<<endl;
		}

		if (t2 > t_min && t2 < t_max && t2 < closest_t) {
			closest_t = t2;
			closest_sphere = _spheres[i]; .// you did this originally
			//cout << "Cloest_t : " << closest_t << endl;
		}
third issue : when i uncomment cout<< at 
			if (t2 > t_min && t2 < t_max && t2 < closest_t) {
			closest_t = t2;
			closest_sphere = _spheres[i]; .// you did this originally
			//cout << "Cloest_t : " << closest_t << endl;
		}
		the program wont work

		 If you uncomment them and the program stops working, it might be related to the console output buffering 
		 or the volume of output generated. If you're running the program from an integrated
		 development environment (IDE), it might have limitations on the amount of console output it can handle.
		
*/





using namespace std;

int window_width = 1000;
int window_height = 800;
Color BGcolour = BLACK;

//initialise canvas width/height
int CanvasWidth = window_width;
int CanvasHeight = window_height;

//initialise viewport width/height
int ViewportWidth = 1;
int ViewportHeight = 1;

//initialise camera
Vector3 O;

// initialise D
Vector3 D;

//initialise color
Color color;

//sphere
struct Sphere {
	Vector3 center;
	float radius;
	Color color;
};
//initiliase sphere
Sphere sphere1 { {0,-1,3},1.0,{255,0,0,255} };
//sphere1.center = { 0,-1,3 };
//sphere1.radius = 1.0;
//sphere1.color = { 255,0,0,255 }; //red

Sphere sphere2{ {2,0,4},1.0,{0,0,255,255} };
//sphere2.center = { 2, 0, 4 };
//sphere2.radius = 1.0;
//sphere2.color = { 0, 0, 255 ,255 };// Blue

Sphere sphere3{ {-2,0,4},1.0,{0,255,0,255} };
//sphere3.center = { -2, 0, 4 };
//sphere3.radius = 1.0;
//sphere3.color = { 0, 255, 0 ,255 }; // Green*/

//sphere array
#define SPHERES 3
Sphere _spheres[SPHERES] = {sphere1,sphere2,sphere3};


// Traceray
float closest_t;
Sphere closest_sphere;


void initialise() {
	InitWindow(window_width, window_height, "Graphics Engine");
	






}
void DrawPixelCenter(int posXorigin, int posYorigin, Color color) {
	/*The upside-down output is likely due to the difference in coordinate systems between traditional computer graphics and the coordinate system used by Raylib.
	Raylib uses a top-left origin coordinate system, where the top-left corner is (0,0), and the y-coordinate increases as you go down the window. On the other hand, in traditional computer graphics, the bottom-left corner is often considered the origin, with the y-coordinate increasing as you go up.
	In your rendering loop, when you call DrawPixelCenter(x, y, color), y is likely being used as a traditional graphics coordinate where positive values go up, but Raylib expects it to be a top-left origin coordinate.*/
	/*Invert Y-Coordinate:
	In your DrawPixelCenter function, invert the y coordinate before passing it to DrawPixel:*/
	// original code -> DrawPixel((window_width / 2 + posXorigin), (window_height / 2 + posYorigin), color);
	DrawPixel((window_width / 2 + posXorigin), (window_height / 2 - posYorigin), color);
}
Vector3 CanvasCoordstoViewportCoords(float canvasX,float canvasY) {
	return {canvasX*ViewportWidth/CanvasWidth,canvasY*ViewportHeight/CanvasHeight,1}; // 1 is distance between camera&projection plane
}

/*void IntersectRaySphere(Vector3 O, Vector3 D, Sphere sphere, float* t1, float* t2) {
	 float r = sphere.radius;
	 Vector3 CO = Vector3Subtract(O, sphere.center);

	 float a = Vector3DotProduct(D, D);
	 float b = 2 * Vector3DotProduct(CO, D);
	 float c = Vector3DotProduct(CO, CO) - (r * r);
	 
	 float discriminant = (b * b) - (4 * a * c);
	 const float epsilon = 0.0001;
	 if (fabs(discriminant) < epsilon) {
		 // Treat discriminant as 0 (one real root)
		 *t1 = (-b - sqrt(discriminant)) / (2 * a);
		 *t2 = (-b + sqrt(discriminant)) / (2 * a);
		 cout << "Discriminant positive" << endl;
	 }
	 else if (discriminant < 0) {
		 // No real roots
		 *t1 = INFINITY;
		 *t2 = INFINITY;
		// cout << "Discriminant : " << discriminant << endl;
	 }
	 else {
		 *t1 = (-b - sqrt(discriminant)) / (2 * a);
		 *t2 = (-b + sqrt(discriminant)) / (2 * a);
		 cout << "Discriminant positive" << endl;
	 }
	 
}*/
void IntersectRaySphere(Vector3 O, Vector3 D, Sphere sphere, float* t1, float* t2) {
	float r = sphere.radius;
	Vector3 CO = Vector3Subtract(O, sphere.center);

	float a = Vector3DotProduct(D, D);
	float b = 2 * Vector3DotProduct(CO, D);
	float c = Vector3DotProduct(CO, CO) - (r * r);

	float discriminant = (b * b) - (4 * a * c);

	const float epsilon = 0.0001;

	if (fabs(discriminant) < epsilon) {
		// Treat discriminant as 0 (one real root)
		*t1 = (-b - sqrt(discriminant)) / (2 * a);
		*t2 = (-b + sqrt(discriminant)) / (2 * a);

		// Debug output
		//cout << "Discriminant is 0: " << discriminant << endl;
		//cout << "Roots: " << *t1 << ", " << *t2 << endl;
	}
	else if (discriminant < 0) {
		// No real roots
		*t1 = INFINITY;
		*t2 = INFINITY;
		//cout << "Discriminant is negative: " << discriminant << endl;
	}
	else {
		*t1 = (-b - sqrt(discriminant)) / (2 * a);
		*t2 = (-b + sqrt(discriminant)) / (2 * a);

		// Debug output
		//cout << "Discriminant is positive: " << discriminant << endl;
		//cout << "Roots: " << *t1 << ", " << *t2 << endl;
	}
}

Color TraceRay(Vector3 O, Vector3 D, float t_min, float t_max) {
	closest_t = INFINITY;
	closest_sphere = { Vector3{0, 0, 0}, 0, Color{0, 0, 0, 0} };
	for (int i = 0; i < SPHERES; i++) {
		float t1;
		float t2;
		IntersectRaySphere(O, D, _spheres[i],&t1,&t2);

		if (t1 > t_min && t1 < t_max && t1 < closest_t) {
			closest_t = t1;
			//make copy construtcor
			closest_sphere.center.x = _spheres[i].center.x;
			closest_sphere.center.y = _spheres[i].center.y;
			closest_sphere.center.z = _spheres[i].center.z;

			closest_sphere.radius = _spheres[i].radius;

			closest_sphere.color.r = _spheres[i].color.r;
			closest_sphere.color.g = _spheres[i].color.g;
			closest_sphere.color.b = _spheres[i].color.b;
			closest_sphere.color.a = _spheres[i].color.a;

			//why when i uncomment this the program wont work?
			//cout << "Cloest_t : " << closest_t<<endl;
		}

		if (t2 > t_min && t2 < t_max && t2 < closest_t) {
			closest_t = t2;

			closest_sphere.center.x = _spheres[i].center.x;
			closest_sphere.center.y = _spheres[i].center.y;
			closest_sphere.center.z = _spheres[i].center.z;

			closest_sphere.radius = _spheres[i].radius;

			closest_sphere.color.r = _spheres[i].color.r;
			closest_sphere.color.g = _spheres[i].color.g;
			closest_sphere.color.b = _spheres[i].color.b;
			closest_sphere.color.a = _spheres[i].color.a;
			//why when i uncomment this the program wont work?
			//cout << "Cloest_t : " << closest_t << endl;
		}
	}
	const float epsilon = 0.0001;

	// Use epsilon for floating-point comparison
	if (fabs(closest_sphere.radius) < epsilon) {
		//cout << " BG COLOUR RETURNED" << endl;
		return BGcolour;

	}
		
	//cout << "COLOUR RETURNED : " << "R : " << closest_sphere.color.r << "G : " << closest_sphere.color.g << "B : " << closest_sphere.color.b << "A : " << closest_sphere.color.a<<endl;
	return closest_sphere.color;


}




void quit() {
	CloseWindow();
}
void input() {

}
void update() {

}
void render() {
	BeginDrawing();
	//ClearBackground(BGcolour);
	O = { 0,0,0 };
	
	for (int x = -CanvasWidth / 2; x <= CanvasWidth / 2; x++) {
		for (int y = -CanvasHeight / 2; y <= CanvasHeight / 2; y++) {
			D = CanvasCoordstoViewportCoords(x, y);
			color = TraceRay(O,D,1,INFINITY);
			//cout << "R: " << color.r << "G: " << color.g << "B: " << color.b << "A: " << color.a << endl;
			DrawPixelCenter(x, y, color);
		}
	}

	EndDrawing();
}







int main() {
	initialise();
	while (!WindowShouldClose()) {
		input();
		update();
		render();
	}
	quit();
	return 0;
}



