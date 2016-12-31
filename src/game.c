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

#include "includes/game.h"
#include "includes/dim.h"
#include "includes/maze.h"
#include "includes/mazeCG.h"
#include "includes/cgUtils.h"
#include "includes/player.h"
#include "includes/playerCG.h"
#include <math.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

static void (* __idle)(void) = NULL;	/* this keeps the last idle callback funcion to restore it after a pause */ 
static GLboolean __pause = GL_FALSE;	/* pause animation? */
static GLboolean __showFPS = GL_FALSE; 
static GLboolean __walking = GL_FALSE;
static GLboolean __showHUD = GL_TRUE;
static GLboolean __fog = GL_TRUE;
static GLboolean __minimap = GL_TRUE;
static GLboolean __keepSeed = GL_FALSE;
static GLboolean __lightCG = GL_TRUE;
static unsigned int __seed = 1;
static int __time, __frame = 0, __timebase = 0;	/* variable used to calculate the framerate */
static char __fpsString[30];


static GLfloat __eyeTheta = PI/3.0;     /* eye angle around xz plane */                                                                              
static GLfloat __eyeAlpha = -PI/2;		/* eye angle around y axiz */
static GLfloat __aimcur[2] = { ELEM_SIDE/2.0, ELEM_SIDE/2.0 };	/* current aim coordinates on the xz plane */
static GLuint __selected[2];			/* mouse selected floor tyle coordinates */

static unsigned short __turn = 0;	/* turn counter */

#define sign(x) (x)/fabs((x))
#define rad2deg(x) (GLfloat)((x)*180.0/PI)

/* 
 * from_urand: return a pseudo-random number from /dev/urand to be used as a 
 * 	seed for a pseudo-random number generator 
 */
static unsigned int __from_urand(void) {                                                                                                                                                                                    
	int fd;                                                                                                                                                                      
	unsigned int ret = 0;                                                                                                                                                        

	if ((fd  = open("/dev/urandom", O_RDONLY)) >= 0) {                                                                                                                           
		if (read(fd, &ret, sizeof(unsigned int)) < 0)                                                                                                                        
			perror("read from /dev/urandom");                                                                                                                            
		close(fd);                                                                                                                                                           
	} else {                                                                                                                                                                     
		perror("open /dev/urandom");                                                                                                                                         
	}                                                                                                                                                                            
	return ret;                                                                                                                                                                  
}

static void __toggleFog(void) {
	if (__fog) {
		glEnable(GL_FOG);
	}
	else {
		glDisable(GL_FOG);
	}
}

static void __initPosition(void) {
	__selected[0] = div(getMazeCol(), 2).quot;
	__selected[1] = div(getMazeRow(), 2).quot;
	setPlayerui(PL_COL, __selected[0]);
	setPlayerui(PL_ROW, __selected[1]);
	__aimcur[0] = (GLfloat)(__selected[0]+0.5)*ELEM_SIDE;
	__aimcur[1] = (GLfloat)(__selected[1]+0.5)* ELEM_SIDE;
}

static void __light0(void) {
	GLfloat position[] = { 10.0, 10.0, 10.0, 0.0 };
	GLfloat ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat diffuse[] = { 0.5, 0.5, 0.5, 0.5 };
	GLfloat specular[] = { 0.0, 0.0, 0.0, 1.0 };
	
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	
	glEnable(GL_LIGHT0);
}

void initGame(GLuint ncol, GLuint nrow) {
	if (!__keepSeed)
		__seed = __from_urand();

	srand(__seed);
	fprintf(stdout, "random seed %u\n", __seed);
	initPlayerGraphic();
	initPlayer();
	initMazeGeometry();
	initMaze(ncol, nrow);
	__initPosition();
	__turn = 0;
	__toggleFog();
}

enum menu { 
	MENU_SKIP_TURN = ' ', 
	MENU_EXIT = 27, 
	MENU_RESET = 'r', 
	MENU_FPS = 'f', 
	MENU_HUD = 'h',
	MENU_FOG = 'g',
	MENU_MAZE = 'l',
	MENU_PAUSE = 'p',
	MENU_MINIMAP = 'm',
	MENU_NEW_GAME = 'n',
	MENU_NEW_SMALL,
	MENU_NEW_MEDIUM,
	MENU_NEW_BIG,
	MENU_NEW_HUGE
};

static void __gameMenu(int value) {
	keyboard((char)value, 0, 0);
}

static void __mazesMenu(int value) {
	GLuint size;
	switch (value) {
	case MENU_NEW_SMALL:
		size = 5;
		break;	
	case MENU_NEW_MEDIUM:
		size = 7;
		break;	
	case MENU_NEW_BIG:
		size = 9;
		break;	
	case MENU_NEW_HUGE:
		size = 11;
		break;	
	default :
		break;
	}
	setAnimation(NULL);
	__keepSeed = GL_FALSE;
	initGame(size, size);
	setAnimation(display);
}

void initMenu(void) {
	int game, mazes, display;
	
	mazes = glutCreateMenu(__mazesMenu);
	glutAddMenuEntry("Small (5x5)", MENU_NEW_SMALL);
	glutAddMenuEntry("Medium (7x7)", MENU_NEW_MEDIUM);
	glutAddMenuEntry("Big (9x9)", MENU_NEW_BIG);
	glutAddMenuEntry("Huge (11x11)", MENU_NEW_HUGE);
	
	game = glutCreateMenu(__gameMenu);
	glutAddMenuEntry("Reset game [r]", MENU_RESET);
	glutAddMenuEntry("New game [n]", MENU_NEW_GAME);
	glutAddSubMenu("Change maze size", mazes);
	
	display = glutCreateMenu(__gameMenu);
	glutAddMenuEntry("Toggle FPS [f]", MENU_FPS);
	glutAddMenuEntry("Toggle score [h]", MENU_HUD);
	glutAddMenuEntry("Toggle fog [g]", MENU_FOG);
	glutAddMenuEntry("Toggle minimap [m]", MENU_MINIMAP);
	glutAddMenuEntry("Toggle full maze render [l]", MENU_MAZE);
	
	glutCreateMenu(__gameMenu);
	glutAddSubMenu("Game", game);
	glutAddSubMenu("Display", display);
	glutAddMenuEntry("Skip turn [SPACE]", MENU_SKIP_TURN);
	glutAddMenuEntry("Toggle pause [p]", MENU_PAUSE);
	glutAddMenuEntry("Exit [Esc]", MENU_EXIT);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void togglePause(void) {
	__pause = !__pause;
	if (__pause) {
		glutIdleFunc(NULL);
		glutMouseFunc(NULL);
		glutSpecialFunc(NULL);
	}
	else {
		glutIdleFunc(__idle);
		glutMouseFunc(mouse);
		glutSpecialFunc(special);
	}
}

void setAnimation(void (* animation)(void)) {
	__idle = animation;
	if (!__pause) glutIdleFunc(__idle);
}

void getCurrentPosition(GLuint* x, GLuint* y) {
	if (x != NULL && y != NULL) {
		*x = __selected[0];
		*y = __selected[1];
	} else 
		fprintf(stderr, "ERROR: getCurrentPosition(), passed NULL pointer as an argument\n");
}

static void __handleShift(void) {
	GLubyte index;
	Direction dir;
 
	dir = randomShift(&index, __selected[0], __selected[1]);
	// TODO: hndle the shift animation
}

static void __handleEvent(void) {
	MazeElement* e = getMazeElement(__selected[1], __selected[0]);
	if (e->type == CE_GOAL) {
		e->type = CE_NULL;
		e->state = A_STILL;
		decrementGoals();
	} else if (e->type == CE_TRAP) {
		e->state = A_STD;
		setPlayerState(A_ALT);
	}
}

static void __updateEnvironment(GLfloat* ex, GLfloat* ey, GLfloat* ez) {
	GLfloat __aimdest[2] = { __aimcur[0], __aimcur[1] };
	char sgn[2];
	
	*ex = EYE_DIST * cos(__eyeTheta) * cos(__eyeAlpha);
	*ey = EYE_DIST * sin(__eyeTheta);
	*ez = EYE_DIST * cos(__eyeTheta) * sin(__eyeAlpha);
	
	/* reset player position */
	if (isPlayerToReset()) {
		__initPosition();
		togglePlayerReset();
	}
	
	/* Player is moving, update the paramenter for the view */
	if (__walking) {
		__aimdest[0] = 	ELEM_SIDE * (__selected[0] + 0.5);
		__aimdest[1] = 	ELEM_SIDE * (__selected[1] + 0.5);

		if (__aimdest[0]-SHIFT_DELTA > __aimcur[0] || __aimdest[0]+SHIFT_DELTA < __aimcur[0]) {
			sgn[0] = sign(__aimdest[0] - __aimcur[0]);
			__aimcur[0] += sgn[0] * SHIFT_DELTA;
		} else __aimcur[0] = __aimdest[0];
		if (__aimdest[1]-SHIFT_DELTA > __aimcur[1] || __aimdest[1]+SHIFT_DELTA < __aimcur[1]) {
			sgn[1] = sign(__aimdest[1] - __aimcur[1]);
			__aimcur[1] += sgn[1] * SHIFT_DELTA;
		} else __aimcur[1] = __aimdest[1];

		/* we reached our destination. Handle event for this tyle */
		if (__aimdest[0] == __aimcur[0] && __aimdest[1] == __aimcur[1]) {
			__walking = GL_FALSE;
			stopPlayer();
			__handleEvent();
			__handleShift();
		}
	}
}

static void __showMinimap(void) {
	GLuint col = getMazeCol();
	GLuint row = getMazeRow();
	
	
	glDisable(GL_LIGHTING);
	if (__fog) glDisable(GL_FOG);
	
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, 800, 0, 600);	
	glMatrixMode(GL_MODELVIEW);
	
	glTranslatef(5.0,5.0,0.0);
	
	glPushMatrix();
	glTranslatef((col*MINMAP_OBJ_SIZE)/2, (row*MINMAP_OBJ_SIZE)/2, 0.0);
	glRotatef(rad2deg(__eyeAlpha)+90, 0.0, 0.0, 1.0);
	glScalef(-1,1,1);
	glTranslatef(-(col*MINMAP_OBJ_SIZE)/2, -(row*MINMAP_OBJ_SIZE)/2, 0.0);
	
	glColor3f(0.8, 0.2, 0.2);
	glLineWidth(2); 
	glBegin (GL_LINE_LOOP);	/* minimap outline */
	glVertex2f(0.0, 0.0);
	glVertex2f((col*MINMAP_OBJ_SIZE), 0.0);
	glVertex2f((col*MINMAP_OBJ_SIZE), (row*MINMAP_OBJ_SIZE));
	glVertex2f(0.0, (row*MINMAP_OBJ_SIZE));
	glEnd();
	
	drawMiniMap();
	drawMiniPlayer();
	glPopMatrix();
	
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	
	glEnable(GL_LIGHTING);
	if (__fog) glEnable(GL_FOG);
}

static void __renderHUD(void) {
	int font=(int)GLUT_BITMAP_8_BY_13;
	char turn[10];
	char goals[16];
	
	sprintf(turn, "Turn: %d", __turn);
	sprintf(goals, "Goals left: %d", getNumGoals());

	glDisable(GL_LIGHTING);
	if (__fog) glDisable(GL_FOG);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, 800, 0, 600);
	glScalef(1, -1, 1);
	glTranslatef(0, -600, 0);
	glMatrixMode(GL_MODELVIEW);
	glColor3f(0.0, 1.0, 0.0);
	renderBitmapString(660,35,(void *)font, turn);
	renderBitmapString(660,55,(void *)font, goals);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_LIGHTING);
	if (__fog) glEnable(GL_FOG);
}

void __renderFPS(void) {
	int font=(int)GLUT_BITMAP_8_BY_13;
	
	__frame++;
	__time=glutGet(GLUT_ELAPSED_TIME);
	if (__time - __timebase > 1000) {
		sprintf(__fpsString,"FPS:%4.2f", //calculateFPS());
				__frame*1000.0/(__time-__timebase));
		__timebase = __time;		
		__frame = 0;
	}
	glDisable(GL_LIGHTING);
	if (__fog) glDisable(GL_FOG);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, 800, 0, 600);
	glScalef(1, -1, 1);
	glTranslatef(0, -600, 0);
	glMatrixMode(GL_MODELVIEW);
	glColor3f(0.0, 1.0, 0.0);
	renderBitmapString(30,35,(void *)font,__fpsString);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_LIGHTING);
	if (__fog) glEnable(GL_FOG);
}

// TODO: place this inside the rendering function
void display(void) {
	GLfloat eye[3];
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	__light0();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	/* calculate eye coordinates and direction */
	__updateEnvironment(&eye[0], &eye[1], &eye[2]);
	
	gluLookAt(eye[0], eye[1], eye[2],
			0.0, 0.0, 0.0,
			0.0, 1.0, 0.0);
	
	/* render the scene */
	drawPlayer(getPlayer());
	glTranslatef(-__aimcur[0], 0.0, -__aimcur[1]);
	if (__lightCG)
		drawLightMaze(getMaze(), __selected[0], __selected[1], GL_RENDER);
	else drawMaze(getMaze(), GL_RENDER);

	/* some in-game information */
	if (__showHUD) __renderHUD();
	if (__showFPS) __renderFPS();
	if (__minimap) __showMinimap();
	
	glutSwapBuffers();
}

void reshape(int w, int h) {
	glViewport(0.0, 0.0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0,
			(GLfloat)w / (GLfloat)h,
			1.0,
			ELEM_SIDE*5.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

static void __processHits(GLint hits, GLuint *buffer) {
	unsigned int i, j;
	GLuint names, *ptr, curr[2];
	Direction dir = 0;
	Maze *maze = getMaze();
	
	/* save the current position, needed later... */
	curr[0] = __selected[0];
	curr[1] = __selected[1];
	
	/* estract the selected tyle coordinates from the selection buffer */
#if DEBUG > 0 
	fprintf(stdout, "DEBUG: hits = %d\n", hits);
#endif
	ptr = (GLuint*)buffer; /* point s ate the beginning of the hits buffer */
	for (i = 0; i < hits; i++) {	/* for each hit */
		names = *ptr;	/* point to the number of names */
#if DEBUG > 0
		fprintf(stdout, "DEBUG: number of names for this hit = %d\n", names);
#endif
		ptr++;	/* points to z-near */
#if DEBUG > 0
		fprintf(stdout, "\tz1 is %g;", (float) *ptr/0x7fffffff);
#endif
		ptr++;	/* points to z-far */
#if DEBUG > 0
		fprintf(stdout, "\tz2 is %g;", (float) *ptr/0x7fffffff);
#endif
		ptr++;	/* points to first name */
#if DEBUG > 0
		fprintf(stdout, "\tnames are ");
#endif
		for (j = 0; j < names; j++) {
#if DEBUG > 0
			fprintf(stdout, "%d ", *ptr);
#endif
			__selected[j] = (GLfloat) *ptr;
			ptr++;	/* points to the next name */
		}
#if DEBUG > 0
		fprintf(stdout, "\n");
#endif
	}
	
	/* calculate direction */
	if (__selected[0] - curr[0] == 1 && __selected[1] == curr[1])
		dir = DIR_WEST;
	else if (__selected[0] - curr[0] == -1 && __selected[1] == curr[1])
		dir = DIR_EAST;
	else if (__selected[1] - curr[1] == 1 && __selected[0] == curr[0])
		dir = DIR_NORTH;
	else if (__selected[1] - curr[1] == -1 && __selected[0] == curr[0])
		dir = DIR_SOUTH;
	
	/* if direction is invalid (diagonal) or there's a wall, reset selection */
	if (dir == 0 ||
			maze->matrix[curr[0]][curr[1]]->wall == dir || 
			maze->matrix[__selected[0]][__selected[1]]->wall == -dir) {
		__selected[0] = curr[0];
		__selected[1] = curr[1];
	} else {	/* ok,let's move */
		// TODO: remove one goal from the maze, change alsa the length of the array in the drawMaze() finction, or else it will cause a segmentation fault
		__walking = GL_TRUE;
		movePlayer(dir);
		setPlayerui(PL_COL, __selected[0]);
		setPlayerui(PL_ROW,  __selected[1]);
		__turn++;
	}
}

#define BUFSIZE 512
static void __selectObject(int x, int y) {
	GLuint selectBuf[BUFSIZE];
	GLint hits;
	GLint view[4];
	
	glGetIntegerv(GL_VIEWPORT, view);
	glSelectBuffer(BUFSIZE, selectBuf);
	(void) glRenderMode(GL_SELECT);
	
	glInitNames();
	glPushName(0);
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPickMatrix((GLdouble)x, (GLdouble)(view[3] - y), 1.0, 1.0, view);
	gluPerspective(45.0, 
			(GLfloat)(view[2] - view[0])/(GLfloat)(view[3] - view[1]),
			ELEM_SIDE, 
			ELEM_SIDE * 5.0);
	glMatrixMode(GL_MODELVIEW);
	drawLightMaze(getMaze(), __selected[0], __selected[1], GL_SELECT);
	
	/* return to Projection mode before popping the previously pushed matrix  */
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glutSwapBuffers();

	hits = glRenderMode(GL_RENDER);
	__processHits(hits, selectBuf);
}

void mouse(int button, int state, int x, int y) {
	if (getPlayerState() != A_STILL) return;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		__selectObject(x, y);
	} else 
		return;
}

void keyboard(unsigned char key, int x, int y) {
	switch(key) {
	case 27:	/* quit */
		freeMaze();
		glutDestroyWindow(glutGetWindow());
		exit(0);
		break;
	case ' ':
		__handleShift();
		__turn++;
		break;
	case 'p':	/* toggle pause */
	case 'P':
		togglePause();
		break;
	case 'f':	/* toggle show framerate */
	case 'F':
		__showFPS = !__showFPS; 
		break;
	case 'h':
	case 'H':	
		__showHUD = !__showHUD;
		break;
	case 'g':
	case 'G':
		__fog = !__fog;
		__toggleFog();
		break;
	case 'l':
	case 'L':
		__lightCG = !__lightCG;
		break;
	case 'm':
	case 'M':
		__minimap = !__minimap;
		break;
	case 'n':	/* new game */
	case 'N':
		setAnimation(NULL);
		__keepSeed = GL_FALSE;
		initGame(getMazeCol(),getMazeRow());
		setAnimation(display);
		break;
	case 'r':	/* restart current game (keep random seed) */
	case 'R':
		setAnimation(NULL);
		__keepSeed = GL_TRUE;
		initGame(getMazeCol(),getMazeRow());
		setAnimation(display);
		break;
	default:
		fprintf(stderr, "key '%c' pressed at (%d, %d)\n", key, x, y);
		break;
	}
}

void special(int key, int x, int y) {
    switch(key) {
    case GLUT_KEY_UP:
        if (__eyeTheta <= PI/3) {
            __eyeTheta += 0.1;
        }
        break;
    case GLUT_KEY_DOWN:
        if (__eyeTheta >= 0.3) {
            __eyeTheta -= 0.1;
        }
        break;
    case GLUT_KEY_LEFT:
        if (__eyeAlpha >= 2*PI) __eyeAlpha = 0.0;
        __eyeAlpha += 0.1;
        break;
    case GLUT_KEY_RIGHT:
        if (__eyeAlpha >= 2*PI) __eyeAlpha = 0.0;
        __eyeAlpha -= 0.1;
        break;
    default:
        break;
    }                                                                            
}
