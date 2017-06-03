#include "glos.h"

#include <GL/GL.H>
#include <GL/GLU.H>
#include <GL/GLAux.h>

#include <stdio.h>

void myinit(void);
void CALLBACK display(void);
void CALLBACK myReshape(GLsizei w, GLsizei h);


void CALLBACK display(void)
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);


	glFlush();
}

void myinit(void)
{
	glShadeModel(GL_FLAT);
}

void CALLBACK myReshape(GLsizei w, GLsizei h)
{
	if (!h) return;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(65.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -5.0);
}


int main(int argc, char** argv)
{
	auxInitDisplayMode(AUX_SINGLE | AUX_RGB);
	auxInitPosition(0, 0, 400, 400);
	auxInitWindow("Airplane");

	myinit();
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	auxReshapeFunc(myReshape);
	auxMainLoop(display);
	return(0);
}
