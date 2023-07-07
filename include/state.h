#pragma once

#include "raylib.h"
#include "ADTList.h"

#define PLATFORM_NUM 20		// πόσες πλατφόρμες δημιουργούνται στην πίστα
#define SCREEN_WIDTH 900	// Πλάτος της οθόνης
#define SCREEN_HEIGHT 450	// Υψος της οθόνης


typedef enum {
	IDLE, JUMPING, FALLING, MOVING_UP, MOVING_DOWN
} VerticalMovement;

// Πληροφορίες για κάθε αντικείμενο
typedef struct object {
	Rectangle rect;				// Θέση και μέγεθος του αντικειμένου. Το Rectangle ορίζεται στο include/raylib.h, line 213
	float rotation;				// Περιστροφή
	VerticalMovement vert_mov;	// Τύπος κατακόρυφης κίνησης
	float vert_speed;			// Κατακόρυφη ταχύτητα
}* Object;

// Γενικές πληροφορίες για την κατάσταση του παιχνιδιού
typedef struct state_info {
	Object car;					// πληροφορίες για τη μπάλα
	bool playing;					// true αν το παιχνίδι είναι ενεργό (false μετά από game over)
	bool paused;					// true αν το παιχνίδι είναι paused
}* StateInfo;

// Πληροφορίες για το ποια πλήκτρα είναι πατημένα
typedef struct key_state {
	bool up;						// true αν το αντίστοιχο πλήκτρο είναι πατημένο
	bool down;
	bool left;
	bool right;
	bool enter;
	bool n;
	bool p;
	bool q;
	bool w;
}* KeyState;

// Η κατάσταση του παιχνιδιού (handle)
typedef struct state* State;


// Δημιουργεί και επιστρέφει την αρχική κατάσταση του παιχνιδιού

State state_create();

// Επιστρέφει τις βασικές πληροφορίες του παιχνιδιού στην κατάσταση state

StateInfo state_info(State state);


// Ενημερώνει την κατάσταση state του παιχνιδιού μετά την πάροδο 1 frame.
// Το keys περιέχει τα πλήκτρα τα οποία ήταν πατημένα κατά το frame αυτό.

void state_update(State state, KeyState keys);

// Καταστρέφει την κατάσταση state ελευθερώνοντας τη δεσμευμένη μνήμη.

void state_destroy(State state);