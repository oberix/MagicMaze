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

#ifndef GAME_H_
#define GAME_H_

#include <GL/glut.h>

extern void initGame(GLuint ncol, GLuint nrow);
extern void initMenu(void);

/* animation manageing functions */
extern void togglePause(void);
extern void setAnimation(void (* animation)(void));
extern void stdAnim(void);
extern void walkAnim(void);
extern GLuint getAnimationTimer(void);

/* callback functions to handle I/O events */
extern void special(int key, int x, int y);
extern void keyboard(unsigned char key, int x, int y);
extern void mouse(int button, int state, int x, int y);
extern void display(void);
extern void reshape(int w, int h);

#endif /*GAME_H_*/
