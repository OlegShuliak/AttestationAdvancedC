
#include <stdio.h>
#include <curses.h>
#include <stdint.h>
#include "enums.c"


struct control_buttons{    
	int down;    
	int up;    
	int left;    
	int right; 
}control_buttons; 

struct food{    
	int x;    
	int y; 
	int color;   
	time_t put_time;    
	char point;    
	uint8_t enable; 
} food[MAX_FOOD_SIZE];

typedef struct snake_t{ 
	int x; 
	int y; 
	int direction; 
	size_t tsize; 
	struct tail_t *tail; 
	struct control_buttons *controls; 
	int color;
	int ai;
} snake_t;

typedef struct tail_t{ 
	int x; 
	int y; 
} tail_t;

struct base{
	int x;
	int y;
	int color;
	size_t value;
} base;

struct control_buttons default_controls[CONTROLS] = {{'s', 'w', 'a', 'd'}, {'S', 'W', 'A', 'D'}, {KEY_DOWN, KEY_UP, KEY_LEFT, KEY_RIGHT}};


