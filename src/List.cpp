#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include <time.h>


#include "List.h"
#include "DebugUtils.h"

#define PRINT_IN_LOG( str, ... ) fprintf( list->log_file, str, ##__VA_ARGS__ );

ListStatus_t ListCtor( List_t* list, const size_t data_capacity ) {
    PRINT_IN;
    my_assert( list, "Null pointer on `list`" );

    list->capacity = data_capacity + 1;

    list->data = ( Element_t* ) calloc ( list->capacity, sizeof( *list->data ));
    assert( list->data && "Memory allocation error" );

    list->head = NULL;
    list->tail = NULL;

    for ( size_t idx = 1; idx < list->capacity; idx++ ) {
        list->data[ idx ].next  = ( int ) idx + 1;
        list->data[ idx ].value = poison;
        list->data[ idx ].prev  = -1;
    }

    list->free = 1;

    PRINT_OUT;
    return SUCCESS;
}

ListStatus_t ListDtor( List_t* list ) {
    PRINT_IN;
    my_assert( list, "Null pointer on `list`" );

    free( list->data );
    list->data = NULL;
    list->size = 0;
    list->capacity = 0;

    PRINT_OUT;
    return SUCCESS;
}

ListStatus_t ListInsert( List_t* list, const int index, const ElementData_t number  ) {
    PRINT_IN;
    my_assert( list,      "Null pointer on `list`" );
    my_assert( index > 0, "Index below 1" );

    if ( ( size_t ) index > list->size + 1 ) {
        fprintf( stderr, "Error: index %d out of range ( size: %lu ) \n", index, list->size );
        return FAIL;;
    }

    int next_free_idx = list->data[ list->free ].next;
    if ( list->size == 1 ) {
        if ( index == 0 ) {
            list->data[ list->free ].value = number;
            list->free = next_free_idx;

            list->size++;

            return SUCCESS;
        }
    }

    list->data[ list->free ].value = number;

    list->data[ list->free ].next = list->data[ index ].next;
    list->data[ index ].next = list->free;

    list->data[ list->free ].prev = list->data[ list->data[ list->free ].next ].prev;
    list->data[ list->data[ list->free ].next ].prev = index;

    list->free = next_free_idx;

    list->size++;

    PRINT_OUT;
    return SUCCESS;
}

ListStatus_t ListDelete( List_t* list, const int index ) {
    PRINT_IN;
    my_assert( list, "Null pointer on `list`" );
    my_assert( isfinite( index ), "Infinite index" );

    if ( list->size == 0 ) {
        fprintf( stderr, "Error: delete from empty list \n" );
        return FAIL;
    }

    list->data[ list->data[ index ].prev ].next = list->data[ index ].next;
    list->data[ list->data[ index ].next ].prev = list->data[ index ].prev;

    list->data[ index ].value = poison;
    list->data[ index ].prev  = -1;
    list->data[ index ].next  = list->free;
    list->free                = index;
    list->size--;

    PRINT_OUT;
    return SUCCESS;
}

#ifdef _DEBUG
    void ListVerify( List_t* list ) {
        my_assert( list, "Null pointer on `list`" );

        if ( list->capacity == 0 )      list->varInfo.error_code |= LIST_ERROR_NULL_CAPACITY;
        if ( list->size > list->capacity ) list->varInfo.error_code |= LIST_ERROR_SIZE_OVER_CAPACITY;

        size_t idx = ( size_t ) list->free;
        while ( list->data[ idx ].next != 0 ) {
            if ( abs( list->data[ idx ].value - poison ) >= DBL_EPSILON ) {
                list->varInfo.error_code |= LIST_ERROR_ISNT_POISON;
            }
        }
    }
#endif

void ListLog( List_t* list, LogModes mode ) {
    my_assert( list, "Null pointer on `list`" );

    switch ( mode ) {
        case BEGIN_OF_PROGRAMM: {
            FileAndFolderCreate( list );

            list->log_file = fopen( list->log_file_address, "w" );
            my_assert( list->log_file, "File opening error" );

            fprintf( list->log_file, "<pre>\n" );

            break;
        }
        case DUMP: {
            ListDump( list );

            break;
        }
        case END_OF_PROGRAM: {
            fprintf( list->log_file, "<\\pre>" );

            int close_result = fclose( list->log_file );
            my_assert( close_result == 0, "File closing error" );

            break;
        }
        default: {
            fprintf( stderr, "Unkmown mode\n" );
            
            break;
        }       
    }
}

void FileAndFolderCreate( List_t* list) {
    my_assert( list, "Null pointer on `list`" );

    time_t current_time = time( NULL );
    struct tm* local_time;
    local_time = localtime( &current_time );
    
    char buffer[ MAX_LEN_PATH ] = "";
    strftime( buffer, MAX_LEN_PATH, "%d.%m_%H:%M", local_time );
    snprintf( list->log_file_folder, MAX_LEN_PATH, "./logs/%s", buffer );
    snprintf( list->log_file_address, MAX_LEN_PATH, "%s/LOGFILE.htm", list->log_file_folder );

    char command[ 30 + MAX_LEN_PATH ] = "";

    snprintf( command, 30 + MAX_LEN_PATH, "mkdir -p %s/images", list->log_file_folder);
    system( command );

    snprintf( command, 30, "touch %s", list->log_file_address );
    system( command );
}

void ListDump( const List_t* list ) {
    my_assert( list, "Null pointer to `list`" );

    PRINT_IN_LOG( "<h3> DUMP </h3> \n" );

    PRINT_IN_LOG( "List { %s:%lu } \n", list->varInfo.file, list->varInfo.line );

    // PREV
    PRINT_IN_LOG( "  PREV: " );
    for ( size_t idx = 0; idx < list->capacity; idx++ ) {
        PRINT_IN_LOG( "%3d%s", list->data[ idx ].prev, ( idx == list->capacity - 1) ? ( "\n" ) : ( " " ) );
    }

    // DATA
    PRINT_IN_LOG( "  DATA: " );
    for ( size_t idx = 0; idx < list->capacity; idx++ ) {
        PRINT_IN_LOG( "%3.0lf%s", list->data[ idx ].value, ( idx == list->capacity - 1) ? ( "\n" ) : ( " " ) );
    }

    // NEXT
    PRINT_IN_LOG( "  NEXT: " );
    for ( size_t idx = 0; idx < list->capacity; idx++ ) {
        PRINT_IN_LOG( "%3d%s", list->data[ idx ].next, ( idx == list->capacity - 1) ? ( "\n" ) : ( " " ) );
    }

    PRINT_IN_LOG( "\n\n" );
}

