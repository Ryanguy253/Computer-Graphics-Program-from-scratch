#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include<iostream>
#include <math.h>

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
Color BGcolour = RAYWHITE;

//initialise canvas width/height
int CanvasWidth = window_width;
int CanvasHeight = window_height;

//initialise viewport width/height
int ViewportWidth = 1;
int ViewportHeight = 1;

//initialise camera
Vector3 O{ 0,0,0 };

// initialise D
Vector3 D;

//initialise color
Color color;

//sphere
struct Sphere {
	Vector3 center;
	float radius;
	Color color;
	int specular;
};
//initiliase sphere
Sphere sphere1 { {0,-1,3},1.0,{255,0,0,255},50 };
//sphere1.center = { 0,-1,3 };
//sphere1.radius = 1.0;
//sphere1.color = { 255,0,0,255 }; //red
//sphere1.specular = 500 //shiny

Sphere sphere2{ {2,0,4},1.0,{0,0,255,255},50 };
//sphere2.center = { 2, 0, 4 };
//sphere2.radius = 1.0;
//sphere2.color = { 0, 0, 255 ,255 };// Blue
//sphere2.specular = 500//shiny

Sphere sphere3{ {-2,0,4},1.0,{0,255,0,255}, 100 };
//sphere3.center = { -2, 0, 4 };
//sphere3.radius = 1.0;
//sphere3.color = { 0, 255, 0 ,255 }; // Green*/
//sphere3.specular = 100 // somewhat shiny

Sphere sphere4{ {0,-5001,0},5000.0,{65,152,10,255},10 };


//sphere array
#define SPHERES 4
Sphere _spheres[SPHERES] = {sphere1,sphere2,sphere3,sphere4};

// light
// 1 ambient
// 2 point
// 3 directional

struct Light {
	int type;
	float intensity;
	Vector3 position;
	Vector3 direction;
};

//initialise lights
//Light light_ambient{ 1,0.2,{0} ,{0} };
//Light light_point{ 2,0.6,{2,1,0},{0} };
//Light light_directional{ 3,0.2,{0},{1,4,4} };
Light light_ambient{ 1,0.4,{0} ,{0} };
Light light_point1{ 2,0.1,{-2,1,0},{0} };
Light light_point2{ 3,0.5,{0},{0,3,0} };
Light light_directional{ 0,0.1,{0},{1,4,4} };

//light array
#define LIGHTS 4
//Light _lights[LIGHTS] = { light_ambient,light_point,light_directional };
Light _lights[LIGHTS] = { light_ambient,light_point1,light_point2,light_directional };

// Traceray
float closest_t;
Sphere closest_sphere;

//shadow
Sphere closest_shadow;

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
void ClosestIntersection(Vector3 O, Vector3 D, float t_min, float t_max) {
	closest_t = INFINITY;
	closest_sphere = { Vector3{0, 0, 0}, 0, Color{0, 0, 0, 0} };
	for (int i = 0; i < SPHERES; i++) {
		float t1;
		float t2;
		IntersectRaySphere(O, D, _spheres[i], &t1, &t2);

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

			closest_sphere.specular = _spheres[i].specular;
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

			closest_sphere.specular = _spheres[i].specular;
		}
	}
}
void ClosestShadowIntersection(Vector3 O, Vector3 D, float t_min, float t_max) {
	closest_t = INFINITY;
	closest_shadow = { Vector3{0, 0, 0}, 0, Color{0, 0, 0, 0} };
	for (int i = 0; i < SPHERES; i++) {
		float t1;
		float t2;
		IntersectRaySphere(O, D, _spheres[i], &t1, &t2);

		if (t1 > t_min && t1 < t_max && t1 < closest_t) {
			closest_t = t1;
			//make copy construtcor
			closest_shadow.center.x = _spheres[i].center.x;
			closest_shadow.center.y = _spheres[i].center.y;
			closest_shadow.center.z = _spheres[i].center.z;

			closest_shadow.radius = _spheres[i].radius;

			closest_shadow.color.r = _spheres[i].color.r;
			closest_shadow.color.g = _spheres[i].color.g;
			closest_shadow.color.b = _spheres[i].color.b;
			closest_shadow.color.a = _spheres[i].color.a;

			//why when i uncomment this the program wont work?
			//cout << "Cloest_t : " << closest_t<<endl;

			closest_shadow.specular = _spheres[i].specular;
		}

		if (t2 > t_min && t2 < t_max && t2 < closest_t) {
			closest_t = t2;

			closest_shadow.center.x = _spheres[i].center.x;
			closest_shadow.center.y = _spheres[i].center.y;
			closest_shadow.center.z = _spheres[i].center.z;

			closest_shadow.radius = _spheres[i].radius;

			closest_shadow.color.r = _spheres[i].color.r;
			closest_shadow.color.g = _spheres[i].color.g;
			closest_shadow.color.b = _spheres[i].color.b;
			closest_shadow.color.a = _spheres[i].color.a;
			//why when i uncomment this the program wont work?
			//cout << "Cloest_t : " << closest_t << endl;

			closest_shadow.specular = _spheres[i].specular;
		}
	}
}

float ComputeLighting(Vector3 P, Vector3 N, Vector3 V, int s) {
	float i = 0;
	float t_max =1;
	Vector3 L = { 0 };
	Vector3 R = { 0 };
	float n_dot_l;
	const float epsilon= 0.0001;
	float r_dot_v;
	

	for (int j = 0; j < LIGHTS; j++) {
		if (_lights[j].type == 1) {
			i += _lights[j].intensity;
		}
		else {
			if (_lights[j].type == 2) {
			L = Vector3Subtract(_lights[j].position, P);
			t_max = 1;
			}

			if (_lights[j].type == 3) {
				L.x = _lights[j].direction.x;
				L.y = _lights[j].direction.y;
				L.z = _lights[j].direction.z;
				t_max = 10000000000;
			}
		
			//compute shadows
			ClosestShadowIntersection(P, L, 0.001, t_max);
			if (closest_shadow.radius != 0) {
				continue;
			}
		
		}

		//diffuse lighting
		n_dot_l = Vector3DotProduct(N, L);
		if (fabs(n_dot_l) < epsilon) {
			continue; //treat as 0
		}
		if (n_dot_l > 0) {
			i += _lights[j].intensity * n_dot_l / (Vector3Length(N) * Vector3Length(L));
		}

		//specular lighting
		if (s != -1) {

			Vector3 _2N = Vector3Scale(Vector3Normalize(N), 2.0);
			Vector3 _2N_dotNL = Vector3Scale(_2N, Vector3DotProduct(N, L));

			R = Vector3Subtract(_2N_dotNL, L);
			r_dot_v = Vector3DotProduct(R, V);

			if (r_dot_v > 0) {
				i += _lights[j].intensity * pow((r_dot_v / (Vector3Length(R) * Vector3Length (V))), s);
			}

		}
	}

	//cout << "LIGHT INTENSITY: " << i << endl;
	return i;
}

Color TraceRay(Vector3 O, Vector3 D, float t_min, float t_max) {
	Vector3 P;
	Vector3 N;
	
	ClosestIntersection(O, D, t_min, t_max);

	const float epsilon = 0.0001;
	// Use epsilon for floating-point comparison
	if (fabs(closest_sphere.radius) < epsilon) {
		//cout << " BG COLOUR RETURNED" << endl;
		return BGcolour;

	}
		
	//cout << "COLOUR RETURNED : " << "R : " << closest_sphere.color.r << "G : " << closest_sphere.color.g << "B : " << closest_sphere.color.b << "A : " << closest_sphere.color.a<<endl;
	P = Vector3Add(O, Vector3Scale(D,closest_t)); // Compute intersection
	N = Vector3Subtract(P, closest_sphere.center); // Compute sphere normal at intersection
	N = Vector3Scale( N, Vector3Length(N));

	closest_sphere.color.r = (unsigned char)Clamp(closest_sphere.color.r * ComputeLighting(P, N,Vector3Negate(D),closest_sphere.specular),0,255);
	closest_sphere.color.g = (unsigned char)Clamp(closest_sphere.color.g * ComputeLighting(P, N,Vector3Negate(D),closest_sphere.specular),0,255);
	closest_sphere.color.b = (unsigned char)Clamp(closest_sphere.color.b * ComputeLighting(P, N,Vector3Negate(D),closest_sphere.specular),0,255);
	/*When you multiply the RGB components of the color with the specular reflection intensity, 
	you might end up with a color that is too dark or saturated, especially if the specular reflection intensity is high.
	Instead, you should scale the intensity while keeping the color values within a valid range.*/
	//cout << "R: " << closest_sphere.color.r << endl;
	//WaitTime(1);
	//cout << "COLOUR RETURNED After Lighting: " << "R : " << closest_sphere.color.r << "G : " << closest_sphere.color.g << "B : " << closest_sphere.color.b << "A : " << closest_sphere.color.a << endl;
	return closest_sphere.color;
}




void quit() {
	CloseWindow();
}
void input() {
	if (IsKeyDown(KEY_W)) {
		O.z += 1;
	}
	if (IsKeyDown(KEY_S)){
		O.z -=1;
	}
	if (IsKeyDown(KEY_A)){
		O.x -= 1;
	}
	if (IsKeyDown(KEY_D)) {
		O.x += 1;
	}
	if (IsKeyDown(KEY_SPACE)) {
		O.y += 1;
	}
	if (IsKeyDown(KEY_LEFT_SHIFT)) {
		O.y -= 1;
	}
	//fps
	cout << "FPS : " << GetFPS() << endl;
}
void update() {

}
void render() {
	BeginDrawing();
	//ClearBackground(BGcolour);
	
	
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



