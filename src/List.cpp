// TODO: ListHead, ListTail, which will return HeadIndex and TailIndex from null element

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include <time.h>
#include <stdint.h>


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

    list->elements[0].next = 0;
    list->elements[0].prev = 0;

    for ( int idx = 1; idx <= list->capacity; idx++ ) {
        list->elements[ idx ].next  = idx + 1;
        list->elements[ idx ].value = poison;
        list->elements[ idx ].prev  = -1;
    }

    list->elements[ list->capacity ].next = 0;

    list->free = 1;

    list->logging.image_number = 0;

    ListLog( list, DUMP, "After <font color=\"green\"> Constructor </font>" );

    if ( ListVerify( list ) != SUCCESS )  {
        PRINT_STATUS_FAIL;
        return  FAIL;
    }

    PRINT_STATUS_OK;
    return SUCCESS;
}

ListStatus_t ListDtor( List_t* list ) {
    PRINT_EXECUTING;
    my_assert( list, "Null pointer on `list`" );

    ListLog( list, DUMP, "Before <font color=\"orange\"> Destructor </font>" );

    free( list->elements );
    list->elements = NULL;
    list->size = 0;
    list->capacity = 0;

    PRINT_STATUS_OK;
    return SUCCESS;
}

void ListRealloc( List_t* list, const int new_capacity ) {
    my_assert( list,             "Null pointer on `list`" );
    my_assert( new_capacity > 0, "New capacity below 0"   );

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

        idx++;
    }
}

ListStatus_t ListInsert( List_t* list, const int index, const ElementData_t number ) {
    PRINT_EXECUTING;
    my_assert( list,      "Null pointer on `list`" );

    ListLog( list, DUMP, "Before <font color=\"blue\"> INSERT </font>" );

    VERIFY(
        int next_free_idx = list->elements[ list->free ].next;

        // TODO: submit to ListVerify
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

        list->elements[ list->free ].value = number;

        list->elements[ list->free ].next = list->elements[ index ].next;
        list->elements[ list->free ].prev = index;

        list->elements[ index ].next = list->free;
        list->elements[ list->elements[ list->free ].next ].prev = list->free;

        list->free = next_free_idx;

        list->size++;
    );

    ListLog( list, DUMP, "After <font color=\"blue\"> INSERT </font>" );

    PRINT_STATUS_OK;
    return SUCCESS;
}

ListStatus_t ListDelete( List_t* list, const int index ) {
    PRINT_EXECUTING;
    my_assert( list, "Null pointer on `list`" );
    my_assert( isfinite( index ), "Infinite index" );

    ListLog( list, DUMP, "Before <font color=\"red\"> DELETE </font>" );

    VERIFY(
        // TODO: submit to ListVerify
        if ( list->size == 0 ) {
            fprintf( stderr, "Error: delete from empty list \n" );
            return FAIL;
        }

        list->elements[ list->elements[ index ].prev ].next = list->elements[ index ].next;
        list->elements[ list->elements[ index ].next ].prev = list->elements[ index ].prev;

        list->elements[ index ].value = poison;
        list->elements[ index ].prev  = -1;
        list->elements[ index ].next  = list->free;
        list->free                    = index;
        list->size--;
    );

    ListLog( list, DUMP, "After <font color=\"red\"> DELETE </font>" );

    PRINT_STATUS_OK;
    return SUCCESS;
}

#ifdef _DEBUG
    ListStatus_t ListVerify( List_t* list ) {
        my_assert( list, "Null pointer on `list`" );

        if ( list->capacity == 0 )         list->var_info.error_code |= LIST_ERR_NULL_CAPACITY;
        if ( list->size > list->capacity ) list->var_info.error_code |= LIST_ERR_SIZE_OVER_CAPACITY;

        int idx = list->free;
        int next_idx = list->elements[ idx ].next;

        while ( next_idx != list->elements[0].next ) {
            if ( !( abs( list->elements[ idx ].value - poison ) <= DBL_EPSILON && list->elements[ idx ].prev == -1 ) ) {
                list->var_info.error_code |= LIST_ERR_MISSING_NODE_DATA;

                break;
            }

            idx = next_idx;
            next_idx = list->elements[ idx ].next;
        }

        if ( list->var_info.error_code != LIST_ERR_NONE ) {
            return FAIL;
        }

        return SUCCESS;
    }

    void ListLog( List_t* list, const LogModes mode, const char* service_message ) {
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
                ListDump( list, service_message );
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
        
        char buffer[ 15 ] = "";     /* The buffer needs 15 bytes to safely store the "%d.%m_%H:%M:%S" string. */
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

    void ListDump( const List_t* list, const char* service_message ) {
        my_assert( list, "Null pointer to `list`" );

        PRINT_IN_LOG( "<h3> DUMP %s </h3> \n", ( service_message != nullptr ) ? service_message : "" );

        PRINT_IN_LOG( 
            "List { %s:%lu } \n", 
            list->var_info.file, 
            list->var_info.line 
        );

        PRINT_IN_LOG( 
            "SIZE = %d; CAPACITY = %d \n\n", 
            list->size, 
            list->capacity 
        );

        PRINT_IN_LOG( 
            "HEAD = %d; TAIL = %d; FREE = %d \n", 
            list->elements[0].next, 
            list->elements[0].prev, 
            list->free 
        );

        // INDEX
        PRINT_IN_LOG( "IDX  | " );
        for ( int idx = 1; idx <= list->capacity; idx++ ) {
            PRINT_IN_LOG( 
                "%3d%s", 
                idx, 
                ( idx == list->capacity ) ? ( " |\n" ) : ( " | " ) 
            );
        }

        // DATA
        PRINT_IN_LOG( "DATA | " );
        for ( int idx = 1; idx <= list->capacity; idx++ ) {
            PRINT_IN_LOG( 
                "%3lg%s", 
                list->elements[ idx ].value, 
                ( idx == list->capacity ) ? ( " |\n" ) : ( " | " ) 
            );
        }

        // NEXT
        PRINT_IN_LOG( "NEXT | " );
        for ( int idx = 1; idx <= list->capacity; idx++ ) {
            PRINT_IN_LOG( 
                "%3d%s", 
                list->elements[ idx ].next, 
                ( idx == list->capacity ) ? ( " |\n" ) : ( " | " ) 
            );
        }

        // PREV
        PRINT_IN_LOG( "PREV | " );
        for ( int idx = 1; idx <= list->capacity; idx++ ) {
            PRINT_IN_LOG( 
                "%3d%s", 
                list->elements[ idx ].prev, 
                ( idx == list->capacity ) ? ( " |\n" ) : ( " | " ) 
            );
        }

        PRINT_IN_LOG( 
            "\n<img src=\"images/image%lu.png\" height=\"100px\">\n",
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

    PRINT_IN_GRAPHIC( "digraph {\n\trankdir=LR\n\tsplines=ortho;\n\n" );

    // SERVICE NODE
    PRINT_IN_GRAPHIC( 
        "\tnode0 [shape=Mrecord; style=filled; fillcolor=orange; label = \" prev=%d | next=%d \" ]; \n", 
        list->elements[0].prev,
        list->elements[0].next
    );

    // HEAD, TAIL AND FREE NODES
    PRINT_IN_GRAPHIC( 
        "\thead [shape=note; style=filled; fillcolor=\"#FFFF99\"; label=\"HEAD\\n %d\"];\n", 
        list->elements[0].next 
    );
    PRINT_IN_GRAPHIC( 
        "\ttail [shape=note; style=filled; fillcolor=\"#FFFF99\"; label=\"TAIL\\n %d\"];\n", 
        list->elements[0].prev 
    );
    PRINT_IN_GRAPHIC( 
        "\tfree [shape=note; style=filled; fillcolor=\"#FFFF99\"; label=\"FREE\\n %d\"];\n", 
        list->free
    );

    // NODE ANNOUNCEMENT
    for ( int idx = 1; idx <= list->capacity; idx++ ) {
        PRINT_IN_GRAPHIC( 
            "\tnode%d [shape=Mrecord; style=filled; fillcolor=\"#%X\"; label=\" prev=%d | value=%lg | next=%d \" ]; \n", 
            idx, 
            ( uint32_t ) ( ( list->elements[ idx ].prev == -1 ) ? COLOR_FREE : COLOR_OCCUPIED ),
            list->elements[ idx ].prev,
            list->elements[ idx ].value,
            list->elements[ idx ].next
        );
    }

    PRINT_IN_GRAPHIC( "\n\n" );


    // FOR THE HORIZONTAL ARRAGEMENT OF NODES 
    for ( int idx = 0; idx < list->capacity; idx++ ) {
        PRINT_IN_GRAPHIC( 
            "node%d -> node%d [style=invis];\n\t", 
            idx, 
            idx + 1
        );
    }

    PRINT_IN_GRAPHIC( "\n\n\t")
    

    // ONE RANK FOR HEAD AND ITS ELEMENT
    PRINT_IN_GRAPHIC( "\t{ rank=same; head; " );
    if ( list->elements[0].next != 0 ) {
        PRINT_IN_GRAPHIC( "node%d ", list->elements[0].next );
    }
    PRINT_IN_GRAPHIC( "}\n" );
    
    // ONE RANK FOR TAIL AND ITS ELEMENT
    PRINT_IN_GRAPHIC( "\t{ rank=same; tail; " );
    if ( list->elements[0].next != 0 ) {
        PRINT_IN_GRAPHIC( "node%d ", list->elements[0].prev );
    }
    PRINT_IN_GRAPHIC( "}\n" );
    
    // ONE RANK FOR FREE AND ITS ELEMENT
    PRINT_IN_GRAPHIC( "\t{ rank=same; free; " );
    if ( list->elements[0].next != 0 ) {
        PRINT_IN_GRAPHIC( "node%d ", list->free );
    }
    PRINT_IN_GRAPHIC( "}\n\t" );
    

    // CONNECT HEAD, TAIL AND FREE TO ACTUAL ELEMENTS
    PRINT_IN_GRAPHIC( "head -> node%d [arrowhead=vee];\n\t", list->elements[0].next );
    PRINT_IN_GRAPHIC( "tail -> node%d [arrowhead=vee];\n\t", list->elements[0].prev );
    PRINT_IN_GRAPHIC( "free -> node%d [arrowhead=vee];\n\t", list->free );


    // THE BRANCH OF FILLED ELEMENTS OF LIST
    int idx = list->elements[0].next;
    if ( idx != 0 ) {
        int next_idx = list->elements[ idx ].next;

        while ( next_idx != list->elements[0].next ) {
            PRINT_IN_GRAPHIC( 
                "node%d -> node%d [constraint=false, color=blue];\n\t", 
                idx, 
                next_idx
            );

            idx = next_idx;
            next_idx = list->elements[ idx ].next;
        }
    }

    idx = list->elements[0].prev;
    if ( idx != 0 ) {
        int prev_idx = list->elements[ idx ].prev;

        while ( prev_idx != list->elements[0].prev ) {
            PRINT_IN_GRAPHIC( 
                "node%d -> node%d [constraint=false, color=green];\n\t", 
                idx, 
                prev_idx
            );

            idx = prev_idx;
            prev_idx = list->elements[ idx ].prev;
        }
    }

    // THE BRANCH OF FREE ELEMENTS OF LIST
    idx = list->free;
    if ( idx != 0 ) {
        int next_idx = list->elements[ idx ].next;
        while ( idx != 0 && list->elements[ idx ].prev == -1 && next_idx != 0 ) {
            PRINT_IN_GRAPHIC( "node%d -> node%d [constraint=false, color=\"#95A5A6\", style=dashed];\n\t", idx, next_idx );
            idx = next_idx;
            next_idx = list->elements[ idx ].next;
        }
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
#endif