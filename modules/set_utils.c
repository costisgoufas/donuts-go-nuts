#include "set_utils.h"
#include <stdlib.h>

int compare_ints(void* a, void* b){
    return *(int*)a - *(int*)b;
}

int* create_int(int value){
    int* pointer = malloc(sizeof(int));
    *pointer = value;
    return pointer;
}


// Επιστρέφει την μοναδική τιμή του set που είναι ισοδύναμη με value,
// ή αν δεν υπάρχει, την μικρότερη τιμή του set που είναι μεγαλύτερη
// από value. Αν δεν υπάρχει καμία τότε επιστρέφει NULL.

Pointer set_find_eq_or_greater(Set set, Pointer value){
    Pointer find = set_find(set,value);
    if(find != NULL)
        return find;
    else{
        set_insert(set,value);
        SetNode node = set_next(set,set_find_node(set,value));
        set_remove(set,value);
        return set_node_value(set,node);
    }
}

// Επιστρέφει την μοναδική τιμή του set που είναι ισοδύναμη με value,
// ή αν δεν υπάρχει, την μεγαλύτερη τιμή του set που είναι μικρότερη
// από value. Αν δεν υπάρχει καμία τότε επιστρέφει NULL.

Pointer set_find_eq_or_smaller(Set set, Pointer value){
    Pointer find = set_find(set,value);
    if(find != NULL)
        return find;
    else{
        set_insert(set,value);
        SetNode node = set_previous(set, set_find_node(set,value) );
        set_remove(set,value);
        return set_node_value(set,node);
    }
}