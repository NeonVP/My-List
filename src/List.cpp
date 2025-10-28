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

    list->elements = ( Element_t* ) calloc ( ( size_t ) list->capacity + 1, sizeof( *( list->elements ) ) );
    assert( list->elements && "Memory allocation error" );

    list->head = 0;
    list->tail = 0;

    for ( int idx = 1; idx <= list->capacity; idx++ ) {
        list->elements[ idx ].next  = idx + 1;
        list->elements[ idx ].value = poison;
        list->elements[ idx ].prev  = -1;
    }

    list->elements[ list->capacity ].next = 0;

    list->free = 1;

    list->logging.image_number = 0;

    PRINT_STATUS_OK;
    return SUCCESS;
}

ListStatus_t ListDtor( List_t* list ) {
    PRINT_EXECUTING;
    my_assert( list, "Null pointer on `list`" );

    free( list->elements );
    list->elements = NULL;
    list->size = 0;
    list->capacity = 0;

    PRINT_STATUS_OK;
    return SUCCESS;
}

ListStatus_t ListInsert( List_t* list, const int index, const ElementData_t number ) {
    PRINT_EXECUTING;
    my_assert( list,      "Null pointer on `list`" );

    // TODO: think about this if
    // if ( index > list->size ) {
    //     fprintf( stderr, "Error: index %d out of range ( size: %lu ) \n", index, list->size );

    //     PRINT_STATUS_FAIL;
    //     return FAIL;;
    // }

    int next_free_idx = list->elements[ list->free ].next;

    if ( list->size == 0 && index > 0 ) {
        PRINT_STATUS_FAIL;
        return FAIL;
    }

    if ( list->elements[ index ].prev == -1 ) {
        PRINT_STATUS_FAIL;
        return FAIL;   
    }

    list->elements[ list->free ].value = number;

    list->elements[ list->free ].next = list->elements[ index ].next;
    list->elements[ list->free ].prev = index;

    list->elements[ index ].next = list->free;
    list->elements[ list->elements[ list->free ].next ].prev = list->free;

    if ( list->elements[ list->free ].prev == 0 ) {
        list->elements[0].next = list->free;
        list->head         = list->free;
    }

    if ( list->elements[ list->free ].next == 0 ) {
        list->elements[0].prev = list->free;
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

    list->elements[ list->elements[ index ].prev ].next = list->elements[ index ].next;
    list->elements[ list->elements[ index ].next ].prev = list->elements[ index ].prev;

    list->elements[ index ].value = poison;
    list->elements[ index ].prev  = -1;
    list->elements[ index ].next  = list->free;
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
        while ( list->elements[ idx ].next != 0 ) {
            if ( abs( list->elements[ idx ].value - poison ) >= DBL_EPSILON ) {
                list->var_info.error_code |= LIST_ERROR_ISNT_POISON;
            }
        }
    }
#endif

void ListLog( List_t* list, const LogModes mode ) {
    my_assert( list, "Null pointer on `list`" );

    switch ( mode ) {   
        case BEGIN_OF_PROGRAMM: {
            FileAndFolderCreate( list );

            list->logging.log_file = fopen( list->logging.log_file_path, "w" );
            assert( list->logging.log_file && "File opening error" );

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
            assert( close_result == 0 &&"File closing error" );            

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
    snprintf( list->logging.log_directory,  sizeof( list->logging.log_directory ),  "./logs/%s",      buffer );
    snprintf( list->logging.log_file_path, sizeof( list->logging.log_file_path ), "%s/LOGFILE.htm", list->logging.log_directory );

    char command[ 2 * MAX_LEN_PATH ] = "";

    snprintf( command, sizeof( command ), "mkdir -p %s/images", list->logging.log_directory);
    PRINT( "%s\n", command );
    system( command );

    snprintf( command, sizeof( command ), "mkdir -p %s/image_sources", list->logging.log_directory);
    PRINT( "%s\n", command );
    system( command );

    snprintf( command, sizeof( command ), "touch %s", list->logging.log_file_path );
    PRINT( "%s\n", command );
    system( command );
}

void ListDump( const List_t* list ) {
    my_assert( list, "Null pointer to `list`" );

    PRINT_IN_LOG( "<h3> DUMP </h3> \n" );

    PRINT_IN_LOG( 
        "List { %s:%lu } \n", 
        list->var_info.file, 
        list->var_info.line 
    );

    PRINT_IN_LOG( 
        "HEAD = %d; TAIL = %d; FREE = %d \n", 
        list->head, 
        list->tail, 
        list->free 
    );

    PRINT_IN_LOG( 
        "SIZE = %d; CAPACITY = %d \n", 
        list->size, 
        list->capacity 
    );

    // DATA
    PRINT_IN_LOG( "\tDATA: " );
    for ( int idx = 1; idx <= list->capacity; idx++ ) {
        PRINT_IN_LOG( 
            "%3.0lf%s", 
            list->elements[ idx ].value, 
            ( idx == list->capacity ) ? ( "\n" ) : ( " " ) 
        );
    }

    // NEXT
    PRINT_IN_LOG( "\tNEXT: " );
    for ( int idx = 1; idx <= list->capacity; idx++ ) {
        PRINT_IN_LOG( 
            "%3d%s", 
            list->elements[ idx ].next, 
            ( idx == list->capacity ) ? ( "\n" ) : ( " " ) 
        );
    }

    // PREV
    PRINT_IN_LOG( "\tPREV: " );
    for ( int idx = 1; idx <= list->capacity; idx++ ) {
        PRINT_IN_LOG( 
            "%3d%s", 
            list->elements[ idx ].prev, 
            ( idx == list->capacity ) ? ( "\n" ) : ( " " ) 
        );
    }

    PRINT_IN_LOG( 
        "<img src=\"images/image%lu.png\" height=\"50px\">\n",
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
        list->logging.log_directory, list->logging.image_number
    );

    char command[ 4 * MAX_LEN_PATH ] = "";

    snprintf( command, sizeof( command ), "touch %s", graphic_source_address );
    system( command );

    FILE* graphic_file = fopen( graphic_source_address, "w" );
    assert( graphic_file && "File opening error" );

    PRINT_IN_GRAPHIC( "digraph {\n\trankdir=LR\n\tsplines=ortho;\n" );

    // NODE ANNOUNCEMENT
    for ( int idx = 1; idx <= list->capacity; idx++ ){
        PRINT_IN_GRAPHIC( 
            "\tnode%d [shape=Mrecord; style = filled; fillcolor = \"#%X\"; label = \" prev = %d | value = %lg | next = %d \" ] \n", 
            idx, 
            ( uint32_t ) ( ( list->elements[ idx ].prev == -1 ) ? COLOR_FREE : COLOR_OCCUPIED ),
            list->elements[ idx ].prev,
            list->elements[ idx ].value,
            list->elements[ idx ].next
        );
    }

    PRINT_IN_GRAPHIC( "\n\n\t" );

    // FOR THE HORIZONTAL ARRAGEMENT OF NODES 
    for ( int idx = 1; idx <= list->capacity; idx++ ) {
        PRINT_IN_GRAPHIC( 
            "node%d%s", 
            idx, 
            ( idx == list->capacity ) ? " " : " -> " 
        );
    }
    PRINT_IN_GRAPHIC( "[style=invis];\n\n\t")

    // THE BRANCH OF FILLED ELEMENTS OF LIST
    int idx = list->head;
    int next_idx = list->elements[ idx ].next;

    while ( next_idx != list->head ) {
        PRINT_IN_GRAPHIC( 
            "node%d%s", 
            idx, 
            ( next_idx == 0 ) ? "\n" : " -> " 
        );

        idx = next_idx;
        next_idx = list->elements[ idx ].next;
    }
    PRINT_IN_GRAPHIC( "\n\t" );

    // THE BRANCH OF FREE ELEMENTS OF LIS
    idx = list->free;
    next_idx = list->elements[ idx ].next;
    while ( list->elements[ idx ].prev == -1 ) {
        PRINT_IN_GRAPHIC( 
            "node%d%s", 
            idx, 
            ( next_idx == 0 ) ? "\n" : " -> " 
        );

        idx = next_idx;
        next_idx = list->elements[ idx ].next;
    }
    PRINT_IN_GRAPHIC( "}" );

    int close_result = fclose( graphic_file );
    assert( close_result == 0 && "File closing error" );

    snprintf( 
        command, sizeof( command ), 
        "dot %s -Tpng -o %s/images/image%lu.png", 
        graphic_source_address,
        list->logging.log_directory,
        list->logging.image_number
    );

    list->logging.image_number++;

    system( command );
}
