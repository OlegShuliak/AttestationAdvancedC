#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include "structs.c"

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

void printLevel(struct base b){ // счетчик уровня (заполнения базы)
	getmaxyx(stdscr, max_y, max_x);
	mvprintw(0, max_x - 10, "Value: %ld", b.value);
}

void printExit(struct base b){
	int max_x = 0, max_y = 0;
	getmaxyx(stdscr, max_y, max_x);
	mvprintw(max_y /2, max_x /2 - 5, "Base value is %ld", b.value);
	refresh();
	getchar();
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
