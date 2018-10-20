/*
 * Copyright (C), 2010-2017, ATCOM Tech. Co., Ltd.
 *
 * File Name: node.cpp
 * Author: liaoxuecheng
 * mail: liaoxuecheng@atcom.com.cn
 * Created Time: Fri 21 Sep 2018 11:44:44 AM CST
 * ▼◆☀■△□█◇
 */

#include<iostream>
#include<memory>
#include<algorithm>
#include<stdio.h>
#include<pthread.h>
#include <termios.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>
 #include <signal.h>
#include <stdlib.h>

#include"node.h"

using std::cin;
using std::cout;
using std::endl;

class Node;
class Queue;

int game[WALL_SIZE][WALL_SIZE]={SPACE};
pthread_t snake_thread;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
char ch=SNAKE_RIGHT;

class Stay
{
    friend class Node;
    friend class Queue;
    friend bool operator==(const Stay &a, const Stay &b)
    {
        return a.x == b.x && a.y == b.y;
    }

    public:
        Stay()
        {
//            cout << "Stay" << endl;
        }
        void set(int x, int y)
        {
            this->x = x;
            this->y = y;
        }
        void set(const Stay& stay)
        {
            x = stay.x;
            y = stay.y;
        }
        void get(int &x, int &y)
        {
            x = Stay::x;
            y = Stay::y;
        }
        void CreateHead()
        {
            x = START_HEAD_X;
            y = START_HEAD_Y;
            game[x][y] = SNAKE_HEAD;
        }
        void CreateApple()
        {
            while(1)
            {
                x=rand()%SPACE_SIZE+1;//随机生成苹果坐标
                y=rand()%SPACE_SIZE+1;
                if(game[x][y]==SPACE)
                    break;
            }
            game[x][y]=3;//将成功生成苹果的位置设置为苹果
            x = x;
            y = y;
        }
        Stay& CreateTail()
        {
            game[x][y]=SNAKE_BODY;
            return *this;
        }
        void remove_last()
        {
            game[x][y]=SPACE;
        }
        void MoveTail()
        {
            game[x][y]=SNAKE_BODY;//将此位置设为蛇身
        }
        int MoveHead(char ch)
        {
            static char last_ch;
            game[x][y]=SPACE;//将旧的蛇头位置清零
            if(ch != SNAKE_UP && ch != SNAKE_DOWN && ch != SNAKE_LEFT && ch != SNAKE_RIGHT)
            {
                ch = last_ch;
            }
            switch(ch)
            {   
                case SNAKE_UP:x--;//向上移动
                              break;
                case SNAKE_DOWN:x++;//向下移动
                                break;
                case SNAKE_LEFT:y--;//向左移动
                                break;
                case SNAKE_RIGHT:y++;//向右移动
                                 break;
                default:break;
            }   
            last_ch = ch;
            if(game[x][y]!=SPACE && game[x][y]!=APPLE)//如果蛇头的位置既不为空又不是苹果位置，说明蛇头撞墙或是>咬到了自己
            {   
                return -1;//返回失败状态
            }   
            game[x][y]=SNAKE_HEAD;//将新位置设置为蛇头
            return 0;
        }
    protected:
        int x;
        int y;
};

class Node:protected Stay
{
    friend class Queue;
    public:
        Node(const class Stay &a)
        {
            x = a.x;
            y = a.y;
        }
        void print()
        {
            cout << "(x, y) = (" << x << "," << y << ")" << endl;
        }

    private:
    class Node *next;
};

class Queue
{
    public:
        Queue(): head(NULL), tail(NULL), size(0){}
        ~Queue()
        {
            class Node *tmp;
            tmp = head;
            while(tmp)
            {
                delete tmp;
                tmp = tmp->next;
            }
            head = NULL;
            tail = NULL;
            size = 0;
        }
        void first()
        {
        }
        void last()
        {
        }
        void enqueue(const class Stay &data)
        {
            class Node *node = new class Node(data);
            node->next = NULL;
            if(head == NULL)
            {
                head = node;
            }
            else
            {
                tail->next=node;
            }
            tail = node;
            size++;
        }
        void enqueue(class Node * &node)
        {
            node->next = NULL;
            if(head == NULL)
                head = node;
            else
                tail->next=node;
            tail = node;
        }
        void dequeue(class Stay &data)
        {
            class Node *node = head;
            if(head)
            {
                data.x = head->x;
                data.y = head->y;
                head = head->next;
                delete node;
                if(head == NULL)
                {
                    tail = NULL;
                }
            }
            size--;
        }
        void reset_tail_and_head(const class Stay &data)
        {
            class Node *node = head;
            node->next = NULL;
            head = head->next;
            if(head)
            {
                head = head->next;
                if(head == NULL)
                {
                    tail = NULL;
                }
            }
            game[node->x][node->y]=SPACE;
            node->x = data.x;
            node->y = data.y;
            enqueue(node);
            game[data.x][data.y]=SNAKE_BODY;//将此位置设为蛇身
            
        }
        bool isEmpty()
        {
            return !size;
        }
        size_t length()
        {
            return size;
        }
    private:
        class Node *head;
        class Node *tail;
        size_t size;
};

/*键盘控制*/
int getch (void)
{
    struct termios old;
    ioctl (STDIN_FILENO, TCGETS, &old);
    struct termios new1 = old;
    new1.c_lflag &= ~(ECHO | ICANON);
    ioctl (STDIN_FILENO, TCSETS, &new1);
    int ch = getchar ();
    ioctl (STDIN_FILENO, TCSETS, &old);
    return ch;
}


void* snake_move(void *arg)
{
    while(1)
    {   
        pthread_mutex_lock(&mut);
        ch=getch();
        if(strchr("Qq",ch))
        {   
            break;
        }   
        ch=getch();
        ch=getch();
        pthread_mutex_unlock(&mut);
    }   
    pthread_exit(NULL);
}

void show(class Queue &queue)
{
    int i,j, flag = 1;
    for(i=0;i<WALL_SIZE;++i)
    {
        for(j=0;j<WALL_SIZE;++j)
        {
            if(i==0 || j==0 || i== WALL_SIZE-1 || j==WALL_SIZE-1)
            {
                game[i][j]=WALL;
                printf(SHOW_WALL);
            }
            else if(game[i][j]==SNAKE_HEAD)printf(SHOW_HEAD);
            else if(game[i][j]==SNAKE_BODY)printf(SHOW_BODY);
            else if(game[i][j]==APPLE)printf(SHOW_APPLE);
            else printf(SHOW_SPACE);
        }

        if(flag && i == WALL_SIZE / 2)
        {
            while(SCORE_X - j > 0)
            {
                j++;
                printf(SHOW_SPACE);
            }
            flag = 0;
            printf("score:%d", queue.length());
        }
        printf("\n");
    }
    char cmd[1204];
    printf("enter q to exit\n");
    sprintf(cmd, "cat /proc/%d/status | grep Vm | xargs", getpid());
    //system(cmd);
}

class Queue queue;
class Stay snake_head,last_head,apple,last_tail;

void handler_func(int arg)
{
    return;
}

int main(int argc, char *argv[])
{
    srand(time(0));//种下伪随机数种子
    snake_head.CreateHead();
    apple.CreateApple();//初始化苹果位置

    signal(SIGINT, handler_func);// ctrl+c

    pthread_create(&snake_thread,NULL,snake_move,NULL);//开启键盘操作线程

    printf("\033[2J\033[?25l");
    while(1)
    {
        if(snake_head == apple)
        {
            apple.CreateApple();
            queue.enqueue(last_head.CreateTail());
        }
        else if(!queue.isEmpty())
        {
#if 0
            queue.reset_tail_and_head(last_head);
#else
            queue.dequeue(last_tail);
            last_tail.remove_last();
            queue.enqueue(last_head);
            last_head.MoveTail();
#endif
        }
        printf("\033[0;0H");
        show(queue);
        usleep(TURN_NEXT);
        if(strchr("Qq",ch))
        {
            break;
        }

        last_head.set(snake_head);
        if(snake_head.MoveHead(ch))
        {
            printf("Game over!\nPush \'Q\' to quit\n");
            break;
        }
    }

    printf("\033[?25h");
    pthread_join(snake_thread,NULL);
    return 0;
}
