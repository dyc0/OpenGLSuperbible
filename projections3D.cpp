#include "base/application.hpp"
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>
#include <bits/chrono.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>
#include <cmath>
#include <cstddef>
#include <glm/trigonometric.hpp>
#include <string>
#include <fstream>

#define APP_VERT_ARRAY_SIZE 28
#define APP_ELEM_ARRAY_SIZE 6

class AppVertices: public Application
{
	public:
	AppVertices()
	{
		float tmpVert[] = { 
	   //Position	  Color				Texture
		-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Top-left
		 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top-right
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Bottom-right
		-0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f  // Bottom-left
		};

		float tmpElem[] = {
		0, 1, 2,
		2, 3, 0
		};

		for (size_t i=0; i < APP_VERT_ARRAY_SIZE; i++)
			vertices[i] = tmpVert[i];

		for(size_t i=0; i < APP_ELEM_ARRAY_SIZE; i++)
			elements[i] = tmpElem[i];
	}

	float vertices[APP_VERT_ARRAY_SIZE];
	GLuint elements[APP_ELEM_ARRAY_SIZE];
	GLuint vbo, ebo;
	GLuint tex;
	GLuint vertexShader, fragmentShader, shaderProgram;
	GLint uniColor;
	GLint uniTrans, uniView, uniProj, uniFlip;
	glm::mat4 trans, view, proj, flip;

	std::chrono::time_point<std::chrono::high_resolution_clock> tStart, tNow;
	float time;

	virtual void startup() override
	{
		tStart = std::chrono::high_resolution_clock::now();

		trans = glm::mat4(1.0f);
		trans = glm::rotate(trans, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		view = glm::lookAt(glm::vec3(1.2f, 1.2f, 1.2f),
			   			   glm::vec3(0.0f, 0.0f, 0.0f),
			               glm::vec3(0.0f, 0.0f, 1.0f));

		proj = glm::perspective(glm::radians(45.0f),
			   				   (float)info.WindowWidth / (float)info.WindowHeight,
							   1.0f, 10.0f);

		flip = glm::mat4(1.0f);

		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
	
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		std::string vShaderStr = load_shader("proj.vert");
		const char* vShaderChar = vShaderStr.c_str();
		glShaderSource(vertexShader, 1, &vShaderChar, NULL);
		glCompileShader(vertexShader);
		if (!checkShaderError(vertexShader)) return;

		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		std::string fShaderStr = load_shader("tx2.frag");
		const char* fShaderChar = fShaderStr.c_str();
		glShaderSource(fragmentShader, 1, &fShaderChar, NULL);
		glCompileShader(fragmentShader);
		if (!checkShaderError(fragmentShader)) return;

		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);	
		glBindFragDataLocation(shaderProgram, 0, "outColor");
		glLinkProgram(shaderProgram);
		glUseProgram(shaderProgram);

		GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE,
							  7*sizeof(float), 0);

		GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
		glEnableVertexAttribArray(colAttrib);
		glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE,
							  7*sizeof(float), (void*)(2*sizeof(float)));
		
		GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
		glEnableVertexAttribArray(texAttrib);
		glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE,
							  7*sizeof(float), (void*)(5*sizeof(float)));
	
		GLuint textures[2];
		glGenTextures(2, textures);

		int twidth, theight;
		unsigned char* image; 
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[0]);		
		image =	SOIL_load_image("sample.png", &twidth, &theight, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glUniform1i(glGetUniformLocation(shaderProgram, "texKitten"), 0);	
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures[1]);		
		image =	SOIL_load_image("sample2.png", &twidth, &theight, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glUniform1i(glGetUniformLocation(shaderProgram, "texPuppy"), 1);	
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);

		uniTrans = glGetUniformLocation(shaderProgram, "trans");
		glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(trans));

		uniView = glGetUniformLocation(shaderProgram, "view");
		glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

		uniProj = glGetUniformLocation(shaderProgram, "proj");
		glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

		uniFlip = glGetUniformLocation(shaderProgram, "flip");
		glUniformMatrix4fv(uniFlip, 1, GL_FALSE, glm::value_ptr(flip));
	}

	
	virtual void keyCallback(int key, int scancode, int action, int mods) override
	{
		if (action == GLFW_PRESS)
			if (key == GLFW_KEY_SPACE)
			{
				flip = glm::rotate(flip, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 	
				std::cout << "FLIP!" << std::endl;
			}
	}

	void render(double currentTime) override
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		tNow = std::chrono::high_resolution_clock::now();
		time = std::chrono::duration_cast<std::chrono::duration<float>>(tNow - tStart).count();

		trans = glm::rotate(trans, time/1000 + glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(trans));
		glUniformMatrix4fv(uniFlip, 1, GL_FALSE, glm::value_ptr(flip));

		glDrawElements(GL_TRIANGLES, APP_ELEM_ARRAY_SIZE, GL_UNSIGNED_INT, 0);
	}

	bool checkShaderError(GLuint shader)
	{
		GLint status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (!status)
		{
			char buffer[512];
			glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
			for(size_t i = 0; buffer[i] != '\0'; i++)
			{
				if (i >= 512) break;
				std::cout << buffer[i];
    		}
			std::cerr << std::endl;
		}
		return status;
	}

	std::string load_shader(const std::string& fileName) const
	{
		std::ifstream file;
		file.open(fileName);

		std::string output, line;

		if (file.is_open())
			while (file.good())
			{
				getline(file, line);
				output.append(line + "\n");
			}
		else
			std::cerr << "Unable to load shader from file " << fileName << std::endl;

		return output;
	}
};

int main()
{
	AppVertices* app = new AppVertices();
	app->run();
	return 0;
}
