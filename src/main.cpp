#include "List.h"

int main() {
    List_t list = { INIT( list ) };
    ListCtor( &list, 5 );
    ListLog( &list, BEGIN_OF_PROGRAMM );

    ListInsert( &list, 1, 10 );
    ListLog( &list, DUMP );
    ListInsert( &list, 2, 20 );
    ListLog( &list, DUMP );
    ListInsert( &list, 3, 30 );
    ListLog( &list, DUMP );
    ListDelete( &list, 2 );
    ListLog( &list, DUMP );

    ListLog( &list, END_OF_PROGRAM );

    ListDtor( &list );
}