
#include <stdlib.h>

#include "ADTVector.h"
#include "ADTList.h"
#include "state.h"


// Οι ολοκληρωμένες πληροφορίες της κατάστασης του παιχνιδιού.
// Ο τύπος State είναι pointer σε αυτό το struct, αλλά το ίδιο το struct
// δεν είναι ορατό στον χρήστη.

struct state {
	Vector objects;			// περιέχει στοιχεία Object (Πλατφόρμες, Αστέρια)
	struct state_info info;	// Γενικές πληροφορίες για την κατάσταση του παιχνιδιού
	float speed_factor;		// Πολλαπλασιαστής ταχύτητς (1 = κανονική ταχύτητα, 2 = διπλάσια, κλπ)
	Object current_platform;
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
		vector_insert_last(state->objects, platform);

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
			vector_insert_last(state->objects, star);
		}

		start_x = platform->rect.x + platform->rect.width;	// μετακίνηση των επόμενων αντικειμένων προς τα δεξιά
	}
}

// Δημιουργεί και επιστρέφει την αρχική κατάσταση του παιχνιδιού

State state_create() {
	// Δημιουργία του state
	State state = malloc(sizeof(*state));

	// Γενικές πληροφορίες
	state->info.playing = false;				// Το παιχνίδι ξεκινάει αμέσως
	state->info.paused = true;				// Χωρίς να είναι paused.
	state->speed_factor = 1;				// Κανονική ταχύτητα
	state->info.score = 0;				// Αρχικό σκορ 0

	// Δημιουργούμε το vector των αντικειμένων, και προσθέτουμε αντικείμενα
	// ξεκινώντας από start_x = 0.
	state->objects = vector_create(0, NULL);
	add_objects(state, 0);

	// Δημιουργούμε την μπάλα τοποθετώντας τη πάνω στην πρώτη πλατφόρμα
	Object first_platform = vector_get_at(state->objects, 0);

	// Αποθηκεύουμε πάντα την επόμενη πλατφόρμα
	state->current_platform = first_platform;
	state->info.ball = create_object(
		BALL,
		first_platform->rect.x,			// x στην αρχή της πλατφόρμας
		first_platform->rect.y - 40,	// y πάνω στην πλατφόρμα
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
	for(VectorNode node = vector_first(state->objects);
		node != VECTOR_EOF;
		node = vector_next(state->objects,node)){

		Object object = vector_node_value(state->objects, node);
		if(object->rect.x >= x_from && object->rect.x <= x_to)
			list_insert_next(objects, list_last(objects), object);
	}
	
	return objects;
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
			state->current_platform = new_state->current_platform;
			state->info = new_state->info;
			vector_destroy(state->objects);
			state->objects = new_state->objects;
			state->speed_factor = new_state->speed_factor;
			state_update(state, keys);
		}
	}
	// Game is not paused. Running normally
	// Or N is pressed so we go into debug mode.
	else if(  (state->info.playing && !state->info.paused)  ||  keys->n ){
		// Pause the game
		if(keys->p)
			state->info.paused = true;

        // Never Ending
		Object last_platform = vector_node_value(state->objects, vector_last(state->objects));
		if(last_platform->rect.x - state->info.ball->rect.x <= SCREEN_WIDTH){
			add_objects(state, last_platform->rect.x);
			state->speed_factor = 1.1 * state->speed_factor;
		}

		float speed = state->speed_factor;

		// Ball movement
	
		// Horizontal
		state->info.ball->rect.x += speed * 4.0;
		if(keys->right) 
			state->info.ball->rect.x += speed * 2.0;
		if(keys->left)  
			state->info.ball->rect.x -= speed * 3.0;

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
			
			if(state->info.ball->vert_speed > 7.0)
				state->info.ball->vert_speed = 7.0;
			
			// We search for the next platform.
			List close_objects = state_objects(state, state->info.ball->rect.x - 200.0, state->info.ball->rect.x + 200.0);
			for(ListNode node = list_first(close_objects);
				node != LIST_EOF;
				node = list_next(close_objects, node)){

				Object object = list_node_value(close_objects, node);
				if(object->type == 	PLATFORM && CheckCollisionRecs(state->info.ball->rect, object->rect) ){
					// In case of collision we set it as current platform and ball goes into IDLE.
					state->current_platform = object;
					state->info.ball->vert_mov = IDLE;
					state->info.ball->vert_speed = 0;
					// If we collide with an unstable platform it starts falling.
					if(object->unstable)
						object->vert_mov = FALLING;
				} 
			}
		}
		// IDLE, meaning on a platform
		else if(state->info.ball->vert_mov == IDLE){

			// Αν είναι idle θέλουμε το y του Ball να είναι ίσο με current_platform.y - 40
			state->info.ball->rect.y = state->current_platform->rect.y - 45.0;

			// Αν η μπάλα γλιστρίσει από το current_platform μπαίνει σε πτώση
			if(state->info.ball->rect.x > state->current_platform->rect.x + state->current_platform->rect.width){
				state->info.ball->vert_mov = FALLING;
				state->info.ball->vert_speed = 1.5;
			}

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
					// If a falling platform reaches the end of screen it gets removed from the vector.
					// To remove the platform, we place it at the last position of the vector and remove it.
					// But to retain the proper order in the vector, we shift all the objects from the platform
					// to the end, by one position.
					if(object->rect.y >= SCREEN_HEIGHT){
						// We find the index of object in the state->objects vector.
						int index;
						for(index = 0; index < vector_size(state->objects); index++){
							if(vector_get_at(state->objects, index) == object)
								break;
						}
						// We shift all the objects from that index one position.
						for(int i = index; i < vector_size(state->objects) - 1; i++)
							vector_set_at(state->objects,i, vector_get_at(state->objects,i+1));
						// We remove the last object which will be the platform.
						vector_remove_last(state->objects);
					}else
						object->rect.y += speed * 4.0;
				}
			}
			else if(object->type == STAR){
				// In case of collision with a star we remove it from the vector with the same technique as above.
				if(CheckCollisionRecs(state->info.ball->rect, object->rect)){
					state->info.score += 10;
					// We find the index of object in the state->objects vector.
					int index;
					for(index = 0; index < vector_size(state->objects); index++){
						if(vector_get_at(state->objects, index) == object)
							break;
					}
					// We shift all the objects from that index one position.
					for(int i = index; i < vector_size(state->objects) - 1; i++)
						vector_set_at(state->objects,i, vector_get_at(state->objects,i+1));
					// We remove the last object which will be the star.
					vector_remove_last(state->objects);
				}
			}
		}
	}
}

// Καταστρέφει την κατάσταση state ελευθερώνοντας τη δεσμευμένη μνήμη.

void state_destroy(State state) {
	// Προς υλοποίηση
	free(state);
}