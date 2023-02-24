#include "tamagotchi.h"
#include "GLCD/GLCD.h"
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "TouchPanel/TouchPanel.h"
#include "adc/adc.h"
#include "music/music.h"

#define RIT_SEMIMINIMA 8
#define RIT_MINIMA 16
#define RIT_INTERA 32

#define UPTICKS 1


uint8_t ss;
uint8_t hh;
uint8_t mm;
/*variables of age string*/

uint8_t satiety;
uint8_t happiness;
/*effective counters of happiness and satiety*/

uint8_t count_happiness;
uint8_t count_satiety;
/*decreasing counters, every second they decrease by 1, starting from 5*/

uint8_t flag;
/* flag used to exit go left/right animations*/

uint8_t current_action;			//selected action
uint8_t status; 						//0 idle, 1 eating meal, 2 eating snack, 3 cuddles, represents the status of the tamagotchi;
uint16_t curr_posX;					//current position over X axis of the character
uint16_t posY = 105;				//fixed position of the character over the y axis
uint8_t direction;					//0 idle, 1 go left, 2 go right

uint8_t gameover;
uint8_t init;


extern uint8_t count_anim;
/*counter of the times that cuddles_animation is called*/

extern uint8_t count_cuddles;
extern uint16_t posX_left;
extern uint16_t posY_left;
extern uint16_t posX_right;
extern uint16_t posY_right;
/*variables for cuddles animation, posX/Y are for heart placing*/

static char *str = "00:00:00"; 	/*initial value of the string age is printed*/

static int currentNote = 0;
static int ticks = 0;							/*variables to play music*/

/*songs definition*/

NOTE song[] = 
{
	{e5, time_semicroma},
	{pause, time_biscroma},
	{e5, time_semicroma},
	{pause, time_semicroma},
	{e5, time_croma},
	{pause, time_biscroma},
	{c5, time_semicroma},
	{e5, time_semicroma},
	{pause, time_semicroma},
	{g5, time_croma},
	{pause, time_croma},
	{g4, time_semiminima},
	{pause, time_semicroma}
};

NOTE snack[] = {
	{c5, time_semicroma},
	{pause, time_semicroma},
	{c5, time_semicroma},
	{pause, time_semicroma},
	{a4, time_semicroma},
	{c5, time_semicroma},
	{pause, time_semicroma},
	{c5, time_semicroma},
	{pause, time_semicroma},
	{a4, time_semicroma},
	{c5, time_semicroma},
	{pause, time_semicroma},
	{c5, time_semicroma},
	{pause, time_semicroma},
	{b4, time_semicroma},
	{pause, time_semicroma},
	{b4, time_semicroma},
	{pause, time_semicroma},
	{b4, time_semicroma},
	{pause, time_semicroma},
	{g4, time_semicroma},
	{b4, time_semicroma},
	{pause, time_semicroma},
	{b4, time_semicroma},
	{pause, time_semicroma},
	{g4, time_semicroma},
	{b4, time_semicroma},
	{pause, time_semicroma},
	{b4, time_semicroma},
	{pause, time_semicroma}
};

NOTE meal[] = {
	{g3, time_semicroma},
	{c4, time_semicroma},
	{e4, time_semicroma},
	{g4, time_semicroma},
	{c5, time_semicroma},
	{e5, time_semicroma},
	{g5, time_semiminima},
	{c5, time_semiminima}
};

NOTE over[] = 
{
	{c5, time_croma},
	{pause, time_croma},
	{g4, time_croma},
	{pause, time_croma},
	{e4, time_semiminima},
	{a4, time_croma},
	{b4, time_croma},
	{a4, time_croma},
	{g4, time_semiminima},
	{b4b, time_semiminima},
	{a4b, time_semiminima},
	{g4, time_croma},
	{f4, time_croma},
	{g4, time_minima}
	
};

NOTE cuddles[] = 
{
	{e5, time_semicroma},
	{pause, time_croma},
	{e5, time_semicroma},
	{e5, time_semicroma},
	{pause, time_semicroma},
	{g4, time_croma},
	{a4, time_semiminima},
	{g4, time_semiminima},
	{e5, time_semicroma},
	{pause, time_croma},
	{e5, time_semicroma},
	{e5, time_semicroma},
	{pause, time_semicroma},
	{g5, time_semicroma},
	{pause, time_semicroma},
	{a5, time_semiminima},
	{g5, time_semiminima}
};

extern void speaker_init(void);

void Tamagotchi_init(void){																/*flags are set, age is set to 0, character is put in idle position*/
	happiness = 5;
	satiety = 5;
	gameover=0;
	direction = 0;
	current_action = 0;
	currentNote=0;
	status=0;
	init=1;
	curr_posX = 85;
	screen_setup();
	ADC_start_conversion();
	init_timer(0, 0x017D7840);
	init_timer(1, 0x00487AB0);
	init_RIT(0x004C4B40);
	enable_RIT();
	enable_timer(1);
	Tamagotchi_setCounter();
}

void Tamagotchi_setCounter(void){										/*this initializes timer 0 and counter starts, also init song starts*/
	ss=0;
	mm=0;
	hh=0;
	count_satiety=5;
	count_happiness=5;
	GUI_Text(38, 3, (uint8_t*)str, Black, White);
	enable_timer(0);
	playInit();
}

void Tamagotchi_gameover(void){										/*game over handler*/
	uint8_t i=0;
	currentNote = 0;
	disable_timer(0);
	disable_timer(1);
	reset_timer(1);
	if(status == 3){
		drop_heart(posX_left, posY_left, 0);
		drop_heart(posX_right, posY_right, 0);
	}
	status = 0;
	gameover++;
	current_action = 3;
	drop_star(190, 190, 0);
	drop_mushroom(3, 190, 0);
	enable_timer(1);
	while(1){
		if(i==0){
			playOver();
			i++;
		}
		if(check_exit_gameover())
			break;
	}
	disable_timer(1);
	disable_timer(0);
	GUI_Text(43, 288, (uint8_t*)"MEAL", White, White);
	GUI_Text(163, 288, (uint8_t*)"SNACK", White, White);
	GUI_Text (80, 160,(uint8_t*)"GAME OVER!", Red, White);
	GUI_Text (28, 175, (uint8_t*)"Press SELECT to restart", Red, White);
	GUI_Text (97, 288, (uint8_t*)"SELECT", White, Red);
}

void highlight_action(uint8_t selected_action){														/* this highlights the action after pressing left/right*/
	if(selected_action == 1){
		GUI_Text(43, 288, (uint8_t*)"MEAL", White, Red);
	}else{
		GUI_Text(163, 288, (uint8_t*)"SNACK", White, Red);
	}
}

void reset_action(uint8_t unselected_action){														/*reset_of the action after pressing select*/
	if(unselected_action==1){
		GUI_Text(43, 288, (uint8_t*)"MEAL", Black, White);
	}else{
		GUI_Text(163, 288, (uint8_t*)"SNACK", Black, White);
	}
}

void Tamagotchi_eatMeal(void){                                 /* eat meal handler */
	uint8_t i = 0;
	disable_timer(1);
	reset_timer(1);
	count_anim=8;
	status=1;
	flag=1;
	direction=1;
	drop_mushroom(3, 190, 1);
	enable_timer(1);
	while(1){
		if(!check_exit()){
			break;
		}
		if(check_gameover()){
			break;
		}
	}
	if(gotoGameover()){
		return;
	}
	disable_timer(1);
	drop_mushroom(3, 190, 0);
	direction=2;
	enable_timer(1);
	if(satiety < 5 && !gameover){
		draw_lives(110+satiety*21, 44, 1);
		satiety++;
	}
	count_satiety=5;
	enable_timer(1);
	while(1){
		if(i == 0){
			playMeal();
			i++;
		}
		if(check_exit()){
			break;
		}
		if(check_gameover()){
			break;
		}
	}
	if(gotoGameover()){
		return;
	}
	direction = 0;
	status = 0;
	currentNote=0;
	return;
}

void Tamagotchi_eatSnack(void){																			/* eat snack handler */
	uint8_t i=0;	
	disable_timer(1);
	reset_timer(1);
	count_anim=0;
	flag=0;
	status = 2;
	direction=2;
	drop_star(190, 190, 1);
	enable_timer(1);
	while(1){
		if(i == 0){
			playSnack();
			i++;
		}
		if(check_exit()){
			i=0;
			break;
		}
		if(check_gameover()){
			break;
		}
	}
	if(gotoGameover()){
		return;
	}
	disable_timer(1);
	drop_star(190, 190, 0);
	if(happiness < 5 && !gameover){
		draw_lives(110+happiness*21, 22, 0);
		happiness++;
	}
	count_happiness=5;
	direction=1;
	enable_timer(1);
	while(1){
		if(i == 0){
			playSnack();
			i++;
		}
		if(!check_exit()){
			i=0;
			break;
		}
		currentNote = 0;
		if(check_gameover()){
			break;
		}
	}
	if(gotoGameover()){
		return;
	}
	direction = 0;
	status = 0;
	currentNote = 0;
	return;
}

void Tamagotchi_cuddles(void){																					/* cuddles handler */
	uint8_t i = 0;
	status=3;
	count_cuddles=0;
	while(1)
	{
		if(i == 0){
			playCuddles();
			i++;
		}
		if(check_status()){
			break;
		}
	}
	if(happiness < 5 && !gameover){
		draw_lives(110+happiness*21, 22, 0);
		happiness++;
	}
	status = 0;
	count_happiness = 5;
	currentNote = 0;
	return;
}

uint8_t check_exit(void){																						/* functions to exit while(1) loops */
	return flag;																											/* checking the variable wasn't working */
}

uint8_t check_exit_gameover(void){
	return curr_posX > 240;
}

uint8_t check_gameover(void){
	return gameover;
}

uint8_t check_status(void){
	return (status == 0);
}

uint8_t gotoGameover(void){
	if(gameover == 2)
		return 1;
	else if (gameover == 1){
		Tamagotchi_gameover();
		return 1;
	}
	return 0;
}

uint8_t checkAreaTP(void){
	if(display.x >= curr_posX && display.x <= curr_posX + 60){
		if(display.y >= posY && display.y <= posY + 94){
			return 1;
		}
		return 0;
	}		
	return 0;
}
/*from now on all the functions that play the songs of the game*/

void playSnack(void){
	uint16_t noteNumber = (sizeof(snack)/sizeof(snack[0]));
	ticks = 0;
	while(currentNote < noteNumber){
		if(!isNotePlaying())
			{
				++ticks;
				if(ticks == UPTICKS)
				{
					ticks = 0;
					playNote(snack[currentNote++]);
				}
			}
		if(direction == 2 && check_exit()){
			break;
		}
		if(direction == 1 && !check_exit()){
			break;
		}
		if(gameover){
			break;
		}
	}
	if(currentNote == noteNumber)
		currentNote = 0;
}

void playMeal(void){
	uint16_t noteNumber = (sizeof(meal)/sizeof(meal[0]));
	ticks = 0;
	while(currentNote < noteNumber){
		if(!isNotePlaying())
			{
				++ticks;
				if(ticks == UPTICKS)
				{
					ticks = 0;
					playNote(meal[currentNote++]);
				}
			}
		if(direction == 2 && check_exit()){
			break;
		}
		if(direction == 1 && !check_exit()){
			break;
		}
		if(gameover){
			break;
		}
	}
	if(currentNote == noteNumber)
		currentNote = 0;
}

void playOver(void){
	uint16_t noteNumber = (sizeof(over)/sizeof(over[0]));
	ticks = 0;
	while(currentNote < noteNumber){
		if(!isNotePlaying())
			{
				++ticks;
				if(ticks == UPTICKS)
				{
					ticks = 0;
					playNote(over[currentNote++]);
				}
			}
	}
	if(currentNote == noteNumber)
		currentNote = 0;
}

void playCuddles(void){
	uint16_t noteNumber = (sizeof(cuddles)/sizeof(cuddles[0]));
	ticks = 0;
	while(currentNote < noteNumber){
		if(!isNotePlaying())
			{
				++ticks;
				if(ticks == UPTICKS)
				{
					ticks = 0;
					playNote(cuddles[currentNote++]);
				}
			}
		if(gameover){
			break;
		}
	}
	if(currentNote == noteNumber)
		currentNote = 0;
}

void playInit(void){
	uint16_t noteNumber = (sizeof(song)/sizeof(song[0]));
	ticks = 0;
	while(currentNote < noteNumber){
		if(!isNotePlaying())
			{
				++ticks;
				if(ticks == UPTICKS)
				{
					ticks = 0;
					playNote(song[currentNote++]);
				}
			}
	}
	currentNote = 0;
	init = 0;
}
