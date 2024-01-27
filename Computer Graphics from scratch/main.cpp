#include "raylib.h"
#include "raymath.h"
#include "raygui.h"
#include <stdio.h>
#include<iostream>
#include <math.h>

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
	float reflective;
};
//reflections
bool drawReflection = false;
const int recursion_depth = 2;

//initiliase sphere
Sphere sphere4{ {0,-5001,0},5000.0,{65,152,10,255},10,0.1 };
Sphere sphere1{ {0,-1,3},1.0,{255,0,0,255},500, 0.2};
Sphere sphere2{ {2, 0, 4},1.0, { 0, 0, 255 ,255 },500,0.2 };
Sphere sphere3{ { -2, 0, 4 } ,1.0,{ 0, 255, 0 ,255 },100,0.2 };

//sphere array
#define SPHERES 5
Sphere _spheres[SPHERES] = {sphere1,sphere2,sphere3,sphere4};
int _spheresCount = 4;

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

Light light_ambient{ 1,0.2,{0} ,{0} };
Light light_point1{ 2,0.5,{2,1,0},{0} };
Light light_point2{ 3,0.8,{0},{0,3,0} };

Light light_directional{ 3,0.1,{0},{1,4,4} };

//light array
#define LIGHTS 5
Light _lights[LIGHTS] = {light_ambient,light_point1};
int _lightsCount = 3;

const char* returnLighttype(Light *light) {
	switch (light->type) {
	case 1 :
		return "Ambient Light";
		break;
	case 2: 
		return "Point Light";
		break;
	case 3:
		return "Directional Light";
		break;
	case 0:
		return "No Light Source";
		break;
	}
	
}

// Traceray
float closest_t;
Sphere closest_sphere;

//shadow
Sphere closest_shadow;

//Rotation
// Float Beta (radians)
float B = PI/2;
//Matrix
//Z-rotation
Matrix Z_Rotation(float B)
{
	return Matrix{ cos(B),-sin(B),0,0,
			sin(B),cos(B),0,0,
			0,0,1,0,
			0,0,0,1 };
}
//Y Rotation
Matrix Y_Rotation(float B)
{
	return Matrix{ cos(B),0,sin(B),0,
			0,1,0,0,
			-sin(B),0,cos(B),0,
			0,0,0,1 };
}
//X Rotation
Matrix X_Rotation (float B) 
{
	return Matrix{ 1,0,0,0,
			0,cos(B),-sin(B),0,
			0,sin(B),cos(B),0,
			0,0,0,1 };
}

float _x_rotation = 0;
float _y_rotation = 0;
float _z_rotation = 0 ;

//UI
Sphere* currentSphereSelected = &_spheres[0];
int currentSphereIndex = 0;
bool drawUI = false;
bool drawEditor = false;
bool changeColour = false;

Light* currentLightSelected = &_lights[0];
int currentLightIndex = 0;

void DrawUI() {
	DrawFPS(18, 20);
	DrawText(TextFormat("Camera Position : %.2f , %.2f, %.2f ", O.x, O.y, O.z), 20, 40, 20, BLACK);
	DrawText(TextFormat("Camera Rotation (Degrees) : %.2f , %.2f, %.2f ", fmod(_x_rotation,365.0f), fmod(_y_rotation, 365.0f), fmod(_z_rotation, 365.0f)), 20, 60, 20, BLACK);
	DrawText(TextFormat("Spawn Sphere (Press B) MAX : %d / %d ",_spheresCount ,SPHERES), 20, 80, 20, BLACK);
	DrawText(TextFormat("Press C to cycle between spheres (Current Sphere: (X : %.2f, Y : %.2f, Z : %.2f))",currentSphereSelected->center.x, currentSphereSelected->center.y, currentSphereSelected->center.z),20,100,20,BLACK);
	DrawText("Press X to delete sphere ", 20,120,20,BLACK);

	DrawText(TextFormat("Press R to toggle Reflections (%s)",drawReflection? "ON":"OFF"), 20, 140, 20, BLACK);
	
	DrawText(TextFormat("Press L to cycle between light sources (MAX : %d / %d )",_lightsCount,LIGHTS), 20, 180, 20, BLACK);
	DrawText(TextFormat("(%s ,Intensity:%.2f, Position(X:%.2f,Y:%.2f,Z:%.2f, Direction(X:%.2f,Y:%.2f,Z:%.2f)))", returnLighttype(currentLightSelected),currentLightSelected->intensity,currentLightSelected->position.x,currentLightSelected->position.y,currentLightSelected->position.z,currentLightSelected->direction.x, currentLightSelected->direction.y, currentLightSelected->direction.z),20, 200, 20, BLACK);
	DrawText("Press J to add light source (Use KEYS 1 or 2 to change type)", 20, 220, 20, BLACK);
	
	DrawText("(Use KEY U to increase intensity and KEY I to decrease intensity)", 20, 240, 20, BLACK);
	DrawText("Press K to delete light source", 20, 260, 20, BLACK);
	DrawText("Press TAB to toggle editor", 20, 280, 20, BLACK);

	if (drawEditor) {

		Rectangle window{ 20,280,500,400 };
		GuiPanel(window, "Editor");

		DrawText("Press F to confirm colour change", 80, 280, 20, BLACK);

		Rectangle radius{ 80,310,200,20 };
		GuiSlider(radius, "Radius 0.1", "Radius 10", &(currentSphereSelected->radius), 0.1, 10);

		Rectangle specular{ 80,340,200,20 };
		GuiSpinner(specular, "Specular ", &(currentSphereSelected->specular), 1, 5000, drawEditor);

		Rectangle reflection{ 110,370,200,20 };
		GuiSlider(reflection, "Reflection 0.0","Reflection 1.0" ,& (currentSphereSelected->reflective), 0, 1.0);

		Rectangle colour{ 30,400,200,200 };
		GuiColorPicker(colour, "Colour Picker", &(currentSphereSelected->color));
	}

}


void selectionFlash() {
	static Sphere* lastSelectedSphere = nullptr;
	static Color originalColour;

	// Check if the selected sphere has changed or colour has changed
	if (currentSphereSelected != lastSelectedSphere || changeColour) {
		// Update the last selected sphere
		lastSelectedSphere = currentSphereSelected;

		// Copy original colour only once when the sphere changes
		originalColour = currentSphereSelected->color;
		changeColour = false;
	}

	// Flashing logic
	int interval = 1; // Adjust as needed
	int time = GetTime();

	if (time % (interval * 2) < interval) {
		// Toggle between the original color and neon
		if (currentSphereSelected->color.r == 57 && currentSphereSelected->color.g == 255 && currentSphereSelected->color.b == 20 && currentSphereSelected->color.a == 255) {
			currentSphereSelected->color = originalColour;
		}
		else {
			currentSphereSelected->color = { 57, 255, 20, 255 };
		}
	}
	else {
		// Reset to the original color outside the flashing interval
		currentSphereSelected->color = originalColour;
	}

}

void deleteSphere() {
	*(currentSphereSelected) = { 0 };
	_spheresCount -= 1;

	// arrange array
	for (int i = 0; i < SPHERES - 1; i++) {
		if (_spheres[i].radius == 0) {
			for (int x = i; x < SPHERES - 1; x++) {
				_spheres[x] = _spheres[x + 1];
			}

			// After shifting elements, the last element needs to be set to zero
			_spheres[SPHERES - 1] = { 0 };
		}
	}

}

void deleteLight() {
	*(currentLightSelected) = { 0 };
	_lightsCount -= 1;

	// arrange array
	for (int i = 0; i < LIGHTS - 1; i++) {
		if (_lights[i].type == 0) {
			for (int x = i; x < LIGHTS - 1; x++) {
				_lights[x] = _lights[x + 1];
			}
			// After shifting elements, the last element needs to be set to zero
			_lights[LIGHTS - 1] = { 0 };
		}
	}
}

void initialise() {
	InitWindow(window_width, window_height, "Graphics Engine");
}

void DrawPixelCenter(int posXorigin, int posYorigin, Color color) {
	DrawPixel((window_width / 2 + posXorigin), (window_height / 2 - posYorigin), color);
}

Vector3 CanvasCoordstoViewportCoords(float canvasX,float canvasY) {
	return {canvasX*ViewportWidth/CanvasWidth,canvasY*ViewportHeight/CanvasHeight,1}; // 1 is distance between camera&projection plane
}

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

	}
	else if (discriminant < 0) {
		// No real roots
		*t1 = INFINITY;
		*t2 = INFINITY;
	}
	else {
		*t1 = (-b - sqrt(discriminant)) / (2 * a);
		*t2 = (-b + sqrt(discriminant)) / (2 * a);
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
			
			closest_sphere.center.x = _spheres[i].center.x;
			closest_sphere.center.y = _spheres[i].center.y;
			closest_sphere.center.z = _spheres[i].center.z;

			closest_sphere.radius = _spheres[i].radius;

			closest_sphere.color.r = _spheres[i].color.r;
			closest_sphere.color.g = _spheres[i].color.g;
			closest_sphere.color.b = _spheres[i].color.b;
			closest_sphere.color.a = _spheres[i].color.a;

			closest_sphere.specular = _spheres[i].specular;
			
			if (drawReflection) {
				closest_sphere.reflective = _spheres[i].reflective;
			}
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
			
			closest_sphere.specular = _spheres[i].specular;

			if (drawReflection) {
				closest_sphere.reflective = _spheres[i].reflective;
			}
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
			
			closest_shadow.center.x = _spheres[i].center.x;
			closest_shadow.center.y = _spheres[i].center.y;
			closest_shadow.center.z = _spheres[i].center.z;

			closest_shadow.radius = _spheres[i].radius;

			closest_shadow.color.r = _spheres[i].color.r;
			closest_shadow.color.g = _spheres[i].color.g;
			closest_shadow.color.b = _spheres[i].color.b;
			closest_shadow.color.a = _spheres[i].color.a;

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

			closest_shadow.specular = _spheres[i].specular;
		}
	}
}

Vector3 ReflectRay(Vector3 R,Vector3 N) {
	return Vector3Subtract(Vector3Scale(N, 2.0f * Vector3DotProduct(N, R)), R);

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
		if (_lights[j].type == 0) {
			break;
		}

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

	return i;
}
Color TraceRay(Vector3 O, Vector3 D, float t_min, float t_max, int recursion_depth) {
	Vector3 P;
	Vector3 N;
	Vector3 R;
	
	Color local_color;
	float r = 0;
	
	ClosestIntersection(O, D, t_min, t_max);

	const float epsilon = 0.0001;
	// Use epsilon for floating-point comparison
	if (fabs(closest_sphere.radius) < epsilon) {
		return BGcolour;
	}
		
	P = Vector3Add(O, Vector3Scale(D,closest_t)); // Compute intersection
	N = Vector3Subtract(P, closest_sphere.center); // Compute sphere normal at intersection
	N = Vector3Scale( N, Vector3Length(N));

	//local colour
	local_color.r = (unsigned char)Clamp(closest_sphere.color.r * ComputeLighting(P, N,Vector3Negate(D),closest_sphere.specular),0,255);
	local_color.g = (unsigned char)Clamp(closest_sphere.color.g * ComputeLighting(P, N,Vector3Negate(D),closest_sphere.specular),0,255);
	local_color.b = (unsigned char)Clamp(closest_sphere.color.b * ComputeLighting(P, N,Vector3Negate(D),closest_sphere.specular),0,255);

	closest_sphere.color.r = (unsigned char)Clamp(closest_sphere.color.r * ComputeLighting(P, N, Vector3Negate(D), closest_sphere.specular), 0, 255);
	closest_sphere.color.g = (unsigned char)Clamp(closest_sphere.color.g * ComputeLighting(P, N, Vector3Negate(D), closest_sphere.specular), 0, 255);
	closest_sphere.color.b = (unsigned char)Clamp(closest_sphere.color.b * ComputeLighting(P, N, Vector3Negate(D), closest_sphere.specular), 0, 255);

	r = closest_sphere.reflective;

	if (recursion_depth <= 0 || r<=0 || !drawReflection) {
		return closest_sphere.color;
	}
	
	if (drawReflection) {
		Color reflected_color;
		//compute reflected colour
		R = ReflectRay(Vector3Negate(D), N);
		reflected_color = TraceRay(P, R, 0.001, INFINITY, (recursion_depth - 1));

		//clamp values
		local_color.r = (unsigned char)Clamp(local_color.r * (1 - r) + reflected_color.r * r, 0, 255);
		local_color.g = (unsigned char)Clamp(local_color.g * (1 - r) + reflected_color.g * r, 0, 255);
		local_color.b = (unsigned char)Clamp(local_color.b * (1 - r) + reflected_color.b * r, 0, 255);

		return local_color;
	}
}

void quit() {
	CloseWindow();
}
void input() {
	//Controls
	{
		if (IsKeyDown(KEY_W)) {
			O.z += cos(DEG2RAD * _y_rotation);  // Update Z based on the rotation
			O.x += sin(DEG2RAD * _y_rotation);  // Update X based on the rotation
		}
		if (IsKeyDown(KEY_S)) {
			O.z -= cos(DEG2RAD * _y_rotation);
			O.x -= sin(DEG2RAD * _y_rotation);
		}
		if (IsKeyDown(KEY_A)) {
			O.x -= cos(DEG2RAD * _y_rotation);
			O.z += sin(DEG2RAD * _y_rotation);
		}
		if (IsKeyDown(KEY_D)) {
			O.x += cos(DEG2RAD * _y_rotation);
			O.z -= sin(DEG2RAD * _y_rotation);
		}
		if (IsKeyDown(KEY_SPACE)) {
			O.y += 1;
		}
		if (IsKeyDown(KEY_LEFT_SHIFT)) {
			O.y -= 1;
		}

		if (IsKeyDown(KEY_DOWN)) {
			_x_rotation += 5;

		}
		if (IsKeyDown(KEY_UP)) {
			_x_rotation -= 5;
		}
		if (IsKeyDown(KEY_RIGHT)) {
			_y_rotation += 5;
		}
		if (IsKeyDown(KEY_LEFT)) {
			_y_rotation -= 5;
		}
	}
	//draw UI
	if (IsKeyPressed(KEY_Q)) {
		drawUI = !drawUI;
	}
	//draw editor
	if (drawUI && IsKeyPressed(KEY_TAB)) {
		drawEditor = !drawEditor;
	}

	//spawn Sphere
	if (IsKeyPressed(KEY_B) && drawUI && _spheresCount<SPHERES) {
		//create sphere object at camera position
		Sphere temp = { 0 };
		temp.center.x = O.x;
		temp.center.y = O.y;
		temp.center.z = O.z;
		
		temp.color = { 255,255,255,255 };

		temp.radius = 1.0;
		temp.specular = 100;

		_spheres[_spheresCount] = temp;
		_spheresCount++;
	}

	int delay = 2;
	int time = GetTime();

	if (IsKeyPressed(KEY_C) && drawUI && (time % delay == 1)) {
		
		currentSphereIndex++;
		if (currentSphereIndex >= _spheresCount) {
			currentSphereIndex = 0; 
		}
		currentSphereSelected = &_spheres[currentSphereIndex];

	}
	
	if (IsKeyPressed(KEY_X) && drawUI&&_spheresCount>=0) {
		deleteSphere();
	}

	if (IsKeyPressed(KEY_F) && drawEditor) {
		changeColour = true;
	}

	if (IsKeyPressed(KEY_L) && drawUI ) {
		currentLightIndex++;
		if (currentLightIndex >= _lightsCount) {
			currentLightIndex = 0;
		}
		currentLightSelected = &_lights[currentLightIndex];	
	}
	if (IsKeyPressed(KEY_K) && drawUI && _lightsCount >= 0) {
		deleteLight();
	}

	if (IsKeyPressed(KEY_R) && drawUI) {
		drawReflection = !drawReflection;
	}

	if (IsKeyPressed(KEY_J) && drawUI && _lightsCount < LIGHTS) {
		Light temp = { 0 };

		temp.type = 2;
		temp.position.x = O.x;
		temp.position.y = O.y;
		temp.position.z = O.z;

		temp.intensity = 0.2;
		
		_lights[_lightsCount] = temp;
		_lightsCount++;
	}

	if (IsKeyPressed(KEY_ONE)&&drawUI) {
		currentLightSelected->type = 1;
	}
	if (IsKeyPressed(KEY_TWO) && drawUI) {
		currentLightSelected->type = 2;
	}

	if (IsKeyPressed(KEY_U) && drawUI && currentLightSelected->intensity <= 1.0) {
		currentLightSelected->intensity += 0.1;
	}
	if (IsKeyPressed(KEY_I) && drawUI && currentLightSelected->intensity >=0.0) {
		currentLightSelected->intensity -= 0.1;
	}



}

void render() {
	BeginDrawing();
	
	for (int x = -CanvasWidth / 2; x <= CanvasWidth / 2; x++) {
		for (int y = -CanvasHeight / 2; y <= CanvasHeight / 2; y++) {
			Vector3 ViewportCoords = CanvasCoordstoViewportCoords(x, y);
			
			D =	Vector3Transform(Vector3Transform(ViewportCoords, X_Rotation(DEG2RAD * _x_rotation)), Y_Rotation(DEG2RAD * _y_rotation));
			color = TraceRay(O,D,1,INFINITY,recursion_depth);
			DrawPixelCenter(x, y, color);
		}
	}
	if (drawUI) {
		DrawUI();
	}
	if (drawUI && !drawEditor) {
		selectionFlash();
	}

	EndDrawing();
}

int main() {
	initialise();
	while (!WindowShouldClose()) {
		input();
		render();
	}
	quit();
	return 0;
}



