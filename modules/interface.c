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
	ClearBackground(WHITE);
	StateInfo info = state_info(state);

	// Game start.
	if(!info->playing && info->paused){
		ClearBackground(BLACK);
		DrawText("Press enter to start game", 180, 200, 40, RED);
	}
	else{
		// DrawRectangleRec(info->car->rect, BLACK);
		
		Vector2 origin;
		origin.x = 0;
		origin.y = 0;
		DrawRectanglePro(info->car->rect, origin, info->car->rotation, BLACK);
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
		DrawText("Paused\nPress p to continue", SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 30, BLUE);
	}

	EndDrawing();
}