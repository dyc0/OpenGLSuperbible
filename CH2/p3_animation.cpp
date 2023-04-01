#include <GL/gl.h>
#include <GL/glut.h>


GLfloat x1 = 0.0f;
GLfloat y1cor = 0.0f;
GLfloat rsize = 25;

GLfloat xstep = 1.0f;
GLfloat ystep = 1.0f;

GLfloat windowWidth;
GLfloat windowHeight;

void RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(1.0f, 0.0f, 0.0f);
	glRectf(x1, y1cor, x1 + rsize, y1cor - rsize);

	glutSwapBuffers();
}


void TimerFunction(int value)
{
	if (x1 > windowWidth-rsize || x1 < -windowWidth) xstep = -xstep;
	if (y1cor > windowHeight || y1cor < -windowHeight-rsize) ystep = -ystep;

	x1 += xstep;
	y1cor += ystep;

	if (x1 > windowWidth-rsize+xstep) x1 = windowWidth-rsize-1;
	else if (x1 < -windowWidth-xstep) x1 = -windowWidth-1;
	
	if (y1cor > windowHeight+ystep) y1cor = windowHeight-1;
	else if (y1cor<-windowHeight+rsize-ystep) y1cor = -windowHeight+rsize-1;

	glutPostRedisplay();
	glutTimerFunc(33, TimerFunction, 1);
}


void SetupRC()
{
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
}

void ChangeSize(GLsizei w, GLsizei h)
{
	GLfloat aspectRatio;

	if (h == 0) h = 1;
	
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	aspectRatio = (GLfloat) w / (GLfloat) h;
	if (w <= h)
	{
		windowWidth = 100.0f;
		windowHeight = 100.0f/aspectRatio;
	}		
	else
	{
		windowWidth = 100.0f*aspectRatio;
		windowHeight = 100.0f;
	}
	glOrtho(-windowWidth, windowWidth, -windowHeight, windowHeight, 1.0, -1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutCreateWindow("Bounce");
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);
	glutTimerFunc(33, TimerFunction, 1);

	SetupRC();
	glutMainLoop();

	return 0;
}
