#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include<iostream>

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

//Direction of ray
Vector3 Destination_Viewpointcoords;

//camera origin 
Vector3 o = { 0,0,0 };

//inf (infinity)
float inf = 999999999;

//color
Color color;

struct QuadraticX1X2 {
	float x1;
	float x2;
};

//initialise sphere
struct Sphere {
	Vector3 center;
	float radius;
	Color color;
};

// viewport size = 1x1
//projection plane d = 1
#define SPHERES 3
Sphere _nullSphere{ {0,0,0 }, 0, { 0,0,0 } };
Sphere sphere1{ {0,0,0 }, 0.1, { 255,0,0 } }; // Red sphere 1
Sphere sphere2{ {0,0,0},0.1,{0,0,255} }; // Blue Sphere 2
Sphere sphere3{ {0,0,0},0.1,{0,255,0} }; // Green Sphere 3

Sphere _sceneSpheres[SPHERES] = { sphere1,sphere2,sphere3 };


bool isSphereNULL(const Sphere& sphere){
	return sphere.center.x == 0 && sphere.center.y == 0 && sphere.center.z == 0 && sphere.radius == 0 && sphere.color.a == 0
		&& sphere.color.g == 0&& sphere.color.b==0&&sphere.color.r==0;
}




void initialise() {
	InitWindow(window_width, window_height, "Graphics Engine");
}
void DrawPixelCenter(int posXorigin, int posYorigin, Color color) {

	DrawPixel((window_width / 2 + posXorigin), (window_height / 2 + posYorigin), color);
}
Vector3 CanvasCoordstoViewportCoords(float canvasX,float canvasY) {
	return {canvasX*ViewportWidth/CanvasWidth,canvasY*ViewportHeight/CanvasHeight,1}; // 1 is distance between camera&projection plane
}

QuadraticX1X2 IntersectRaySphere(Vector3 origin, Vector3 ViewportCoords, Sphere sphere_param) {
	float r = sphere_param.radius;
	Vector3 CO = { origin.x - sphere_param.center.x,origin.y - sphere_param.center.y,origin.z - sphere_param.center.z };

	cout << "CO X: " << CO.x << " Y: " << CO.y << " Z: " << CO.z << endl;
	//WaitTime(1);

	float a = Vector3DotProduct(ViewportCoords, ViewportCoords);
	cout << "Viewport Coords : X: " << ViewportCoords.x << " Y: " << ViewportCoords.y << " Z: " << ViewportCoords.z << endl;
	//cout << "Vector D Coords : X: " << D.x << " Y: " <<D.y<<" Z: " <<D.z <<endl;
	//cout << "A: " << a << endl;

	float b = 2 * (Vector3DotProduct(CO, ViewportCoords));
	//cout << "B: " << b << endl;
	float c = Vector3DotProduct(CO, CO) - (r * r);
	//cout << "C: " << c << endl;
	//WaitTime(3);
	float discriminant = (b * b) - (4 * a * c);
	//cout << "r: " << r << endl;
	cout << "Discriminant : " << discriminant << endl;
	cout<< endl;
	
	
	if (discriminant < 0) {
		return { inf,inf };
	}

	float t1 = (-b + sqrt(discriminant)) / (2 * a);
	//cout << "ADFJHJSKDGSKDLJGDS" << endl;
	//WaitTime(3);
	float t2 = (-b - sqrt(discriminant)) / (2 * a);
	return { t1,t2 };
}

//tracesphere
float closest_t = inf;
Sphere closest_sphere{ {0,0,0 }, 0, { 0,0,0 } };

Color TraceRay(Vector3 origin, Vector3 ViewportCoords, int range_min, int range_max) {
	
	QuadraticX1X2 t1t2;
	for (int i = 0; i < SPHERES; i++) {
		t1t2 = IntersectRaySphere(origin, ViewportCoords, _sceneSpheres[i]);
		
		if ((t1t2.x1) <= range_max && (t1t2.x1) >= range_min && t1t2.x1 < closest_t) {
			closest_t = t1t2.x1;
			//cout << t1t2.x1 << endl;
			closest_sphere = _sceneSpheres[i];
		}
		if ((t1t2.x2) <= range_max && (t1t2.x2) >= range_min && t1t2.x2 < closest_t) {
			closest_t = t1t2.x2;
			closest_sphere = _sceneSpheres[i];
		}
	}
	if (isSphereNULL(closest_sphere)) {
		return BGcolour;
	}
	//cout << closest_sphere.color.r << endl;
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
	for (int x = -window_width / 2; x <= window_width / 2; x++) {
		for (int y = -window_height / 2; y <= window_height / 2; y++) {
			Destination_Viewpointcoords = CanvasCoordstoViewportCoords(x, y);
			color = TraceRay(o, Destination_Viewpointcoords, 1, inf);
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



