#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include <stdarg.h>


#include "List.h"
#include "DebugUtils.h"


ListStatus_t ListCtor( List_t* list, const int data_capacity ) {
    PRINT_EXECUTING;
    my_assert( list, "Null pointer on `list`" );

    list->capacity = data_capacity;
    list->size = 0;

    list->elements = ( Element_t* ) calloc ( ( size_t ) list->capacity + 1, sizeof( *( list->elements ) ) );
    assert( list->elements && "Memory allocation error" );

    list->elements[0].next = 0;
    list->elements[0].prev = 0;
    list->free = 1;

    for ( int idx = 1; idx <= list->capacity; idx++ ) {
        list->elements[ idx ] = { .prev = -1, .value = poison, .next = idx + 1 };
    }

    list->elements[ list->capacity ].next = 0;

    list->logging.image_number = 0;

    ListLog( list, DUMP, "After <font color=\"orange\"> Constructor </font>" );

    #ifdef _DEBUG
        if ( ListVerify( list ) != SUCCESS )  {
            PRINT_STATUS_FAIL;
            return  FAIL;
        }
    #endif

    PRINT_STATUS_OK;
    return SUCCESS;
}

ListStatus_t ListDtor( List_t* list ) {
    PRINT_EXECUTING;
    my_assert( list, "Null pointer on `list`" );

    ListLog( list, DUMP, "Before <font color=\"orange\">Destructor</font>" );

    free( list->elements );
    list->elements = NULL;
    list->size = 0;
    list->capacity = 0;

    #ifdef _DEBUG
        free( list->logging.common_log_directory );
        free( list->logging.log_img_directory );
        free( list->logging.log_img_src_directory );
        free( list->logging.log_file_path );
    #endif

    PRINT_STATUS_OK;
    return SUCCESS;
}

int ListGetHead( List_t* list ) {
    my_assert( list, "Null pointer on `list`" );

    return list->elements[0].next;
}

int ListGetTail( List_t* list ) {
    my_assert( list, "Null pointer on `list`" );

    return list->elements[0].prev;
}

int ListGetSize( const List_t *list ) {
    my_assert( list, "Null pointer on `list`" );

    return list->size;
}

int ListGetCapacity( const List_t *list ) {
    my_assert( list, "Null pointer on `list`" );

    return list->capacity;
}

int ListGetFree( const List_t* list ) {
    my_assert( list, "Null pointer on `list`" );

    return list->free;
}

int ListGetNext   ( const List_t* list, const int index ) {
    my_assert( list, "Null pointer on `list`" );

    return list->elements[ index ].next;
}

int ListGetPrev   ( const List_t* list, const int index ) {
    my_assert( list, "Null pointer on `list`" );

    return list->elements[ index ].prev;
}

ElementData_t ListGetElement( const List_t* list, const int index ) {
    my_assert( list, "Null pointer on `list`" );

    return list->elements[ index ].value;
}



void ListRealloc( List_t* list, const int new_capacity ) {
    PRINT_EXECUTING;
    my_assert( list,             "Null pointer on `list`" );
    my_assert( new_capacity > 0, "New capacity below 0"   );

    ListLog( list, DUMP, "Before <font color\"orange\">Realloc</font>" );

    list->elements = ( Element_t* ) realloc ( list->elements, ( ( size_t ) new_capacity ) * sizeof( *list->elements ) );
    assert( list->elements && "Memory reallocation error" );

    int idx = list->free;
    int next_idx = list->elements[ idx ].next;

    while ( next_idx != 0 ) {
        idx = next_idx;
        next_idx = list->elements[ idx ].next;
    }

    while ( idx <= new_capacity ) {
        list->elements[ idx ].value = poison;
        list->elements[ idx ].next  = idx + 1;
        list->elements[ idx ].prev  = -1;
        list->elements[ idx ] = {
            .prev = -1,
            .value = poison,
            .next = idx + 1
        };

        idx++;
    }

    list->elements[ idx ].next = 0;
}

ListStatus_t ListInsertAfter( List_t* list, const int index, const ElementData_t number ) {
    PRINT_EXECUTING;
    my_assert( list, "Null pointer on `list`" );

    // ListLog( list, DUMP, "Before <font color=\"blue\"> INSERT AFTER</font>", number );
    ListLog( list, DUMP, "Before <font color=\"blue\"> INSERT ( %lg ) AFTER <%d> </font>", number, index );

    VERIFY(
        int next_free_idx = list->elements[ list->free ].next;

        if ( list->size == 0 && index > 0 ) {
            PRINT_STATUS_FAIL;
            return FAIL;
        }

        if ( list->elements[ index ].prev == -1 ) {
            PRINT_STATUS_FAIL;
            return FAIL;   
        }

        if ( list->size + 1 > list->capacity ) {
            ListRealloc( list, list->capacity * 2 );
        }

        list->elements[ list->free ] = { 
            .prev = index, 
            .value = number, 
            .next = list->elements[ index ].next 
        };

        list->elements[ index ].next = list->free;
        list->elements[ list->elements[ list->free ].next ].prev = list->free;

        list->free = next_free_idx;

        list->size++;
    );

    ListLog( list, DUMP, "After <font color=\"blue\"> INSERT (%lg) AFTER <%d></font>", number, index );

    PRINT_STATUS_OK;
    return SUCCESS;
}

ListStatus_t ListInsertBefore( List_t* list, const int index, const ElementData_t number ) {
    PRINT_EXECUTING;
    my_assert( list,      "Null pointer on `list`" );

    ListLog( list, DUMP, "Before <font color=\"blue\"> <font color=\"blue\"> INSERT BEFORE </font>", number, index );

    VERIFY(
        if ( list->size == 0 && index > 2 ) {
            PRINT_STATUS_FAIL;
            return FAIL;
        }

        if ( list->elements[ index ].prev == -1 ) {
            PRINT_STATUS_FAIL;
            return FAIL;   
        }

        if ( list->size + 1 > list->capacity ) {
            ListRealloc( list, list->capacity * 2 );
        }

        int next_free_idx = list->elements[ list->free ].next;

        list->elements[ list->free ].value = number;

        list->elements[ list->free ].prev = list->elements[ index ].prev;
        list->elements[ list->free ].next = index;

        list->elements[ index ].prev = list->free;
        list->elements[ list->elements[ list->free ].prev ].next = list->free;

        list->free = next_free_idx;

        list->size++;
    );

    ListLog( list, DUMP, "After <font color=\"blue\"> INSERT BEFORE</font>", number, index );

    PRINT_STATUS_OK;
    return SUCCESS;
}

ListStatus_t ListDelete( List_t* list, const int index ) {
    PRINT_EXECUTING;
    my_assert( list, "Null pointer on `list`" );

    ListLog( list, DUMP, "Before <font color=\"red\"> DELETE</font>", index );        // TODO: add func VA_ARGS for print Before\After Insert\Delete and others

    VERIFY(
        if ( list->size == 0 ) {
            fprintf( stderr, "Error: delete from empty list \n" );
            PRINT_STATUS_FAIL;
            return FAIL;
        }

        if ( list->elements[ index ].prev == -1 ) {
            fprintf( stderr, "Error: delete free element \n" );
            PRINT_STATUS_FAIL;
            return FAIL;
        }

        list->elements[ list->elements[ index ].prev ].next = list->elements[ index ].next;
        list->elements[ list->elements[ index ].next ].prev = list->elements[ index ].prev;

        list->elements[ index ].value = poison;
        list->elements[ index ].prev  = -1;
        list->elements[ index ].next  = list->free;
        list->elements[ index ] = {
            .prev = -1,
            .value = poison,
            .next = list->free
        };

        list->free = index;
        list->size--;
    );

    ListLog( list, DUMP, "After <font color=\"red\"> DELETE</font>", index );

    PRINT_STATUS_OK;
    return SUCCESS;
}

#ifdef _DEBUG
    ListStatus_t ListVerify( List_t* list ) {
        my_assert( list, "Null pointer on `list`" );

        if ( list->size > list->capacity ) { list->var_info.error_code |= LIST_ERR_SIZE_CAPACITY_CORRUPTION; return  FAIL; }

        // int idx = LIST_HEAD( list );
        // int next_idx = list->elements[ idx ].next;
        // int cnt = 0;

        // while ( idx != 0 ) {
        //     cnt++;
        //     idx = next_idx;
        //     next_idx = list->elements[ idx ].next;

        //     if ( cnt > list->size ) {
        //         list->var_info.error_code |= LIST_ERR_LIST_ERR_LOOP_STRAIGHT;
        //         break;
        //     }
        // }
        // if ( cnt < list->size ) list->var_info.error_code |= LIST_ERR_LACK_OF_ELEMENTS;

        int size = ListGetSize( list );
        int capacity = 0;

        int idx = 0;
        int counter = 0;

        // while ( idx != 0) {
        //     idx = ListGetNext( list, idx );
        //     counter++;

        //     if ( counter > size ) {
        //         list->var_info.error_code |= LIST_ERR_STRAIGHT_LOOP;

        //         break;
        //     }
        // }

        // if ( counter < list->size ) {
        //     list->var_info.error_code |= LIST_ERR_LACK_OF_ELEMENTS;
        // }

        #define CHECK_FOR_LOOP( node1, node2, err ) \
            idx = ListGet##node1( list );      \
            counter = 0;                       \
            PRINT( " \nBefore check --- %d - idx\t%d - cnt\n", idx, counter ); \
                                               \
            while ( idx != 0) {                \
                PRINT( "Before idx.next --- %d - idx\t%d - cnt\n", idx, counter ); \
                idx = ListGet##node2( list, idx ); \
                counter++;                     \
                PRINT( "Before idx.prev --- %d - idx\t%d - cnt\n", idx, counter ); \
                                               \
                if ( counter > size ) {        \
                    list->var_info.error_code |= LIST_ERR_##err##_LOOP; \
                    PRINT( "I'm in if about counter>size | %d - idx\t%d - cnt\t%d - size\n", idx, counter, size ); \
                                               \
                    break;                     \
                }                              \
            }                                  \
                                               \
            if ( counter < list->size ) {      \
                list->var_info.error_code |= LIST_ERR_LACK_OF_ELEMENTS; \
            }
        
        CHECK_FOR_LOOP( Head, Next, STRAIGHT );
        // CHECK_FOR_LOOP( Tail, Prev, REVERSE  );
        // CHECK_FOR_LOOP( Free, Next, FREE );


        // idx = LIST_FREE( list );
        // next_idx = list->elements[ idx ].next;

        // while ( idx != 0 ) {
        //     if ( !( abs( list->elements[ idx ].value - poison ) <= DBL_EPSILON && list->elements[ idx ].prev == -1 ) ) {
        //         list->var_info.error_code |= LIST_ERR_POISON_CORRUPTION;

        //         break;
        //     }
        //     idx = next_idx;
        //     next_idx = list->elements[ idx ].next;

        //     if ( cnt > list->capacity - list->size ) {
        //         list->var_info.error_code |= LIST_ERR_LIST_ERR_LOOP_FREE;
        //         break;
        //     }
        // }

        // idx = LIST_TAIL( list );
        // int prev_idx = list->elements[ idx ].prev;
        // cnt = 0;

        // while ( idx != 0 ) {
        //     cnt++;
        //     idx = prev_idx;
        //     prev_idx = list->elements[ idx ].prev;

        //     if ( cnt > list->size ) {
        //         list->var_info.error_code |= LIST_ERR_LIST_ERR_LOOP_REVERSE;
        //         break;
        //     }
        // }
        // if ( cnt < list->size ) list->var_info.error_code |= LIST_ERR_LACK_OF_ELEMENTS;

        // PRINT( "%u ", list->var_info.error_code );
        // if ( list->var_info.error_code != LIST_ERR_NONE ) {
        //     ListLog( list, DUMP, "<font color=\"red\">ERROR</font>" );
        //     return FAIL;
        // }

        return SUCCESS;
    }
#endif