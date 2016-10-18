#include "OBJObject.h"
#include <iostream>


OBJObject::OBJObject(const char *filepath) 
{
	toWorld = glm::mat4(1.0f);
	parse(filepath);
	tranX = 0;
	tranY = 0;
	tranZ = 0;
	scale = 1;
	orbit = 0;
	minX, minY, minZ = std::numeric_limits<float>::infinity();; 
	maxX, maxY, maxZ = 0;
	big = 0;
	clk = true;
}

void OBJObject::parse(const char *filepath) 
{
	FILE* fp;     // file pointer
	float x, y, z;  // vertex coordinates
	float r, g, b;  // vertex color
	int c1, c2;    // characters read from file
	//float t; //trash

	fp = fopen(filepath, "rb");  // make the file name configurable so you can load other files
	if (fp == NULL) { std::cerr << "error loading file" << std::endl; exit(-1); }  // just in case the file can't be found or is corrupt

	while (1) {
		c1 = fgetc(fp);
		if (c1 == EOF) {
			break;
		}		
		//vertices
		if (c1 == 'v') {
			c2 = fgetc(fp);
			if (c2 == ' ') {
				fscanf(fp, "%f %f %f %f %f %f", &x, &y, &z, &r, &g, &b);
				glm::vec3 v2Add = glm::vec3(x, y, z); //create vector out of points
				vertices.push_back(v2Add); //push vector onto vertices

				//finding max, min
				
				if (x < minX) { minX = x; }
				if (y < minY) { minY = y; }
				if (z < minZ) { minZ = z; }
				if (x > maxX) { maxX = x; }
				if (y > maxY) { maxY = y; }
				if (z > maxZ) { maxZ = z; }
			
			}
			else if (c2 == 'n') {
				//vertex normals

				fscanf(fp, "%f %f %f", &x, &y, &z);
				glm::vec3 vn2Add = glm::vec3(x, y, z);
				normals.push_back(vn2Add);
			}
		}

		//faces
		else  if (c1 == 'f'){ 
			c2 = fgetc(fp);
			if (c2 == ' ') {
				//WARNING: Indexing obj files begins at 1
				//not sure what that means though
				fscanf(fp, "%f//%f %f//%f %f//%f", &x, &r, &y, &g, &z, &b);
				//std::cout << "x : " << x << " y : " << y << " z: " << z << std::endl;
				glm::vec3 ind2Add = glm::vec3(x, y, z);
				indices.push_back(ind2Add);

			}
		}
	}
	std::cout << "MaxX: " << maxX << " MaxY: " << maxY << " MaxZ: " << maxZ << std::endl << "minX: " << minX << " minY: " << minY << " minZ: " << minZ << std::endl;
	avX = (maxX - minX) / 2;
	avY = (maxY - minY) / 2;
	avZ = (maxZ - minZ) / 2;
	big = maxAxis( avX, avY, avZ ); //need to know which axis is the biggest?
	//std::cout << "max axis is of size: " << big << std::endl;
	fclose(fp);
}

void OBJObject::draw() 
{
	glMatrixMode(GL_MODELVIEW);

	// Push a save state onto the matrix stack, and multiply in the toWorld matrix
	glPushMatrix();
	glMultMatrixf(&(toWorld[0][0]));

	glBegin(GL_POINTS);
	// Loop through all the vertices of this OBJ Object and render them
	for (unsigned int i = 0; i < vertices.size(); ++i) 
	{
		//set color
		glm::vec3 color = glm::normalize(normals[i]);
		glColor3f((color.x + 1) / 2, (color.y + 1) / 2, (color.z + 1) / 2);

		//set points
		glVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);
	}
	glEnd();

	// Pop the save state off the matrix stack
	// This will undo the multiply we did earlier
	glPopMatrix();
}
void OBJObject::update()
{
	spin(1.0f);
}

void OBJObject::spin(float deg)
{

	glm::mat4 currM = this->toWorld;
	this->angle += deg;
	if (this->angle > 360.0f || this->angle < -360.0f) this->angle = 0.0f;
	// This creates the matrix to rotate the cube
	this->toWorld = this->toWorld* glm::rotate(glm::mat4(1.0f), deg / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
	

}

void OBJObject::orb() {
	if (this->clk) {
		this->toWorld = glm::rotate(glm::mat4(1.0f), -50.0f, glm::vec3(0.0f, 0.0f, 1.0f)) * toWorld;
	}
	else {
		this->toWorld = glm::rotate(glm::mat4(1.0f), 50.0f, glm::vec3(0.0f, 0.0f, 1.0f)) * toWorld;
	}
}


void OBJObject::translatex(int d){
	this->toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(d, 0, 0)) * this->toWorld;
}

void OBJObject::translatey(int d) {
	this->toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(0, d, 0)) * this->toWorld;
}
void OBJObject::translatez(int d) {
	this->toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, d)) * this->toWorld;
}

void OBJObject::rotate() {
	this->toWorld = glm::rotate(glm::mat4(1.0f), this->orbit, glm::vec3(0, 0, 1)) *this->toWorld;
}

void OBJObject::toscale(bool up) {
	if (up) {
		
		this->toWorld = toWorld* glm::scale(glm::mat4(1.0f), glm::vec3(1.50f, 1.50f, 1.50f));
	}
	else if(!up) {
		this->toWorld = toWorld* glm::scale(glm::mat4(0.1f), glm::vec3(0.5f, 0.5f, 0.5f));
	}
	
}

void OBJObject::reset() {
	this->toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(-tranX, -tranY, -tranZ)) * this->toWorld;
	//this->toWorld = glm::rotate(glm::mat4(1.0f), -(this->orbit), glm::vec3(0, 0, 1)) *this->toWorld;
}



std::vector<glm::vec3> OBJObject::getVert() {
	return this->vertices;
}
std::vector<glm::vec3> OBJObject::getNorm() {
	return this->normals;
}

float maxAxis(float x, float y, float z) {
	//float axis = x;
	//if (axis < y) { axis = y; }
	//if (axis < z) { axis = z; }
	
	return x;
}