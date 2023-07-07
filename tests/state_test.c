//////////////////////////////////////////////////////////////////
//
// Test για το state.h module
//
//////////////////////////////////////////////////////////////////

#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing

#include "state.h"
#include "ADTVector.h"
#include "set_utils.h"

void test_state_create() {

	State state = state_create();
	TEST_ASSERT(state != NULL);

	StateInfo info = state_info(state);
	TEST_ASSERT(info != NULL);

	TEST_ASSERT(!info->playing);
	TEST_ASSERT(info->paused);
	TEST_ASSERT(info->score == 0);

	// Προσθέστε επιπλέον ελέγχους
	List objects = state_objects(state, -100, SCREEN_WIDTH);
	for(ListNode node = list_first(objects);
		node != LIST_EOF;
		node = list_next(objects,node)){

		Object object = list_node_value(objects,node);
		TEST_CHECK(object->rect.x > -100 && object->rect.x < SCREEN_WIDTH);
	}
	TEST_ASSERT(list_size(objects) != 0);
}

void test_state_update() {
	State state = state_create();
	TEST_ASSERT(state != NULL && state_info(state) != NULL);

	// Πληροφορίες για τα πλήκτρα (αρχικά κανένα δεν είναι πατημένο)
	struct key_state keys = { false, false, false, false, false, false, false };

	keys.enter = true;
	state_update(state,&keys);
	keys.enter = false;
	
	// Χωρίς κανένα πλήκτρο, η μπάλα μετακινείται 4 pixels δεξιά
	Rectangle old_rect = state_info(state)->ball->rect;
	state_update(state, &keys);
	Rectangle new_rect = state_info(state)->ball->rect;

	TEST_ASSERT( new_rect.x == old_rect.x + 4 );

	// Με πατημένο το δεξί βέλος, η μπάλα μετακινείται 6 pixels δεξιά
	keys.right = true;
	old_rect = state_info(state)->ball->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->ball->rect;

	TEST_CHECK( new_rect.x == old_rect.x + 6 );

	// Με πατημένο το πάνω βέλος, η μπάλα μπαίνει σε JUMPING movement.
	keys.right = false;
	keys.up = true;
	state_update(state,&keys);
	VerticalMovement mov = state_info(state)->ball->vert_mov;

	TEST_CHECK( mov == JUMPING );

	keys.up = false;
	// Ελέγχουμε οτι μετα απο 1000 frames χωρις πατημένο κουμπί φτάνουμε σε game over.
	for(int i = 0; i < 1000; i++)
		state_update(state, &keys);


	TEST_ASSERT(state_info(state)->playing == false && state_info(state)->paused == false);
	
}

void test_set_utils(){
	Set set = set_create(compare_ints, NULL);
	for(int i = 0; i < 100; i++){
		int x = i + (rand() % 5);
		set_insert(set, create_int(x));
	}
	// Ψάχνουμε τον ίδιο ή αμέσως μικρότερο/μεγαλύτερο του τυχαίου x.
	int x = rand() % 50;
	printf("\nWe are looking for %d\n", x);
	
	int greater = *(int*)set_find_eq_or_greater(set, create_int(x));
	int smaller = *(int*)set_find_eq_or_smaller(set, create_int(x));

	printf("Greater = %d\nSmaller = %d\n", greater, smaller);
	printf("Is %d<=%d<=%d ?\n", smaller, x, greater);
	TEST_ASSERT(greater >= x && smaller <= x);
	
}


// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	{ "test_state_create", test_state_create },
	{ "test_state_update", test_state_update },
	{ "test_set_utils", test_set_utils },

	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};