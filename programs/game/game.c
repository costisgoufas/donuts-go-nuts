#include <stdio.h>
#include "raylib.h"

#include "interface.h"
#include "state.h"

State state;

void update_and_draw() {
    struct key_state keys = { false, false, false, false, false, false, false };
	if(IsKeyDown(KEY_UP)) keys.up = true;
	if(IsKeyDown(KEY_DOWN)) keys.down = true;
	if(IsKeyDown(KEY_RIGHT)) keys.right = true;
	if(IsKeyDown(KEY_LEFT)) keys.left = true;
	if(IsKeyDown(KEY_ENTER)) keys.enter = true;
	if(IsKeyDown(KEY_P)) keys.p = true;
	if(IsKeyDown(KEY_N)) keys.n = true;
	if(IsKeyDown(KEY_Q)) keys.q = true;
	if(IsKeyDown(KEY_W)) keys.w = true;

	state_update(state,&keys);
	interface_draw_frame(state);
}

int main() {
	state = state_create();
	interface_init();

	// Η κλήση αυτή καλεί συνεχόμενα την update_and_draw μέχρι ο χρήστης να κλείσει το παράθυρο
	start_main_loop(update_and_draw);

	interface_close();

	return 0;
}



