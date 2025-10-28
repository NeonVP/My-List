#include "List.h"

int main() {
    List_t list = { INIT( list ) };
    ON_DEBUG( ListLog( &list, BEGIN_OF_PROGRAMM ); )

    CHECK_STATUS( ListCtor( &list, 10 ) );
    ListLog( &list, DUMP );

    CHECK_STATUS( ListInsert( &list, 0, 10 ) );
    ListLog( &list, DUMP );
    CHECK_STATUS( ListInsert( &list, 1, 20 ) );
    ListLog( &list, DUMP );
    CHECK_STATUS( ListInsert( &list, 2, 30 ) );
    ListLog( &list, DUMP );
    CHECK_STATUS( ListInsert( &list, 3, 40 ) );
    ListLog( &list, DUMP );
    CHECK_STATUS( ListInsert( &list, 4, 50 ) );
    ListLog( &list, DUMP );
    CHECK_STATUS( ListInsert( &list, 5, 60 ) );
    ListLog( &list, DUMP );
    CHECK_STATUS( ListInsert( &list, 3, 35 ) );
    ListLog( &list, DUMP );
    ListDelete( &list, 6 );
    ListLog( &list, DUMP );

    ListDtor( &list );

    ON_DEBUG( ListLog( &list, END_OF_PROGRAM ); )
}