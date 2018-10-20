/*
 * Copyright (C), 2010-2017, ATCOM Tech. Co., Ltd.
 *
 * File Name: node.h
 * Author: liaoxuecheng
 * mail: liaoxuecheng@atcom.com.cn
 * Created Time: Fri 21 Sep 2018 05:12:52 PM CST
 * ▼◆☀■△□█◇
 */

#ifndef _NODE_H_
#define _NODE_H_

#define SPACE_SIZE 20
#define WALL_SIZE 22

#define SCORE_X 25
#define WALL -1
#define SPACE 0
#define SNAKE_HEAD 1
#define SNAKE_BODY 2
#define APPLE 3
#define SNAKE_UP 65
#define SNAKE_DOWN 66
#define SNAKE_LEFT 68
#define SNAKE_RIGHT 67
#define TURN_NEXT 500000
#define SHOW_HEAD "\033[1;35m@\033[0m"
#define SHOW_BODY "\033[1;36m■\033[0m"
#define SHOW_APPLE "\033[1;33m☀\033[0m"
#define SHOW_WALL "#"
#define SHOW_SPACE " "
#define START_HEAD_X 5
#define START_HEAD_Y 5

#endif //_NODE_H_
