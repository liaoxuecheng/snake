/*
 * Copyright (C), 2010-2017, ATCOM Tech. Co., Ltd.
 *
 * File Name: tetris.h
 * Author: liaoxuecheng
 * mail: liaoxuecheng@atcom.com.cn
 * Created Time: Thu 27 Sep 2018 08:01:01 PM CST
 *
 */

#ifndef _TETRIS_H_
#define _TETRIS_H_

#define WALL_H 20
#define WALL_W 13
#define SCORE_W 9
#define SPACE_SIZE (WALL_W-2)

#define WALL -1
#define SPACE 0
#define BODY 1

#define PWALL "#"
#define PSPACE " "
#define PBODY "■"

#define DIRECTION_UP 65
#define DIRECTION_DOWN 66
#define DIRECTION_LEFT 68
#define DIRECTION_RIGHT 67
#define DIRECTION_PAUSE 112
#define DIRECTION_RESTART 114
#define DIRECTION_ONLY_PRINT 0

#endif //_TETRIS_H_
