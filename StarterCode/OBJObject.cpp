#include "OBJObject.h"
#include "Window.h"
#include <iostream>


OBJObject::OBJObject(const char *filepath) 
{
	toWorld = glm::mat4(1.0f);
	parse(filepath);
	
	tranX , tranY, tranZ, big = 0;
	scale = 1;
	orbit = 0;
	minX, minY, minZ = std::numeric_limits<float>::infinity();; 
	maxX, maxY, maxZ = 0;
	clk = true;
	type = 1;

	//lighting stuff
	dirLightDir.x = -0.2f;
	dirLightDir.y = -1.0f;
	ptLightDir = { 0.0f, 0.0f, -5.0f };
	
	sptLightIn = 7.0f;
	sptLightOut = 9.0f;
	sptLightPos = { 0.0f, 0.0f, -5.0f };


	//From Cube.cpp
	// Create array object
	glGenVertexArrays(1, &VAO);
	
	//Create buff objects so they can carry me through this project
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &NBO); //Normal Buffer Array for throwing shade
	
	glBindVertexArray(VAO); //Bind the vertex object array

	glBindBuffer(GL_ARRAY_BUFFER, VBO); //Vertex Buffer Object
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW); //populate vbo with data	
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0); //arguments found in Cube.cpp
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); //Element Buffer Object
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * sizeof(unsigned int), ind.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, NBO); //Normal Buffer Object
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0); 
	
	

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

OBJObject::~OBJObject() {
		// Delete previously generated buffers. Note that forgetting to do this can waste GPU memory in a 
		// large project! This could crash the graphics driver due to memory leaks, or slow down application performance!
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &NBO);
		glDeleteBuffers(1, &EBO);
}


void OBJObject::parse(const char *filepath) 
{
	FILE* fp;     // file pointer
	float x, y, z;  // vertex coordinates
	float r, g, b;  // vertex color
	int c1, c2;    // characters read from file
	unsigned int i1, i2, i3; //unsigned ints for the indices for faces
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

				//finding max, min of x, y, and z
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
				fscanf(fp, "%i//%i %i//%i %i//%i", &i1, &i1, &i2, &i2, &i3, &i3);
				ind.push_back(i1-1); //-1 to properly index the data
				ind.push_back(i2-1);
				ind.push_back(i3-1);

			}
		}
	}
	
	//Finding the center
	std::cout << "MaxX: " << maxX << " MaxY: " << maxY << " MaxZ: " << maxZ << std::endl << "minX: " << minX << " minY: " << minY << " minZ: " << minZ << std::endl;
	avX = (maxX + minX) / 2;
	avY = (maxY + minY) / 2;
	avZ = (maxZ + minZ) / 2;
	
	std::cout << "avX: " << avX << " avY: " << avY << " avZ: " << avZ << std::endl;
	
	//max (maxX - minX, maxY - minY, maxZ - minZ)
	big = (maxX - minX); 
	if (big < (maxY - minY)) { big = (maxY - minY); }
	if (big < (maxZ-minZ)) { big = (maxZ-minZ); } 
	
	fclose(fp);

	//second parse through all of the vertices to change their size to fit inside the imaginary 2x2x2 square 
	for (unsigned int i = 0; i < vertices.size(); i++) {
		float curX = vertices[i].x;
		curX = (curX - avX)/big;

		float curY = vertices[i].y;
		curY = (curY - avY) / big;
		
		float curZ = vertices[i].z;
		curZ = (curZ - avZ) / big;
		
		glm::vec3 newVert = glm::vec3(curX, curY, curZ);
		vertices[i] = newVert;
	}

}

void OBJObject::draw(GLuint shaderProgram)
{
	//From Cube.cpp
	// Calculate the combination of the model and view (camera inverse) matrices
	glm::mat4 modelview = Window::V * toWorld;
	// We need to calcullate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
	// Consequently, we need to forward the projection, view, and model matrices to the shader programs
	// Get the location of the uniform variables "projection" and "modelview"
	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModelview = glGetUniformLocation(shaderProgram, "modelview");

	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);

	/////Material values for the object///
	GLint matAmbientLoc = glGetUniformLocation(shaderProgram, "material.ambient");
	GLint matDiffuseLoc = glGetUniformLocation(shaderProgram, "material.diffuse");
	GLint matSpecularLoc = glGetUniformLocation(shaderProgram, "material.specular");
	GLint matShineLoc = glGetUniformLocation(shaderProgram, "material.shininess");

	glUniform3f(matAmbientLoc, material[0].x, material[0].y, material[0].z);
	glUniform3f(matDiffuseLoc, material[1].x, material[1].y, material[1].z);
	glUniform3f(matSpecularLoc, material[2].x, material[2].y, material[2].z);
	glUniform1f(matShineLoc, 32.0f);

	///////lighting////////
	glm::vec3 pointLightColors[] = {

		glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.8f, 0.8f, 0.8f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(0.4f, 0.7f, 0.1f)
	};

		// Directional light
	

		glUniform3f(glGetUniformLocation(shaderProgram, "dirLight.direction"), dirLightDir.x, dirLightDir.y, -0.3f);
		glUniform3f(glGetUniformLocation(shaderProgram, "dirLight.ambient"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(shaderProgram, "dirLight.diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(shaderProgram, "dirLight.specular"), 1.0f, 1.0f, 1.0f);

		// Point light 1

		glUniform3f(glGetUniformLocation(shaderProgram, "pointLights[0].position"), ptLightDir.x, ptLightDir.y, ptLightDir.z);
		glUniform3f(glGetUniformLocation(shaderProgram, "pointLights[0].ambient"), pointLightColors[0].x , pointLightColors[0].y, pointLightColors[0].z);
		glUniform3f(glGetUniformLocation(shaderProgram, "pointLights[0].diffuse"), pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
		glUniform3f(glGetUniformLocation(shaderProgram, "pointLights[0].specular"), pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
		glUniform1f(glGetUniformLocation(shaderProgram, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "pointLights[0].linear"), 0.07);
		glUniform1f(glGetUniformLocation(shaderProgram, "pointLights[0].quadratic"), 0.017);

		//SpotLight
		glUniform3f(glGetUniformLocation(shaderProgram, "spotLight.position"), 0.0f, 0.0f, sptLightPos.z);
		glUniform3f(glGetUniformLocation(shaderProgram, "spotLight.direction"), sptLightPos.x, sptLightPos.y, sptLightPos.z);
		glUniform3f(glGetUniformLocation(shaderProgram, "spotLight.ambient"), 0.1f, 0.1f, 0.1f);
		glUniform3f(glGetUniformLocation(shaderProgram, "spotLight.diffuse"), 0.8f, 0.8f, 0.8f);
		glUniform3f(glGetUniformLocation(shaderProgram, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "spotLight.spotExponent"), 0.2f);
		glUniform1f(glGetUniformLocation(shaderProgram, "spotLight.quadratic"), 0.017);
		glUniform1f(glGetUniformLocation(shaderProgram, "spotLight.cutOff"), glm::cos(glm::radians(sptLightIn)));
		glUniform1f(glGetUniformLocation(shaderProgram, "spotLight.outerCutOff"), glm::cos(glm::radians(sptLightOut))); 
		
		glUniform1i(glGetUniformLocation(shaderProgram, "type"), type); //for light type
		glUniform1i(glGetUniformLocation(shaderProgram, "mode"), mode); //for mode


	// Now draw the cube. We simply need to bind the VAO associated with it.
	glBindVertexArray(VAO);
	// Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
	glDrawElements(GL_TRIANGLES, ind.size(), GL_UNSIGNED_INT, 0);
	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);
	
	/*Old Draw
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
	*/
}
void OBJObject::update()
{
	//spin(1.0f);
}

void OBJObject::spin(float deg)
{

	//glm::mat4 currM = this->toWorld;
	//this->angle += deg;
	//if (this->angle > 360.0f || this->angle < -360.0f) this->angle = 0.0f;
	// This creates the matrix to rotate the cube
	//this->toWorld = this->toWorld* glm::rotate(glm::mat4(1.0f), deg / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
	

}

void OBJObject::orb() {
	if (this->clk) {
		this->toWorld = glm::rotate(glm::mat4(1.0f), -50.0f, glm::vec3(0.0f, 0.0f, 1.0f)) * toWorld;
	}
	else {
		this->toWorld = glm::rotate(glm::mat4(1.0f), 50.0f, glm::vec3(0.0f, 0.0f, 1.0f)) * toWorld;
	}
}


void OBJObject::translatex(int d) {
	this->toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(d, 0, 0)) * this->toWorld;
}
void OBJObject::translatey(int d) {
	this->toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(0, d, 0)) * this->toWorld;
}
void OBJObject::translatez(int d) {
	this->toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, d)) * this->toWorld;
}

void OBJObject::trans() {
	translate.x = xPos;
	translate.y = yPos;
	translate.z = zPos;

	toWorld = glm::translate(glm::mat4(1.0f), translate)* toWorld;

	xPos = 0;
	yPos = 0;
	zPos = 0;

}

void OBJObject::rotate() {
	this->toWorld = glm::rotate(glm::mat4(1.0f), this->orbit, glm::vec3(0, 0, 1)) *this->toWorld;
}

void OBJObject::toscale(bool up) {
	if (up) {
		this->toWorld = toWorld* glm::scale(glm::mat4(1.0f), glm::vec3(1.50f, 1.50f, 1.50f));
	}
	else if(!up) {
		this->toWorld = toWorld* glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
	}
	
}

void OBJObject::reset() {
	this->toWorld = glm::mat4(1.0f);
}



std::vector<glm::vec3> OBJObject::getVert() {
	return this->vertices;
}
std::vector<glm::vec3> OBJObject::getNorm() {
	return this->normals;
}



//things done:
//Indexed data so the off by one thing doesn't ruin our shit
//added face parsing
//calculated center of the object (kinda)