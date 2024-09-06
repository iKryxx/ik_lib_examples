#include "ik_lib.h"

int main() {
	// initializes the screen with a width of 40px 
	// and a height of 20px, ' ' is the background 
	// character and the screen has a capped Update rate of 5
	// use -1 if you want to unlock the Update rate.
	ik_screen_init(40, 20, ' ', 5); 

	// this initializes the input handler. note that
	// ik_set_input_type() specifies if you want your
	// input to be handeled normaly (yoour input goes 
	// to the console) or like in games, where your 
	// input doesnt interfer with anything by default.

	ik_init_input();
	ik_set_input_type(keyboardhit);
	
	//this is the correct syntax for your game loop
	for (;;) {
		ik_screen_clear_screen();
		if (SCREEN_UPDATE) {
			//update the input handler
			ik_update_input();
			//set your pixels!
			ik_screen_set_pixel(0, 0, '#', none, none);
			//print your screen
			ik_screen_print();
		}
	}	
}