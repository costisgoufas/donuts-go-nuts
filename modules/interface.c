#include "raylib.h"
#include "state.h"
#include "set_utils.h"
#include "interface.h"
#include <stdio.h>

void interface_init() {
	// Αρχικοποίηση του παραθύρου
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "game");
	SetTargetFPS(60);
    InitAudioDevice();

}

void interface_close() {
	CloseAudioDevice();
	CloseWindow();
}

// Draw game (one frame)
void interface_draw_frame(State state) {
	BeginDrawing();

	// Καθαρισμός, θα τα σχεδιάσουμε όλα από την αρχή
	ClearBackground(BLUE);
	StateInfo info = state_info(state);

	// Game start.
	if(!info->playing && info->paused){
		ClearBackground(BLACK);
		DrawText("Press enter to start game", 180, 200, 40, RED);
	}
	else{
		DrawText(TextFormat("%d", info->score), 20, 20, 40, GRAY);

		// We look for the first platform in [149,231]
		List plat_search = state_objects(state, 149,231);
		Object first_platform = list_node_value(plat_search, list_first(plat_search) );
		// Θελουμε η μπαλα να μένει σε σταθερό x 
		Rectangle ball = info->ball->rect;
		ball.x = first_platform->rect.x;
		DrawRectangleRec(ball, ORANGE);
		DrawRectangle( - info->ball->rect.x + first_platform->rect.x, first_platform->rect.y, first_platform->rect.width, first_platform->rect.height, BLACK);

		List objects = state_objects(state, info->ball->rect.x - SCREEN_WIDTH, info->ball->rect.x + SCREEN_WIDTH);
		for(ListNode node = list_first(objects);
			node != LIST_EOF;
			node = list_next(objects,node)){

			Object object = list_node_value(objects,node);
			if(object->type == PLATFORM){
				if(object->unstable)
					DrawRectangle(object->rect.x - info->ball->rect.x + first_platform->rect.x, object->rect.y, object->rect.width, object->rect.height, RED);
				else
					DrawRectangle(object->rect.x - info->ball->rect.x + first_platform->rect.x, object->rect.y, object->rect.width, object->rect.height, BLACK);
			}
			else if(object->type == STAR)
				DrawRectangle(object->rect.x - info->ball->rect.x, object->rect.y, object->rect.width, object->rect.height, YELLOW);
		}

	}
	// Game over.
	if(info->playing == false && info->paused == false){
		Rectangle rec;
		rec.height = 120;
		rec.width = SCREEN_WIDTH;
		rec.x = 0;
		rec.y = SCREEN_HEIGHT/2 - 20;
		DrawRectangleRec(rec, RED);
		DrawText("You lost", 350 , SCREEN_HEIGHT/2 - 10, 40 , BLACK);
		DrawText("Press enter to play again", 185 , SCREEN_HEIGHT/2 + 40, 40 , BLACK);
	}
	// Paused.
	if(info->playing == true && info->paused == true){
		Rectangle rec;
		rec.height = 120;
		rec.width = SCREEN_WIDTH;
		rec.x = 0;
		rec.y = SCREEN_HEIGHT/2 - 20;
		DrawRectangleRec(rec, RED);
		DrawText("Paused", 350 , SCREEN_HEIGHT/2 - 10, 40 , BLACK);
		DrawText("Press P to continue", 185 , SCREEN_HEIGHT/2 + 40, 40 , BLACK);
	}
	
	
	EndDrawing();
}