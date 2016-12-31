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

#ifndef CGUTILS_H_
#define CGUTILS_H_

#include <GL/glut.h>

/* glmReadPPM: read a PPM raw (type P6) file.
 * this function is part of the glm library by Nate Robins 1997. 2000
 * nate@pobox.com, http://www.pobox.com/~nate  */ 
extern GLubyte* glmReadPPM(char* filename, int* width, int* height);

extern GLubyte* loadDefaultText2D(char* filename, int* width, int* hight);

extern void renderBitmapString(float x, float y, void* font,char* string);

//extern void renderFPS(void);

extern void resetMatDefault(void);

#endif /*CGUTILS_H_*/
