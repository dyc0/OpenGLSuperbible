#include "application.hpp"

#include <cmath>

class new_app: public Application
{
	public:
	new_app()
	{
		info.flags.debug = false;
	}

	void render(double currentTime) override
	{
		GLfloat color[] = { (float) std::sin(currentTime)*0.5f + 0.5f, (float) std::cos(currentTime)*0.5f + 0.5f, 0.0f, 0.0f };
		
		glClearBufferfv(GL_COLOR, 0, color);
	}
};

int main()
{
	new_app* app = new new_app();
	app->run();
}
