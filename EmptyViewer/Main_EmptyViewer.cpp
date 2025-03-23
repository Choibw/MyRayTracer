#include <Windows.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/freeglut.h>

#define GLFW_INCLUDE_GLU
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <vector>

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Ray.h"
#include "Plane.h"
#include "Sphere.h"
#include "Scene.h"
#include "Camera.h"

using namespace glm;

// -------------------------------------------------
// Global Variables
// -------------------------------------------------
int Width = 512;
int Height = 512;

vec3 eye = glm::vec3(0.0f, 0.0f, 0.0f); // 카메라 위치
vec3 U = glm::vec3(1.0f, 0.0f, 0.0f); // x방향
vec3 V = glm::vec3(0.0f, 1.0f, 0.0f); // y방향
vec3 W = glm::vec3(0.0f, 0.0f, 1.0f); // z방향 (카메라는 -z를 본다고 가정)
std::vector<float> OutputImage;
// -------------------------------------------------

static Scene scene;
static Camera* camera = nullptr;

void setupScene()
{
	// 1) 카메라 생성
	camera = new Camera(
		glm::vec3(0, 0, 0),       // eye
		glm::vec3(1, 0, 0),       // U
		glm::vec3(0, 1, 0),       // V
		glm::vec3(0, 0, 1),       // W (주의: -z 방향을 본다고 하면, W=(0,0,1) but look direction is -W)
		-0.1f, 0.1f,            // l, r
		-0.1f, 0.1f,            // b, t
		0.1f,                   // d
		512, 512                // Nx, Ny
	);

	// 2) 오브젝트들 추가
	// Plane y=-2 => n=(0,1,0), d=2
	scene.addObject(new Plane(glm::vec3(0, 1, 0), 2.0f));

	// Sphere 1
	scene.addObject(new Sphere(glm::vec3(-4, 0, -7), 1.0f));
	// Sphere 2
	scene.addObject(new Sphere(glm::vec3(0, 0, -7), 2.0f));
	// Sphere 3
	scene.addObject(new Sphere(glm::vec3(4, 0, -7), 1.0f));
}

void render()
{
	//Create our image. We don't want to do this in 
	//the main loop since this may be too slow and we 
	//want a responsive display of our beautiful image.
	//Instead we draw to another buffer and copy this to the 
	//framebuffer using glDrawPixels(...) every refresh
	OutputImage.clear();
	OutputImage.reserve(Width * Height * 3);

	for (int j = 0; j < Height; ++j) 
	{
		for (int i = 0; i < Width; ++i) 
		{
			// ---------------------------------------------------
			// --- Implement your code here to generate the image
			// ---------------------------------------------------

			// (A) 카메라에서 광선 생성
			//     camera->getRay(i,j)가 (i,j) 픽셀을 위한 Ray(eye, dir)를 반환
			Ray ray = camera->getRay(i, j);

			// (B) Scene 교차 검사
			float tHit;
			Surface* hitObj = scene.trace(ray, 0.001f, 1e30f, tHit);

			// (C) 색상 결정
			//     교차가 있으면 흰색, 없으면 검정
			glm::vec3 color(0.0f);
			if (hitObj != nullptr)
			{
				color = glm::vec3(1.0f);
			}

			// 6) Store the color into the output image buffer.
			OutputImage.push_back(color.x); // R
			OutputImage.push_back(color.y); // G
			OutputImage.push_back(color.z); // B
		}
	}
}


void resize_callback(GLFWwindow*, int nw, int nh) 
{
	//This is called in response to the window resizing.
	//The new width and height are passed in so we make 
	//any necessary changes:
	Width = nw;
	Height = nh;
	//Tell the viewport to use all of our screen estate
	glViewport(0, 0, nw, nh);

	//This is not necessary, we're just working in 2d so
	//why not let our spaces reflect it?
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0.0, static_cast<double>(Width)
		, 0.0, static_cast<double>(Height)
		, 1.0, -1.0);

	//Reserve memory for our render so that we don't do 
	//excessive allocations and render the image
	OutputImage.reserve(Width * Height * 3);
	render();
}


int main(int argc, char* argv[])
{
	// -------------------------------------------------
	// Initialize Window
	// -------------------------------------------------

	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(Width, Height, "OpenGL Viewer", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	//We have an opengl context now. Everything from here on out 
	//is just managing our window or opengl directly.

	//Tell the opengl state machine we don't want it to make 
	//any assumptions about how pixels are aligned in memory 
	//during transfers between host and device (like glDrawPixels(...) )
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	setupScene();

	//We call our resize function once to set everything up initially
	//after registering it as a callback with glfw
	glfwSetFramebufferSizeCallback(window, resize_callback);
	resize_callback(NULL, Width, Height);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		//Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		// -------------------------------------------------------------
		//Rendering begins!
		glDrawPixels(Width, Height, GL_RGB, GL_FLOAT, &OutputImage[0]);
		//and ends.
		// -------------------------------------------------------------

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		//Close when the user hits 'q' or escape
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS
			|| glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
