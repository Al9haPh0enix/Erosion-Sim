#include <iostream>
#include <string>
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include "Main.h"

#define pi 3.14159265

// Settings //
const int movement_const = 5;
const int* background = new int[50, 127, 50];
const float alignment = .01;
const float gravity = 1.5;
const float surfTention = 2;
int numOfWater = 100;
const int numOfSediment = 50000;

// Variables //
float highest = 1;

int randint(int end) {
	end += 1;
	return rand() % end;
}

float dist(float x1, float y1, float x2, float y2) {
	return sqrt(((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)));
}

float sed_len(sediment* arr) {
	return *(&arr + 1) - arr;
}

float wat_len(water* arr) {
	return *(&arr + 1) - arr;
}

// Sediment class //
sediment::sediment(float xx, float yy, int r) {
	x = xx;
	y = yy;
	radius = r;
}

void sediment::move(float angle, float vx, float vy) {
	x += ((cos(angle * (pi / 180)) * movement_const) + vx) / 5;
	y += ((sin(angle * (pi / 180)) * movement_const) + vy) / 5;
}

// Water class //
water::water(float xx, float yy, float a, int r) {
	x = xx;
	y = yy;
	angle = a;
	radius = r;
	velx = 0;
	vely = 0;
}

void water::check(sediment* sedimentL) {
	for (int ind = 0; ind < numOfSediment; ind++) {
		sediment* i = &sedimentL[ind];
		if (dist( x + cos(angle * (pi / 180)),     y + sin(angle * (pi / 180)),     (*i).x,     (*i).y ) < (*i).radius) {
			sedimentL[ind].move(angle + (randint(4) - 2), velx, vely);
			angle += randint(10) - 5;
			vely *= .25f;
			velx *= (randint(2) - 1) * .25;
		}
	}
}

void water::move(sediment* sedL, water* watL) {

	while (angle > 360) {
		angle -= 360;
	}
	while (angle < 0) {
		angle += 360;
	}

	vely -= gravity;

	for (int ind = 0; ind < numOfWater; ind++) {
		water* i = &watL[ind];
		if (dist(x, y, (*i).x, (*i).y) < (radius + (*i).radius) * surfTention) {
			if ((*i).angle-180 < angle-180) {
				angle -= alignment;
			}
			if ((*i).angle-180 > angle-180) {
				angle += alignment;
			}
		}
	}

	velx += cos(angle * (pi / 180));
	vely += sin(angle * (pi / 180));
	check(sedL);

	x += velx / 7.5f;
	y += vely / 7.5f;

	velx *= .99f;
	vely *= .99f;
}

// Vertex Shader source code
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
//Fragment Shader source code
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f);\n"
"}\n\0";


void setPixel(int x, int y, GLfloat* c)
{
	glEnable(GL_SCISSOR_TEST);
	glScissor(x, y, 1, 1);
	glClearColor(c[0], c[1], c[2], c[3]);
	glClear(GL_COLOR_BUFFER_BIT);
	// Remember to disable scissor test, or, perhaps reset the scissor rectangle:
	glDisable(GL_SCISSOR_TEST);
}

void circle(int x, int y, int r, GLfloat* color)
{
	double i, angle, x1, y1;

	while(r >= 0){
		for (i = 0; i < 360; i += 0.1)
		{
			angle = i;
			x1 = r * cos(angle * pi / 180);
			y1 = r * sin(angle * pi / 180);
			setPixel(x + x1, y + y1, color);
		}
		r -= 1;
	}
}

int main()
{

	sediment* seds = (sediment*)malloc(sizeof(sediment) * numOfSediment);

	for (int i = 0; i < numOfSediment; i++) {
		seds[i] = sediment(randint(400) + 200, randint(400), 1);
	}

	water* wats = (water*)malloc(sizeof(water) * numOfWater);

	for (int i = 0; i < numOfWater; i++) {
		wats[i] = water(400, 400, randint(360), 1);
	}

	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object of 800 by 800 pixels, naming it "Water Simlutation"
	GLFWwindow* window = glfwCreateWindow(800, 800, "Water Simlutation", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, 800, 800);

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		highest = 0;

		GLfloat color[4] = { 0.2, 0.2, 1.0, 1.0 };
		GLfloat color2[4] = { 0.6, 0.1, 0.1, 1.0 };

		// Specify the color of the background
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT);


		for (int ind = 0; ind < numOfWater; ind++) {
			water* i = &wats[ind];

			(*i).move(seds, wats);

			setPixel((*i).x, (*i).y, color);
			if( (*i).y > highest ){
				highest = (*i).y;
			}
		}

		if (highest == 0) {
			for (int i = 0; i < numOfWater; i++) {
				wats[i] = water(400, 400, randint(360), 1);
			}
		}

		for (int ind = 0; ind < numOfSediment; ind++) {
			setPixel(seds[ind].x, seds[ind].y, color2);
			highest = 1;
		}

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}