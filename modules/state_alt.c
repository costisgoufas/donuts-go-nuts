#include <stdio.h>
#include <stdlib.h>

#include "ADTVector.h"
#include "ADTList.h"
#include "ADTSet.h"
#include "set_utils.h"
#include "state.h"


// Οι ολοκληρωμένες πληροφορίες της κατάστασης του παιχνιδιού.
// Ο τύπος State είναι pointer σε αυτό το struct, αλλά το ίδιο το struct
// δεν είναι ορατό στον χρήστη.

struct state {
	Set objects;			// περιέχει στοιχεία Object (Πλατφόρμες, Αστέρια)
	struct state_info info;	// Γενικές πληροφορίες για την κατάσταση του παιχνιδιού
	float speed_factor;		// Πολλαπλασιαστής ταχύτητς (1 = κανονική ταχύτητα, 2 = διπλάσια, κλπ)
	Object current_platform; // 
};


// Δημιουργεί και επιστρέφει ένα αντικείμενο

static Object create_object(ObjectType type, float x, float y, float width, float height, VerticalMovement vert_mov, float speed, bool unstable) {
	Object obj = malloc(sizeof(*obj));
	obj->type = type;
	obj->rect.x = x;
	obj->rect.y = y;
	obj->rect.width = width;
	obj->rect.height = height;
	obj->vert_mov = vert_mov;
	obj->vert_speed = speed;
	obj->unstable = unstable;
	return obj;
}

int compare_objects(void* one, void* two){
    Object a = one;
    Object b = two;
    float ax = a->rect.x;
    float ay = a->rect.y;
    float bx = b->rect.x;
    float by = b->rect.y;
    if(ax != bx) return ax-bx;
    else return by - ay; 
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

static void add_objects(State state, float start_x) {
	// Προσθέτουμε PLATFORM_NUM πλατφόρμες, με τυχαία χαρακτηριστικά.

	for (int i = 0; i < PLATFORM_NUM; i++) {
		Object platform = create_object(
			PLATFORM,
			start_x + 150 + rand() % 80,						// x με τυχαία απόσταση από το προηγούμενο στο διάστημα [150, 230]
			SCREEN_HEIGHT/4 + rand() % SCREEN_HEIGHT/2,			// y τυχαία στο διάστημα [SCREEN_HEIGHT/4, 3*SCREEN_HEIGHT/4]
			i == 0 ? 250 : 50 + rand()%200,						// πλάτος τυχαία στο διάστημα [50, 250] (η πρώτη πάντα 250)
			20,													// ύψος
			i < 3 || rand() % 2 == 0 ? MOVING_UP : MOVING_DOWN,	// τυχαία αρχική κίνηση (οι πρώτες 3 πάντα πάνω)
			0.6 + 3*(rand()%100)/100,							// ταχύτητα τυχαία στο διάστημα [0.6, 3.6]
			i > 0 && (rand() % 10) == 0							// το 10% (τυχαία) των πλατφορμών είναι ασταθείς (εκτός από την πρώτη)
		);
		set_insert(state->objects, platform);

		// Στο 50% των πλατφορμών (τυχαία), εκτός της πρώτης, προσθέτουμε αστέρι
		if(i != 0 && rand() % 2 == 0) {
			Object star = create_object(
				STAR,
				start_x + 200 + rand() % 60,				// x με τυχαία απόσταση από την προηγούμενη πλατφόρμα στο διάστημα [200,260]
				SCREEN_HEIGHT/8 + rand() % SCREEN_HEIGHT/2,	// y τυχαία στο διάστημα [SCREEN_HEIGHT/8, 5*SCREEN_HEIGHT/8]
				30, 30,										// πλάτος, ύψος
				IDLE,										// χωρίς κίνηση
				0,										 	// ταχύτητα 0
				false										// 'unstable' πάντα false για τα αστέρια
			);
			set_insert(state->objects, star);
		}

		start_x = platform->rect.x + platform->rect.width;	// μετακίνηση των επόμενων αντικειμένων προς τα δεξιά
	}
}

// Δημιουργεί και επιστρέφει την αρχική κατάσταση του παιχνιδιού

State state_create() {
	// Δημιουργία του state
	State state = malloc(sizeof(*state));

	// Γενικές πληροφορίες
	state->info.playing = false;			// Το παιχνίδι ξεκινάει not playing, paused
	state->info.paused = true;				// Και περιμένουμε να πατήσει enter ο παίχτης
	state->speed_factor = 1;				// Κανονική ταχύτητα
	state->info.score = 0;				// Αρχικό σκορ 0

	// Δημιουργούμε το vector των αντικειμένων, και προσθέτουμε αντικείμενα
	// ξεκινώντας από start_x = 0.
	state->objects = set_create(compare_objects, NULL);
	add_objects(state, 0);

	// Δημιουργούμε την μπάλα τοποθετώντας τη πάνω στην πρώτη πλατφόρμα
	Object first_platform = set_node_value(state->objects, set_first(state->objects) );
	state->current_platform = first_platform;
	state->info.ball = create_object(
		BALL,
		first_platform->rect.x,			// x στην αρχή της πλατφόρμας
		first_platform->rect.y - 45,	// y πάνω στην πλατφόρμα
		45, 45,							// πλάτος, ύψος
		IDLE,							// χωρίς αρχική κατακόρυφη κίνηση
		0,								// αρχική ταχύτητα 0
		false							// "unstable" πάντα false για την μπάλα
	);

	return state;
}

// Επιστρέφει τις βασικές πληροφορίες του παιχνιδιού στην κατάσταση state

StateInfo state_info(State state) {
	StateInfo info = malloc(sizeof(StateInfo));
	info->ball = state->info.ball;
	info->paused = state->info.paused;
	info->playing = state->info.playing;
	info->score = state->info.score;
	info->current_platform = state->current_platform;
	return info;
}

// Επιστρέφει μια λίστα με όλα τα αντικείμενα του παιχνιδιού στην κατάσταση state,
// των οποίων η συντεταγμένη x είναι ανάμεσα στο x_from και x_to.

List state_objects(State state, float x_from, float x_to) {
	List objects = list_create(NULL);
    Object first = set_find_eq_or_greater(state->objects, create_object(PLATFORM, x_from, SCREEN_HEIGHT, 1,1, IDLE, 1, false) );
    Object last = set_find_eq_or_smaller(state->objects, create_object(PLATFORM, x_to, 0, 1, 1, IDLE, 1, false) );
    list_insert_next(objects, list_last(objects), first);
	if(first == last)
		return objects;
    for(SetNode node = set_next(state->objects, set_find_node(state->objects, first));
        node != set_find_node(state->objects, last);
        node = set_next(state->objects, node) ) {

		Object object = set_node_value(state->objects, node);
		if(object != NULL)
	        list_insert_next(objects, list_last(objects), object);
    }
    list_insert_next(objects, list_last(objects), last);
    return objects;
}

// Ενημερώνει την κατάσταση state του παιχνιδιού μετά την πάροδο 1 frame.
// Το keys περιέχει τα πλήκτρα τα οποία ήταν πατημένα κατά το frame αυτό.

void state_update(State state, KeyState keys) {
	// Game start. We prompt the player to press enter
	if(state->info.playing == false && state->info.paused == true){
		if(keys->enter){
			state->info.playing = true;
			state->info.paused = false;
		}
	}
	// Game is paused. Enter or P continues the game
	else if(state->info.playing == true && state->info.paused == true){
		if(keys->enter || keys->p)
			state->info.paused = false;
	}
	// Game is over and enter is pressed.
	else if(state->info.playing == false && state->info.paused == false){
		if(keys->enter){		
			State new_state = state_create();		
			state->current_platform = new_state->current_platform;
			state->info = new_state->info;
			set_destroy(state->objects);
			state->objects = new_state->objects;
			state->speed_factor = new_state->speed_factor;
			state_update(state, keys);
		}
	}
	// Running normally
	// Or N is pressed so we go into debug mode.
	else if(  (state->info.playing == true && state->info.paused == false)  ||  keys->n ){
		// Pause the game
		if(keys->p)
			state->info.paused = true;

        // Never Ending
		Object last_platform = set_node_value(state->objects, set_last(state->objects) );
		if(last_platform->rect.x - state->info.ball->rect.x <= SCREEN_WIDTH){
			add_objects(state, last_platform->rect.x);
			state->speed_factor = 1.1 * state->speed_factor;
		}

		float speed = state->speed_factor;

		// Star - Ball -> Collision Check
		Object star = set_find_eq_or_greater(state->objects, state->info.ball);
		if(star->type == STAR && CheckCollisionRecs(star->rect, state->info.ball->rect)){
			set_remove(state->objects, star);
			state->info.score += 10;
		}
		else{
			star = set_find_eq_or_smaller(state->objects, state->info.ball);
			if(star->type == STAR && CheckCollisionRecs(star->rect, state->info.ball->rect)){
				set_remove(state->objects, star);
				state->info.score += 10.0;
			}
		}
		// Ball movement
	
		// Horizontal
		state->info.ball->rect.x += speed*4.0;
		if(keys->right) 
			state->info.ball->rect.x += speed*2.0;
		if(keys->left)  
			state->info.ball->rect.x -= speed*3.0;

		// Vertical
		// JUMPING
		if(state->info.ball->vert_mov == JUMPING){
			
			state->info.ball->rect.y -= state->info.ball->vert_speed;
			state->info.ball->vert_speed = 0.85 * state->info.ball->vert_speed;
			if(state->info.ball->vert_speed <= 0.5){
				state->info.ball->vert_mov = FALLING;
			}
		}
		// FALLING
		else if(state->info.ball->vert_mov == FALLING){

			state->info.ball->rect.y += state->info.ball->vert_speed;
			state->info.ball->vert_speed = 1.1 * state->info.ball->vert_speed;
			
			// Terminal velocity.
			if(state->info.ball->vert_speed > 7.0)
				state->info.ball->vert_speed = 7.0;
			
			// Βρίσκουμε την current_platform
			Object platform = set_find_eq_or_smaller(state->objects, state->info.ball);
			if(platform->type != PLATFORM)
				platform = set_find_eq_or_smaller(state->objects, platform);
			state->current_platform = platform;

			// Ελέγχουμε αν υπάρχει σύγκρουση μεταξύ μπάλας και current platform
			if(CheckCollisionRecs(state->info.ball->rect, state->current_platform->rect) ){
				// Σε περίπτωση σύγκρουσης μεταξύ μπάλας και unstable πλατφόρμας, η πλατφόρμα μπαίνει
				// σε FALLING.
				if(platform->unstable)
					platform->vert_mov = FALLING;
				state->info.ball->vert_mov = IDLE;
				state->info.ball->vert_speed = 0;
			}
		}
		// IDLE, meaning on a platform
		else if(state->info.ball->vert_mov == IDLE){

			// Αν είναι idle θέλουμε το y του Ball να είναι ίσο με current_platform.y - 45
			state->info.ball->rect.y = state->current_platform->rect.y - 45.0;

			// Αν η μπάλα γλιστρίσει από το current_platform μπαίνει σε πτώση
			if(state->info.ball->rect.x > state->current_platform->rect.x + state->current_platform->rect.width){
				state->info.ball->vert_mov = FALLING;
				state->info.ball->vert_speed = 1.5;
			}

			// Α πατηθεί το up όσο είμαστε σε πλατφόρμα η μπάλα μπαίνει σε JUMPING
			if(keys->up){
				state->info.ball->vert_speed = 17.0;
				state->info.ball->vert_mov = JUMPING;

			}
		}
		
		// Ball has reached end of screen 
		// Game Over -> (playing,paused) = (false,false)
		if(state->info.ball->rect.y >= SCREEN_HEIGHT - 40)
			state->info.playing = false;


		// Platform Movement
        List close_objects = state_objects(state, state->info.ball->rect.x - SCREEN_WIDTH, state->info.ball->rect.x + 2*SCREEN_WIDTH);
		for(ListNode node = list_first(close_objects);
            node != LIST_EOF;
            node = list_next(close_objects, node)){

			Object object = list_node_value(close_objects, node);
			if(object->type == PLATFORM){
				if(object->vert_mov == MOVING_UP){
					object->rect.y -= speed * object->vert_speed;
					if(object->rect.y < SCREEN_HEIGHT/4)
						object->vert_mov = MOVING_DOWN;
				}
				else if(object->vert_mov == MOVING_DOWN){
					object->rect.y += speed * object->vert_speed;
					if(object->rect.y > 3*SCREEN_HEIGHT/4)
						object->vert_mov = MOVING_UP;
				}
				else{
					// Αν μία πλατφόρμα ξεπεράσει τα όρια της οθόνης αφαιρείται απο το σετ
					if(object->rect.y >= SCREEN_HEIGHT)
						set_remove(state->objects, object);
					else
						object->rect.y += speed * 4.0;
				}
			}
		}
	}
}


// Καταστρέφει την κατάσταση state ελευθερώνοντας τη δεσμευμένη μνήμη.

void state_destroy(State state) {
	free(state);

}