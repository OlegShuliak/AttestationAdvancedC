
#include <stdlib.h>
#include <curses.h>
#include <time.h>
#include "interface.c"

#define MIN_Y  1


void initFood(struct food f[], size_t size){  //Инициализация еды (тыквы)  
	struct food init = {0,0,6,0,0,0};          
	for(size_t i=0; i<size; i++) {        
		f[i] = init;    
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

