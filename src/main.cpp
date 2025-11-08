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
    // list.elements[4].value = 690;
    // list.elements[5].value = 600;
    // CHECK_STATUS( ListInsertAfter( &list, 6, 35 ) );
    // CHECK_STATUS( ListLinearizer( &list, 20 ) );
    

    ListDtor( &list );

    ON_DEBUG( ListLog( &list, END_OF_PROGRAM, nullptr ); )
}

// ListStatus_t test1() {
//     PRINT_EXECUTING;

//     List_t list = { INIT( list_test1 ) };

//     ListCtor( &list, 10 );

//     ListInsertAfter( &list, 0, 10 );
//     ListInsertAfter( &list, 1, 20 );
//     ListInsertAfter( &list, 2, 30 );
//     ListInsertAfter( &list, 3, 40 );
//     ListInsertAfter( &list, 4, 50 );
//     ListInsertAfter( &list, 5, 60 );
//     ListInsertAfter( &list, 3, 35 );
//     ListDelete( &list, 6 );
//     list.elements[5].next = 690;
//     list.elements[3].prev = 600;
//     ListInsertAfter( &list, 6, 35 );

//     if ( list.var_info.error_code == LIST_ERR_NONE ) {
//         ListDtor( &list );

//         PRINT_STATUS_OK;
//         return SUCCESS;
//     }
//     else {
//         ListDtor( &list );

//         PRINT_STATUS_FAIL;
//         return FAIL;
//     }
// }