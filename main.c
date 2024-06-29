
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <curses.h>
#include <time.h>

#define MIN_Y  1

 
enum {LEFT, UP, RIGHT, DOWN, STOP_GAME=KEY_F(10), PAUSE_GAME='p'}; 
enum {MAX_TAIL_SIZE=5, START_TAIL_SIZE=0, MAX_FOOD_SIZE=20, FOOD_EXPIRE_SECONDS=10, SEED_NUMBER=10, CONTROLS=3};

double DELAY = 0.1;
int max_y=0, max_x=0;
uint8_t PLAYERS = 1;

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

void initFood(struct food f[], size_t size){  //Инициализация еды (тыквы)  
	struct food init = {0,0,6,0,0,0};          
	for(size_t i=0; i<size; i++) {        
		f[i] = init;    
	} 
}

void initColorPairs(){    //инициализация цветов
	start_color();    
	init_pair(1, COLOR_RED, COLOR_BLACK);    
	init_pair(2, COLOR_BLUE, COLOR_BLACK);    
	init_pair(3, COLOR_GREEN, COLOR_BLACK);
	init_pair(4, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(5, COLOR_WHITE, COLOR_BLACK);
	init_pair(6, COLOR_YELLOW, COLOR_BLACK);
	init_pair(7, COLOR_CYAN, COLOR_BLACK);
}

void setColor(int objectType){    //выбор цвета
	initColorPairs();    
	attroff(COLOR_PAIR(1));    
	attroff(COLOR_PAIR(2));    
	attroff(COLOR_PAIR(3)); 
	attroff(COLOR_PAIR(4)); 
	attroff(COLOR_PAIR(5)); 
	attroff(COLOR_PAIR(6)); 
	attroff(COLOR_PAIR(7)); 
	switch (objectType){ 
		case 1:{                
			attron(COLOR_PAIR(1)); 
			break; 
		} 
		case 2:{                
			attron(COLOR_PAIR(2)); 
			break; 
		} 
		case 3:{               
			attron(COLOR_PAIR(3)); 
			break; 
		} 
		case 4:{               
			attron(COLOR_PAIR(4)); 
			break; 
		} 
		case 5:{               
			attron(COLOR_PAIR(5)); 
			break; 
		} 
		case 6:{               
			attron(COLOR_PAIR(6)); 
			break; 
		} 
		case 7:{               
			attron(COLOR_PAIR(7)); 
			break; 
		} 
	} 
}

void putFoodSeed(struct food *fp){    // Обновить/разместить текущее зерно на поле ()тыкву на поле  
	char spoint[2] = {0};   
	getmaxyx(stdscr, max_y, max_x);    
	mvprintw(fp->y, fp->x, " ");    
	fp->x = rand() % (max_x - 1) + 1;  // Не занимаем крайний левый столбец  
	fp->y = rand() % (max_y - 2) + 1; //Не занимаем верхнюю строку    
	fp->put_time = time(NULL);    
	fp->point = '$';  
	setColor(fp->color);   
	fp->enable = 1;    
	spoint[0] = fp->point;    
	mvprintw(fp->y, fp->x, "%s", spoint); 
}

void putFood(struct food f[], size_t number_seeds){    //Разместить еду (тыкву) на поле
	for(size_t i=0; i<number_seeds; i++){        
		putFoodSeed(&f[i]);    
	} 
}

void refreshFood(struct food f[], int nfood){    //Обновление еды  (тыквы)  
	for(size_t i=0; i<nfood; i++){        
		if( f[i].put_time ){            
			if( !f[i].enable || (time(NULL) - f[i].put_time) > FOOD_EXPIRE_SECONDS ){                
				putFoodSeed(&f[i]);            
			}        
		}    
	} 
}

_Bool haveEat(struct snake_t *head, struct food f[]){    //Поедания зерна змейкой (сбор дроном тыквы)
	for(size_t i=0; i<MAX_FOOD_SIZE; i++){        
		if(f[i].enable && head->x == f[i].x && head->y == f[i].y){            
			f[i].enable = 0;            
			return 1;        
		} 
	}   
	return 0; 
}

void initTail(struct tail_t t[], size_t size) {   //инициализация хвоста (тележки) 
	struct tail_t init_t={0,0}; 
	for(size_t i=0; i<size; i++) {   
		t[i]=init_t; 
	} 
}

void initHead(struct snake_t *head, int x, int y) {  //инициализация головы (дрона)
	head->x = x; 
	head->y = y; 
	head->direction = RIGHT; 
}

void initSnake(snake_t *head[], size_t size, int x, int y,int i){  // инициализация змейки (дрон + тележка)
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

void addTail(struct snake_t *head){    //Увеличение хвоста на 1 элемент (положили тыкву в тележку)
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

_Bool isCrush(snake_t* snake){    //cтолкновение головы с хвостом (дрона с тележкой)
	for(size_t i=1; i<snake->tsize; i++){ 
		if(snake->x == snake->tail[i].x && snake->y == snake->tail[i].y){ 
			return 1;
		}
	}
	return 0; 
}

void repairSeed(struct food f[], size_t nfood, struct snake_t *head){    //Проверка корректности выставления зерна (тыквы)
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

void printLevel(struct base b){ // счетчик уровня (заполнения базы)
	getmaxyx(stdscr, max_y, max_x);
	mvprintw(0, max_x - 10, "LEVEL: %ld", b.value);
}

void printExit(struct base b){
	int max_x = 0, max_y = 0;
	getmaxyx(stdscr, max_y, max_x);
	mvprintw(max_y /2, max_x /2 - 5, "Base value is %ld", b.value);
	refresh();
	getchar();
}

int distance(const snake_t snake, const struct food food){   // вычисляет количество ходов до еды  (тыквы)  
	return (abs(snake.x - food.x) + abs(snake.y - food.y)); 
}

void autoChangeDirection(snake_t *snake, struct food food[], int foodSize){    
	int pointer = 0;    
	for (int i = 1; i < foodSize; i++){   // ищем ближайшую еду        
		pointer = (distance(*snake, food[i]) < distance(*snake, food[pointer])) ? i : pointer;    
	}    
	if ((snake->direction == RIGHT || snake->direction == LEFT) && (snake->y != food[pointer].y)){  // горизонтальное движение      
		if (food[pointer].y > snake->y){
			snake->direction = DOWN;
		} else {
			snake->direction = UP;
		}
	} else if ((snake->direction == DOWN || snake->direction == UP) && (snake->x != food[pointer].x)){  // вертикальное движение        
		if (food[pointer].x > snake->x){
			snake->direction = RIGHT;
		} else {
			snake->direction = LEFT;
		}     
	} 
}

void buildBase(struct base b){
	getmaxyx(stdscr, max_y, max_x);
	mvprintw(b.y, b.x, "                    ");
	char ch = 'B';
	setColor(b.color);
	mvprintw(b.y, b.x, "%c", ch);
}

void goBase(struct snake_t *snake, struct base b){
	if ((snake->direction == RIGHT || snake->direction == LEFT) && (snake->y != b.y)){  // горизонтальное движение      
		if (b.y > snake->y){
			snake->direction = DOWN;
		} else {
			snake->direction = UP;
		}
	} else if ((snake->direction == DOWN || snake->direction == UP) && (snake->x != b.x)){  // вертикальное движение        
		if (b.x > snake->x){
			snake->direction = RIGHT;
		} else {
			snake->direction = LEFT;
		}     
	} 
}

_Bool atBase (struct snake_t *snake, struct base b){
	if(snake->x == b.x && snake->y == b.y){                                
			return 1;        
		}   
	return 0;
}

void update(snake_t *head, struct food f[],struct base b, int key,int ai){   // Версия с возможностью ручного управления и использования ИИ
	if (ai == 1){
		if (head->tsize < MAX_TAIL_SIZE){    
			autoChangeDirection(head,f,SEED_NUMBER);
		} else {
			goBase(head, b);
		}
	} 
	go(head); 
	goTail(head); 
	if (checkDirection(head,key)){    
		changeDirection(head, key); 
	}
	refreshFood(food, SEED_NUMBER);// Обновляем еду 
	if (haveEat(head,food)){    
		addTail(head);        
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
	setColor(1); 
	mvprintw(1,1,"1. Start");
	setColor(2);
	mvprintw(3,1,"2. Exit");
	setColor(6);
	mvprintw(7, 30, "@******************************@");
	mvprintw(7, 30, "  SNAKE  SNAKE  SNAKE SNAKE     ");
	mvprintw(7, 30, "@******************************@");
	char ch = (0);
	while(1) {
		setColor(1);
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
	initscr(); 
	keypad(stdscr, TRUE); // Включаем F1, F2, стрелки и т.д. 
	raw();            // Откдючаем line buffering 
	noecho();        // Отключаем echo() режим при вызове getch 
	curs_set(FALSE); //Отключаем курсор 
	mvprintw(0, 0," Press 'F10' for EXIT"); 
	timeout(0); //Отключаем таймаут после нажатия клавиши в цикле
	startMenu();
	snake_t* snakes[PLAYERS];    
		for (int i = 0; i < PLAYERS; i++){        
			initSnake(snakes,START_TAIL_SIZE,10+i*35,10+i,i);
		}
	initFood(food, MAX_FOOD_SIZE);
	struct base base = {1, 1, 7, 0}; 
	int key_pressed=0; 
	putFood(food, SEED_NUMBER);
	_Bool isFinish = 0;
	buildBase(base);
	printLevel(base);
	while( key_pressed != STOP_GAME && !isFinish){
		clock_t begin = clock();    
		key_pressed = getch(); // Считываем клавишу 
		for (int i = 0; i < PLAYERS; i++){
			update(snakes[i], food, base, key_pressed, 1);
			if (atBase(snakes[i], base)){
				base.value += snakes[i]->tsize;
				buildBase(base);
				printLevel(base);
				free(snakes[i]->tail);        
				free(snakes[i]);
				initSnake(snakes,START_TAIL_SIZE,base.x + 1,base.y,i); 
			}          
			if(isCrush(snakes[i])){    
				printExit(base);    
				isFinish = 1; 
			}
			if (base.value == 50){    
				isFinish = 1;
			}
		}
		if (key_pressed == PAUSE_GAME){
			pause();
		}
		while ((double)(clock() - begin)/CLOCKS_PER_SEC<DELAY)        
		{}
		refresh();  //Обновление экрана, вывели кадр анимации
	}
	printExit(base);
	for (int i = 0; i < PLAYERS; i++){                
		free(snakes[i]->tail);        
		free(snakes[i]);    
	}
	endwin(); // Завершаем режим curses mod
	return 0;
}

