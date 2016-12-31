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

#include "includes/player.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

static Player* __player;

void initPlayer(void) {
	if (__player != NULL) freePlayer();
	__player = (Player*)malloc(sizeof(Player));
	__player->col = 0;
	__player->row = 0;
	__player->alpha = 0;
	__player->dir = DIR_NORTH;
	__player->state = A_STILL;
	__player->reset = GL_FALSE;
}

Player* getPlayer(void) {
	return __player;
}

void movePlayer(Direction dir) {
	__player->dir = dir;
	__player->state = A_STD;
}

void stopPlayer(void) {
	__player->state = A_STILL;
	__player->alpha = 0;
}

void freePlayer(void) {
	free(__player);
}

void setPlayerui(PLparam param, GLuint value) {
	switch (param) {
	case PL_COL:
		__player->col = (GLuint)value;
		break;
	case PL_ROW:
		__player->row = (GLuint)value;
		break;
	case PL_ALPHA:
		__player->alpha = (GLuint)value;
		break;
//	case PL_DIR:
//		__player->dir = (Direction)value;
//		break;
//	case PL_STATE:
//		__player->state = (AnimState)value;
//		break;
	default:
		fprintf(stderr, "ERROR: setPlayerui, unknown parameter\n");
		break;
	}
}

void setPlayerDirection(Direction dir) {
	__player->dir = dir;
}

void setPlayerState(AnimState state) {
	__player->state = state;
}

GLuint getPlayeri(PLparam param) {
	switch(param) {
	case PL_ROW: return __player->row;
	case PL_COL: return __player->col;
	case PL_ALPHA: return __player->alpha;
	default: fprintf(stderr, "ERROR: getPlayerui, unknown parameter\n");
	}
	return -1;
}

AnimState getPlayerState(void) {
	return __player->state;
}

GLboolean isPlayerToReset(void) {
	return __player->reset;
}

void togglePlayerReset(void) {
	__player->reset = !__player->reset;
}
