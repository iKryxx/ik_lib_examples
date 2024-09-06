#include "ik_lib.h"
#include <time.h>
int x = 0;

#define NORTH ((u8)0)
#define EAST ((u8)1)
#define SOUTH ((u8)2)
#define WEST ((u8)3)

#define BODY ((u8) 0)
#define TAIL ((u8) 1)
#define HEAD ((u8) 2)

typedef struct {
	u8 type;	//0: body 1: tail 2: head
	u8 dir;		//0: North 1: East 2: South 3: West
	u8 x, y;
} snake_body;
typedef enum {
	PLAYING,
	GAMEOVER,
	PAUSE
}GAMESTATE;
typedef struct {
	u8 x; u8 y;
}coord;

void init_snake(ik_array *snake);
void update_direction();
void update_snake();
void grow_snake();
void check_collisions();
void fill_border();
void update_valid_food_spawns();

ik_array snake;
int next_dir = 1;
int score = 0;
GAMESTATE state;
ik_random random;

coord current_Food;

ik_string GAME_OVER_TEXT = { };
ik_string PRESS_Q_TO_EXIT = { };
ik_string SCORE = { };
clock_t t;

int main() {

	ik_random_init(&random, time(NULL));
	ik_string_make(&GAME_OVER_TEXT, "Game Over!");
	ik_string_make(&PRESS_Q_TO_EXIT, "Press Q to exit!");
	ik_screen_init(40, 20, ' ', 5);
	ik_init_input();
	ik_set_input_type(keyboardhit);
	init_snake(&snake);
	for (;;) {
		ik_screen_clear_screen();
		fill_border();
		t = clock();
		if (SCREEN_UPDATE) {
			ik_string_destroy(&SCORE);
			ik_string_make(&SCORE, "Your Score: ");
			char scorechar[10];
			sprintf(scorechar, "%i", score);
			ik_string_append(&SCORE, scorechar);

			check_collisions();
			ik_update_input();
			if (state == PLAYING) {
				update_direction();
				update_snake();
				if (state == GAMEOVER) continue;
				ik_screen_set_pixel(current_Food.x, current_Food.y, '#', yellow, yellow);
				for (size_t i = 0; i < snake.size; i++)
				{
					snake_body* _curr = (snake_body*)ik_array_get(&snake, i);
					ik_screen_set_pixel(_curr->x, _curr->y, '#', red, none);
				}
				for (size_t i = 0; i < SCORE.size; i++)
				{
					ik_screen_set_pixel(
						SCREEN_WIDTH / 2 - SCORE.size / 2 + i,
						SCREEN_HEIGHT - 1,
						SCORE.cstring[i],
						none,
						none
					);
				}
			}
			else if (state == GAMEOVER) {
				for (size_t i = 0; i < GAME_OVER_TEXT.size; i++)
				{
					ik_screen_set_pixel(
						SCREEN_WIDTH / 2 - GAME_OVER_TEXT.size / 2 + i, 
						SCREEN_HEIGHT / 2, 
						GAME_OVER_TEXT.cstring[i], 
						red, 
						none
					);
				}
				for (size_t i = 0; i < SCORE.size; i++)
				{
					ik_screen_set_pixel(
						SCREEN_WIDTH / 2 - SCORE.size / 2 + i,
						SCREEN_HEIGHT / 2 + 1,
						SCORE.cstring[i],
						red,
						none
					);
				}
				for (size_t i = 0; i < PRESS_Q_TO_EXIT.size; i++)
				{
					ik_screen_set_pixel(
						SCREEN_WIDTH / 2 - PRESS_Q_TO_EXIT.size / 2 + i,
						SCREEN_HEIGHT / 2 + 2,
						PRESS_Q_TO_EXIT.cstring[i],
						red,
						none
					);
				}
				if (ik_get_key_state('Q', pressed)) exit(0);
			}
			ik_screen_print();
		}
	}
	
}

void init_snake(ik_array *snake) {
	current_Food = { };
	ik_array_make(snake, 4 * sizeof(u8), 10);
	snake_body head = { 2, 1, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
	snake_body body = { 0, 1, SCREEN_WIDTH / 2 - 1, SCREEN_HEIGHT / 2 };
	snake_body tail = { 1, 1, SCREEN_WIDTH / 2 - 2, SCREEN_HEIGHT / 2 };
	ik_array_append(snake, &head);
	ik_array_append(snake, &body);
	ik_array_append(snake, &tail);
	update_valid_food_spawns();
	state = PLAYING;
}
void update_direction() {
	if (ik_get_key_state('T', pressed)) grow_snake();

	if (ik_get_key_state('W', pressed) && next_dir != 2) next_dir = NORTH;
	else if (ik_get_key_state('A', pressed) && next_dir != 1) next_dir = WEST;
	else if (ik_get_key_state('S', pressed) && next_dir != 0) next_dir = SOUTH;
	else if (ik_get_key_state('D', pressed) && next_dir != 3) next_dir = EAST;
}
void update_snake() {
	for (i32 i = snake.size - 1; i >= 0; i--)
	{
		snake_body* _curr = (snake_body*)ik_array_get(&snake, i);
		snake_body* _next = (snake_body*)ik_array_get(&snake, ik_max(i - 1, 0));
		
		if (_curr->type == TAIL) { //tail
			_curr->dir = _next->dir;
		}
		else if (_curr->type == BODY) { //body
			_curr->dir = _next->dir;
		}
		if (_curr->type == HEAD) { //head
			_curr->dir = next_dir;
		}
		if (_curr->dir == NORTH) _curr->y--;
		else if (_curr->dir == EAST) _curr->x++;
		else if (_curr->dir == SOUTH) _curr->y++;
		else if (_curr->dir == WEST) _curr->x--;
		
	}
}
void grow_snake() {
	score += 100;
	snake_body* old_tail;
	snake_body new_tail = { };
	old_tail = (snake_body*)ik_array_get(&snake, snake.size - 1);

	new_tail.dir = old_tail->dir;
	new_tail.type = TAIL;
	old_tail->type = BODY;
	if (old_tail->dir == NORTH) {
		new_tail.y = old_tail->y + 1;
		new_tail.x = old_tail->x;
	}
	else if (old_tail->dir == EAST) {
		new_tail.y = old_tail->y;
		new_tail.x = old_tail->x - 1;
	}
	else if (old_tail->dir == SOUTH) {
		new_tail.y = old_tail->y - 1;
		new_tail.x = old_tail->x;
	}
	else if (old_tail->dir == WEST) {
		new_tail.y = old_tail->y;
		new_tail.x = old_tail->x + 1;
	}
	ik_array_append(&snake, &new_tail);
}
void check_collisions() {
	snake_body *head;
	head = (snake_body*)ik_array_get(&snake, 0);

	for (size_t i = 1; i < snake.size; i++)
	{
		snake_body* _curr;
		_curr = (snake_body*)ik_array_get(&snake, i);

		if ((head->x == _curr->x && _curr->y == head->y) || (head->x == SCREEN_WIDTH - 1 || head->x == 0 || head->y == SCREEN_HEIGHT - 2 || head->y == 0)) {
			state = GAMEOVER;
			return;
		}
		if (head->x == current_Food.x && head->y == current_Food.y) {
			current_Food = { };
			update_valid_food_spawns();
			grow_snake();
		}
	}
}
void fill_border() {
	for (size_t x = 0; x < SCREEN_WIDTH; x++)
	{
		ik_screen_set_pixel(x, 0, '#', red, red);
		ik_screen_set_pixel(x, SCREEN_HEIGHT - 2, '#', red, red);
	}
	for (size_t y = 1; y < SCREEN_HEIGHT - 1; y++)
	{
		ik_screen_set_pixel(0, y, '#', red, red);
		ik_screen_set_pixel(SCREEN_WIDTH - 1, y, '#', red, red);
	}
}
void update_valid_food_spawns() {
	ik_array valid_spots = { };
	ik_array_make(&valid_spots, 2 * sizeof(u8), 30);
	
	for (size_t x = 1; x < SCREEN_WIDTH - 1; x++)
	{
		for (size_t y = 1; y < SCREEN_HEIGHT - 2; y++)
		{
			for (size_t i = 0; i < snake.size; i++)
			{
				snake_body* _curr;
				_curr = (snake_body*)ik_array_get(&snake, i);
				if (x != _curr->x && y != _curr->y) {
					coord valid = { x ,y };
					ik_array_append(&valid_spots, &valid);
				}
			}
		}
	}
	int index = 0;
	ik_random_next(&random, &index);
	index %= valid_spots.size;
	
	coord* new_food = (coord*)ik_array_get(&valid_spots, index);
	current_Food.x = new_food->x;
	current_Food.y = new_food->y;

	ik_array_destroy(&valid_spots);
}