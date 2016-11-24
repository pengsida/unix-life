//
//  main.c
//
//  Created by pengsida on 2016/11/20.
//  Copyright © 2016年 pengsida. All rights reserved.
//
//

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<sys/time.h>
#include <errno.h>

#define MAX 1000

#define NORTH 1
#define EAST 2
#define WEST 3
#define SOUTH 4

#define CPL0 0
#define CPL1 0

int remaining_sources;
int direction;
size_t len;
int north_num;
int east_num;
int west_num;
int south_num;

pthread_mutex_t manager_lock;
pthread_mutex_t count_lock;
pthread_mutex_t remaining_lock;

pthread_mutex_t source_a;
pthread_mutex_t source_b;
pthread_mutex_t source_c;
pthread_mutex_t source_d;

pthread_mutex_t first_north_lock;
pthread_mutex_t first_east_lock;
pthread_mutex_t first_west_lock;
pthread_mutex_t first_south_lock;

pthread_mutex_t* north_order_lock;
pthread_mutex_t* east_order_lock;
pthread_mutex_t* west_order_lock;
pthread_mutex_t* south_order_lock;

pthread_mutex_t north_manager_lock;
pthread_mutex_t east_manager_lock;
pthread_mutex_t west_manager_lock;
pthread_mutex_t south_manager_lock;

pthread_mutex_t a_queue_lock;
pthread_mutex_t b_queue_lock;
pthread_mutex_t c_queue_lock;
pthread_mutex_t d_queue_lock;

pthread_cond_t manager_lock_cond;
pthread_cond_t manager_unlock_cond;

pthread_cond_t* north_order;
pthread_cond_t* east_order;
pthread_cond_t* west_order;
pthread_cond_t* south_order;

pthread_cond_t first_north;
pthread_cond_t first_east;
pthread_cond_t first_west;
pthread_cond_t first_south;

pthread_cond_t north_wait_manager;
pthread_cond_t east_wait_manager;
pthread_cond_t west_wait_manager;
pthread_cond_t south_wait_manager;

pthread_cond_t north_manager_cond;
pthread_cond_t east_manager_cond;
pthread_cond_t west_manager_cond;
pthread_cond_t south_manager_cond;

pthread_cond_t request_source_a;
pthread_cond_t request_source_b;
pthread_cond_t request_source_c;
pthread_cond_t request_source_d;

pthread_cond_t south_wait_a;
pthread_cond_t west_wait_a;
pthread_cond_t east_wait_b;
pthread_cond_t south_wait_b;
pthread_cond_t north_wait_c;
pthread_cond_t east_wait_c;
pthread_cond_t west_wait_d;
pthread_cond_t north_wait_d;

struct car_num
{
    int total_num;
    int direction_num;
};

typedef struct car_num car_num;

struct request_queue
{
    int directions[2];
    int len;
};

typedef struct request_queue request_queue;

request_queue request_a_queue;
request_queue request_b_queue;
request_queue request_c_queue;
request_queue request_d_queue;

void sleep(int seconds)
{
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_t cond;
    pthread_cond_init(&cond, NULL);
    struct timeval now;
    struct timespec outtime;
    gettimeofday(&now, NULL);
    outtime.tv_sec = now.tv_sec + seconds;
    outtime.tv_nsec = now.tv_usec * 1000;
    pthread_mutex_lock(&mutex);
    pthread_cond_timedwait(&cond, &mutex, &outtime);
    pthread_mutex_unlock(&mutex);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}

void err_exit()
{
    printf("somthing wrong!!!\n");
    exit(1);
}

void * from_north(void *arg)
{
    car_num num_struct = *(car_num*)arg;
    int num = num_struct.total_num;
    int north_num = num_struct.direction_num;
    
    pthread_mutex_lock(north_order_lock+north_num);
    pthread_cond_wait(north_order+north_num, north_order_lock+north_num);
    pthread_mutex_unlock(north_order_lock+north_num);
    
    pthread_mutex_lock(&c_queue_lock);
    request_c_queue.directions[request_c_queue.len] = NORTH;
    ++request_c_queue.len;
    pthread_cond_signal(&request_source_c);
    pthread_cond_wait(&north_wait_c, &c_queue_lock);
    pthread_mutex_unlock(&c_queue_lock);
    
    pthread_mutex_lock(&source_c);
    printf("car %d from North arrives at crossing\n", num);
    
    --remaining_sources;
    if(remaining_sources == 0)
    {
        direction = NORTH;
        pthread_cond_signal(&manager_lock_cond);
        pthread_mutex_unlock(&source_c);
        pthread_cond_wait(&north_wait_manager, &manager_lock);
        pthread_mutex_lock(&source_c);
    }
    
    pthread_mutex_lock(&d_queue_lock);
    request_d_queue.directions[request_d_queue.len] = NORTH;
    ++request_d_queue.len;
    pthread_cond_signal(&request_source_d);
    pthread_cond_wait(&north_wait_d, &d_queue_lock);
    pthread_mutex_unlock(&d_queue_lock);
    
    pthread_mutex_lock(&source_d);
    sleep(1);
    pthread_mutex_unlock(&source_c);
    
    pthread_mutex_lock(&c_queue_lock);
    if(request_c_queue.len != 0)
        pthread_cond_signal(&request_source_c);
    pthread_mutex_unlock(&c_queue_lock);
    
//    pthread_mutex_lock(&north_manager_lock);
    pthread_mutex_lock(north_order_lock+north_num);
    pthread_mutex_unlock(north_order_lock+north_num);
    pthread_cond_signal(&north_manager_cond);
//    pthread_mutex_unlock(&north_manager_lock);
    
    if(remaining_sources == 0)
        pthread_cond_signal(&manager_unlock_cond);
    
    printf("car %d from North leaving crossing\n", num);
    ++remaining_sources;
    pthread_mutex_unlock(&source_d);
    
    pthread_mutex_lock(&d_queue_lock);
    if(request_d_queue.len != 0)
        pthread_cond_signal(&request_source_d);
    pthread_mutex_unlock(&d_queue_lock);
    
    return (void*)0;
}

void * from_west(void *arg)
{
    car_num num_struct = *(car_num*)arg;
    int num = num_struct.total_num;
    int west_num = num_struct.direction_num;
    
    pthread_mutex_lock(west_order_lock+west_num);
    pthread_cond_wait(west_order+west_num, west_order_lock+west_num);
    pthread_mutex_unlock(west_order_lock+west_num);
    
    pthread_mutex_lock(&d_queue_lock);
    request_d_queue.directions[request_d_queue.len] = WEST;
    ++request_d_queue.len;
    pthread_cond_signal(&request_source_d);
    pthread_cond_wait(&west_wait_d, &d_queue_lock);
    pthread_mutex_unlock(&d_queue_lock);
    
    pthread_mutex_lock(&source_d);
    printf("car %d from West arrives at crossing\n", num);
    
    // pthread_mutex_lock(&remaining_sources_lock);
    --remaining_sources;
    if(remaining_sources == 0)
    {
        direction = WEST;
        pthread_cond_signal(&manager_lock_cond);
        pthread_mutex_unlock(&source_d);
        pthread_cond_wait(&west_wait_manager, &manager_lock);
        pthread_mutex_lock(&source_d);
    }
    
    pthread_mutex_lock(&a_queue_lock);
    request_a_queue.directions[request_a_queue.len] = WEST;
    ++request_a_queue.len;
    pthread_cond_signal(&request_source_a);
    pthread_cond_wait(&west_wait_a, &a_queue_lock);
    pthread_mutex_unlock(&a_queue_lock);
    
    pthread_mutex_lock(&source_a);
    sleep(1);
    pthread_mutex_unlock(&source_d);
    
    pthread_mutex_lock(&d_queue_lock);
    if(request_d_queue.len != 0)
        pthread_cond_signal(&request_source_d);
    pthread_mutex_unlock(&d_queue_lock);
    
//    pthread_mutex_lock(&west_manager_lock);
    pthread_mutex_lock(west_order_lock+west_num);
    pthread_mutex_unlock(west_order_lock+west_num);
    pthread_cond_signal(&west_manager_cond);
//    pthread_mutex_unlock(&west_manager_lock);
    
    if(remaining_sources == 0)
        pthread_cond_signal(&manager_unlock_cond);
    
    printf("car %d from West leaving crossing\n", num);
    ++remaining_sources;
    pthread_mutex_unlock(&source_a);
    
    pthread_mutex_lock(&a_queue_lock);
    if(request_a_queue.len != 0)
        pthread_cond_signal(&request_source_a);
    pthread_mutex_unlock(&a_queue_lock);
    
    return (void*)0;
}

void * from_south(void *arg)
{
    car_num num_struct = *(car_num*)arg;
    int num = num_struct.total_num;
    int south_num = num_struct.direction_num;
    
    pthread_mutex_lock(south_order_lock+south_num);
    pthread_cond_wait(south_order+south_num, south_order_lock+south_num);
    pthread_mutex_unlock(south_order_lock+south_num);
    
    pthread_mutex_lock(&a_queue_lock);
    request_a_queue.directions[request_a_queue.len] = SOUTH;
    ++request_a_queue.len;
    pthread_cond_signal(&request_source_a);
    pthread_cond_wait(&south_wait_a, &a_queue_lock);
    pthread_mutex_unlock(&a_queue_lock);
    
    pthread_mutex_lock(&source_a);
    printf("car %d from South arrives at crossing\n", num);
    
    --remaining_sources;
    if(remaining_sources == 0)
    {
        direction = SOUTH;
        pthread_cond_signal(&manager_lock_cond);
        pthread_mutex_unlock(&source_a);
        pthread_cond_wait(&south_wait_manager, &manager_lock);
        pthread_mutex_lock(&source_a);
    }
    
    pthread_mutex_lock(&b_queue_lock);
    request_b_queue.directions[request_b_queue.len] = SOUTH;
    ++request_b_queue.len;
    pthread_cond_signal(&request_source_b);
    pthread_cond_wait(&south_wait_b, &b_queue_lock);
    pthread_mutex_unlock(&b_queue_lock);
    
    pthread_mutex_lock(&source_b);
    sleep(1);
    pthread_mutex_unlock(&source_a);
    
    pthread_mutex_lock(&a_queue_lock);
    if(request_a_queue.len != 0)
        pthread_cond_signal(&request_source_a);
    pthread_mutex_unlock(&a_queue_lock);
    
//    pthread_mutex_lock(&south_manager_lock);
    pthread_mutex_lock(south_order_lock+south_num);
    pthread_mutex_unlock(south_order_lock+south_num);
    pthread_cond_signal(&south_manager_cond);
//    pthread_mutex_unlock(&south_manager_lock);
    
    if(remaining_sources == 0)
        pthread_cond_signal(&manager_unlock_cond);
    
    printf("car %d from South leaving crossing\n", num);
    ++remaining_sources;
    pthread_mutex_unlock(&source_b);
    
    pthread_mutex_lock(&b_queue_lock);
    if(request_b_queue.len != 0)
        pthread_cond_signal(&request_source_b);
    pthread_mutex_unlock(&b_queue_lock);
    
    return (void*)0;
}

void * from_east(void *arg)
{
    car_num num_struct = *(car_num*)arg;
    int num = num_struct.total_num;
    int east_num = num_struct.direction_num;
    
    pthread_mutex_lock(east_order_lock+east_num);
    pthread_cond_wait(east_order+east_num, east_order_lock+east_num);
    pthread_mutex_unlock(east_order_lock+east_num);
    
    pthread_mutex_lock(&b_queue_lock);
    request_b_queue.directions[request_b_queue.len] = EAST;
    ++request_b_queue.len;
    pthread_cond_signal(&request_source_b);
    pthread_cond_wait(&east_wait_b, &b_queue_lock);
    pthread_mutex_unlock(&b_queue_lock);
    
    pthread_mutex_lock(&source_b);
    printf("car %d from East arrives at crossing\n", num);
    
    --remaining_sources;
    if(remaining_sources == 0)
    {
        direction = EAST;
        pthread_cond_signal(&manager_lock_cond);
        pthread_mutex_unlock(&source_b);
        pthread_cond_wait(&east_wait_manager, &manager_lock);
        pthread_mutex_lock(&source_b);
    }
    
    pthread_mutex_lock(&c_queue_lock);
    request_c_queue.directions[request_c_queue.len] = EAST;
    ++request_c_queue.len;
    pthread_cond_signal(&request_source_c);
    pthread_cond_wait(&east_wait_c, &c_queue_lock);
    pthread_mutex_unlock(&c_queue_lock);
    
    pthread_mutex_lock(&source_c);
    sleep(1);
    pthread_mutex_unlock(&source_b);
    
    pthread_mutex_lock(&b_queue_lock);
    if(request_b_queue.len != 0)
        pthread_cond_signal(&request_source_b);
    pthread_mutex_unlock(&b_queue_lock);
    
//    pthread_mutex_lock(&east_manager_lock);
    pthread_mutex_lock(east_order_lock+east_num);
    pthread_mutex_unlock(east_order_lock+east_num);
    pthread_cond_signal(&east_manager_cond);
//    pthread_mutex_unlock(&east_manager_lock);
    
    if(remaining_sources == 0)
        pthread_cond_signal(&manager_unlock_cond);
    
    printf("car %d from East leaving crossing\n", num);
    ++remaining_sources;
    pthread_mutex_unlock(&source_c);
    
    pthread_mutex_lock(&c_queue_lock);
    if(request_c_queue.len != 0)
        pthread_cond_signal(&request_source_c);
    pthread_mutex_unlock(&c_queue_lock);
    
    return (void*)0;
}

void * source_a_manager(void *arg)
{
    pthread_mutex_t lock;
    int south_level = CPL0;
    int west_level = CPL1;
    
    pthread_mutex_init(&lock, NULL);
    pthread_cond_signal(&south_wait_a);
    request_a_queue.len = 0;
    while(1)
    {
        pthread_mutex_lock(&lock);
        pthread_cond_wait(&request_source_a, &lock);
        
        pthread_mutex_lock(&a_queue_lock);
        if(request_a_queue.len == 1)
        {
            --request_a_queue.len;
            south_level = CPL0;
            west_level = CPL1;
            switch(request_a_queue.directions[0])
            {
                case SOUTH:
                    pthread_cond_signal(&south_wait_a);
                    break;
                case WEST:
                    pthread_cond_signal(&west_wait_a);
                    break;
                default:
                    err_exit();
                    break;
            }
        }
        else if(request_a_queue.len == 2)
        {
            --request_a_queue.len;
            if(south_level == CPL0)
            {
                south_level = CPL1;
                west_level = CPL0;
                request_a_queue.directions[0] = WEST;
                pthread_cond_signal(&south_wait_a);
            }
            else
            {
                south_level = CPL0;
                west_level = CPL1;
                request_a_queue.directions[0] = SOUTH;
                pthread_cond_signal(&west_wait_a);
            }
        }
        else
            err_exit();
        pthread_mutex_unlock(&a_queue_lock);
        
        pthread_mutex_unlock(&lock);
    }
    return (void*)0;
}

void * source_b_manager(void *arg)
{
    pthread_mutex_t lock;
    int east_level = CPL0;
    int south_level = CPL1;
    
    pthread_mutex_init(&lock, NULL);
    pthread_cond_signal(&east_wait_b);
    request_b_queue.len = 0;
    while(1)
    {
        pthread_mutex_lock(&lock);
        pthread_cond_wait(&request_source_b, &lock);
        
        pthread_mutex_lock(&b_queue_lock);
        if(request_b_queue.len == 1)
        {
            --request_b_queue.len;
            east_level = CPL0;
            south_level = CPL1;
            switch(request_b_queue.directions[0])
            {
                case SOUTH:
                    pthread_cond_signal(&south_wait_b);
                    break;
                case EAST:
                    pthread_cond_signal(&east_wait_b);
                    break;
                default:
                    err_exit();
                    break;
            }
        }
        else if(request_b_queue.len == 2)
        {
            --request_b_queue.len;
            if(east_level == CPL0)
            {
                east_level = CPL1;
                south_level = CPL0;
                request_b_queue.directions[0] = SOUTH;
                pthread_cond_signal(&east_wait_b);
            }
            else
            {
                east_level = CPL0;
                south_level = CPL1;
                request_b_queue.directions[0] = EAST;
                pthread_cond_signal(&south_wait_b);
            }
        }
        else
            err_exit();
        pthread_mutex_unlock(&b_queue_lock);
        
        pthread_mutex_unlock(&lock);
    }
    return (void*)0;
}

void * source_c_manager(void *arg)
{
    pthread_mutex_t lock;
    int north_level = CPL0;
    int east_level = CPL1;
    
    pthread_mutex_init(&lock, NULL);
    pthread_cond_signal(&north_wait_c);
    request_c_queue.len = 0;
    while(1)
    {
        pthread_mutex_lock(&lock);
        pthread_cond_wait(&request_source_c, &lock);
        
        pthread_mutex_lock(&c_queue_lock);
        if(request_c_queue.len == 1)
        {
            --request_c_queue.len;
            north_level = CPL0;
            east_level = CPL1;
            switch(request_c_queue.directions[0])
            {
                case EAST:
                    pthread_cond_signal(&east_wait_c);
                    break;
                case NORTH:
                    pthread_cond_signal(&north_wait_c);
                    break;
                default:
                    err_exit();
                    break;
            }
        }
        else if(request_c_queue.len == 2)
        {
            --request_c_queue.len;
            if(north_level == CPL0)
            {
                north_level = CPL1;
                east_level = CPL0;
                request_c_queue.directions[0] = EAST;
                pthread_cond_signal(&north_wait_c);
            }
            else
            {
                north_level = CPL0;
                east_level = CPL1;
                request_c_queue.directions[0] = NORTH;
                pthread_cond_signal(&east_wait_c);
            }
        }
        else
            err_exit();
        pthread_mutex_unlock(&c_queue_lock);
        
        pthread_mutex_unlock(&lock);
    }
    return (void*)0;
}

void * source_d_manager(void *arg)
{
    pthread_mutex_t lock;
    int west_level = CPL0;
    int north_level = CPL1;
    
    pthread_mutex_init(&lock, NULL);
    pthread_cond_signal(&west_wait_d);
    request_d_queue.len = 0;
    while(1)
    {
        pthread_mutex_lock(&lock);
        pthread_cond_wait(&request_source_d, &lock);
        
        pthread_mutex_lock(&d_queue_lock);
        if(request_d_queue.len == 1)
        {
            --request_d_queue.len;
            west_level = CPL0;
            north_level = CPL1;
            switch (request_d_queue.directions[0])
            {
                case NORTH:
                    pthread_cond_signal(&north_wait_d);
                    break;
                case WEST:
                    pthread_cond_signal(&west_wait_d);
                    break;
                default:
                    err_exit();
                    break;
            }
        }
        else if(request_d_queue.len == 2)
        {
            --request_d_queue.len;
            if(west_level == CPL0)
            {
                west_level = CPL0;
                north_level = CPL1;
                request_d_queue.directions[0] = NORTH;
                pthread_cond_signal(&west_wait_d);
            }
            else
            {
                west_level = CPL1;
                north_level = CPL0;
                request_d_queue.directions[0] = WEST;
                pthread_cond_signal(&north_wait_d);
            }
        }
        else
            err_exit();
        pthread_mutex_unlock(&d_queue_lock);
        
        pthread_mutex_unlock(&lock);
    }
    return (void*)0;
}

void * north_manager(void *arg)
{
    int i;
    sleep(1);
    for(i = 0; i < north_num; ++i)
    {
//        pthread_mutex_lock(&north_manager_lock);
        pthread_mutex_lock(north_order_lock+i);
        pthread_mutex_unlock(north_order_lock+i);
        pthread_cond_signal(north_order+i);
        
        pthread_mutex_lock(north_order_lock+i);
        pthread_cond_wait(&north_manager_cond, north_order_lock+i);
        pthread_mutex_unlock(north_order_lock+i);
//        pthread_mutex_unlock(&north_manager_lock);
    }
    
    return (void*)0;
}

void * east_manager(void *arg)
{
    int i;
    sleep(1);
    for(i = 0; i < east_num; ++i)
    {
//        pthread_mutex_lock(&east_manager_lock);
        pthread_mutex_lock(east_order_lock+i);
        pthread_mutex_unlock(east_order_lock+i);
        pthread_cond_signal(east_order+i);
        
        pthread_mutex_lock(east_order_lock+i);
        pthread_cond_wait(&east_manager_cond, east_order_lock+i);
        pthread_mutex_unlock(east_order_lock+i);
//        pthread_mutex_unlock(&east_manager_lock);
    }
    
    return (void*)0;
}

void * west_manager(void *arg)
{
    int i;
    sleep(1);
    for(i = 0; i < west_num; ++i)
    {
//        pthread_mutex_lock(&west_manager_lock);
        pthread_mutex_lock(west_order_lock+i);
        pthread_mutex_unlock(west_order_lock+i);
        pthread_cond_signal(west_order+i);
        
        pthread_mutex_lock(west_order_lock+i);
        pthread_cond_wait(&west_manager_cond, west_order_lock+i);
        pthread_mutex_unlock(west_order_lock+i);
//        pthread_mutex_unlock(&west_manager_lock);
    }
    
    return (void*)0;
}

void * south_manager(void *arg)
{
    int i;
    sleep(1);
    for(i = 0; i < south_num; ++i)
    {
//        pthread_mutex_lock(&south_manager_lock);
        pthread_mutex_lock(south_order_lock+i);
        pthread_mutex_unlock(south_order_lock+i);
        pthread_cond_signal(south_order+i);
        
        pthread_mutex_lock(south_order_lock+i);
        pthread_cond_wait(&south_manager_cond, south_order_lock+i);
        pthread_mutex_unlock(south_order_lock+i);
//        pthread_mutex_unlock(&south_manager_lock);
    }
    
    return (void*)0;
}

void * manager(void *arg)
{
    while(1)
    {
        pthread_mutex_lock(&manager_lock);
        pthread_cond_wait(&manager_lock_cond, &manager_lock);
        switch(direction)
        {
            case NORTH:
                printf("DEADLOCK: car jam detected, signalling East to go\n");
                break;
            case EAST:
                printf("DEADLOCK: car jam detected, signalling South to go\n");
                break;
            case WEST:
                printf("DEADLOCK: car jam detected, signalling North to go\n");
                break;
            case SOUTH:
                printf("DEADLOCK: car jam detected, signalling West to go\n");
                break;
            default:
                err_exit();
                break;
        }
        pthread_cond_wait(&manager_unlock_cond, &manager_lock);
        switch(direction)
        {
            case NORTH:
                pthread_cond_signal(&north_wait_manager);
                break;
            case EAST:
                pthread_cond_signal(&east_wait_manager);
                break;
            case WEST:
                pthread_cond_signal(&west_wait_manager);
                break;
            case SOUTH:
                pthread_cond_signal(&south_wait_manager);
                break;
            default:
                err_exit();
                break;
        }
        pthread_mutex_unlock(&manager_lock);
    }
    return (void*)0;
}

int main(int argc, char ** argv)
{
    int i = 0;
    int err;
    pthread_t manager_tid;
    pthread_t north_manager_tid;
    pthread_t east_manager_tid;
    pthread_t west_manager_tid;
    pthread_t south_manager_tid;
    pthread_t source_a_manager_tid;
    pthread_t source_b_manager_tid;
    pthread_t source_c_manager_tid;
    pthread_t source_d_manager_tid;
    len = strlen(argv[1]);
    
    north_num = 0;
    east_num = 0;
    west_num = 0;
    south_num = 0;
    
    request_a_queue.len = 0;
    request_a_queue.directions[0] = 0;
    request_a_queue.directions[1] = 0;
    request_b_queue.len = 0;
    request_b_queue.directions[0] = 0;
    request_b_queue.directions[1] = 0;
    request_c_queue.len = 0;
    request_c_queue.directions[0] = 0;
    request_c_queue.directions[1] = 0;
    request_d_queue.len = 0;
    request_d_queue.directions[0] = 0;
    request_d_queue.directions[1] = 0;
    
    char* cars = (char*)malloc((len+1) * sizeof(char));
    pthread_t * tids = (pthread_t*)malloc((len+1) * sizeof(pthread_t));
//    int* nums = (int*)malloc(len * sizeof(int));
    car_num* car_nums = (car_num*)malloc(len * sizeof(car_num));
    
    remaining_sources = 4;
    
    pthread_mutex_init(&manager_lock, NULL);
    pthread_mutex_init(&count_lock, NULL);
    pthread_mutex_init(&remaining_lock, NULL);
    
    pthread_mutex_init(&source_a, NULL);
    pthread_mutex_init(&source_b, NULL);
    pthread_mutex_init(&source_c, NULL);
    pthread_mutex_init(&source_d, NULL);
    
    pthread_mutex_init(&first_north_lock, NULL);
    pthread_mutex_init(&first_east_lock, NULL);
    pthread_mutex_init(&first_west_lock, NULL);
    pthread_mutex_init(&first_south_lock, NULL);
    
    pthread_mutex_init(&north_manager_lock, NULL);
    pthread_mutex_init(&east_manager_lock, NULL);
    pthread_mutex_init(&west_manager_lock, NULL);
    pthread_mutex_init(&south_manager_lock, NULL);
    
    pthread_mutex_init(&a_queue_lock, NULL);
    pthread_mutex_init(&b_queue_lock, NULL);
    pthread_mutex_init(&c_queue_lock, NULL);
    pthread_mutex_init(&d_queue_lock, NULL);
    
    pthread_cond_init(&manager_lock_cond, NULL);
    pthread_cond_init(&manager_unlock_cond, NULL);
    pthread_cond_init(&north_wait_manager, NULL);
    pthread_cond_init(&east_wait_manager, NULL);
    pthread_cond_init(&west_wait_manager, NULL);
    pthread_cond_init(&south_wait_manager, NULL);
    
    pthread_cond_init(&first_north, NULL);
    pthread_cond_init(&first_east, NULL);
    pthread_cond_init(&first_south, NULL);
    pthread_cond_init(&first_west, NULL);
    
    pthread_cond_init(&north_manager_cond, NULL);
    pthread_cond_init(&east_manager_cond, NULL);
    pthread_cond_init(&west_manager_cond, NULL);
    pthread_cond_init(&south_manager_cond, NULL);
    
    pthread_cond_init(&request_source_a, NULL);
    pthread_cond_init(&request_source_b, NULL);
    pthread_cond_init(&request_source_c, NULL);
    pthread_cond_init(&request_source_d, NULL);
    
    pthread_cond_init(&south_wait_a, NULL);
    pthread_cond_init(&south_wait_b, NULL);
    pthread_cond_init(&east_wait_b, NULL);
    pthread_cond_init(&east_wait_c, NULL);
    pthread_cond_init(&north_wait_c, NULL);
    pthread_cond_init(&north_wait_d, NULL);
    pthread_cond_init(&west_wait_d, NULL);
    pthread_cond_init(&west_wait_a, NULL);
    
    for(i = 0; i < len; ++i)
    {
        cars[i] = argv[1][i];
//        nums[i] = i + 1;
        car_nums[i].total_num = i + 1;
        switch(cars[i])
        {
            case 'n':
                car_nums[i].direction_num = north_num;
                ++north_num;
                break;
            case 'e':
                car_nums[i].direction_num = east_num;
                ++east_num;
                break;
            case 'w':
                car_nums[i].direction_num = west_num;
                ++west_num;
                break;
            case 's':
                car_nums[i].direction_num = south_num;
                ++south_num;
                break;
            default:
                err_exit();
                break;
        }
    }
    cars[i] = '\0';
    
    north_order = (pthread_cond_t*)malloc(north_num * sizeof(pthread_cond_t));
    east_order = (pthread_cond_t*)malloc(east_num * sizeof(pthread_cond_t));
    west_order = (pthread_cond_t*)malloc(west_num * sizeof(pthread_cond_t));
    south_order = (pthread_cond_t*)malloc(south_num * sizeof(pthread_cond_t));
    
    north_order_lock = (pthread_mutex_t*)malloc(north_num * sizeof(pthread_mutex_t));
    east_order_lock = (pthread_mutex_t*)malloc(east_num * sizeof(pthread_mutex_t));
    west_order_lock = (pthread_mutex_t*)malloc(west_num * sizeof(pthread_mutex_t));
    south_order_lock = (pthread_mutex_t*)malloc(south_num * sizeof(pthread_mutex_t));
    
    for(i = 0; i < north_num; ++i)
    {
        pthread_cond_init(north_order+i, NULL);
        pthread_mutex_init(north_order_lock+i, NULL);
    }
    
    for(i = 0; i < east_num; ++i)
    {
        pthread_cond_init(east_order+i, NULL);
        pthread_mutex_init(east_order_lock+i, NULL);
    }
    
    for(i = 0; i < south_num; ++i)
    {
        pthread_cond_init(south_order+i, NULL);
        pthread_mutex_init(south_order_lock+i, NULL);
    }
    
    for(i = 0; i < west_num; ++i)
    {
        pthread_cond_init(west_order+i, NULL);
        pthread_mutex_init(west_order_lock+i, NULL);
    }
        
    err = pthread_create(&manager_tid, NULL, manager, NULL);
    if(err != 0)
        err_exit();
    
    for(i = 0; i < len; ++i)
    {
        switch(cars[i])
        {
            case 'n':
                err = pthread_create(tids+i, NULL, from_north, (void*)(car_nums + i));
                if(err != 0)
                    err_exit();
                break;
            case 'w':
                err = pthread_create(tids+i, NULL, from_west, (void*)(car_nums + i));
                if(err != 0)
                    err_exit();
                break;
            case 's':
                err = pthread_create(tids+i, NULL, from_south, (void*)(car_nums + i));
                if(err != 0)
                    err_exit();
                break;
            case 'e':
                err = pthread_create(tids+i, NULL, from_east, (void*)(car_nums + i));
                if(err != 0)
                    err_exit();
                break;
            default:
                err_exit();
                break;
        }
    }
    
    if(0 != pthread_create(&north_manager_tid, NULL, north_manager, NULL))
        err_exit();
    if(0 != pthread_create(&east_manager_tid, NULL, east_manager, NULL))
        err_exit();
    if(0 != pthread_create(&west_manager_tid, NULL, west_manager, NULL))
        err_exit();
    if(0 != pthread_create(&south_manager_tid, NULL, south_manager, NULL))
        err_exit();
    
    if(0 != pthread_create(&source_a_manager_tid, NULL, source_a_manager, NULL))
        err_exit();
    if(0 != pthread_create(&source_b_manager_tid, NULL, source_b_manager, NULL))
        err_exit();
    if(0 != pthread_create(&source_c_manager_tid, NULL, source_c_manager, NULL))
        err_exit();
    if(0 != pthread_create(&source_d_manager_tid, NULL, source_d_manager, NULL))
        err_exit();
    
    for(i = 0; i < len; ++i)
    {
        err = pthread_join(tids[i], NULL);
        if(err != 0)
            err_exit();
    }
    
    return 0;
}
