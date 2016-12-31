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

#include "includes/maze.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

static Maze* __maze = NULL;

static MazeElement* __newMazeElement(GLubyte col, GLubyte row, CenterType ctype) {
	MazeElement* me = (MazeElement*)malloc(sizeof(MazeElement));
	me->col = col;
	me->row = row;
	me->type = ctype;
	me->wall = randDir();
	switch (ctype) {
	case CE_TRAP:
	case CE_NULL:
		me->state = A_STILL;
		break;
	case CE_GOAL:
		me->state = A_STD;
		break;
	default:
		fprintf(stderr, "error: state unnown\n");
	}
	if (ctype == CE_GOAL) 
		me->alpha = (GLfloat)(rand() % GOAL_NSTATES); //((GLfloat)rand()/(GLfloat)RAND_MAX)*2.0*PI;
	else 
		me->alpha = 0.0;
	return me;
}

#if DEBUG > 2
static void __printMazeElement(MazeElement* e) {
	printf("\t\tposition (col, row): %d, %d\n", e->col, e->row);
	printf("\t\ttype: ");
	switch(e->type) {
	case CE_NULL:
		printf("CE_NULL\n");
		break;
	case CE_TRAP:
		printf("CE_TRAP\n");
		break;
	case CE_GOAL:
		printf("CE_GOAL\n");
		break;
	default:
		printf("unnown\n");
		break;
	}
	printf("\n");
}
#endif

void initMaze(GLubyte ncol, GLubyte nrow) {
	CenterType assigner[ncol][nrow];
	GLubyte i;

	fprintf(stdout, "initializing game scheme...\n");
	
	/* allocalte and initialize maze */
	
	freeMaze();
	__maze = (Maze*)malloc(sizeof(Maze));
	__maze->nrow = nrow;
	__maze->ncol = ncol;
	__maze->numGoals = NUM_GOALS;
	__maze->matrix = (MEObject**)calloc(ncol, sizeof(MEObject*));
	for (ncol = 0; ncol < __maze->ncol; ncol++) {
		__maze->matrix[ncol] = (MazeElement**)calloc(__maze->nrow, sizeof(MazeElement));
	}
	
	/* initialize the array to assign the goals and traps positions */
	for (ncol = 0; ncol < __maze->ncol; ncol++) {
		for (nrow = 0; nrow < __maze->nrow; nrow++) {
			assigner[ncol][nrow] = CE_NULL;
		}
	}
	
	/* assign goals initial positions */
	for(i = 0; i < NUM_GOALS; i++) {
		do {
			ncol = rand()%__maze->ncol;
			nrow = rand()%__maze->nrow;
		} while (assigner[ncol][nrow] != CE_NULL);
		assigner[ncol][nrow] = CE_GOAL;
#if DEBUG > 1
		fprintf(stdout, "DEBUG:\tgoal assigned at (%d,%d)\n", nrow, ncol);
#endif
	}
#if DEBUG > 0
	fprintf(stdout, "DEBUG:\t%d goals assigned\n", i);
#endif
	
	/* assign traps initial positions */
	for(i = 0; i < NUM_TRAPS; i++) {
		do {
			ncol = rand()%__maze->ncol;
			nrow = rand()%__maze->nrow;
		} while (assigner[ncol][nrow] != CE_NULL);
		assigner[ncol][nrow] = CE_TRAP;
#if DEBUG > 1
		fprintf(stdout "DEBUG:\ttrap assigned at (%d,%d)\n", nrow, ncol);
#endif
	}
#if DEBUG > 0
	fprintf(stdout, "DEBUG:\t%d traps assigned\n", i);
#endif
	
	/* create maze elements */
	fprintf(stdout, "\tgenerating maze elements on %d columns and %d rows...\n", __maze->ncol, __maze->nrow);
	for (ncol = 0; ncol < __maze->ncol; ncol++) {
		for (nrow = 0; nrow < __maze->nrow; nrow++) {
			__maze->matrix[ncol][nrow] = __newMazeElement(ncol, nrow, assigner[ncol][nrow]);
#if DEBUG > 1
			__printMazeElement(__maze->matrix[ncol][nrow]);
#endif
		}
	}
#if DEBUG > 0
	fprintf(stdout, "done\n");
#endif
}

void shiftMaze(Direction dim, GLubyte index) {
	MazeElement* oldme;
	GLubyte i;
	
	switch(dim) {
	case DIR_WEST:
		if (index > __maze->nrow) {
			fprintf(stderr, "ERROR: shiftMaze(): index out of range\n");
			exit(1);
		}
		oldme = __maze->matrix[__maze->ncol-1][index];
		for (i = __maze->ncol-1; i >= 1; i--) {
			__maze->matrix[i][index] = __maze->matrix[i-1][index];
			__maze->matrix[i][index]->col++;
		}
		oldme->col = 0;
		__maze->matrix[0][index] = oldme;
		break;
	case DIR_EAST:
		if (index > __maze->nrow) {
			fprintf(stderr, "ERROR: shiftMaze(): index out of range\n");
			exit(1);
		}
		oldme = __maze->matrix[0][index];
		for (i = 0; i < __maze->ncol-1; i++) {
			__maze->matrix[i][index] = __maze->matrix[i+1][index];
			__maze->matrix[i][index]->col--;
		}
		oldme->col = __maze->ncol-1;
		__maze->matrix[__maze->ncol-1][index] = oldme;
		break;
	case DIR_NORTH:
		if (index > __maze->ncol) {
			fprintf(stderr, "ERROR: shiftMaze(): index out of range\n");
			exit(1);
		}
		oldme = __maze->matrix[index][__maze->nrow-1];
		for (i = __maze->nrow-1; i >= 1; i--) {
			__maze->matrix[index][i] = __maze->matrix[index][i-1];
			__maze->matrix[index][i]->row++;
		}
		oldme->row = 0;
		__maze->matrix[index][0] = oldme;
		break;
	case DIR_SOUTH:
		if (index > __maze->ncol) {
			fprintf(stderr, "ERROR: shiftMaze(): index out of range\n");
			exit(1);
		}
		oldme = __maze->matrix[index][0];
		for (i = 0; i < __maze->nrow-1; i++) {
			__maze->matrix[index][i] = __maze->matrix[index][i+1];
			__maze->matrix[index][i]->row--;
		}
		oldme->row = __maze->nrow-1;
		__maze->matrix[index][__maze->nrow-1] = oldme;
		break;
	default:
		fprintf(stderr, "ERROR: shiftMaze(): wrong parameter for direction\n");
		exit(1);
	}
}

Direction randomShift(GLubyte* ret, GLuint x, GLuint y) {
	Direction dir = randDir();
	GLubyte index;
	
	switch(dir) {
	case DIR_NORTH:
	case DIR_SOUTH:
		do
		index = (GLubyte)rand()%__maze->ncol;
		while (index == x);	/* this is to avoid shifting the floor underneeth 
							 * the player's feet, causing it to be pulled off 
							 * the maze in some occasions */
		break;
	case DIR_WEST:
	case DIR_EAST:
		do
		index = (GLubyte)rand()%__maze->nrow;
		while (index == y);
		break;
	default:
		break;
	}
#if DEBUG > 0
	fprintf(stdout, "DEBUG: dir = %d, index = %d\n", dir, index);
#endif
	shiftMaze(dir, index);
	if (ret != NULL) *ret = index;
	return dir;
}

void freeMaze(void) {
	GLubyte col, row;
	if (__maze != NULL) {
		for (col = 0; col < __maze->ncol; col++) {
			for (row = 0; row < __maze->nrow; row++) {
				free(__maze->matrix[col][row]);
			}
		}
		free(__maze);
	}
}

Maze* getMaze(void) {
	if (__maze != NULL)
		return __maze;
	fprintf(stderr, "ERROR: freeMaze(): trying to get uniitialized Maze\n");
	exit(1);
}

MazeElement* getMazeElement(GLuint row, GLuint col) {
	return __maze->matrix[col][row];
}

GLuint getNumGoals(void) {
	return __maze->numGoals;
}

GLuint getMazeCol(void) {
	return __maze->ncol;
}
GLuint getMazeRow(void) {
	return __maze->nrow;
}

void decrementGoals(void) {
	__maze->numGoals--;
}
