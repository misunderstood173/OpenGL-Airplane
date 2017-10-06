#include "glos.h"

#include <GL/GL.H>
#include <GL/GLU.H>
#include <GL/GLAux.h>

#include <math.h>

void myinit(void);
void CALLBACK display(void);
void CALLBACK myReshape(GLsizei w, GLsizei h);

int rotationAngle = 0;
int propellerAngle = 0;
float width, height;
float position;

void CALLBACK rotUp()
{
	rotationAngle = (rotationAngle - 1) % 360;
}

void CALLBACK rotDown()
{
	rotationAngle = (rotationAngle + 1) % 360;
}

GLfloat points[3][3] = { { -2 , 0.0, 10.0 },{ 2, 0.0, 10.0 },{ 2, -4.0, -5.0 } }; //3 puncte de pe planul pe care apare umbra - date in sens trigonometric
GLfloat coef[4];
GLfloat sourcePosition[4] = { 0.0, 10.0, 0.0, 1.0 };
GLfloat shadowMatrix[4][4];

void CALLBACK calcCoefPlan(float P[3][3], float coef[4])
{
	float v1[3], v2[3];
	float length;
	static const int x = 0;  
	static const int y = 1;  
	static const int z = 2;  
							 //calculeaza doi vectori din 3 puncte
	v1[x] = P[0][x] - P[1][x];
	v1[y] = P[0][y] - P[1][y];
	v1[z] = P[0][z] - P[1][z];
	v2[x] = P[1][x] - P[2][x];
	v2[y] = P[1][y] - P[2][y];
	v2[z] = P[1][z] - P[2][z];
	// se calculeaza produsul vectorial al celor 2 vectori
	//care reprezinta un al treilea vector perpendicular pe plan
	//ale carui componente sunt chiar coeficientii A,B,C din ecuatiea planului

	coef[x] = v1[y] * v2[z] - v1[z] * v2[y];
	coef[y] = v1[z] * v2[x] - v1[x] * v2[z];
	coef[z] = v1[x] * v2[y] - v1[y] * v2[x];
	//normalizarea vectorului

	length = (float)sqrt((coef[x] * coef[x]) + (coef[y] * coef[y]) + (coef[z] * coef[z]));
	coef[x] /= length;
	coef[y] /= length;
	coef[z] /= length;
}

void CALLBACK shadowMat(GLfloat points[3][3], GLfloat sourcePosition[4], GLfloat mat[4][4])
{
	GLfloat coefPlan[4];
	GLfloat temp;
	//determina coeficientii planului
	calcCoefPlan(points, coefPlan);
	//determina si pe D
	coefPlan[3] = -((coefPlan[0] * points[2][0]) + (coefPlan[1] * points[2][1]) + (coefPlan[2] * points[2][2]));
	//temp=A*xL+B*yL+C*zL+D*w
	temp = coefPlan[0] * sourcePosition[0] + coefPlan[1] * sourcePosition[1] + coefPlan[2] * sourcePosition[2] + coefPlan[3] * sourcePosition[3];
	//prima coloana
	mat[0][0] = temp - sourcePosition[0] * coefPlan[0];
	mat[1][0] = 0.0f - sourcePosition[0] * coefPlan[1];
	mat[2][0] = 0.0f - sourcePosition[0] * coefPlan[2];
	mat[3][0] = 0.0f - sourcePosition[0] * coefPlan[3];
	//a doua coloana
	mat[0][1] = 0.0f - sourcePosition[1] * coefPlan[0];
	mat[1][1] = temp - sourcePosition[1] * coefPlan[1];
	mat[2][1] = 0.0f - sourcePosition[1] * coefPlan[2];
	mat[3][1] = 0.0f - sourcePosition[1] * coefPlan[3];
	//a treia coloana
	mat[0][2] = 0.0f - sourcePosition[2] * coefPlan[0];
	mat[1][2] = 0.0f - sourcePosition[2] * coefPlan[1];
	mat[2][2] = temp - sourcePosition[2] * coefPlan[2];
	mat[3][2] = 0.0f - sourcePosition[2] * coefPlan[3];
	//a patra coloana
	mat[0][3] = 0.0f - sourcePosition[3] * coefPlan[0];
	mat[1][3] = 0.0f - sourcePosition[3] * coefPlan[1];
	mat[2][3] = 0.0f - sourcePosition[3] * coefPlan[2];
	mat[3][3] = temp - sourcePosition[3] * coefPlan[3];
}

void CALLBACK moveLightSourceLeft()
{
	sourcePosition[0]--;
}

void CALLBACK moveLightSourceRight()
{
	sourcePosition[0]++;
}



void mainBody(float factor)
{
	GLfloat ctrlpoints[4][4][3] = {
		{ { -0.0, -3.5, 0.0 },{ -0.0, -3.5, 0.0 },{ 0.0, -3.5, 0.0 },{ 0.0, -3.5, 0.0 } },
		{ { -1.0, -1.5, 0.0 },{ -0.5, -1.5, 2.0 },{ 0.5, -1.5, 2.0 },{ 1.0, -1.5, 0.0 } },
		{ { -1.0, 1.5, 0.0 },{ -0.5, 1.5, 1.0 },{ 0.5, 1.5, 1.0 },{ 1.0, 1.5, 0.0 } },
		{ { -0.0, 3.0, 0.0 },{ -0.0, 3.0, 0.0 },{ 0.0, 3.0, 0.0 },{ 0.0, 3.0, 0.0 } }
	};

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 3; k++)
				ctrlpoints[i][j][k] = ctrlpoints[i][j][k] * factor;

	glPushMatrix();
	glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4,
		0, 1, 12, 4, &ctrlpoints[0][0][0]);
	glEvalMesh2(GL_FILL, 0, 20, 0, 20); // specifica modul
										// de redare al poligoanelor (GL_FILL, GL_POINT, GL_LINE, 
										// si intervalele de esantionare a suprafetei pentru u si v
	glRotatef(180, 0, 1, 0);
	glEvalMesh2(GL_FILL, 0, 20, 0, 20);
	glPopMatrix();
}


void wing(float factor)
{
	GLfloat ctrlpoints[4][4][3] = {
		{ { -0.6, -3.0, 0.0 },{ -0.0, -4.1, 0.0 },{ 0.0, -4.1, 0.0 },{ 0.6, -3.0, 0.0 } },
		{ { -1.0, -2.0, 0.0 },{ -0.5, -2.5, 0.5 },{ 0.5, -2.5, 0.5 },{ 1.0, -2.0, 0.0 } },
		{ { -1.0, 1.0, 0.0 },{ -0.5, 1.5, 0.5 },{ 0.5, 1.5, 0.5 },{ 1.0, 1.0, 0.0 } },
		{ { -0.8, 2.0, 0.0 },{ -0.0, 2.0, 0.0 },{ 0.0, 2.0, 0.0 },{ 0.8, 2.0, 0.0 } }
	};

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 3; k++)
				ctrlpoints[i][j][k] = ctrlpoints[i][j][k] * factor;

	glPushMatrix();
	glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4,
		0, 1, 12, 4, &ctrlpoints[0][0][0]);
	glEvalMesh2(GL_FILL, 0, 20, 0, 20); // specifica modul
										// de redare al poligoanelor (GL_FILL, GL_POINT, GL_LINE, 
										// si intervalele de esantionare a suprafetei pentru u si v
	glRotatef(180, 0, 1, 0);
	glEvalMesh2(GL_FILL, 0, 20, 0, 20);
	glPopMatrix();
}

void makeAirplane(float size)
{
	float mainBodyFactor = 0.7 * size;
	float mainWingsFactor = 0.5 * size;
	float tailWingsFactor = 0.2 * size;

	mainBody(mainBodyFactor);

	//propeller
	glPushMatrix();
		glTranslatef(0, mainBodyFactor * 2.5, 0);
		glRotatef(propellerAngle, 0, 1, 0);
		auxSolidBox(mainBodyFactor * 2.5, mainBodyFactor * 0.2, mainBodyFactor * 0.2);
		auxSolidBox(mainBodyFactor * 0.2, mainBodyFactor * 0.2, mainBodyFactor * 2.5);
	glPopMatrix();

	//main wings
	glPushMatrix();
		glTranslatef(-2 * mainWingsFactor, 0.1 * mainWingsFactor, 0);
		glRotatef(-90, 0, 0, 1);
		wing(mainWingsFactor);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(2 * mainWingsFactor, 0.1 * mainWingsFactor, 0);
		glRotatef(90, 0, 0, 1);
		wing(mainWingsFactor);
	glPopMatrix();

	//tail wings
	glPushMatrix();
		glTranslatef(2 * tailWingsFactor, -11 * tailWingsFactor, 0);
		glRotatef(90, 0, 0, 1);
		wing(tailWingsFactor);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(-2 * tailWingsFactor, -11 * tailWingsFactor, 0);
		glRotatef(-90, 0, 0, 1);
		wing(tailWingsFactor);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(0, -11 * tailWingsFactor, 0.5 * mainBodyFactor);
		glRotatef(-90, 1, 0, 0);
		glRotatef(-90, 0, 1, 0);
		wing(tailWingsFactor);
	glPopMatrix();



}

void floor()
{
	glPushMatrix();

		glBegin(GL_QUADS);
			glVertex3f(width , 0.0, 20.0);
			glVertex3f(-width, 0.0, 20.0);
			glVertex3f(-width, -height, -10.0);
			glVertex3f(width, -height, -10.0);
		glEnd();
	glPopMatrix();
}

void CALLBACK display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLightfv(GL_LIGHT0, GL_POSITION, sourcePosition);
	shadowMat(points, sourcePosition, shadowMatrix);

	glPushMatrix();
	GLfloat mat_diffuse_floor[] = { 0.3, 0.8, 0.2, 1.0 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse_floor);
	floor();
	
	glPushMatrix();
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glColor3f(0.0, 0.0, 0.0);
		glMultMatrixf((GLfloat*)shadowMatrix);
		
		glTranslatef(position, 2, 0);
		glRotatef(90, 1, 0, 0);
		glRotatef(rotationAngle, 1, 0, 0);
		glRotatef(90, 0, 0, 1);
		makeAirplane(0.3);

	glPopMatrix();
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	GLfloat mat_diffuse[] = { 0.8, 0.2, 0.2, 1.0 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glTranslatef(position, 2, 0);
	glRotatef(-90, 1, 0, 0);
	glRotatef(rotationAngle, 1, 0, 0);
	glRotatef(90, 0, 0, 1);
	makeAirplane(0.3);
	glPopMatrix();

	glFlush();
}

void initlights(void)
{
	GLfloat ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

void myinit(void)
{
	glClearColor(0.24, 0.74, 0.74, 1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MAP2_VERTEX_3); // validarea tipului de evaluare GL_MAP2_VERTEX_3
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE); // pentru iluminare
	glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);// intervalele de esantionare 

	initlights();
}

void CALLBACK idle()
{
	propellerAngle = (propellerAngle + 1) % 360;
	position -= 0.05;
	if (position < -width)
		position = width;
	display();
	Sleep(10);
}

void CALLBACK myReshape(GLsizei w, GLsizei h)
{
	if (!h) return;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= h) {
		glOrtho(-4.0, 4.0, -4.0*(GLfloat)h / (GLfloat)w,
			4.0*(GLfloat)h / (GLfloat)w, -20.0, 20.0);
		width = 4;
		height = 4 * h / w;
	}
	else {
		glOrtho(-4.0*(GLfloat)w / (GLfloat)h,
			4.0*(GLfloat)w / (GLfloat)h, -4.0, 4.0, -20.0, 20.0);
		width = 4 * w / h;
		height = 4;
	}
	width += 2;
	position = width;
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


int main(int argc, char** argv)
{
	auxInitDisplayMode(AUX_SINGLE | AUX_RGB);
	auxInitPosition(0, 0, 400, 400);
	auxInitWindow("Airplane");

	myinit();
	auxReshapeFunc(myReshape);
	auxIdleFunc(idle);
	auxKeyFunc(AUX_LEFT, moveLightSourceLeft);
	auxKeyFunc(AUX_RIGHT, moveLightSourceRight);
	auxKeyFunc(AUX_UP, rotUp);
	auxKeyFunc(AUX_DOWN, rotDown);
	auxMainLoop(display);
	return(0);
}
