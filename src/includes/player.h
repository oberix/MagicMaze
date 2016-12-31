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

#ifndef PLAYER_H_
#define PLAYER_H_

#include "dim.h"
#include <GL/glut.h>

typedef struct __Player {
	GLuint col, row;	/* cirrent player position in the maze */
	GLuint alpha;		/* used to handle animation */
	GLboolean reset;
	Direction dir;		/* the direction the player is heading */
	AnimState state;
} Player;

typedef enum __PLparam { PL_COL, PL_ROW, PL_ALPHA } PLparam;

extern void initPlayer(void);
extern Player* getPlayer(void);
extern void movePlayer(Direction dir);
extern void stopPlayer(void);
extern void freePlayer(void);
extern void setPlayerui(PLparam param, GLuint value);
extern void setPlayerDirection(Direction dir);
extern void setPlayerState(AnimState state);
extern GLuint getPlayeri(PLparam param);
extern AnimState getPlayerState(void);
extern GLboolean isPlayerToReset(void);
extern void togglePlayerReset(void);

#endif /*PLAYER_H_*/
