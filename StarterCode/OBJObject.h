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
	std::vector<glm::vec3> ind_verts;
	std::vector<unsigned int> ind;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	OBJObject(const char* filepath);
	~OBJObject();

	void parse(const char* filepath);
	void draw(GLuint);

	glm::mat4 toWorld;
	float size;
	float angle;
	float tranX;
	float tranY;
	float tranZ;
	float scale;
	float orbit;


	////
	float xPos = 0.0f;
	float yPos = 0.0f;
	float zPos = 0.0f;
	glm::vec3 translate;
	void OBJObject::trans();
	glm::vec3 cpt;
	glm::vec3 ppt;
	int type = 1;
	int mode = 0;


	//for finding the center point
	float minX;
	float maxX;
	float minY;
	float maxY;
	float minZ;
	float maxZ;
	float avX;
	float avY;
	float avZ;
	float big;
	int bigAx;

	//for shaders
	GLuint VAO, VBO, EBO, NBO;
	GLuint uProjection, uModelview;
	glm::mat3 material;

	//for lighting
	glm::vec3 dirLightDir;
	glm::vec3 ptLightDir;
	glm::vec3 sptLightPos;
	bool camCtl;
	float sptLightOut;
	float sptLightIn;

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