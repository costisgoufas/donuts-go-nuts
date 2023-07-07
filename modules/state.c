
#include <stdlib.h>

#include "ADTVector.h"
#include "ADTList.h"
#include "state.h"


// Οι ολοκληρωμένες πληροφορίες της κατάστασης του παιχνιδιού.
// Ο τύπος State είναι pointer σε αυτό το struct, αλλά το ίδιο το struct
// δεν είναι ορατό στον χρήστη.

struct state {
	struct state_info info;	// Γενικές πληροφορίες για την κατάσταση του παιχνιδιού
	Object current_platform;
};


// Δημιουργεί και επιστρέφει ένα αντικείμενο

static Object create_object(float x, float y, float width, float height, float rotation ,VerticalMovement vert_mov, float speed) {
	Object obj = malloc(sizeof(*obj));
	obj->rect.x = x;
	obj->rect.y = y;
	obj->rect.width = width;
	obj->rect.height = height;
	obj->rotation = rotation;
	obj->vert_mov = vert_mov;
	obj->vert_speed = speed;
	return obj;
}

// Προσθέτει αντικείμενα στην πίστα (η οποία μπορεί να περιέχει ήδη αντικείμενα).
// Τα αντικείμενα ξεκινάνε από x = start_x, και επεκτείνονται προς τα δεξιά.
//
// ΠΡΟΣΟΧΗ: όλα τα αντικείμενα έχουν συντεταγμένες x,y σε ένα
// καρτεσιανό επίπεδο.
// - Στο άξονα x το 0 είναι η αρχή της πίστας και οι συντεταγμένες
//   μεγαλώνουν προς τα δεξιά.
//
// - Στον άξονα y το 0 είναι το πάνω μέρος της πίστας, και οι συντεταγμένες μεγαλώνουν
//   προς τα _κάτω_. Η πίστα έχει σταθερό ύψος, οπότε όλες οι
//   συντεταγμένες y είναι ανάμεσα στο 0 και το SCREEN_HEIGHT.
//
// Πέρα από τις συντεταγμένες, αποθηκεύουμε και τις διαστάσεις width,height
// κάθε αντικειμένου. Τα x,y,width,height ορίζουν ένα παραλληλόγραμμο, οπότε
// μπορούν να αποθηκευτούν όλα μαζί στο obj->rect τύπου Rectangle (ορίζεται
// στο include/raylib.h). Τα x,y αναφέρονται στην πάνω-αριστερά γωνία του Rectangle.


// Δημιουργεί και επιστρέφει την αρχική κατάσταση του παιχνιδιού

State state_create() {
	// Δημιουργία του state
	State state = malloc(sizeof(*state));

	// Γενικές πληροφορίες
	state->info.playing = false;				// Το παιχνίδι ξεκινάει αμέσως
	state->info.paused = true;				// Χωρίς να είναι paused.


	// Δημιουργούμε την μπάλα τοποθετώντας τη πάνω στην πρώτη πλατφόρμα
	
	state->info.car = create_object(
		SCREEN_WIDTH/2,			// x στην αρχή της πλατφόρμας
		SCREEN_HEIGHT/2,				// y πάνω στην πλατφόρμα
		55, 80,							// πλάτος, ύψος
		0,								// ξεκινάει κατακόρυφα
		IDLE,							// χωρίς αρχική κατακόρυφη κίνηση
		0								// αρχική ταχύτητα 0
	);

	return state;
}

// Επιστρέφει τις βασικές πληροφορίες του παιχνιδιού στην κατάσταση state

StateInfo state_info(State state) {
	StateInfo info = malloc(sizeof(StateInfo));
	info->car = state->info.car;
	info->paused = state->info.paused;
	info->playing = state->info.playing;
	return info;
}


// Ενημερώνει την κατάσταση state του παιχνιδιού μετά την πάροδο 1 frame.
// Το keys περιέχει τα πλήκτρα τα οποία ήταν πατημένα κατά το frame αυτό.

void state_update(State state, KeyState keys) {
	// Game start. We prompt the player to press enter
	if(!state->info.playing && state->info.paused){
		if(keys->enter){
			state->info.playing = true;
			state->info.paused = false;
		}
	}
	// Game is paused. Enter or P continues the game
	else if(state->info.playing && state->info.paused){
		if(keys->enter || keys->p)
			state->info.paused = false;
	}
	// Game is over and ENTER is pressed.
	else if(!state->info.playing && !state->info.paused){
		if(keys->enter){
			State new_state = state_create();
			state->info = new_state->info;
			state_update(state, keys);
		}
	}
	// Game is not paused. Running normally
	// Or N is pressed so we go into debug mode.
	else if(  (state->info.playing && !state->info.paused)  ||  keys->n ){
		// Pause the game
		if(keys->p)
			state->info.paused = true;

		if(keys->down)
			state->info.car->rect.y += 5;
		if(keys->up)
			state->info.car->rect.y -= 5;
		if(keys->left)
			state->info.car->rect.x -= 5;
		if(keys->right)
			state->info.car->rect.x += 5;
		if(keys->w)
			state->info.car->rotation += 5;
		if(keys->q)
			state->info.car->rotation -= 5;
        
	}
}

// Καταστρέφει την κατάσταση state ελευθερώνοντας τη δεσμευμένη μνήμη.

void state_destroy(State state) {
	// Προς υλοποίηση
	free(state);
}