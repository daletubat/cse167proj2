#include "Window.h"
#include "OBJObject.h"


const char* window_title = "GLFW Starter Project";
Cube * cube;
GLint shaderProgram;

//Object Files
OBJObject* currObj;
OBJObject* bunny;
OBJObject* dragon;
OBJObject* bear;

double initX, initY;
double transX, transY;
double currX;
double currY;
bool rFlag;
bool lFlag;
glm::vec3 vec;


// On some systems you need to change this to the absolute path
#define VERTEX_SHADER_PATH "../shader.vert"
#define FRAGMENT_SHADER_PATH "../shader.frag"

// Default camera parameters
glm::vec3 cam_pos(0.0f, 0.0f, 20.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

int Window::width;
int Window::height;

glm::mat4 Window::P;
glm::mat4 Window::V;

void Window::initialize_objects()
{
	bunny = new OBJObject("C:\\Users\\Dale\\Documents\\_Schoolwork\\Year 4\\Fall 2016\\CSE 167 - Computer Graphics\\Project 2\\cse167proj2\\StarterCode\\bunny.obj");
	bunny->material = { { 1.0f, 0.5f, 0.31f }, { 1.0f, 0.5f, 0.31f }, { 0.5f, 0.5f, 0.5f } };
	dragon = new OBJObject("C:\\Users\\Dale\\Documents\\_Schoolwork\\Year 4\\Fall 2016\\CSE 167 - Computer Graphics\\Project 2\\cse167proj2\\StarterCode\\dragon.obj");
	dragon->material = { {0.25, 0.25, 0.25 },{ 0.4, 0.4, 0.4 },{ 0.774597, 0.774597, 0.774597 } };
	bear = new OBJObject("C:\\Users\\Dale\\Documents\\_Schoolwork\\Year 4\\Fall 2016\\CSE 167 - Computer Graphics\\Project 2\\cse167proj2\\StarterCode\\bear.obj");
	bear->material = { {0.1745, 0.01175, 0.01175},{ 0.61424, 0.04136, 0.04136 },{ 0.727811, 0.626959, 0.626959 } };

	currObj = bunny;

	// Load the shader program. Make sure you have the correct filepath up top
	shaderProgram = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up()
{
	delete(cube);
	delete(bunny);
	delete(dragon);
	delete(bear);
	glDeleteProgram(shaderProgram);
}

GLFWwindow* Window::create_window(int width, int height)
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ // Because Apple hates comforming to standards
	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		fprintf(stderr, "Either GLFW is not installed or your graphics card does not support modern OpenGL.\n");
		glfwTerminate();
		return NULL;
	}

// Make the context of the window
glfwMakeContextCurrent(window);

// Set swap interval to 1
glfwSwapInterval(1);

// Get the width and height of the framebuffer to properly resize the window
glfwGetFramebufferSize(window, &width, &height);
// Call the resize callback to make sure things get drawn immediately
Window::resize_callback(window, width, height);

return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
	Window::width = width;
	Window::height = height;
	// Set the viewport size. This is the only matrix that OpenGL maintains for us in modern OpenGL!
	glViewport(0, 0, width, height);

	if (height > 0)
	{
		P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
}

void Window::idle_callback()
{
	// Call the update function the cube
	//currObj->update();
}

void Window::display_callback(GLFWwindow* window)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use the shader of programID
	glUseProgram(shaderProgram);

	//Render the object
	currObj->draw(shaderProgram);

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key press
	if (action == GLFW_PRESS)
	{
		// Check if escape was pressed
		if (key == GLFW_KEY_ESCAPE)
		{
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		else if (key == GLFW_KEY_F1) {
			currObj = bunny;
		}

		else if (key == GLFW_KEY_F2) {
			currObj = dragon;
		}
		else if (key == GLFW_KEY_F3) {
			currObj = bear;
		}

		else if (key == GLFW_KEY_S)
		{
			if (mods == GLFW_MOD_SHIFT) {
				currObj->toscale(true);

			}
			else {
				currObj->toscale(false);
			}

		}

		else if (key == GLFW_KEY_R)
		{
			currObj->reset();
		}
	
		else if (key == GLFW_KEY_0) {
			if (currObj->camCtl) {
				currObj->camCtl = false;
			}
			else {
				currObj->camCtl = true;
			}
		}

		else if (key == GLFW_KEY_1) {
			currObj->camCtl = false;
			currObj->type = 1;
		}
		else if (key == GLFW_KEY_2) {
			currObj->camCtl = false;
			currObj->type = 2;
		}
		else if (key == GLFW_KEY_3) {
			currObj->camCtl = false;
			currObj->type = 3;
		}

		else if (key == GLFW_KEY_M) {
			if (currObj->mode) {
				currObj->mode--;
			}
			else {
				currObj->mode++;
			}
		}

		else if (key == GLFW_KEY_UP) {
			if (rFlag) {
				if (currObj->sptLightIn < 30.0f) {
					currObj->sptLightIn += 2.5f;
					currObj->sptLightOut += 2.5f;
				}
			}
		}
		
		else if (key == GLFW_KEY_DOWN) {
			if (rFlag) {
				if (currObj->sptLightIn > 3.0f) {
					currObj->sptLightIn -= 1.0f;
					currObj->sptLightOut -= 1.0f;
				}
			}
		}

		else if (key == GLFW_KEY_E) {
			if (mods == GLFW_MOD_SHIFT) {
				if (currObj->sptLightOut - currObj->sptLightIn >= 1.0f) {
					currObj->sptLightIn += 1.0f;
					std::cout << "Inner: " << currObj->sptLightIn << " Outer: " << currObj->sptLightOut << std::endl;

				}
			}
			else {
				if (currObj->sptLightOut - currObj->sptLightIn < 10.0f) {
					if (currObj->sptLightIn > 2.0f) {
						currObj->sptLightIn -= 1.0f;
						std::cout << "Inner: " << currObj->sptLightIn << " Outer: " << currObj->sptLightOut << std::endl;
					}
				}
			}
		}
	}
}
void Window::scroll_callback(GLFWwindow* window, double x_off, double y_off) {
	if (currObj->camCtl) {
		currObj->zPos += y_off;
		currObj->trans();
	}
	else if (currObj->type == 2) {
		currObj->ptLightDir.z += y_off;
	}
	else if (currObj->type == 3) {
		currObj->sptLightPos.z += y_off;
	}
}
void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
	
		if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
			lFlag = true;
			glfwGetCursorPos(window, &initX, &initY);
			vec.x = initX;
			vec.y = initY;
			currObj->ppt = trackBallMapping(vec);
		}

		if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
			lFlag = false;
		}
		if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS) {
			rFlag = true;
			glfwGetCursorPos(window, &initX, &initY);
			currX = initX;
			currY = initY;
		}
		
		if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_RELEASE) {
			rFlag = false;
		}
		
		/*
		if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
			lFlag = true;
			glfwGetCursorPos(window, &initX, &initY);
			currObj->dirLightDir.x = initX;
			currObj->dirLightDir.y = initY;
			//currObj->dirLightDir = trackBallMapping(vec);
			std::cout << "x: " << currObj->dirLightDir.x << "y: " << currObj->dirLightDir.y << std::endl;
		}
		if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
			lFlag = false;
		}
		*/
}
void Window::cursor_pos_callback(GLFWwindow* window, double xP, double yP) {
	//Mouse Control
		if (lFlag) {
			glm::vec3 dir;
			float angle;
			float vel;
			glm::vec3 temp;
			glm::vec4 temp4;
			temp.x = xP;
			temp.y = yP;
			currObj->cpt = trackBallMapping(temp);
			dir = currObj->cpt - currObj->ppt;
			vel = dir.length();
			if (vel > 0.01) {
				glm::vec3 axis;
				axis = glm::cross(currObj->ppt, currObj->cpt);
				angle = vel * 2.0f;
				if (currObj->camCtl) {
					currObj->toWorld = glm::rotate(glm::mat4(1.0f), (angle*glm::pi<float>()) / 180.0f, axis) * currObj->toWorld;
				}
				else if (currObj->type == 1) {
					//currObj->dirLightDir.x = -(axis.x * 100);
					//currObj->dirLightDir.y = -(axis.y * 100);
					temp4.x = currObj->dirLightDir.x;
					temp4.y = currObj->dirLightDir.y;
					temp4.z = currObj->dirLightDir.z;
					temp4.w = 0.0f;
					temp4 = glm::rotate(glm::mat4(1.0f), (angle * glm::pi<float>()) / 180.0f, axis) * temp4;

					currObj->dirLightDir.x = temp4.x;
					currObj->dirLightDir.y = temp4.y;
					currObj->dirLightDir.z = temp4.z;
					//std::cout << "x: " << currObj->dirLightDir.x << "y: " << currObj->dirLightDir.y << std::endl;
				}
				else if (currObj->type == 2) {
					//currObj->ptLightDir.x = -(axis.x * 100);
					//currObj->ptLightDir.y = -(axis.y * 100);
					//currObj->ptLightDir = glm::rotate(glm::vec3(1.0f), (angle*glm::pi<float>()) / 180.0f, axis) * currObj->ptLightDir;
					temp4.x = currObj->ptLightDir.x;
					temp4.y = currObj->ptLightDir.y;
					temp4.z = currObj->ptLightDir.z;
					temp4.w = 0.0f;
					temp4 = glm::rotate(glm::mat4(1.0f), (angle * glm::pi<float>()) / -180.0f, axis) * temp4;

					currObj->ptLightDir.x = temp4.x;
					currObj->ptLightDir.y = temp4.y;
					currObj->ptLightDir.z = temp4.z;
				}
				else if (currObj->type == 3) {
					//currObj->sptLightPos = glm::rotate(glm::vec3(1.0f), (angle*glm::pi<float>()) / 180.0f, axis) * currObj->sptLightPos;
					temp4.x = currObj->sptLightPos.x;
					temp4.y = currObj->sptLightPos.y;
					temp4.z = currObj->sptLightPos.z;
					temp4.w = 0.0f;
					temp4 = glm::rotate(glm::mat4(1.0f), (angle * glm::pi<float>()) / -180.0f, axis) * temp4;

					currObj->sptLightPos.x = temp4.x;
					currObj->sptLightPos.y = temp4.y;
					currObj->sptLightPos.z = temp4.z;
				}
			}
			currObj->ppt = currObj->cpt;


		}

		if (rFlag) {
			if (currObj->camCtl) {
				transX = xP - currX;
				transY = yP - currY;

				currX = xP;
				currY = yP;

				currObj->xPos += transX / 50;
				currObj->yPos -= transY / 50;

				currObj->trans();
			}
		}
	//}
	//Direction Light Control
	/*else if (!(currObj->camCtl) && currObj->type == 1) {
		//get coordinates and translate that into light direction vector
		if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
			lFlag = true;
			glfwGetCursorPos(window, &initX, &initY);
			currObj->dirLightxDir = initX;
			currObj->dirLightyDir = initY;
		}
		if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
			lFlag = false;
		}
	} */
}

glm::vec3 Window::trackBallMapping(glm::vec3 mapIn) {
	glm::vec3 temp;
	float d;
	temp.x = (2.0*mapIn.x - width) / width;
	temp.y = (height - 2.0*mapIn.y) / height;
	temp.z = 0.0;
	d = temp.length();
	if (d > 1.0) {
		d = 1.0;
	}
	temp.z = sqrt(1.001 - d * d);
	temp = glm::normalize(temp);
	return temp;

}