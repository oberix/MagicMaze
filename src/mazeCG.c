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

#include "includes/mazeCG.h"
#include "includes/maze.h"
#include "includes/dim.h"
#include "includes/cgUtils.h"
#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

/* some display lists */
static GLuint __goalList = 0;
static GLuint __wallList = 0;
static GLuint __floorList = 0;
static GLuint __trapList = 0;
static GLuint __myFaceList = 0;

#define WALL_TEXTURE "ppm/brikwall1.ppm"
#define FLOOR_TEXTURE "ppm/stone.ppm"

static void __initGoal(void) {
	GLfloat mat_diffuse_blue[4] = { 0.0, 0.6, 0.6, 0.4 };
	GLfloat mat_diffuse_red[4] = { 1.0, 0.1, 0.1, 1.0 };
	GLfloat mat_specular[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_emission[] = { 0.0, 0.4, 0.7, 0.6 };
	GLfloat mat_zero[] = { 0.0, 0.0, 0.0, 1.0 };
	GLuint state;
	GLfloat alpha;
	GLfloat hight;
	GLfloat deform;

	fprintf(stdout, "\tgenerating goals geometry...\n");

	__goalList = glGenLists(GOAL_NSTATES);
	for (state = 0; state < GOAL_NSTATES; state++) {
		alpha = ((GLfloat)state / (GLfloat)GOAL_NSTATES * 2.0 * PI);
		hight = 0.5 + CENTER_RAD + (sin(alpha)+1)/2;
		deform = 0.9+((cos(alpha)+1)*0.1);

		glNewList(__goalList+state, GL_COMPILE);
		glPushMatrix();
		glTranslatef((GLfloat)ELEM_SIDE/2, hight, (GLfloat)ELEM_SIDE/2);
		/* icosahedron */
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse_red);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT, GL_EMISSION, mat_zero);
		glPushMatrix();
		glScalef(0.5*CENTER_RAD, 0.5*CENTER_RAD, 0.5*CENTER_RAD);
		glutSolidIcosahedron();
		glPopMatrix();

		glScalef(deform, 2-deform, deform);
		/* sphere */
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse_blue);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_emission);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 25.0);
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glutSolidSphere(CENTER_RAD, 30, 30);
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
		glPopMatrix();
		glEndList();
	}
}

static void __drawGoal(MazeElement* e) {
	e->alpha = (GLfloat)((GLuint)(e->alpha + 1) % GOAL_NSTATES);
	glCallList(__goalList + (GLuint)e->alpha);
	resetMatDefault();
}

static void __initTrap(void) {
	GLubyte *texture;
	int w, h;
	GLuint state;	/* states counter */
	GLfloat delta = 90.0 / (GLfloat)TRAP_NSTATES; /* variation of the angle of rotation */
	GLfloat mat_diffuse[] = { 0.6, 0.6, 0.6 };
	GLfloat mat_specular[] = { 0.2, 0.2, 0.2 };
	GLfloat mat_zero[] = { 0.0, 0.0, 0.0, 1.0 };

	texture = glmReadPPM(FLOOR_TEXTURE, &w, &h);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	fprintf(stdout, "\tgenerating trap geometry...\n");
	__trapList = glGenLists(TRAP_NSTATES);
	for (state = 0; state < TRAP_NSTATES; state++) {
		glNewList(__trapList+state, GL_COMPILE);

		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT, GL_EMISSION, mat_zero);

		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, w, h, GL_RGB, GL_UNSIGNED_BYTE, texture);

		glPushMatrix();	/* moving parts (2) */
		/* translate at the center of the tile */
		glTranslatef((ELEM_SIDE/2) - CENTER_RAD, 0.0, (ELEM_SIDE/2) - CENTER_RAD);

		glPushMatrix();	/* moving base part */
		glRotatef((GLfloat)state*delta, 1.0, 0.0, 0.0);
		glBegin(GL_QUADS);
		glTexCoord2f(FLOOR_WIDTH/ELEM_SIDE, 0.5);
		//glNormal3i(0, 1, 0);
		glVertex3f(0.0, 0.0, CENTER_RAD);
		glTexCoord2f(1-(FLOOR_WIDTH/ELEM_SIDE), 0.5);
		//glNormal3i(0, 1, 0);
		glVertex3f(CENTER_RAD*2, 0.0, CENTER_RAD);
		glTexCoord2f(1-(FLOOR_WIDTH/ELEM_SIDE), FLOOR_WIDTH/ELEM_SIDE);
		glNormal3i(0, 1, 0);
		glVertex3f(CENTER_RAD*2, 0.0, 0.0);
		glTexCoord2f(FLOOR_WIDTH/ELEM_SIDE, FLOOR_WIDTH/ELEM_SIDE);
		glNormal3i(0, 1, 0);
		glVertex3f(0.0, 0.0, 0.0);
		glEnd();
		glPopMatrix();	/* END moving base part */

		glPushMatrix();	/* translated part */
		glTranslatef(0.0, 0.0, 2*CENTER_RAD);
		glRotatef(-(GLfloat)state*delta, 1.0, 0.0, 0.0);
		glTranslatef(0.0, 0.0, -CENTER_RAD);
		glBegin(GL_QUADS);
		glTexCoord2f(FLOOR_WIDTH/ELEM_SIDE, 1-(FLOOR_WIDTH/ELEM_SIDE));
		//glNormal3i(0, 1, 0);
		glVertex3f(0.0, 0.0, CENTER_RAD);
		glTexCoord2f(1-(FLOOR_WIDTH/ELEM_SIDE), 1-(FLOOR_WIDTH/ELEM_SIDE));
		//glNormal3i(0, 1, 0);
		glVertex3f(CENTER_RAD*2, 0.0, CENTER_RAD);
		glTexCoord2f(1-(FLOOR_WIDTH/ELEM_SIDE), 0.5);
		glNormal3i(0, 1, 0);
		glVertex3f(CENTER_RAD*2, 0.0, 0.0);
		glTexCoord2f(FLOOR_WIDTH/ELEM_SIDE, 0.5);
		glNormal3i(0, 1, 0);
		glVertex3f(0.0, 0.0, 0.0);
		glEnd();
		glPopMatrix();	/* END translated part */

		glPopMatrix();	/* END moving parts (2) */

		glBegin(GL_QUADS);	/* static part */
		glTexCoord2f(FLOOR_WIDTH/ELEM_SIDE, 1.0);
		//glNormal3i(1, 0, 1);
		glVertex3f(FLOOR_WIDTH, 0.0, ELEM_SIDE);
		glTexCoord2f(1.0, 1.0);
		glNormal3i(0, 1, 0);
		glVertex3f(ELEM_SIDE, 0.0, ELEM_SIDE);	/* outer corner */
		glTexCoord2f(1.0, 1.0-(FLOOR_WIDTH/ELEM_SIDE));
		//glNormal3i(1, 0, 1);
		glVertex3f(ELEM_SIDE, 0.0, ELEM_SIDE-FLOOR_WIDTH);
		glTexCoord2f(FLOOR_WIDTH/ELEM_SIDE, 1.0-(FLOOR_WIDTH/ELEM_SIDE));
		glNormal3i(0, 1, 0);
		glVertex3f(FLOOR_WIDTH, 0.0, ELEM_SIDE-FLOOR_WIDTH);	 /* inner */

		glTexCoord2f(1.0-(FLOOR_WIDTH/ELEM_SIDE), 1.0-(FLOOR_WIDTH/ELEM_SIDE));
		//glNormal3i(0, 1, 0);
		glVertex3f(ELEM_SIDE-FLOOR_WIDTH, 0.0, ELEM_SIDE-FLOOR_WIDTH);	/* inner */
		glTexCoord2f(1.0, 1.0-(FLOOR_WIDTH/ELEM_SIDE));
		//glNormal3i(1, 0, 1);
		glVertex3f(ELEM_SIDE, 0.0, ELEM_SIDE-FLOOR_WIDTH);
		glTexCoord2f(1.0, 0.0);
		glNormal3i(0, 1, 0);
		glVertex3f(ELEM_SIDE, 0.0, 0.0);      /* outer corner */
		glTexCoord2f(1.0-(FLOOR_WIDTH/ELEM_SIDE), 0.0);
		//glNormal3i(1, 0, 1);
		glVertex3f(ELEM_SIDE-FLOOR_WIDTH, 0.0, 0.0);

		glTexCoord2f(0.0, FLOOR_WIDTH/ELEM_SIDE);
		//glNormal3i(1, 0, 1);
		glVertex3f(0.0, 0.0, FLOOR_WIDTH);
		glTexCoord2f(1.0-(FLOOR_WIDTH/ELEM_SIDE), FLOOR_WIDTH/ELEM_SIDE);
		//glNormal3i(0, 1, 0);
		glVertex3f(ELEM_SIDE-FLOOR_WIDTH, 0.0, FLOOR_WIDTH);	/* inner */
		glTexCoord2f(1.0-(FLOOR_WIDTH/ELEM_SIDE), 0.0);
		//glNormal3i(1, 0, 1);
		glVertex3f(ELEM_SIDE-FLOOR_WIDTH, 0.0, 0.0);
		glTexCoord2f(0.0, 0.0);
		glNormal3i(0, 1, 0);
		glVertex3f(0.0, 0.0, 0.0);            /* outer corner */

		glTexCoord2f(0.0, 1.0);
		glNormal3i(0, 1, 0);
		glVertex3f(0.0, 0.0, ELEM_SIDE);      /* outer corner */
		glTexCoord2f(FLOOR_WIDTH/ELEM_SIDE, 1.0);
		//glNormal3i(1, 0, 1);
		glVertex3f(FLOOR_WIDTH, 0.0, ELEM_SIDE);
		glTexCoord2f(FLOOR_WIDTH/ELEM_SIDE, FLOOR_WIDTH/ELEM_SIDE);
		//glNormal3i(0, 1, 0);
		glVertex3f(FLOOR_WIDTH, 0.0, FLOOR_WIDTH); /* inner */
		glTexCoord2f(0.0, FLOOR_WIDTH/ELEM_SIDE);
		//glNormal3i(1, 0, 1);
		glVertex3f(0.0, 0.0, FLOOR_WIDTH);
		glEnd();	/* END static part */
		glEndList();
	}
	free (texture);
}

static void __drawTrap(MazeElement* e) {
	switch (e->state) {
	case A_STILL:
		e->alpha = 0.0;
		glCallList(__floorList);
		break;
	case A_STD:
	case A_ALT:
		if (e->alpha < (GLfloat)TRAP_NSTATES-1)
			e->alpha += 1.0;	/* state counter */
			glCallList(__trapList + (GLuint)e->alpha);
		break;
	default:
		fprintf(stderr, "ERROR: __drawTrap(), invalid state.");
		break;
	}
	//glCallList(__trapList + (GLuint)e->alpha);
	resetMatDefault();
}

static void __initWall(void) {
	GLubyte *texture;
	int w, h;
	GLfloat mat_diffuse[] = { 0.30, 0.20, 0.20 };
	GLfloat mat_specular[] = { 0.10, 0.05, 0.05 };
	GLfloat mat_zero[] = { 0.0, 0.0, 0.0, 1.0 };

	const GLfloat vertices[8][3] = {
			{0.0, 		0.0, 		-WALL_DEPTH},
			{ELEM_SIDE, 0.0, 		-WALL_DEPTH},
			{ELEM_SIDE, WALL_HIGHT, -WALL_DEPTH},
			{0.0, 		WALL_HIGHT, -WALL_DEPTH},
			{0.0, 		0.0, 		WALL_DEPTH},
			{ELEM_SIDE, 0.0, 		WALL_DEPTH},
			{ELEM_SIDE,	WALL_HIGHT,	WALL_DEPTH},
			{0.0,		WALL_HIGHT, WALL_DEPTH},
	};

	fprintf(stdout, "\tgenerating wall geometry...\n");

	texture = glmReadPPM(WALL_TEXTURE, &w, &h);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	__wallList = glGenLists(4);

#define WALL_SOUTH __wallList
	glNewList(__wallList, GL_COMPILE);	/* south wall */
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_zero);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, w, h, GL_RGB, GL_UNSIGNED_BYTE, texture);

	/* front */
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 1.0);
	glNormal3i(0, 0, -1);
	glVertex3fv(vertices[3]);
	glTexCoord2f(1.0, 1.0);
	glNormal3i(0, 0, -1);
	glVertex3fv(vertices[2]);
	glTexCoord2f(1.0, 0.0);
	glNormal3i(0, 0, -1);
	glVertex3fv(vertices[1]);
	glTexCoord2f(0.0, 0.0);
	glNormal3i(0, 0, -1);
	glVertex3fv(vertices[0]);
	glEnd();

	/* left */
	glBegin(GL_POLYGON);
	glNormal3i(1, 0, 0);
	glVertex3fv(vertices[0]);
	glNormal3i(1, 0, 0);
	glVertex3fv(vertices[4]);
	glNormal3i(1, 0, 0);
	glVertex3fv(vertices[7]);
	glNormal3i(1, 0, 0);
	glVertex3fv(vertices[3]);
	glEnd();

	/* back */
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0);
	glNormal3i(0, 0, 1);
	glVertex3fv(vertices[4]);
	glTexCoord2f(1.0, 0.0);
	glNormal3i(0, 0, 1);
	glVertex3fv(vertices[5]);
	glTexCoord2f(1.0, 1.0);
	glNormal3i(0, 0, 1);
	glVertex3fv(vertices[6]);
	glTexCoord2f(0.0, 1.0);
	glNormal3i(0, 0, 1);
	glVertex3fv(vertices[7]);
	glEnd();

	/* right  */
	glBegin(GL_POLYGON);
	glNormal3i(-1, 0, 0);
	glVertex3fv(vertices[1]);
	glNormal3i(-1, 0, 0);
	glVertex3fv(vertices[2]);
	glNormal3i(-1, 0, 0);
	glVertex3fv(vertices[6]);
	glNormal3i(-1, 0, 0);
	glVertex3fv(vertices[5]);
	glEnd();

	/* top  */
	glBegin(GL_POLYGON);
	glNormal3i(0, 1, 0);
	glVertex3fv(vertices[7]);
	glNormal3i(0, 1, 0);
	glVertex3fv(vertices[6]);
	glNormal3i(0, 1, 0);
	glVertex3fv(vertices[2]);
	glNormal3i(0, 1, 0);
	glVertex3fv(vertices[3]);
	glEnd();

	/* bottom  */
	glBegin(GL_POLYGON);
	glVertex3fv(vertices[0]);
	glVertex3fv(vertices[1]);
	glVertex3fv(vertices[5]);
	glVertex3fv(vertices[4]);
	glEnd();
	glEndList();

#define WALL_NORTH __wallList + 1
	glNewList(__wallList + 1, GL_COMPILE); /* north wall */

	glPushMatrix();
	glTranslatef((GLfloat)ELEM_SIDE/2.0, 0.0, (GLfloat)ELEM_SIDE/2.0);
	glRotatef(180.0, 0.0, 1.0, 0.0);
	glTranslatef((GLfloat)-ELEM_SIDE/2.0, 0.0, (GLfloat)-ELEM_SIDE/2.0);
	glCallList(__wallList);
	glPopMatrix();
	glEndList();

#define WALL_EAST __wallList + 2
	glNewList(__wallList + 2, GL_COMPILE); /* north east */

	glPushMatrix();
	glTranslatef((GLfloat)ELEM_SIDE/2.0, 0.0, (GLfloat)ELEM_SIDE/2.0);
	glRotatef(90.0, 0.0, 1.0, 0.0);
	glTranslatef((GLfloat)-ELEM_SIDE/2.0, 0.0, (GLfloat)-ELEM_SIDE/2.0);
	glCallList(__wallList);
	glPopMatrix();
	glEndList();

#define WALL_WEST __wallList + 3
	glNewList(__wallList + 3, GL_COMPILE); /* west wall */

	glPushMatrix();
	glTranslatef((GLfloat)ELEM_SIDE/2.0, 0.0, (GLfloat)ELEM_SIDE/2.0);
	glRotatef(270.0, 0.0, 1.0, 0.0);
	glTranslatef((GLfloat)-ELEM_SIDE/2.0, 0.0, (GLfloat)-ELEM_SIDE/2.0);
	glCallList(__wallList);
	glPopMatrix();
	glEndList();

	free(texture);
}

static void __drawWall(Direction dir) {
	glEnable(GL_TEXTURE_2D);
	switch(dir) {
	case DIR_NORTH:
		glCallList(WALL_NORTH);
		break;
	case DIR_SOUTH:
		glCallList(WALL_SOUTH);
		glCallList(__myFaceList);
		break;
	case DIR_EAST:
		glCallList(WALL_EAST);
		break;
	case DIR_WEST:
		glCallList(WALL_WEST);
		break;
	default:
		fprintf(stderr, "ERROR: drawWall(): invalid direction\n");
		break;
	}
	glDisable(GL_TEXTURE_2D);
	resetMatDefault();
}

static void __initFloor(void) {
	GLubyte* texture;
	int w, h;
	GLfloat mat_diffuse[] = { 0.6, 0.6, 0.6 };
	GLfloat mat_specular[] = { 0.1, 0.1, 0.1 };
	GLfloat mat_zero[] = { 0.0, 0.0, 0.0, 1.0 };

	fprintf(stdout, "\tgenerating floor geometry...\n");

	texture = glmReadPPM(FLOOR_TEXTURE, &w, &h);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	__floorList = glGenLists(1);
	glNewList(__floorList, GL_COMPILE);

	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_zero);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, w, h, GL_RGB, GL_UNSIGNED_BYTE, texture);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0, 1.0);
	glNormal3i(0, 1, 0);
	glVertex3f(ELEM_SIDE, 0.0, ELEM_SIDE);
	glTexCoord2f(1.0, 0.0);
	glNormal3i(0, 1, 0);
	glVertex3f(ELEM_SIDE, 0.0, 0.0);
	glTexCoord2f(0.0, 0.0);
	glNormal3i(0, 1, 0);
	glVertex3f(0.0, 0.0, 0.0);
	glTexCoord2f(0.0, 1.0);
	glNormal3i(0, 1, 0);
	glVertex3f(0.0, 0.0, ELEM_SIDE);
	glEnd();
	glEndList();

	free(texture);
}

static void __drawFloor(MazeElement* e) {
	glEnable(GL_TEXTURE_2D);
	if (e->type == CE_TRAP) {
		__drawTrap(e);
	}
	else {
		glCallList(__floorList);
		resetMatDefault();
	}
	glDisable(GL_TEXTURE_2D);
}

void  initMazeGeometry() {
	fprintf(stdout, "initializing maze geometry...\n");
	if (__goalList == 0)
		__initGoal();

	if (__trapList == 0)
		__initTrap();

	if (__wallList == 0)
		__initWall();

	if (__floorList == 0 )//|| __floorTrapList == 0)
		__initFloor();
}

void drawMaze(Maze* __maze, GLenum mode) {
	/* goals are translucent so they must be drawn after all other objects, we
	 * keep track of the goals position so ther's no need to fetch the whole
	 * array twice.
	 */
	GLuint col, row;
	GLuint gp = 0;
	MazeElement* goals[NUM_GOALS];

	glPushMatrix();
	for (col = 0; col < __maze->ncol; col++) {
		if (mode == GL_SELECT) glLoadName(col);
		for (row = 0; row < __maze->nrow; row++) {
			glPushMatrix();
			/* draw the floor tile giving it a name if in selection mode */
			glTranslatef((GLfloat) (col * ELEM_SIDE), 0.0, (GLfloat) (row * ELEM_SIDE));
			if (mode == GL_SELECT) glPushName(row);
			__drawFloor(__maze->matrix[col][row]);
			if (mode == GL_SELECT) glPopName();

			if (mode == GL_RENDER) {
				/* draw opaque objects (walls) */
				if (
						(col == 0 || !(__maze->matrix[col-1][row]->wall == DIR_WEST && __maze->matrix[col][row]->wall == DIR_EAST))
						&&
						(row == 0 || !(__maze->matrix[col][row-1]->wall == DIR_NORTH && __maze->matrix[col][row]->wall == DIR_SOUTH))
				) __drawWall(__maze->matrix[col][row]->wall);

				/* saves the goals positions */
				/* NOTE: in case of other tranclucent objects,
				 * thy have to be saved here too and drawn later
				 * along with goals */
				if (__maze->matrix[col][row]->type == CE_GOAL) {
					goals[gp++] = __maze->matrix[col][row];
				}
			}
			glPopMatrix();
		}
	}

	/* draws translucent objects (goals) */
	if (mode == GL_RENDER) {
		for (gp = 0; gp < getNumGoals(); gp++) {
			glPushMatrix();
			glTranslatef((GLfloat)(goals[gp]->col * ELEM_SIDE), 0.0, (GLfloat)(goals[gp]->row * ELEM_SIDE));
			__drawGoal(goals[gp]);
			glPopMatrix();
		}
	}

	glPopMatrix();
}

void drawLightMaze(Maze* __maze, GLint posx, GLint posy, GLenum mode) {
	/* goals are translucent so they must be drawn after all other objects, we
	 * keep track of the goals position so ther's no need to fetch the whole
	 * array twice.
	 */
	GLint col, row;
	GLuint gp = 0, gtotal = 0;
	//MazeElement* goals[NUM_GOALS];
	MazeElement* goals[__maze->numGoals];

	glPushMatrix();
	for (col = posx-1; col < posx+2; col++) {
		if (mode == GL_SELECT) glLoadName(col);
		for (row = posy-1; row < posy+2; row++) {
			if (col>=0 && row>=0 && col<__maze->ncol && row<__maze->nrow) {
				glPushMatrix();
				/* draw the floor tile giving it a name if in selection mode */

				glTranslatef((GLfloat) (col * ELEM_SIDE), 0.0, (GLfloat) (row * ELEM_SIDE));
				if (mode == GL_SELECT) glPushName(row);
				__drawFloor(__maze->matrix[col][row]);
				if (mode == GL_SELECT) glPopName();

				if (mode == GL_RENDER) {
					/* draw opaque objects (walls) */
					/* only draw non overlapping walls */
					if (
							(col == 0 || !(__maze->matrix[col-1][row]->wall == DIR_WEST && __maze->matrix[col][row]->wall == DIR_EAST))
							&&
							(row == 0 || !(__maze->matrix[col][row-1]->wall == DIR_NORTH && __maze->matrix[col][row]->wall == DIR_SOUTH))
					) {
						__drawWall(__maze->matrix[col][row]->wall);
					}

					/* saves the goals positions */
					/* NOTE: in case of other tranclucent objects,
					 * thy have to be saved here too and drawn later
					 * along with goals */
					if (__maze->matrix[col][row]->type == CE_GOAL) {
						goals[gtotal++] = __maze->matrix[col][row];
					}

				}
				glPopMatrix();
			}
		}
	}

	/* draws translucent objects (goals) */
	if (mode == GL_RENDER) {
		for (gp = 0; gp < gtotal; gp++) {
			glPushMatrix();
			glTranslatef((GLfloat)(goals[gp]->col * ELEM_SIDE), 0.0, (GLfloat)(goals[gp]->row * ELEM_SIDE));
			__drawGoal(goals[gp]);
			glPopMatrix();
		}
	}

	glPopMatrix();
}

void drawMiniMap(void) {
	Maze* m = getMaze();
	GLuint col, row;
	GLUquadricObj *goal;

	goal = gluNewQuadric();
	gluQuadricDrawStyle(goal, GLU_FILL);
	gluQuadricNormals(goal, GLU_NONE);

	for (col = 0; col < m->ncol; col++) {
		for (row = 0; row < m->nrow; row++) {
			glPushMatrix();
			glTranslatef((col*MINMAP_OBJ_SIZE)+(MINMAP_OBJ_SIZE/2), (row*MINMAP_OBJ_SIZE)+(MINMAP_OBJ_SIZE/2), 0.0);
			if (m->matrix[col][row]->type == CE_GOAL) {	/* eventually draw goal marker */
				glColor3f(0.0, 1.0, 0.5);
				gluDisk(goal, 0.0, MINMAP_OBJ_SIZE/3, 20, 5);
			}
			glPushMatrix();
			glColor3f(0.8, 0.2, 0.2);
			switch (m->matrix[col][row]->wall) {
			case DIR_SOUTH:
				break;
			case DIR_NORTH:
				glRotatef(180.0, 0.0, 0.0, 1.0);
				break;
			case DIR_WEST:
				glRotatef(90.0, 0.0, 0.0, 1.0);
				break;
			case DIR_EAST:
				glRotatef(-90.0, 0.0, 0.0, 1.0);
				break;
			default:
				break;
			}
			glLineWidth(2);
			glBegin(GL_LINES);	/* wall marker */
			glVertex2f(MINMAP_OBJ_SIZE/2, -MINMAP_OBJ_SIZE/2);
			glVertex2f(-MINMAP_OBJ_SIZE/2, -MINMAP_OBJ_SIZE/2 );
			glEnd();
			glPopMatrix();
			glPopMatrix();
		}
	}
}
