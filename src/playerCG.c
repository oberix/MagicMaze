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

#include "includes/playerCG.h"
#include "includes/cgUtils.h"
#include <GL/glut.h>
#include <math.h>

static GLuint __playerList = 0;

#define STEP_WIDTH	(float)20	
#define STEP_SPEED	(float)5	/* higher means slower */
#define FALL_DELTA	(float)0.3
#define STRECH_DELTA	(float)0.2
#define HAT_FALL_DELTA	(float)0.1
#define HAT_START	(float) -4

void initPlayerGraphic(void) {
	GLUquadricObj *brim, *leg;
	GLuint foot, body, hat, state;
	GLfloat strech, shift, feetTheta, hatAlpha, hatHight;
	GLfloat mat_diffuse_skin[] = { 0.80, 0.61, 0.50 };
	GLfloat mat_diffuse_dress[] = { 0.00, 0.31, 0.91 };
	GLfloat mat_diffuse_feet[] = { 0.08, 0.15, 0.17 };
	GLfloat mat_diffuse_brim[] = { 0.9, 0.9, 0.2 };
	GLfloat mat_specular[] = { 0.15, 0.15, 0.15 };
	GLfloat mat_zero[] = { 0.0, 0.0, 0.0, 1.0 };
	
	if (__playerList == 0) {
		/* foot */
		foot = glGenLists(1);
		leg = gluNewQuadric();
		gluQuadricDrawStyle(leg, GL_FILL);
		gluQuadricNormals(leg, GLU_SMOOTH);
		glNewList(foot, GL_COMPILE);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse_feet);
		glPushMatrix();
		glRotatef(-90, 1.0, 0.0, 0.0);
		gluCylinder(leg, 0.15, 0.15, 0.3, 5, 10);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(0.0, 0.0, -0.15);
		glScalef(0.9, 0.2, 1.2);
		glutSolidSphere(0.45, 10, 10);
		glPopMatrix();
		glEndList();

		/* body and head */
		body = glGenLists(1);
		glNewList(body, GL_COMPILE);
		glPushMatrix();
		glTranslatef(0.0, 0.3, 0.0);
		glRotatef(-90.0, 1.0, 0.0, 0.0);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse_dress);
		glPushMatrix();
		glScalef(1.0, 0.9, 1.0);
		glutSolidCone(0.75, 1.5, 10.0, 10.0);
		glPopMatrix();
		glTranslatef(0.0, 0.0, 1.5);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse_skin);
		glutSolidSphere(0.45, 10, 10);
		glPopMatrix();
		glEndList();

		/* hat */
		hat = glGenLists(1);
		brim = gluNewQuadric();
		gluQuadricDrawStyle(brim, GLU_FILL);
		gluQuadricNormals(brim, GLU_SMOOTH);
		glNewList(hat, GL_COMPILE);
		glPushMatrix();
		glTranslatef(0.0, 2.1, 0.0);
		glRotatef(-90.0, 1.0, 0.0, 0.0);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse_brim);
		gluDisk(brim, 0.375, 0.9, 20, 5);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse_dress);
		glutSolidCone(0.45, 0.9, 10, 10);
		glPopMatrix();
		glEndList();

		/* compose the guy as a hierarchical display lists */
		/* the sequence is divided in two parts to achive the two animation 
		 * the charachter must have: walking and falling */
		__playerList = glGenLists(PLAYER_NSTATES);
		for (state = 0; state < PLAYER_NSTATES; state++) {
			if (state < FALLING_OFFSET) {	/* walking */
				feetTheta = (GLfloat)sin((float)state/STEP_SPEED)*STEP_WIDTH;
				strech = 1.0;
				hatHight = 0.0;
				hatAlpha = 0.0;
				shift = 0.0;
			} else {	/* falling */
				feetTheta = 0.0;
				strech += STRECH_DELTA; 
				shift -= FALL_DELTA;
				if (shift < HAT_START) {
					hatHight-=HAT_FALL_DELTA;
					hatAlpha = (GLfloat)(sin(hatHight)*STEP_WIDTH);
				}
			}

			glNewList(__playerList+state, GL_COMPILE);
			glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
			glMaterialfv(GL_FRONT, GL_EMISSION, mat_zero);
			glPushMatrix();    /* tansormations for the falling animation  */
			glScalef(1.0, strech, 1.0);
			glTranslatef(0.0, shift, 0.0);

			/* right foot */
			glPushMatrix();
			glTranslatef(0.0, 1.0, 0.0);
			glRotatef(feetTheta, 1.0, 0.0, 0.0);
			glTranslatef(0.45, -1.0, 0.0);
			glCallList(foot);
			glPopMatrix();

			/* left foot */
			glPushMatrix();
			glTranslatef(0.0, 1.0, 0.0);
			glRotatef(-feetTheta, 1.0, 0.0, 0.0);
			glTranslatef(-0.45, -1.0, 0.0);
			glCallList(foot);
			glPopMatrix();

			/* body and head */
			glCallList(body);
			glPopMatrix();

			/* hat */
			glPushMatrix();
			glRotatef(hatAlpha, 0.0, 0.0, 1.0);
			glTranslatef(0.0, hatHight, 0.0);
			glCallList(hat);
			glPopMatrix();
			glEndList();
		}
	}
}

static void playerLight(void) {
	GLfloat position[] = { 0.0, 5.0, 0.0, 1.0 };
	GLfloat direction[] = { 0.0, -1.0, 0.0 };
	GLfloat ambient[] = { 0.1, 0.1, 0.1, 0,1 };
	GLfloat diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };
	
	glLightfv(GL_LIGHT1, GL_POSITION, position);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
	
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, direction);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 45.0);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 0.0);
}

void drawPlayer(Player* p) {
	glPushMatrix();
	switch (p->dir) {
	case DIR_NORTH:
		glRotatef(180.0, 0.0, 1.0, 0.0);
		break;
	case DIR_EAST:
		glRotatef(90.0, 0.0, 1.0, 0.0);
		break;
	case DIR_WEST:
		glRotatef(-90.0, 0.0, 1.0, 0.0);
		break;
	default:
		break;
	}
	
	playerLight();
	
	glDisable(GL_LIGHT1);
	switch (p->state) {
	case A_STILL:
		glCallList(__playerList);
		break;
	case A_STD:
		p->alpha = (p->alpha + 1) % FALLING_OFFSET;
		glCallList(__playerList + p->alpha);
		break;
	case A_ALT:
		p->alpha = ((p->alpha + 1) % (PLAYER_NSTATES-FALLING_OFFSET)) + FALLING_OFFSET;
		glCallList(__playerList + p->alpha);
		if (p->alpha == PLAYER_NSTATES-1) { 
			p->state = A_STILL;
			p->reset = GL_TRUE;
		}
		break;
	default:
		break;
	}
	glEnable(GL_LIGHT1);
	glPopMatrix();
	resetMatDefault();
}

void drawMiniPlayer(void) {
	GLUquadricObj *player;
	player = gluNewQuadric();
	gluQuadricDrawStyle(player, GLU_FILL);
	gluQuadricNormals(player, GLU_NONE);

	glColor3f(0.0, 0.5, 0.8);
	glPushMatrix();
	
	glTranslatef((getPlayeri(PL_COL)*MINMAP_OBJ_SIZE)+(MINMAP_OBJ_SIZE/2), (getPlayeri(PL_ROW)*MINMAP_OBJ_SIZE)+(MINMAP_OBJ_SIZE/2), 0.0);
	gluDisk(player, 0.0, MINMAP_OBJ_SIZE/3, 20, 5);
	glPopMatrix();
}
