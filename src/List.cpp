#include <stdio.h>

#include "List.h"
#include "DebugUtils.h"

ListStatus_t ListCtor( List_t* list ) {
    PRINT_IN;
    my_assert( list, "Null pointer on list" );


    PRINT_OUT;
}

ListStatus_t ListDtor( List_t* list ) {
    PRINT_IN;
    my_assert( list, "Null pointer on list" );

    
    PRINT_OUT;
}

ListStatus_t ListInsert( List_t* list ) {
    PRINT_IN;
    my_assert( list, "Null pointer on list" );


    PRINT_OUT;
}

ListStatus_t ListDelete( List_t* list ) {
    PRINT_IN;
    my_assert( list, "Null pointer on list" );

    PRINT_OUT;
}