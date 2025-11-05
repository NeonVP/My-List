#include "List.h"

int main() {
    List_t list = { INIT( list ) };
    ON_DEBUG( ListLog( &list, BEGIN_OF_PROGRAMM, nullptr ); )

    CHECK_STATUS( ListCtor( &list, 10 ) );

    CHECK_STATUS( ListInsertAfter( &list, 0, 10 ) );
    CHECK_STATUS( ListInsertAfter( &list, 1, 20 ) );
    CHECK_STATUS( ListInsertAfter( &list, 2, 30 ) );
    CHECK_STATUS( ListInsertAfter( &list, 3, 40 ) );
    CHECK_STATUS( ListInsertAfter( &list, 4, 50 ) );
    CHECK_STATUS( ListInsertAfter( &list, 5, 60 ) );
    CHECK_STATUS( ListInsertAfter( &list, 3, 35 ) );
    CHECK_STATUS( ListDelete( &list, 6 ) );
    list.elements[5].next = 1;
    list.elements[9].next = 8;
    list.elements[1].prev = 5;
    CHECK_STATUS( ListInsertAfter( &list, 6, 35 ) );

    ListDtor( &list );

    ON_DEBUG( ListLog( &list, END_OF_PROGRAM, nullptr ); )
}