#include <cstdint>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include <time.h>


#include "List.h"
#include "DebugUtils.h"

#define PRINT_IN_LOG( str, ... )     fprintf( list->logging.log_file, str, ##__VA_ARGS__ );
// #define PRINT_IN_LOG( str, ... )     fprintf( stderr, str, ##__VA_ARGS__ );
#define PRINT_IN_GRAPHIC( str, ... ) fprintf( graphic_file,           str, ##__VA_ARGS__ );

ListStatus_t ListCtor( List_t* list, const int data_capacity ) {
    PRINT_EXECUTING;
    my_assert( list, "Null pointer on `list`" );

    list->capacity = data_capacity;
    list->size = 0;

    list->data = ( Element_t* ) calloc ( ( size_t ) list->capacity + 1, sizeof( *( list->data ) ) );
    assert( list->data && "Memory allocation error" );

    list->head = 0;
    list->tail = 0;

    for ( int idx = 1; idx < list->capacity; idx++ ) {
        list->data[ idx ].next  = idx + 1;
        list->data[ idx ].value = poison;
        list->data[ idx ].prev  = -1;
    }

    list->data[ list->capacity - 1 ].next = 0;

    list->free = 1;

    PRINT_STATUS_OK;
    return SUCCESS;
}

ListStatus_t ListDtor( List_t* list ) {
    PRINT_EXECUTING;
    my_assert( list, "Null pointer on `list`" );

    free( list->data );
    list->data = NULL;
    list->size = 0;
    list->capacity = 0;

    PRINT_STATUS_OK;
    return SUCCESS;
}

ListStatus_t ListInsert( List_t* list, const int index, const ElementData_t number ) {
    PRINT_EXECUTING;
    my_assert( list,      "Null pointer on `list`" );
    my_assert( index > 0, "Index below 1" );

    // TODO: think about this if
    // if ( index > list->size ) {
    //     fprintf( stderr, "Error: index %d out of range ( size: %lu ) \n", index, list->size );

    //     PRINT_STATUS_FAIL;
    //     return FAIL;;
    // }

    int next_free_idx = list->data[ list->free ].next;

    if ( list->size == 0 ) {
        if ( index == 1 ) {
            list->data[ index ].value = number;
            list->data[ index ].prev  = 0;
            list->data[ index ].next  = 0;

            list->free = next_free_idx;

            list->size++;
            
            list->head         = 1;
            list->data[0].next = 1;

            list->tail         = 1;
            list->data[0].prev = 1;

            PRINT_STATUS_OK;
            return SUCCESS;
        }
        else {
            PRINT_STATUS_FAIL;
            return FAIL;
        }
    }

    if ( list->data[ index ].prev == -1 ) {
        PRINT_STATUS_FAIL;
        return FAIL;   
    }

    list->data[ list->free ].value = number;

    list->data[ list->free ].next = list->data[ index ].next;
    list->data[ list->free ].prev = index;

    list->data[ index ].next = list->free;
    list->data[ list->data[ list->free ].next ].prev = list->free;

    if ( list->data[ list->free ].prev == 0 ) {
        list->data[0].next = list->free;
        list->head         = list->free;
    }

    if ( list->data[ list->free ].next == 0 ) {
        list->data[0].prev = list->free;
        list->tail         = list->free;
    }

    list->free = next_free_idx;

    list->size++;

    PRINT_STATUS_OK;
    return SUCCESS;
}

ListStatus_t ListDelete( List_t* list, const int index ) {
    PRINT_EXECUTING;
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

    PRINT_STATUS_OK;
    return SUCCESS;
}

#ifdef _DEBUG
    void ListVerify( List_t* list ) {
        my_assert( list, "Null pointer on `list`" );

        if ( list->capacity == 0 )      list->var_info.error_code |= LIST_ERROR_NULL_CAPACITY;
        if ( list->size > list->capacity ) list->var_info.error_code |= LIST_ERROR_SIZE_OVER_CAPACITY;

        size_t idx = ( size_t ) list->free;
        while ( list->data[ idx ].next != 0 ) {
            if ( abs( list->data[ idx ].value - poison ) >= DBL_EPSILON ) {
                list->var_info.error_code |= LIST_ERROR_ISNT_POISON;
            }
        }
    }
#endif

void ListLog( List_t* list, LogModes mode ) {
    my_assert( list, "Null pointer on `list`" );

    switch ( mode ) {
        case BEGIN_OF_PROGRAMM: {
            FileAndFolderCreate( list );

            list->logging.log_file = fopen( list->logging.log_file_address, "w" );
            my_assert( list->logging.log_file, "File opening error" );

            fprintf( list->logging.log_file, "<pre>\n" );

            break;
        }
        case DUMP: {
            ListDump( list );
            GraphicPrintoutDraw( list );

            break;
        }
        case END_OF_PROGRAM: {
            int close_result = fclose( list->logging.log_file );
            my_assert( close_result == 0, "File closing error" );

            break;
        }
        default: {
            fprintf( stderr, "Unkmown mode\n" );
            
            break;
        }       
    }
}

void FileAndFolderCreate( List_t* list ) {
    my_assert( list, "Null pointer on `list`" );

    time_t current_time = time( NULL );
    struct tm* local_time;
    local_time = localtime( &current_time );
    
    char buffer[ 15 ] = "";
    strftime( buffer, sizeof( buffer ), "%d.%m_%H:%M:%S", local_time );
    snprintf( list->logging.log_file_folder,  sizeof( list->logging.log_file_folder ),  "./logs/%s",      buffer );
    snprintf( list->logging.log_file_address, sizeof( list->logging.log_file_address ), "%s/LOGFILE.htm", list->logging.log_file_folder );

    char command[ 2 * MAX_LEN_PATH ] = "";

    snprintf( command, sizeof( command ), "mkdir -p %s/images", list->logging.log_file_folder);
    PRINT( "%s\n", command );
    system( command );

    snprintf( command, sizeof( command ), "mkdir -p %s/image_sources", list->logging.log_file_folder);
    PRINT( "%s\n", command );
    system( command );

    snprintf( command, sizeof( command ), "touch %s", list->logging.log_file_address );
    PRINT( "%s\n", command );
    system( command );
}

void ListDump( const List_t* list ) {
    my_assert( list, "Null pointer to `list`" );

    PRINT_IN_LOG( "<h3> DUMP </h3> \n" );

    PRINT_IN_LOG( "List { %s:%lu } \n", list->var_info.file, list->var_info.line );
    PRINT_IN_LOG( "FREE = %d; SIZE = %d; CAPACITY = %d \n", list->free, list->size, list->capacity );

    // DATA
    PRINT_IN_LOG( "  DATA: " );
    for ( int idx = 1; idx < list->capacity; idx++ ) {
        PRINT_IN_LOG( "%3.0lf%s", list->data[ idx ].value, ( idx == list->capacity - 1 ) ? ( "\n" ) : ( " " ) );
    }

    // NEXT
    PRINT_IN_LOG( "  NEXT: " );
    for ( int idx = 1; idx < list->capacity; idx++ ) {
        PRINT_IN_LOG( "%3d%s", list->data[ idx ].next, ( idx == list->capacity - 1 ) ? ( "\n" ) : ( " " ) );
    }

    // PREV
    PRINT_IN_LOG( "  PREV: " );
    for ( int idx = 1; idx < list->capacity; idx++ ) {
        PRINT_IN_LOG( "%3d%s", list->data[ idx ].prev, ( idx == list->capacity - 1 ) ? ( "\n" ) : ( " " ) );
    }

    PRINT_IN_LOG( 
        "<img src=\"images/image%lu.png\" width=\"500px\">\n",
        list->logging.image_number
    );

    PRINT_IN_LOG( "\n\n" );
}

void GraphicPrintoutDraw( List_t* list ) {
    my_assert( list, "Null pointer on `list`" );

    char graphic_source_address[ MAX_LEN_PATH + 30 ] = "";

    snprintf( 
        graphic_source_address, sizeof( graphic_source_address ), 
        "%s/image_sources/image%lu.txt", 
        list->logging.log_file_folder, list->logging.image_number 
    );

    char command[ 4 * MAX_LEN_PATH ] = "";

    snprintf( command, sizeof( command ), "touch %s", graphic_source_address );
    system( command );

    FILE* graphic_file = fopen( graphic_source_address, "w" );
    assert( graphic_file && "File opening error" );

    PRINT_IN_GRAPHIC( "digraph {\n  rankdir=LR\n" );

    // NODE ANNOUNCEMENT
    for ( int idx = 1; idx <= list->capacity; idx++ ){
        PRINT_IN_GRAPHIC( 
            " node%d [shape=Mrecord; style = filled; fillcolor = \"#%X\"; label = \"{ prev = %d | value = %lf | next = %d }\" ] \n", 
            idx, 
            ( uint32_t ) ( ( list->data[ idx ].prev == -1 ) ? COLOR_OCCUPIED : COLOR_FREE ),
            list->data[ idx ].prev,
            list->data[ idx ].value,
            list->data[ idx ].next
        );
    }

    PRINT_IN_GRAPHIC( "\n\n" );

    for ( int idx = 1; idx <= list->capacity; idx++ ) {
        PRINT_IN_GRAPHIC( "node%d%s", idx, ( idx == list->capacity ) ? " " : " -> " );
    }
    PRINT_IN_GRAPHIC( "[color=\"transparent\"];\n\n")

    // TODO: Many if for exceptions: empty list, ...
    size_t idx = ( size_t ) list->head;
    PRINT_IN_GRAPHIC( "node%lu", idx );
    size_t next_idx = ( size_t ) list->data[ idx ].next;

    while ( list->data[ idx ].next != list->head ) {
        PRINT_IN_GRAPHIC( " -> node%lu", idx );
        next_idx = ( size_t ) list->data[ idx ].next;
        idx = next_idx;
    }
    PRINT_IN_GRAPHIC( "\n\n" );

    idx = ( size_t ) list->free;
    PRINT_IN_GRAPHIC( "node%lu", idx );
    next_idx = ( size_t ) list->data[ idx ].next;

    while ( list->data[ idx ].next != list->head ) {
        PRINT_IN_GRAPHIC( " -> node%lu", idx );
        next_idx = ( size_t ) list->data[ idx ].next;
        idx = next_idx;
    }
    PRINT_IN_GRAPHIC( "\n\n}" );

    int close_result = fclose( graphic_file );
    assert( close_result == 0 && "File closing error" );

    snprintf( 
        command, sizeof( command ), 
        "dot %s -Tpng -o %s/images/image%lu.png", 
        graphic_source_address,
        list->logging.log_file_folder,
        list->logging.image_number++
    );

    system( command );
}
