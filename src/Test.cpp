#include "Colors.h"
#include "DebugUtils.h"
#include "List.h"

void test1( List_t* list );
void test2( List_t* list );
void test3( List_t* list );

ListStatus_t ListDtor_test( List_t* list );

int main() {
    List_t list = { INIT( list ) };

    ListLog( &list, BEGIN_OF_PROGRAMM, nullptr );

    test1( &list );
    test2( &list );
    test3( &list );

    ListLog( &list, END_OF_PROGRAM, nullptr );

    free( list.logging.common_log_directory );
    free( list.logging.log_img_directory );
    free( list.logging.log_img_src_directory );
    free( list.logging.log_file_path );
}

void test1( List_t* list ) {
    PRINT( GRID COLOR_BRIGHT_BLUE "Test 1\n");

    // *list = { INIT( list_test1 ) };
    list->var_info = {
        .error_code = LIST_ERR_NONE,
        .name = "list_test1",
        .func = __func__,
        .line = __LINE__,
    };

    ListCtor( list, 10 );

    ListInsertAfter( list, 0, 10 );
    ListInsertAfter( list, 1, 20 );
    ListInsertAfter( list, 2, 30 );
    ListInsertAfter( list, 3, 40 );
    ListInsertAfter( list, 4, 50 );
    ListInsertAfter( list, 5, 60 );
    ListInsertAfter( list, 3, 35 );
    ListDelete( list, 6 );
    list->elements[5].next = 690;
    list->elements[3].prev = 600;

    PRINT( GRID "Status Test1 --- " );
    if ( ListVerify( list ) == FAIL_VERIFY && list->var_info.error_code == ( LIST_ERR_REVERSE_LACK_OF_ELEMENTS | LIST_ERR_STRAIGHT_LOOP ) ) {
        PRINT_STATUS_OK;
    }
    else {
        PRINT_STATUS_FAIL;
    }

    ListDtor_test( list );
}

void test2( List_t* list ) {
    PRINT( GRID COLOR_BRIGHT_BLUE "Test 2\n");

    // *list = { INIT( list_test2 ) };
    list->var_info = {
        .error_code = LIST_ERR_NONE,
        .name = "list_test2",
        .func = __func__,
        .line = __LINE__,
    };

    ListCtor( list, 10 );

    ListInsertAfter( list, 0, 10 );
    ListInsertAfter( list, 1, 20 );
    ListInsertAfter( list, 2, 30 );
    ListInsertAfter( list, 3, 40 );
    ListInsertAfter( list, 4, 50 );
    ListInsertAfter( list, 5, 60 );
    ListInsertAfter( list, 3, 35 );
    ListDelete( list, 6 );
    list->elements[5].next = 1;
    list->elements[9].next = 8;
    list->elements[1].prev = 5;

    PRINT( GRID "Status Test2 --- " );
    if ( ListVerify( list ) == FAIL_VERIFY && list->var_info.error_code == ( LIST_ERR_STRAIGHT_LOOP | LIST_ERR_REVERSE_LOOP | LIST_ERR_FREE_LOOP ) ) {
        PRINT_STATUS_OK;
    }
    else {
        PRINT_STATUS_FAIL;
    }

    ListDtor_test( list );
}

void test3( List_t* list ) {
    PRINT( GRID COLOR_BRIGHT_BLUE "Test 3\n");
    
    // *list = { INIT( list_test3 ) };
    list->var_info = {
        .error_code = LIST_ERR_NONE,
        .name = "list_test3",
        .func = __func__,
        .line = __LINE__,
    };

    ListCtor( list, 10 );

    ListInsertAfter( list, 0, 10 );
    ListInsertAfter( list, 1, 20 );
    ListInsertAfter( list, 2, 30 );
    ListInsertAfter( list, 3, 40 );
    ListInsertAfter( list, 4, 50 );
    ListInsertAfter( list, 5, 60 );
    ListInsertAfter( list, 3, 35 );
    ListDelete( list, 6 );

    list->elements[5].next = 690;
    list->elements[3].prev = 600;
    list->size = 0;

    PRINT( GRID "Status Test3 --- " );
    if ( ListVerify( list ) == FAIL_VERIFY && list->var_info.error_code == ( LIST_ERR_STRAIGHT_LOOP | LIST_ERR_REVERSE_LOOP | LIST_ERR_FREE_LACK_OF_ELEMENTS ) ) {
        PRINT_STATUS_OK;
    }
    else {
        PRINT_STATUS_FAIL;
    }

    ListDtor_test( list );
}

ListStatus_t ListDtor_test( List_t* list ) {
    my_assert( list, "Null pointer on `list`" );

    ListLog( list, DUMP, "Before <font color=\"orange\">Destructor</font>" );

    free( list->elements );
    list->elements = NULL;
    list->size = 0;
    list->capacity = 0;

    // PRINT_STATUS_OK;
    return SUCCESS;
}