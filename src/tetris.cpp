/*
 * Copyright (C), 2010-2017, ATCOM Tech. Co., Ltd.
 *
 * File Name: tetris.cpp
 * Author: liaoxuecheng
 * mail: liaoxuecheng@atcom.com.cn
 * Created Time: Thu 27 Sep 2018 08:08:21 PM CST
 *
 */

#include<iostream>
#include<memory>
#include<algorithm>

#include<time.h>
#include<stdio.h>
#include<string.h>
#include <termios.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#include"tetris.h"

using std::cout;
using std::cin;
using std::endl;

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
class Game_frame *gGame = NULL;

typedef struct Point
{
    friend bool operator==(const struct Point &a, const struct Point &b)
    {
        return a.x == b.x && a.y == b.y;
    }
    friend bool operator<(const struct Point &a, const struct Point &b)
    {
        return false;
    }
    int x;
    int y;
    Point():x(0), y(0){}
    Point(int xx, int yy):x(xx), y(yy){}
}Point;

class Shape
{
    public:
        const Point &operator[](const int index)
        {
            return shape[index];
        }
        Shape()
        {
            int index;
            int minx, miny;
            for(int i = 1; i < 4; i++)
            {
                index = rand() % i;
                growth_direction(index, i);
            }
            get_min_point(minx, miny);
            for(int i = 0; i < 4; i++)
            {
                shape[i].y -= miny;
                shape[i].x -= minx;
                shape[i].x += 4;
            }
        }
        void print(int y, int x)
        {
            int minx, miny;
            get_min_point(minx, miny);
            for(int i = 0; i < 4; i++)
                printf("\033[%d;%dH%s%s%s%s", y + i, x, PSPACE, PSPACE, PSPACE, PSPACE);
            for(int i = 0; i < 4; i++)
                printf("\033[%d;%dH%s", shape[i].y + y - miny, shape[i].x + x - minx, PBODY);
        }
        int print(int direction);
        void get_max_min_y(int &max_y, int &min_y)
        {
            int minx, miny, maxx, maxy, max_right = 0;
            get_min_point(minx, miny);
            get_max_point(maxx, maxy);
            max_y = maxy;
            min_y = miny;
        }
    private:
        void rotate_shape()
        {

            if(shape[0].y >= WALL_H - 1 || shape[1].y >= WALL_H - 1 ||
                     shape[2].y >= WALL_H - 1 || shape[3].y >= WALL_H - 1)
            {
                return;
            }


            print_date(PSPACE);
            int minx, miny, maxx, maxy, max_right = 0;
            get_min_point(minx, miny);
            get_max_point(maxx, maxy);
            int midx = (minx + maxx) / 2 + (minx + maxx) % 2;
            int midy = (miny + maxy) / 2 + (miny + maxy) % 2;
            for(int i = 0; i < 4; i++)
            {
                int tmp = shape[i].y - midy + midx;
                shape[i].y = midy + midx - shape[i].x;
                shape[i].x = tmp;
                if(shape[i].x > max_right) /* //防止右边过界 */
                    max_right = shape[i].x;
            }
            if(max_right > WALL_W-1)
            {
                shape[0].x -= max_right - WALL_W + 1;
                shape[1].x -= max_right - WALL_W + 1;
                shape[2].x -= max_right - WALL_W + 1;
                shape[3].x -= max_right - WALL_W + 1;
            }
            print_date(PBODY);
        }
        void print_date(const char *date)
        {
            for(int i = 0; i < 4; i++)
                printf("\033[%d;%dH%s", shape[i].y, shape[i].x, date);
        }
        void get_min_point(int &minx, int &miny)
        {
            minx = shape[0].x;
            miny = shape[0].y;
            for(int i = 1; i < 4; i++)
            {
                if(shape[i].x < minx)
                    minx = shape[i].x;
                if(shape[i].y < miny)
                    miny = shape[i].y;
            }
        }
        void get_max_point(int &minx, int &miny)
        {
            minx = shape[0].x;
            miny = shape[0].y;
            for(int i = 1; i < 4; i++)
            {
                if(shape[i].x > minx)
                    minx = shape[i].x;
                if(shape[i].y > miny)
                    miny = shape[i].y;
            }
        }
        void growth_direction(const int &index, int &conut)
        {
            bool flag = false;
            do
            {
                int direction = DIRECTION_UP + rand() % 4;
                shape[conut] = shape[index];
                flag = false;
                switch(direction)
                {
                    case DIRECTION_UP:
                        shape[conut].y--; break;
                    case DIRECTION_DOWN:
                        shape[conut].y++; break;
                    case DIRECTION_LEFT:
                        shape[conut].x--; break;
                    case DIRECTION_RIGHT:
                        shape[conut].x++; break;
                }
                for(int i = 0; i < conut; i++)
                {
                    if(shape[conut] == shape[i])
                    {
                        flag = true;
                        break;
                    }
                }
            }while(flag);
        }
        Point shape[4];
};

class Game_frame
{
    public:
        Game_frame()
        {
            game_state = 1;
            create_frame();
        }
        void Score_print()
        {
            printf("\033[10;%dHscore", WALL_W + 3);
            printf("\033[11;%dH%5d", WALL_W + 3, score);
            printf("\033[14;%dHlevel", WALL_W + 3);
            printf("\033[15;%dH\033[33;5m%5d\033[0m", WALL_W + 3, level);
            printf("\033[4;%dHnext", WALL_W + 3);
            next.print(5,WALL_W + 3);
            printf("\033[%d;%dH", WALL_H + 1, 0);
        }
        void move(int direction)
        {
            if(time != 1)
                return;
            if(shape.print(direction))
                update_game_frame();
            fflush(stdout);
        }
        bool reflash()
        {
            alarm(1);
            return 1 == time;
        }
        void change_state(int &state)
        {
            if(state == DIRECTION_RESTART)
            {
                game_state = 1;
                create_frame();
                return;
            }
            if(time == 1)
                time = 100000;
            else
                time = 1;
        }
        void Run()
        {
            printf("\033[2J");
            while(1)
            {
                Frame_print();
                Score_print();
                if(shape.print(DIRECTION_ONLY_PRINT))
                {
                    update_game_frame();
                }
                fflush(stdout);
                sleep(100);
            }
        }
        void Frame_print(bool flag = false)
        {
            printf("\033[0;0H");
            for(int i = 0; i < WALL_H; i++)
            {
                for(int j = 0; j < WALL_W+SCORE_W ; j++)
                {
                    switch(frame[i][j])
                    {
                        case WALL:
                            printf(PWALL); break;
                        case BODY:
                            printf(PBODY); break;
                        default:
                            printf(PSPACE); break;
                    }
                }
                cout << endl;
            }
                cout << "press ← → ↑ ↓ or p to control this game" << endl;
        }
        bool get_game_empty_h(const Point &point)
        {
            if(point.y >= WALL_H - 1)
                return true;
            if(point.y <= 0 || point.x <= 0)
                return false;
            else if(frame[point.y][point.x-1] == BODY || frame[point.y][point.x-1] == WALL)
                return true;
            return false;
        }
        int get_game_empty_h(const Point (&point)[4], const int &direction)
        {
            for(int i = 0; i < 4; i++)
            {
                if(point[i].y <= 0 || point[i].x <= 0)
                {
                    return 0;
                }
                if(point[i].y >= WALL_H - 1)
                    return 1;
                if(direction == DIRECTION_DOWN &&
                        (frame[point[i].y][point[i].x-1] == BODY || frame[point[i].y][point[i].x-1] == WALL))
                {
                    return 1;
                }
            }
            return 0;
        }
        bool get_game_empty_w(const Point (&point)[4], const int &direction)
        {
            if(direction != DIRECTION_LEFT && direction != DIRECTION_RIGHT)
            {
                return false;
            }
            for(int i = 0; i < 4; i++)
            {
                if(point[i].x <= 2 && direction == DIRECTION_LEFT)
                    break;
                else if(point[i].x > WALL_W - 1 && direction == DIRECTION_RIGHT)
                    break;
                if(direction == DIRECTION_LEFT && point[i].y > 0 &&
                        (frame[point[i].y-1][point[i].x-2] == BODY || frame[point[i].y-1][point[i].x-2] == WALL))
                {
                    return true;
                }
                else if(direction == DIRECTION_RIGHT && point[i].y > 0 &&
                        (frame[point[i].y-1][point[i].x] == BODY || frame[point[i].y-1][point[i].x] == WALL))
                {
                    return true;
                }
            }
            return false;
        }
    private:
        void create_frame()
        {
            if(game_state == 0)
                return;
            memset(frame, 0, sizeof(frame));
            level = 1;
            score = 0;
            time = 1;
            for(int i = 1; i < WALL_H - 1; i++)
            {
                frame[i][0] = frame[i][WALL_W] = frame[i][WALL_W+SCORE_W-1] = WALL;
            }
            for(int i = 0; i < WALL_W+SCORE_W; i++)
            {
                frame[0][i] = WALL;
                frame[WALL_H-1][i] = WALL;
            }
#if 0
            memset(&frame[WALL_H-2][1], BODY, WALL_W - 1);
            memset(&frame[WALL_H-3][1], BODY, WALL_W - 1);
            memset(&frame[WALL_H-4][1], BODY, WALL_W - 1);
            frame[WALL_H-4][3] = 0;
            frame[WALL_H-4][4] = 0;
            frame[WALL_H-3][3] = 0;
            frame[WALL_H-3][4] = 0;
            frame[WALL_H-2][3] = 0;
#endif
        }
        void update_game_frame()
        {
            bool flag = true;
            int count = 0;
            class Shape tmp;
            int miny, maxy;
            frame[shape[0].y-1][shape[0].x -1] = BODY;
            frame[shape[1].y-1][shape[1].x -1] = BODY;
            frame[shape[2].y-1][shape[2].x -1] = BODY;
            frame[shape[3].y-1][shape[3].x -1] = BODY;
            shape.get_max_min_y(maxy, miny);

            if(miny <= 1)
            {
                if(game_state)
                {
                    game_state = 0;
                    create_frame();
                }
                return;
            }

            shape = next;
            next = tmp;

            for(int i = miny - 1; i < maxy; i++)
            {
                flag = true;
                for(int j = 1; j < WALL_W - 1; j++)
                {
                    if(frame[i][j] != BODY)
                    {
                        flag = false;
                        break;
                    }
                }
                if(flag)
                {
                    count++;
                    for(int j = i; j > 2; j--)
                    {
                        memcpy(&frame[j][1], &frame[j-1][1], WALL_W - 1);
                    }
                }
            }
            if(count == 1)
                score += 1;
            else if(count == 2)
                score += 2;
            else if(count == 3)
                score += 4;
            else if(count == 4)
                score += 8;
            check_level();
            Frame_print();
            Score_print();
        }
        inline void check_level()
        {
            if(score > 100)
                level = 2;
        }
        signed char frame[WALL_H][WALL_W+SCORE_W];
        int score;
        int level;
        int time;
        class Shape shape;
        class Shape next;
        unsigned char game_state;
};


int Shape::print(int direction = DIRECTION_DOWN)
{
    if(gGame == NULL)
        return 0;

    int ret = gGame->get_game_empty_h(shape, direction);

    if(ret < 0)
    {
        return 0;
    }
    else if(ret > 0)
    {
        return 1;
    }

# if 0
    if(gGame->get_game_empty_h(shape[0]) || gGame->get_game_empty_h(shape[1]) ||
            gGame->get_game_empty_h(shape[2]) || gGame->get_game_empty_h(shape[3]))
    {
        return 1;
    }
#endif

    if(direction == DIRECTION_ONLY_PRINT)
    {
        print_date(PBODY);
        return 0;
    }

    if(gGame->get_game_empty_w(shape, direction))
    {
        print_date(PBODY);
        return 0;
    }

    switch(direction)
    {
        case DIRECTION_UP:
            rotate_shape();
            break;
        case DIRECTION_DOWN:
            print_date(PSPACE);
            shape[0].y++;
            shape[1].y++;
            shape[2].y++;
            shape[3].y++; break;
        case DIRECTION_LEFT:
            if(shape[0].x <= 2 || shape[1].x <= 2 || shape[2].x <= 2 || shape[3].x <= 2)
                return 0;
            print_date(PSPACE);
            shape[0].x--;
            shape[1].x--;
            shape[2].x--;
            shape[3].x--; break;
        case DIRECTION_RIGHT:
            if(shape[0].x > WALL_W-1 || shape[1].x > WALL_W-1 ||shape[2].x > WALL_W-1 || shape[3].x > WALL_W-1)
                return 0;
            print_date(PSPACE);
            shape[0].x++;
            shape[1].x++;
            shape[2].x++;
            shape[3].x++; break;
    }
    print_date(PBODY);
    for(int i = 0; i < 4; i++)
    {
        //    cout << "x = "<< shape[i].x << "y = " << shape[i].y << endl;
    }
    return 0;
}




    int ch;

/*键盘控制*/
    struct termios old;

int getch (void)
{
    ioctl (STDIN_FILENO, TCGETS, &old);
    struct termios new1 = old;
    new1.c_lflag &= ~(ECHO | ICANON);
    ioctl (STDIN_FILENO, TCSETS, &new1);
    int ch = getchar (); 
    ioctl (STDIN_FILENO, TCSETS, &old);
    return ch; 
}


void* tetris_move_move(void *arg)
{
    while(1)
    {   
        pthread_mutex_lock(&mut);
        ch=getch();
        if(strchr("Qq",ch))
        {   
            int kill(pid_t pid, int sig);
            kill(getpid(), SIGINT);
            break;
        }   
        switch(ch)
        {
            case DIRECTION_UP:
            case DIRECTION_DOWN:
            case DIRECTION_LEFT:
            case DIRECTION_RIGHT:
                gGame->move(ch);
                break;
            case DIRECTION_PAUSE:
            case DIRECTION_RESTART:
                gGame->change_state(ch);
                break;
        }
        pthread_mutex_unlock(&mut);
    }   
    pthread_exit(NULL);
}

void handler_func(int sig)
{
    if(sig == SIGTERM || sig == SIGINT)
    {
        ioctl (STDIN_FILENO, TCSETS, &old);
        printf("\033[2J\033[?25h\033[0;0H");
        exit(0);
    }
    if(gGame->reflash())
        gGame->move(DIRECTION_DOWN);
}

int main(int argc, char *argv[])
{
    pthread_t thread;
    pthread_create(&thread, NULL, tetris_move_move, NULL);

    printf("\033[?25l");
    srand(time(NULL));
    class Game_frame b;
    gGame = &b;

    signal(SIGALRM, handler_func);// ctrl+c
    signal(SIGUSR1, handler_func);// ctrl+c
    signal(SIGTERM, handler_func);// ctrl+c
    signal(SIGINT, handler_func);// ctrl+c
    alarm(1);

    b.Run();
    printf("\033[?25h\n");
    return 0;
}
