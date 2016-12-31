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

#ifndef DIM_H_
#define DIM_H_

#include <GL/glut.h>

#define DEBUG 0

#define PI 			(GLfloat) 3.141592654
#define TIME_DELTA	(GLfloat) PI/30
//#define GOAL_NSTATES	90
//#define TRAP_NSTATES	9
#define SHIFT_DELTA	(GLfloat) ELEM_SIDE/50.0

#define ELEM_SIDE	(GLfloat) 6.0
#define WALL_HIGHT	(GLfloat) 3.0
#define WALL_DEPTH	(GLfloat) 0.5
#define CENTER_RAD	(GLfloat) ELEM_SIDE / 6.0
#define FLOOR_WIDTH	(GLfloat) ((ELEM_SIDE/ 2.0) - CENTER_RAD)
#define MINMAP_OBJ_SIZE	(GLfloat)	20

#define TRAPS_RATIO	(GLfloat) 0.10
#define GOALS_RATIO	(GLfloat) 0.10
#define NUM_GOALS	(GLuint) ceilf(__maze->ncol*__maze->nrow*GOALS_RATIO)
#define NUM_TRAPS	(GLuint) ceilf(__maze->ncol*__maze->nrow*TRAPS_RATIO)
GLuint numgoals;

#define EYE_DIST	(GLfloat) 20.0

typedef enum __Direction { 
	DIR_NORTH = -1, 
	DIR_SOUTH = 1,
	DIR_EAST = 2, 
	DIR_WEST = -2 
} Direction;

typedef enum __AnimState { 
	A_STILL,	/* no amimation */ 
	A_STD, 		/* standard animation */
	A_ALT 		/* alternative animation */
} AnimState;

/* randDir(void): return a random direction */
extern Direction randDir(void);

#endif /*DIM_H_*/
