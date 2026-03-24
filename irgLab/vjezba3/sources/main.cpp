// Local Headers
#include "Shader.h"
#include "FPSManager.h"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Standard Headers
#include <cstdio>
#include <cstdlib>

#include <iostream>

int width = 800, height = 800;
glm::vec3 drawingColor = glm::vec3(1, 1, 1);
glm::vec3 adjustingColor = glm::vec3(1.0, 0, 0);
float mouseX = 0, mouseY = 0;

std::vector<float> polygonVertices;
std::vector<unsigned int> polygonIndices;
std::vector<unsigned int> linesIndices;
GLuint VAO[3];
GLuint VBO[2];
GLuint EBO[2];

//malo je nespretno napravljeno jer ne koristimo c++17, a treba podrzati i windows i linux,
//slobodno pozivajte new Shader(...); direktno
Shader* loadShader(char* path, char* naziv) {
	std::string sPath(path);
	std::string pathVert;
	std::string pathFrag;

	pathVert.append(path, sPath.find_last_of("\\/") + 1);
	pathFrag.append(path, sPath.find_last_of("\\/") + 1);
	if (pathFrag[pathFrag.size() - 1] == '/') {
		pathVert.append("shaders/");
		pathFrag.append("shaders/");
	}
	else if (pathFrag[pathFrag.size() - 1] == '\\') {
		pathVert.append("shaders\\");
		pathFrag.append("shaders\\");
	}
	else {
		std::cerr << "nepoznat format pozicije shadera";
		exit(1);
	}

	pathVert.append(naziv);
	pathVert.append(".vert");
	pathFrag.append(naziv);
	pathFrag.append(".frag");

	return new Shader(pathVert.c_str(), pathFrag.c_str());
}

//funkcija koja se poziva prilikom mijenjanja velicine prozora, moramo ju povezati pomocu glfwSetFramebufferSizeCallback
void framebuffer_size_callback(GLFWwindow * window, int Width, int Height)
{
	width = Width;
	height = Height;
	glViewport(0, 0, width, height);
}
 
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key==GLFW_KEY_R && action==GLFW_PRESS) {
		adjustingColor = glm::vec3(1, 0, 0);
	}
	if (key==GLFW_KEY_G && action==GLFW_PRESS) {
		adjustingColor = glm::vec3(0, 1, 0);
	}
	if (key==GLFW_KEY_B && action==GLFW_PRESS) {
		adjustingColor = glm::vec3(0, 0, 1);
	}
	glm::vec3 minColorComponent(0.0f);
	glm::vec3 maxColorComponent(1.0f);
	if (key==GLFW_KEY_DOWN && action==GLFW_PRESS) {
		drawingColor = glm::clamp(drawingColor - 0.1f * adjustingColor, minColorComponent, maxColorComponent);
	}
	if (key==GLFW_KEY_UP && action==GLFW_PRESS) {
		drawingColor = glm::clamp(drawingColor + 0.1f * adjustingColor, minColorComponent, maxColorComponent);
	}
}

void mouse_pos_callback(GLFWwindow* window, double xpos, double ypos) {
	mouseX = xpos/(width/2)-1;
	mouseY = (height-ypos)/(height/2)-1;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button==GLFW_MOUSE_BUTTON_LEFT && action==GLFW_PRESS) {
		std::vector<float> tmp = { mouseX, mouseY, drawingColor.r, drawingColor.g, drawingColor.b };
		polygonVertices.insert(polygonVertices.end(), tmp.begin(), tmp.end());

		if (polygonVertices.size()>=3*5) {
			polygonIndices.push_back(polygonVertices.size()/5-3);
			polygonIndices.push_back(polygonVertices.size()/5-2);
			polygonIndices.push_back(polygonVertices.size()/5-1);
		}
		linesIndices.push_back(linesIndices.size());
		
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, (polygonVertices.size()+5) * sizeof(float), polygonVertices.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (polygonIndices.size()+5) * sizeof(unsigned int), polygonIndices.data(), GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (linesIndices.size()) * sizeof(unsigned int), linesIndices.data(), GL_DYNAMIC_DRAW);
	}
}

int main(int argc, char * argv[]) {
	std::cout << argv[0] << std::endl;
	/*********************************************************************************************/
	//postavljanje OpenGL konteksta, dohvacanje dostupnih OpenGL naredbi
	GLFWwindow* window;

	glfwInit();
	gladLoadGL();

	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);


	window = glfwCreateWindow(width, height, "Zadatak X", nullptr, nullptr);
	// provjeri je li se uspio napraviti prozor
	if (window == nullptr) {
		fprintf(stderr, "Failed to Create OpenGL Context");
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

	// dohvati sve dostupne OpenGL funkcije
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		fprintf(stderr, "Failed to initialize GLAD");
		exit(-1);
	}
	fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));


	//glEnable(GL_DEPTH_TEST); //ukljuci z spremnik (prikazuju se oni fragmenti koji su najblizi promatracu)
	glDepthFunc(GL_LESS);

	//glEnable(GL_CULL_FACE); //ukljuci uklanjanje straznjih poligona -- za ovaj primjer je iskljuceno
	//glCullFace(GL_BACK); 

	glClearColor(0.15f, 0.1f, 0.1f, 1.0f); //boja brisanja platna izmedu iscrtavanja dva okvira


	glfwSwapInterval(0); //ne cekaj nakon iscrtavanja (vsync)

	FPSManager FPSManagerObject(window, 60, 1.0, "Zadatak 3");

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //funkcija koja se poziva prilikom mijenjanja velicine prozora
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_pos_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	
	std::cout << "Choose color component with keys r,g,b and change intensity of component with up and down arrow." << std::endl;
	std::cout << "Bigger square represents current drawing color and smaller square represents chosen color component." << std::endl;

	glGenVertexArrays(3, VAO);
	glGenBuffers(2, VBO);
	glGenBuffers(2, EBO);

	// shader za color picker i crte
	Shader *colorPickerShader = loadShader(argv[0], "color_picker");
	GLint uniformVarPos = glGetUniformLocation(colorPickerShader->ID, "color");

	// kvadrat za color picker
	float squarePos[] = {
		-0.8f, -0.8f,
		 0.8f, -0.8f,
		-0.8f,  0.8f,
		-0.8f,  0.8f,
		 0.8f, -0.8f,
		 0.8f,  0.8f
	};

	glBindVertexArray(VAO[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(squarePos), squarePos, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, (void *) 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	// crte
	glBindVertexArray(VAO[2]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);

	glBindVertexArray(0);

	// poligon
	Shader* polygonShader = loadShader(argv[0], "polygon");
	glBindVertexArray(VAO[1]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	//glBufferData(GL_ARRAY_BUFFER, polygonVertices.size() * sizeof(float), NULL, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, (void *) 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, (void *)(sizeof(float)*2));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, polygonIndices.size() * sizeof(unsigned int), NULL, GL_DYNAMIC_DRAW);

	glBindVertexArray(0);

	/*********************************************************************************************/
	//glavna petlja za prikaz
	while (glfwWindowShouldClose(window) == false) {

		float deltaTime = (float)FPSManagerObject.enforceFPS(true);

		//pobrisi platno
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, width, height);

		/****************************/
		// dodavanje trenutne pozicije misa
		if (polygonVertices.size()>=2*5) {
			std::vector<float> tmp = { mouseX, mouseY, drawingColor.r, drawingColor.g, drawingColor.b };
			polygonVertices.insert(polygonVertices.end(), tmp.begin(), tmp.end());

			polygonIndices.push_back(polygonVertices.size()/5-3);
			polygonIndices.push_back(polygonVertices.size()/5-2);
			polygonIndices.push_back(polygonVertices.size()/5-1);
		}

		// iscrtavanje poligona
		glUseProgram(polygonShader->ID);
		glBindVertexArray(VAO[1]);
		glBufferSubData(GL_ARRAY_BUFFER, (polygonVertices.size()-5)*sizeof(float), 5*sizeof(float), polygonVertices.data()+polygonVertices.size()-5);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (polygonIndices.size()-3)*sizeof(unsigned int), 3*sizeof(unsigned int), polygonIndices.data()+polygonIndices.size()-3);
		glDrawElements(GL_TRIANGLES, polygonIndices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// uklanjanje trenutne pozicije misa
		if (polygonVertices.size()>=3*5) {
			for (int i = 0; i<5; i++) {
				polygonVertices.pop_back();
			}
			for (int i = 0; i<3; i++) {
				polygonIndices.pop_back();
			}
		}

		glUseProgram(colorPickerShader->ID);

		// iscrtavanje crta
		glBindVertexArray(VAO[2]);
		glUniform3f(uniformVarPos, 0, 0, 0);
		glLineWidth(3);
		glDrawElements(GL_LINE_STRIP, linesIndices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// color picker i odabir komponente
		glBindVertexArray(VAO[0]);

		glUniform3f(uniformVarPos, drawingColor.r, drawingColor.g, drawingColor.b);
		glViewport(0, 0, width/8, height/8);
		glDrawArrays(GL_TRIANGLES, 0, 6); // kvadrat za boju crtanja

		glUniform3f(uniformVarPos, adjustingColor.r, adjustingColor.g, adjustingColor.b);
		glViewport(width/8, height/32, width/16, height/16);
		glDrawArrays(GL_TRIANGLES, 0, 6); // kvadrat za odabir komponente boje

		glBindVertexArray(0);


		glfwSwapBuffers(window);
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
	}   
		
	delete colorPickerShader;
	delete polygonShader;
	glDeleteBuffers(2, VBO);
	glDeleteBuffers(2, EBO);
	glDeleteVertexArrays(3, VAO);

	glfwTerminate();

    return EXIT_SUCCESS;
}
