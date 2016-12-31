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

#ifndef MAZE_H_
#define MAZE_H_

#include "dim.h"
#include <GL/glut.h>

typedef enum __CenterType { CE_NULL, CE_GOAL, CE_TRAP } CenterType;

typedef struct __MazeElement {
	GLubyte col, row;	/* position in the matrix */
	Direction wall;		/* wall position (each square has one wall) */
	CenterType type;	/* type of object in the square */
	AnimState state;	/* animation state */
	GLfloat alpha;		/* used to handle animations */
} MazeElement;

typedef MazeElement* MEObject;

typedef struct __Maze {
	GLubyte ncol, nrow;
	MazeElement* (**matrix);
	GLuint numGoals;
//	MEObject **matrix;
} Maze;

#define GOAL_NSTATES	90
#define TRAP_NSTATES	9

//extern void initMazeGeometry(void);
extern void initMaze(GLubyte col, GLubyte row);
extern void shiftMaze(Direction dim, GLubyte index);
Direction randomShift(GLubyte* ret, GLuint x, GLuint y);
//extern void drawMaze(GLenum mode);
extern Maze* getMaze(void);
extern MazeElement* getMazeElement(GLuint row, GLuint col);
extern GLuint getNumGoals(void);
extern GLuint getMazeCol(void);
extern GLuint getMazeRow(void);
void decrementGoals(void);
extern void freeMaze(void);

#endif /*MAZE_H_*/
