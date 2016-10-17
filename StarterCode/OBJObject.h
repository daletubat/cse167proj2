#ifndef OBJOBJECT_H
#define OBJOBJECT_H

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#include <OpenGL/gl.h> // Remove this line in future projects
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class OBJObject
{
private:



public:
	std::vector<glm::vec3> indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	OBJObject(const char* filepath);

	void parse(const char* filepath);
	void draw();

	glm::mat4 toWorld;
	float size;
	float angle;
	float tranX;
	float tranY;
	float tranZ;
	float scale;
	float orbit;
	void update();
	void spin(float deg);
	bool clk;
	void orb();
	void rotate();
	void toscale(bool up);
	void translatex(int d);
	void translatey(int d);
	void translatez(int d);
	void reset();


	std::vector<glm::vec3> getVert();
	std::vector<glm::vec3> getNorm();
	glm::mat4 get2w();


};

#endif