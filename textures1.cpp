#include "base/application.hpp"
#include <SOIL/SOIL.h>

#include <chrono>
#include <cmath>
#include <cstddef>
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

	virtual void startup() override
	{
		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE, tex);

		int twidth, theight;
		unsigned char* image =
			SOIL_load_image("sample.png", &twidth, &theight, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);	
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);

		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		std::string vShaderStr = load_shader("tx1.vert");
		const char* vShaderChar = vShaderStr.c_str();
		glShaderSource(vertexShader, 1, &vShaderChar, NULL);
		glCompileShader(vertexShader);
		if (!checkShaderError(vertexShader)) return;

		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		std::string fShaderStr = load_shader("tx1.frag");
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
	}


	void render(double currentTime) override
	{
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
