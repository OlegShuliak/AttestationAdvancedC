
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <curses.h>
#include <time.h>

#define MIN_Y  2
#define PLAYERS  2

 
enum {LEFT, UP, RIGHT, DOWN, STOP_GAME=KEY_F(10), PAUSE_GAME='p'}; 
enum {MAX_TAIL_SIZE=100, START_TAIL_SIZE=0, MAX_FOOD_SIZE=20, FOOD_EXPIRE_SECONDS=10, SEED_NUMBER=10, CONTROLS=3};

double DELAY = 0.1;
int max_y=0, max_x=0;

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

struct control_buttons default_controls[CONTROLS] = {{'s', 'w', 'a', 'd'}, {'S', 'W', 'A', 'D'}, {KEY_DOWN, KEY_UP, KEY_LEFT, KEY_RIGHT}};

void initFood(struct food f[], size_t size){  //Инициализация еды  
	struct food init = {0,0,3,0,0,0};      
	getmaxyx(stdscr, max_y, max_x);    
	for(size_t i=0; i<size; i++) {        
		f[i] = init;    
	} 
}

void setColor(int objectType){    
	attroff(COLOR_PAIR(1));    
	attroff(COLOR_PAIR(2));    
	attroff(COLOR_PAIR(3)); 
	switch (objectType){ 
		case 1:{    // SNAKE1            
			attron(COLOR_PAIR(1)); 
			break; 
		} 
		case 2:{    // SNAKE2            
			attron(COLOR_PAIR(2)); 
			break; 
		} 
		case 3:{    // FOOD            
			attron(COLOR_PAIR(3)); 
			break; 
		} 
	} 
}

void putFoodSeed(struct food *fp){    // Обновить/разместить текущее зерно на поле   
	char spoint[2] = {0};   
	getmaxyx(stdscr, max_y, max_x);    
	mvprintw(fp->y, fp->x, " ");    
	fp->x = rand() % (max_x - 1);    
	fp->y = rand() % (max_y - 2) + 1; //Не занимаем верхнюю строку    
	fp->put_time = time(NULL);    
	fp->point = '$';  
	setColor(fp->color);   
	fp->enable = 1;    
	spoint[0] = fp->point;    
	mvprintw(fp->y, fp->x, "%s", spoint); 
}

void putFood(struct food f[], size_t number_seeds){    //Разместить еду на поле
	for(size_t i=0; i<number_seeds; i++){        
		putFoodSeed(&f[i]);    
	} 
}

void refreshFood(struct food f[], int nfood){    //Обновление еды 
	getmaxyx(stdscr, max_y, max_x);    
	for(size_t i=0; i<nfood; i++){        
		if( f[i].put_time ){            
			if( !f[i].enable || (time(NULL) - f[i].put_time) > FOOD_EXPIRE_SECONDS ){                
				putFoodSeed(&f[i]);            
			}        
		}    
	} 
}

_Bool haveEat(struct snake_t *head, struct food f[]){    //Поедания зерна змейкой
	for(size_t i=0; i<MAX_FOOD_SIZE; i++){        
		if(f[i].enable && head->x == f[i].x && head->y == f[i].y){            
			f[i].enable = 0;            
			return 1;        
		} 
	}   
	return 0; 
}

void initTail(struct tail_t t[], size_t size) {    
	struct tail_t init_t={0,0}; 
	for(size_t i=0; i<size; i++) {   
		t[i]=init_t; 
	} 
}

void initHead(struct snake_t *head, int x, int y) { 
	head->x = x; 
	head->y = y; 
	head->direction = RIGHT; 
}

void initSnake(snake_t *head[], size_t size, int x, int y,int i){    
	head[i] = (snake_t*)malloc(sizeof(snake_t));    
	tail_t* tail = (tail_t*) malloc(MAX_TAIL_SIZE*sizeof(tail_t));    
	initTail(tail, MAX_TAIL_SIZE);    
	initHead(head[i], x, y);    
	head[i]->tail = tail; // прикрепляем к голове хвост    
	head[i]->tsize = size+1;
	head[i]->color = i+1;  
	head[i]->controls = default_controls;  
}

void go(struct snake_t *head){ 
	char ch = '@'; 
	setColor(head->color);    
	getmaxyx(stdscr, max_y, max_x); // macro - размер терминала    
	mvprintw(head->y, head->x, " "); // очищаем один символ 
	switch (head->direction) { 
		case LEFT: 
			if(head->x <= 0) {
				head->x = max_x;
			}
			mvprintw(head->y, --(head->x), "%c", ch); 
			break; 
		case RIGHT: 
			if(head->x >= max_x) {       
				head->x = 0;
			}    
			mvprintw(head->y, ++(head->x), "%c", ch); 
			break; 
		case UP: 
			if(head->y <= MIN_Y) {
				head->y = max_y;
			}
			mvprintw(--(head->y), head->x, "%c", ch); 
			break; 
		case DOWN: 
			if(head->y >= max_y) {
				head->y = MIN_Y;
			}    
			mvprintw(++(head->y), head->x, "%c", ch); 
			break; 
		default: 
			break; 
	}    
	refresh(); 
}

void goTail(struct snake_t *head) { 
	char ch = '*';
	setColor(head->color);     
	mvprintw(head->tail[head->tsize-1].y, head->tail[head->tsize-1].x, " "); 
	for(size_t i = head->tsize-1; i>0; i--) {        
		head->tail[i] = head->tail[i-1]; 
		if( head->tail[i].y || head->tail[i].x) {           
			mvprintw(head->tail[i].y, head->tail[i].x, "%c", ch);    
		}    
	}
	head->tail[0].x = head->x;    
	head->tail[0].y = head->y; 
}

void addTail(struct snake_t *head){    //Увеличение хвоста на 1 элемент
	if(head == NULL || head->tsize>MAX_TAIL_SIZE){        
		mvprintw(0, 0, "Can't add tail");        
		return;    
	}    
	head->tsize++; 
}

void changeDirection(snake_t* snake, const int32_t key) {    
	for (int i = 0; i < 3; i++) {
		if (key == snake->controls[i].down) {       
			snake->direction = DOWN;    
		} else if (key == snake->controls[i].up) {
			snake->direction = UP;    
		} else if (key == snake->controls[i].right) {       
			snake->direction = RIGHT;    
		} else if (key == snake->controls[i].left) {       
			snake->direction = LEFT; 
		}
	}
}

int checkDirection(snake_t* snake, int32_t key) {
	int checkDir = 1;
	for (int i = 0; i < 3; i++) {
		if (key == snake->controls[i].down && snake->direction == UP) {       
			checkDir =0;    
		} else if (key == snake->controls[i].up && snake->direction == DOWN) {
			checkDir =0;    
		} else if (key == snake->controls[i].right && snake->direction == LEFT) {       
			checkDir =0;    
		} else if (key == snake->controls[i].left && snake->direction == RIGHT) {       
			checkDir =0; 
		}
	}
	return checkDir;
}

_Bool isCrush(snake_t * snake){    //cтолкновение головы с хвостом
	for(size_t i=1; i<snake->tsize; i++){ 
		if(snake->x == snake->tail[i].x && snake->y == snake->tail[i].y){ 
			return 1;
		}
	}
	return 0; 
}

void repairSeed(struct food f[], size_t nfood, struct snake_t *head){    //Проверка корректности выставления зерна
	for( size_t i=0; i<head->tsize; i++ ){ 
		for( size_t j=0; j<nfood; j++ ){  
			if( f[j].x == head->tail[i].x && f[j].y == head->tail[i].y && f[i].enable ){     //Если хвост совпадает с зерном 
				mvprintw(1, 0, "Repair tail seed %zu",j); 
				putFoodSeed(&f[j]); 
			} 
		}
	} 
	for( size_t i=0; i<nfood; i++ ){ 
		for( size_t j=0; j<nfood; j++ ){  
			if( i!=j && f[i].enable && f[j].enable && f[j].x == f[i].x && f[j].y == f[i].y && f[i].enable ){  //Если два зерна на одной точке 
				mvprintw(1, 0, "Repair same seed %zu",j); 
				putFoodSeed(&f[j]); 
			} 
		} 
	}
}

void printLevel(struct snake_t *head){ // счетчик уровня
	getmaxyx(stdscr, max_y, max_x);
	mvprintw(0, max_x - 10, "LEVEL: %ld", head->tsize);
}

void printExit(struct snake_t *head){
	int max_x = 0, max_y = 0;
	getmaxyx(stdscr, max_y, max_x);
	mvprintw(max_y /2, max_x /2 - 5, "Your LEVEL is %ld", head->tsize);
	refresh();
	getchar();
}

int distance(const snake_t snake, const struct food food){   // вычисляет количество ходов до еды    
	return (abs(snake.x - food.x) + abs(snake.y - food.y)); 
}

void autoChangeDirection(snake_t *snake, struct food food[], int foodSize){    
	int pointer = 0;    
	for (int i = 1; i < foodSize; i++){   // ищем ближайшую еду        
		pointer = (distance(*snake, food[i]) < distance(*snake, food[pointer])) ? i : pointer;    
	}    
	if ((snake->direction == RIGHT || snake->direction == LEFT) && (snake->y != food[pointer].y)){  // горизонтальное движение        
		snake->direction = (food[pointer].y > snake->y) ? DOWN : UP;    
	} else if ((snake->direction == DOWN || snake->direction == UP) && (snake->x != food[pointer].x)){  // вертикальное движение        
		snake->direction = (food[pointer].x > snake->x) ? RIGHT : LEFT;    
	} 
}

void update(snake_t *head, struct food f[], int key,int ai){   // Версия для добавления ИИ, вторая змея соперник
	if (ai == 1){    
		autoChangeDirection(head,f,SEED_NUMBER);
	} 
	go(head); 
	goTail(head); 
	if (checkDirection(head,key)){    
		changeDirection(head, key); 
	} 
	refreshFood(food, SEED_NUMBER);// Обновляем еду 
	if (haveEat(head,food)){    
		addTail(head);    
		printLevel(head);     
	} 
} 

void pause(void){
	int max_x = 0, max_y = 0;
	getmaxyx(stdscr, max_y, max_x);
	mvprintw(max_y /2, max_x /2 - 5, "Press P to continue");
	while (getch() != PAUSE_GAME)
	{}
	mvprintw(max_y /2, max_x /2 - 5, "                   ");
}

void startMenu(){
	initscr();
	curs_set(FALSE);
	cbreak();
	
	if(has_colors() == FALSE){
		endwin();
		printf("Your terminal does not support color\n");
		exit(1);
	}
	start_color();    
	init_pair(1, COLOR_RED, COLOR_BLACK);    
	init_pair(2, COLOR_YELLOW, COLOR_BLACK); 
	attron(COLOR_PAIR(1));
	mvprintw(1,1,"1. Start");
	attroff(COLOR_PAIR(1));
	
	attron(COLOR_PAIR(2));
	mvprintw(3,1,"2. Exit");
	attron(COLOR_PAIR(1));
		mvprintw(7, 30, "@******************************@");
	attron(COLOR_PAIR(2));
		mvprintw(7, 30, "  SNAKE  SNAKE  SNAKE SNAKE     ");
		mvprintw(7, 30, "@******************************@");
	char ch = (0);
	while(1) {
		attron(COLOR_PAIR(1));
		mvprintw(20, 50, "Press any key ...");
		ch = getch();
		if (ch == '1'){
			clear();
			break;
		} else if (ch == '2'){
			endwin();
			exit(0);
		}
	}
	refresh();
	getch();
	endwin();
}

int main(int argc, char **argv){
	snake_t* snakes[PLAYERS];    
		for (int i = 0; i < PLAYERS; i++){        
			initSnake(snakes,START_TAIL_SIZE,10+i*10,10+i*10,i);
		}
	initFood(food, MAX_FOOD_SIZE);
	initscr(); 
	keypad(stdscr, TRUE); // Включаем F1, F2, стрелки и т.д. 
	raw();            // Откдючаем line buffering 
	noecho();        // Отключаем echo() режим при вызове getch 
	curs_set(FALSE); //Отключаем курсор 
	mvprintw(0, 0," Use arrows for control. Press 'F10' for EXIT"); 
	timeout(0); //Отключаем таймаут после нажатия клавиши в цикле 
	int key_pressed=0; 
	putFood(food, SEED_NUMBER);
	_Bool isFinish = 0;
	startMenu();
	start_color();    
	init_pair(1, COLOR_RED, COLOR_BLACK);    
	init_pair(2, COLOR_BLUE, COLOR_BLACK);    
	init_pair(3, COLOR_GREEN, COLOR_BLACK);
	while( key_pressed != STOP_GAME && !isFinish){
		clock_t begin = clock();    
		key_pressed = getch(); // Считываем клавишу
		update(snakes[0], food, key_pressed, 0); 
		update(snakes[1], food, key_pressed, 1); 
		if(isCrush(snakes[0])){    
			printExit(snakes[0]);    
			isFinish = 1; 
		}
		if (key_pressed == PAUSE_GAME){
			pause();
		}
		while ((double)(clock() - begin)/CLOCKS_PER_SEC<DELAY)        
		{}
		refresh();  //Обновление экрана, вывели кадр анимации
	}
	for (int i = 0; i < PLAYERS; i++){        
		printExit(snakes[i]);        
		free(snakes[i]->tail);        
		free(snakes[i]);    
	}
	endwin(); // Завершаем режим curses mod
	return 0;
}

