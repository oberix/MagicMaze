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

#include "includes/cgUtils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//static int __time, __frame = 0, __timebase = 0;	/* variable used to calculate the framerate */
//static char __fpsString[30];

/* glmReadPPM: read a PPM raw (type P6) file.  The PPM file has a header
 * that should look something like:
 *
 *    P6
 *    # comment
 *    width height max_value
 *    rgbrgbrgb...
 *
 * where "P6" is the magic cookie which identifies the file type and
 * should be the only characters on the first line followed by a
 * carriage return.  Any line starting with a # mark will be treated
 * as a comment and discarded.   After the magic cookie, three integer
 * values are expected: width, height of the image and the maximum
 * value for a pixel (max_value must be < 256 for PPM raw files).  The
 * data section consists of width*height rgb triplets (one byte each)
 * in binary format (i.e., such as that written with fwrite() or
 * equivalent).
 *
 * The rgb data is returned as an array of unsigned chars (packed
 * rgb).  The malloc()'d memory should be free()'d by the caller.  If
 * an error occurs, an error message is sent to stderr and NULL is
 * returned.
 *
 * filename   - name of the .ppm file.
 * width      - will contain the width of the image on return.
 * height     - will contain the height of the image on return.                                                                                                   
 */
GLubyte* glmReadPPM(char* filename, int* width, int* height) {
	FILE* fp;
	int i, w, h, d;
	unsigned char* image;
	char head[70];          /* max line <= 70 in PPM (per spec). */

	fp = fopen(filename, "rb");
	if (!fp) {
		perror(filename);
		return NULL;
	}

	/* grab first two chars of the file and make sure that it has the
	       correct magic cookie for a raw PPM file. */
	fgets(head, 70, fp);
	if (strncmp(head, "P6", 2)) {
		fprintf(stderr, "%s: Not a raw PPM file\n", filename);
		return NULL;
	}

	/* grab the three elements in the header (width, height, maxval). */
	i = 0;
	while(i < 3) {
		fgets(head, 70, fp);
		if (head[0] == '#')     /* skip comments. */
			continue;
		if (i == 0)
			i += sscanf(head, "%d %d %d", &w, &h, &d);
		else if (i == 1)
			i += sscanf(head, "%d %d", &h, &d);
		else if (i == 2)
			i += sscanf(head, "%d", &d);
	}

	/* grab all the image data in one fell swoop. */
	image = (unsigned char*)malloc(sizeof(unsigned char)*w*h*3);
	fread(image, sizeof(unsigned char), w*h*3, fp);
	fclose(fp);

	*width = w;
	*height = h;
	return image;
}

GLubyte* loadDefaultText2D(char* filename, int* width, int* height) {
	GLubyte *texture;
//	int w, h;
	GLfloat border[] = { 1.0, 1.0, 1.0, 0.0 };
	GLfloat env[] = { 1.0, 1.0, 1.0, 0.0 };

	texture = glmReadPPM(filename, width, height);

	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
	glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, env);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, w, h, GL_RGB, GL_UNSIGNED_BYTE, texture);

	/* return the pointer to the memory that have to be free()d by the caller */
	return (texture);
}

void renderBitmapString(float x, float y, void *font, char* string) {
  char *c;
  // set position to start drawing fonts
  glRasterPos2f(x, y);
  // loop all the characters in the string
  for (c=string; *c != '\0'; c++) {
    glutBitmapCharacter(font, *c);
  }
}

//void renderFPS(void) {
//	int font=(int)GLUT_BITMAP_8_BY_13;
//	
//	__frame++;
//	__time=glutGet(GLUT_ELAPSED_TIME);
//	if (__time - __timebase > 1000) {
//		sprintf(__fpsString,"FPS:%4.2f", //calculateFPS());
//				__frame*1000.0/(__time-__timebase));
//		__timebase = __time;		
//		__frame = 0;
//	}
//	glDisable(GL_LIGHTING);
//	if (__fog) glDisable(GL_FOG);
//	glColor3f(1.0, 1.0, 1.0);
//	glPushMatrix();
//	glLoadIdentity();
//	glMatrixMode(GL_PROJECTION);
//	glPushMatrix();
//	glLoadIdentity();
//	gluOrtho2D(0, 800, 0, 600);
//	glScalef(1, -1, 1);
//	glTranslatef(0, -600, 0);
//	glMatrixMode(GL_MODELVIEW);
//	renderBitmapString(30,35,(void *)font,__fpsString);
//	glPopMatrix();
//	glMatrixMode(GL_PROJECTION);
//	glPopMatrix();
//	glMatrixMode(GL_MODELVIEW);
//	glEnable(GL_LIGHTING);
//	if (__fog) glEnable(GL_FOG);
//}

void resetMatDefault(void) {
	GLfloat ambient[] =  {0.2, 0.2, 0.2, 1.0};
	GLfloat diffuse[] = {0.8, 0.8, 0.8, 1.0};
	GLfloat specular[] = {0.0, 0.0, 0.0, 1,0};
	
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
}
