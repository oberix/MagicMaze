//
// Copyright (C) 2009 Marco Pattaro
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

#include <GL/glut.h>
#include "includes/game.h"
#include "includes/dim.h"

enum window { mainwin };
GLubyte win[sizeof(enum window)];

/*
 * init: initialize the game and some OpenGL parameter 
 */
static void init(void) {
	GLfloat fogColor[4] = {0.5, 0.5, 0.5, 1.0};
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);                                                                              
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);

	initGame(9,9);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_NORMALIZE);
//	glEnable(GL_TEXTURE_2D);
	
	glFogi (GL_FOG_MODE, GL_LINEAR);
	glFogfv (GL_FOG_COLOR, fogColor);
	glFogf (GL_FOG_DENSITY, 0.12);
	glHint (GL_FOG_HINT, GL_DONT_CARE);
	glFogf (GL_FOG_START, EYE_DIST);
	glFogf (GL_FOG_END, EYE_DIST+ELEM_SIDE);
}

int main (int argc, char** argv) {
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
	
    /* window initialization */
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(50, 50);
    
    /* main window */
    win[mainwin] = glutCreateWindow("Magic Maze");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMouseFunc(mouse);
    setAnimation(display);
    initMenu();

    glutMainLoop();
    
    return 0;
}
