#include "base/application.hpp"

#include <string>
#include <fstream>

class AppVertices: public Application
{
	public:
	AppVertices()
	{
		float tmpVert[] = { 
		 0.0f,  0.5f,
		 0.5f, -0.5f,
		-0.5f, -0.5f
		};

		for (size_t i=0; i < 6; i++)
			vertices[i] = tmpVert[i];
	}

	float vertices[6];
	GLuint vbo;
	GLuint vertexShader, fragmentShader, shaderProgram;

	virtual void startup() override
	{
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		std::string vShaderStr = load_shader("vsh1.vert");
		const char* vShaderChar = vShaderStr.c_str();
		glShaderSource(vertexShader, 1, &vShaderChar, NULL);
		glCompileShader(vertexShader);
		if (!checkShaderError(vertexShader)) return;

		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		std::string fShaderStr = load_shader("vsh1.frag");
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

		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
		glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(posAttrib);
	}


	void render(double currentTime) override
	{
		glDrawArrays(GL_TRIANGLES, 0, 3);
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
