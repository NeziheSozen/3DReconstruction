#pragma once

#include <GL\glew.h>
#include <GL\GL.h>
#include <GL\GLU.h>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <opencv2\core\core.hpp>
#include <iomanip>
#include <assert.h>

#include <string>
#include <vector>

#include "Shader.h"
#include "VisualisationDisplayCameraClass.h"
#include "Model.h"


using namespace std;
using namespace cv;

namespace vis{


class Visualizer{

public:
	Visualizer(void);
	~Visualizer(void);

	void addPoint(GLfloat x, GLfloat y, GLfloat z);
	void mainLoop();

private:
	
	void initGL(void);
	void uploadGeometry(void);
	void generateSphere(void);
	void redraw(void);



	//variables
	Shader shader;
	sf::Window* displayWindow;
	vector<Model> plottedPoints;
	GLfloat* mdl;
	VisualisationDisplayCameraClass cam;
};

}

