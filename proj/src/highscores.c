#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/driver.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

#include "highscores.h"
#include "rtc.h"
#include "sprite.h"
#include "video_gr.h"


//variables that store the three best highscores and also the time and date when they were accomplished
unsigned long score1, hours1, minutes1, day1, month1, year1;
unsigned long score2, hours2, minutes2, day2, month2, year2;
unsigned long score3, hours3, minutes3, day3, month3, year3;


int writeHighscoresFile(){
	FILE *fptr;
	fptr = fopen("/home/lcom/lcom//proj/src/highscores.txt", "w");
	fprintf(fptr,"%d\n",score1);
	fprintf(fptr,"%d\n",hours1);
	fprintf(fptr,"%d\n",minutes1);
	fprintf(fptr,"%d\n",day1);
	fprintf(fptr,"%d\n",month1);
	fprintf(fptr,"%d\n",year1);
	fprintf(fptr,"%d\n",score2);
	fprintf(fptr,"%d\n",hours2);
	fprintf(fptr,"%d\n",minutes2);
	fprintf(fptr,"%d\n",day2);
	fprintf(fptr,"%d\n",month2);
	fprintf(fptr,"%d\n",year2);
	fprintf(fptr,"%d\n",score3);
	fprintf(fptr,"%d\n",hours3);
	fprintf(fptr,"%d\n",minutes3);
	fprintf(fptr,"%d\n",day3);
	fprintf(fptr,"%d\n",month3);
	fprintf(fptr,"%d",year3);
	fclose(fptr);
	return 0;
}

int readHighscoresFile(){
	FILE *fptr;
	fptr = fopen("/home/lcom/lcom//proj/src/highscores.txt", "r");
	fscanf(fptr,"%d",&score1);
	fscanf(fptr,"%d",&hours1);
	fscanf(fptr,"%d",&minutes1);
	fscanf(fptr,"%d",&day1);
	fscanf(fptr,"%d",&month1);
	fscanf(fptr,"%d",&year1);
	fscanf(fptr,"%d",&score2);
	fscanf(fptr,"%d",&hours2);
	fscanf(fptr,"%d",&minutes2);
	fscanf(fptr,"%d",&day2);
	fscanf(fptr,"%d",&month2);
	fscanf(fptr,"%d",&year2);
	fscanf(fptr,"%d",&score3);
	fscanf(fptr,"%d",&hours3);
	fscanf(fptr,"%d",&minutes3);
	fscanf(fptr,"%d",&day3);
	fscanf(fptr,"%d",&month3);
	fscanf(fptr,"%d",&year3);
	fclose(fptr);
	return 0;
}

int highscore_handler(int score_points){
	if(score_points >= score1){
		score3 = score2;
		hours3 = hours2;
		minutes3 = minutes2;
		day3 = day2;
		month3 = month2;
		year3 = year2;
		score2 = score1;
		hours2 = hours1;
		minutes2 = minutes1;
		day2 = day1;
		month2 = month1;
		year2 = year1;
		score1 = score_points;
		rtc_get_date(&day1, &month1, &year1);
		rtc_get_time(&hours1, &minutes1);
		return 0;
	}
	if((score_points < score1) && (score_points >= score2)){
		score3 = score2;
		hours3 = hours2;
		minutes3 = minutes2;
		day3 = day2;
		month3 = month2;
		year3 = year2;
		score2 = score_points;
		rtc_get_date(&day2, &month2, &year2);
		rtc_get_time(&hours2, &minutes2);
		return 0;
	}
	if((score_points < score1) && (score_points < score2) && (score_points >= score3)){
		score3 = score_points;
		rtc_get_date(&day3, &month3, &year3);
		rtc_get_time(&hours3, &minutes3);
		return 0;
	}
	return 0;
}

int draw_highscores(){
	int milhares, centenas, dezenas, unidades;

	if(score1 != 0){
		dezenas = hours1/10;
		unidades = hours1 - dezenas*10;
		draw_number(dezenas,315,350);
		draw_number(unidades,333,350);
		draw_sprite(getTwodots_sprite(),351,350);
		dezenas = minutes1/10;
		unidades = minutes1 - dezenas*10;
		draw_number(dezenas,359,350);
		draw_number(unidades,377,350);
		dezenas = day1/10;
		unidades = day1 - dezenas*10;
		draw_number(dezenas,415,350);
		draw_number(unidades,433,350);
		draw_sprite(getBar_sprite(),451,350);
		dezenas = month1/10;
		unidades = month1 - dezenas*10;
		draw_number(dezenas,465,350);
		draw_number(unidades,483,350);
		draw_sprite(getBar_sprite(),501,350);
		dezenas = year1/10;
		unidades = year1 - dezenas*10;
		draw_number(dezenas,515,350);
		draw_number(unidades,533,350);
		milhares = score1 / 1000;
		centenas = (score1 - milhares*1000)/100;
		dezenas = (score1 - milhares*1000 - centenas*100)/10;
		unidades = (score1 - milhares*1000 - centenas*100 - dezenas*10);
		draw_number(milhares,632,350);
		draw_number(centenas,650,350);
		draw_number(dezenas,668,350);
		draw_number(unidades,686,350);
	}

	if(score2 != 0){
		dezenas = hours2/10;
		unidades = hours2 - dezenas*10;
		draw_number(dezenas,315,400);
		draw_number(unidades,333,400);
		draw_sprite(getTwodots_sprite(),351,400);
		dezenas = minutes2/10;
		unidades = minutes2 - dezenas*10;
		draw_number(dezenas,359,400);
		draw_number(unidades,377,400);
		dezenas = day2/10;
		unidades = day2 - dezenas*10;
		draw_number(dezenas,415,400);
		draw_number(unidades,433,400);
		draw_sprite(getBar_sprite(),451,400);
		dezenas = month2/10;
		unidades = month2 - dezenas*10;
		draw_number(dezenas,465,400);
		draw_number(unidades,483,400);
		draw_sprite(getBar_sprite(),501,400);
		dezenas = year2/10;
		unidades = year2 - dezenas*10;
		draw_number(dezenas,515,400);
		draw_number(unidades,533,400);
		milhares = score2 / 1000;
		centenas = (score2 - milhares*1000)/100;
		dezenas = (score2 - milhares*1000 - centenas*100)/10;
		unidades = (score2 - milhares*1000 - centenas*100 - dezenas*10);
		draw_number(milhares,632,400);
		draw_number(centenas,650,400);
		draw_number(dezenas,668,400);
		draw_number(unidades,686,400);
	}

	if(score3 != 0){
		dezenas = hours3/10;
		unidades = hours3 - dezenas*10;
		draw_number(dezenas,315,450);
		draw_number(unidades,333,450);
		draw_sprite(getTwodots_sprite(),351,450);
		dezenas = minutes3/10;
		unidades = minutes3 - dezenas*10;
		draw_number(dezenas,359,450);
		draw_number(unidades,377,450);
		dezenas = day3/10;
		unidades = day3 - dezenas*10;
		draw_number(dezenas,415,450);
		draw_number(unidades,433,450);
		draw_sprite(getBar_sprite(),451,450);
		dezenas = month3/10;
		unidades = month3 - dezenas*10;
		draw_number(dezenas,465,450);
		draw_number(unidades,483,450);
		draw_sprite(getBar_sprite(),501,450);
		dezenas = year3/10;
		unidades = year3 - dezenas*10;
		draw_number(dezenas,515,450);
		draw_number(unidades,533,450);
		milhares = score3 / 1000;
		centenas = (score3 - milhares*1000)/100;
		dezenas = (score3 - milhares*1000 - centenas*100)/10;
		unidades = (score3 - milhares*1000 - centenas*100 - dezenas*10);
		draw_number(milhares,632,450);
		draw_number(centenas,650,450);
		draw_number(dezenas,668,450);
		draw_number(unidades,686,450);
	}
	return 0;
}
