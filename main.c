
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <curses.h>
#include <time.h>
#include "headers.c"

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

