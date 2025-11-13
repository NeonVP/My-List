#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#include "List.h"
#include "Colors.h"
#include "DebugUtils.h"

struct Element_t {
    int           prev;
    ElementData_t value;
    int           next;
};


struct list_ {    
    int size;
    int capacity;
    int free;

    Element_t* elements;

    int is_linear;

    #ifdef _DEBUG
        VarInfo_t var_info;
        LogStat logging;
    #endif
};


List_t* ListCreate() {
    return ;
}

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
        list->elements[ idx ].value = LIST_POISON_VALUE;
        list->elements[ idx ].prev  = LIST_FREE_PREV_IDX;
        list->elements[ idx ].next  = idx + 1;
    }

    list->elements[ list->capacity ].next = 0;
    list->logging.image_number = 0;

    VERIFY( "After <font color=\"orange\"> Constructor </font>" );

    PRINT_STATUS_OK;
    return SUCCESS;
}

ListStatus_t ListDtor( List_t* list ) {
    PRINT_EXECUTING;
    my_assert( list, "Null pointer on `list`" );

    if ( ListVerify( list ) != SUCCESS ) {
        fprintf( 
            stderr, 
            COLOR_BRIGHT_RED "ERROR: Verification failed while executing `ListDtor`. See logs for details: ./%s. \n",
            list->logging.log_file_path
        );
    }
    ListLog( list, DUMP, "Before <font color=\"orange\">Destructor</font>" );

    free( list->elements );
    list->elements = NULL;
    list->size     = 0;
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

#ifdef _DEBUG
    ListStatus_t ListVerify( List_t* list ) {
        my_assert( list, "Null pointer on `list`" );

        if ( list->size > list->capacity ) { list->var_info.error_code |= LIST_ERR_SIZE_CAPACITY_CORRUPTION; return  FAIL_VERIFY; }

        const int size     = ListGetSize    ( list );
        const int capacity = ListGetCapacity( list );

        int idx = 0;
        int counter = 0;

        // CHECK FOR STRAIGHT LOOP
        idx = ListGetHead( list );
        counter = 0;

        while ( idx > 0) {
            idx = ListGetNext( list, idx );
            counter++;

            if ( counter > size ) {
                list->var_info.error_code |= LIST_ERR_STRAIGHT_LOOP;

                break;
            }
        }

        if ( counter < size ) {
            list->var_info.error_code |= LIST_ERR_STRAIGHT_LACK_OF_ELEMENTS;
        }

        // CHECK FOR REVERSE LOOP
        idx = ListGetTail( list );
        counter = 0;

        while ( idx > 0 ) {
            idx = ListGetPrev( list, idx );
            counter++;

            if ( counter > size ) {
                list->var_info.error_code |= LIST_ERR_REVERSE_LOOP;

                break;
            }
        }

        if ( counter < size ) {
            list->var_info.error_code |= LIST_ERR_REVERSE_LACK_OF_ELEMENTS;
        }

        // CHECK FOR LOOP OF FREE ELEMENTS
        idx = ListGetFree( list );
        counter = 0;

        while ( idx > 0) {
            idx = ListGetNext( list, idx );
            counter++;

            if ( counter > capacity - size ) {
                list->var_info.error_code |= LIST_ERR_FREE_LOOP;

                break;
            }
        }

        if ( counter < capacity - size ) {
            list->var_info.error_code |= LIST_ERR_FREE_LACK_OF_ELEMENTS;
        }

        if ( list->var_info.error_code != LIST_ERR_NONE ) {
            return FAIL_VERIFY;
        }

        return SUCCESS;
    }
#endif


ListStatus_t ListLinearizer( List_t* list ) {
    PRINT_EXECUTING;
    my_assert( list, "Null pointer on `list`" );

    VERIFY( "Before <font color=\"cyan\">Linearizer</font>" );

    Element_t* data = ( Element_t* ) calloc ( ( size_t ) list->size + 1, sizeof( *data ) );
    assert( data && "Memory allocation error" );

    int old_idx = ListGetHead( list );
    int new_idx = 1;

    while ( old_idx != 0 ) {
        data[ new_idx ] = {
            .prev  = new_idx - 1,
            .value = ListGetElement( list, old_idx ),
            .next  = new_idx + 1
        };

        old_idx = ListGetNext( list, old_idx );
        new_idx++;
    }

    data[ new_idx - 1 ].next = 0;

    while ( new_idx <= ListGetCapacity( list ) ) {
        data[ new_idx ].value = LIST_POISON_VALUE;
        data[ new_idx ].next  = new_idx + 1;
        data[ new_idx ].prev  = LIST_FREE_PREV_IDX;

        new_idx++;
    }

    data[ new_idx - 1 ].next = 0;
    data[0] = {
        .prev = ListGetSize( list ),
        .next = ( ListGetSize( list ) > 0 ) ? 1 : 0
    };

    free( list->elements );
    list->elements = data;
    list->free     = ListGetSize( list ) + 1;


    VERIFY( "After <font color=\"cyan\">Linearizer</font>" );

    PRINT_STATUS_OK;
    return SUCCESS;
}

int ListGetHead( const List_t* list ) {
    my_assert( list, "Null pointer on `list`" );

    return list->elements[0].next;
}

int ListGetTail( const List_t* list ) {
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

int ListGetNext( const List_t* list, const int index ) {
    my_assert( list, "Null pointer on `list`" );

    if ( list->capacity < index ) return -1;

    return list->elements[ index ].next;
}

int ListGetPrev( const List_t* list, const int index ) {
    my_assert( list, "Null pointer on `list`" );

    if ( list->capacity < index ) return -1;

    return list->elements[ index ].prev;
}

ElementData_t ListGetElement( const List_t* list, const int index ) {
    my_assert( list, "Null pointer on `list`" );

    return list->elements[ index ].value;
}

ListStatus_t ListInsertAfter( List_t* list, const int index, const ElementData_t number ) {
    PRINT_EXECUTING;
    my_assert( list, "Null pointer on `list`" );

    VERIFY( "Before <font color=\"blue\">INSERT ( %lg ) AFTER <%d></font>", number, index );

    if ( list->size == 0 && index > 0 ) {
        fprintf( stderr, COLOR_BRIGHT_RED "Insert after <%d> in empty list \n", index );
        PRINT_STATUS_FAIL;
        return FAIL;
    }

    if ( ListGetPrev( list, index ) == -1 ) {
        fprintf( stderr, COLOR_BRIGHT_RED "Insert after empty element \n" );
        PRINT_STATUS_FAIL;
        return FAIL;   
    }

    if ( ListGetSize( list ) >= ListGetCapacity( list ) ) {
        ListLinearizer( list, list->capacity * 2 );
    }

    int next_free_idx = ListGetNext( list, ListGetFree( list ) );

    list->elements[ list->free ] = { 
        .prev = index, 
        .value = number, 
        .next = list->elements[ index ].next 
    };

    list->elements[ index ].next = list->free;
    list->elements[ list->elements[ list->free ].next ].prev = list->free;

    list->free = next_free_idx;

    list->size++;

    VERIFY( "After <font color=\"blue\">INSERT (%lg) AFTER <%d></font>", number, index );

    PRINT_STATUS_OK;
    return SUCCESS;
}

ListStatus_t ListInsertBefore( List_t* list, const int index, const ElementData_t number ) {
    PRINT_EXECUTING;
    my_assert( list, "Null pointer on `list`" );
    my_assert( isfinite( number ), "An infinite number" );

    VERIFY( "Before <font color=\"blue\"> INSERT (%lg) BEFORE <%d></font>", number, index );

    if ( list->size == 0 && index > 2 ) {
        PRINT_STATUS_FAIL;
        return FAIL;
    }

    if ( list->elements[ index ].prev == -1 ) {
        fprintf( stderr, COLOR_BRIGHT_RED "Insert before empty element \n" );
        PRINT_STATUS_FAIL;
        return FAIL;   
    }

    if ( list->size >= list->capacity ) {
        ListLinearizer( list, list->capacity * 2 );
    }

    int next_free_idx = list->elements[ list->free ].next;

    list->elements[ list->free ].value = number;

    list->elements[ list->free ].prev = list->elements[ index ].prev;
    list->elements[ list->free ].next = index;

    list->elements[ index ].prev = list->free;
    list->elements[ list->elements[ list->free ].prev ].next = list->free;

    list->free = next_free_idx;

    list->size++;

    VERIFY( "After <font color=\"blue\">INSERT (%lg) BEFORE <%d></font>", number, index );

    PRINT_STATUS_OK;
    return SUCCESS;
}

ListStatus_t ListDelete( List_t* list, const int index ) {
    PRINT_EXECUTING;
    my_assert( list, "Null pointer on `list`" );

    VERIFY( "Before <font color=\"red\">DELETE <%d></font>", index );

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

    list->elements[ index ].value = LIST_POISON_VALUE;
    list->elements[ index ].prev  = -1;
    list->elements[ index ].next  = list->free;

    list->free = index;
    list->size--;

    VERIFY( "After <font color=\"red\">DELETE <%d></font>", index );

    PRINT_STATUS_OK;
    return SUCCESS;
}


// ----------------------------- LOGGING -------------------------------------------
#ifdef _DEBUG
    #define PRINT_IN_LOG( str, ... )        fprintf(  list->logging.log_file, str, ##__VA_ARGS__ );
    #define PRINT_IN_LOG_VAARG( str, args ) vfprintf( list->logging.log_file, str, args );
    #define PRINT_IN_GRAPHIC( str, ... )    fprintf(  graphic_file,           str, ##__VA_ARGS__ );

    #define PASS

    static void CommandExecute( const char* format_string, ... ) {
        my_assert( format_string, "Null pointer of `format_string`" );

        char command[ MAX_LEN_PATH ] = "";
        va_list args;
        va_start( args, format_string );
        vsnprintf( command, MAX_LEN_PATH, format_string, args );
        va_end( args );
        
        system( command );
    }

    static void FolderCreate( List_t* list ) {
        my_assert( list, "Null pointer on `list`" );

        time_t current_time = time( NULL );
        struct tm* local_time;
        local_time = localtime( &current_time );
        
        char folder_name[ MAX_LEN_PATH ] = "";
        strftime( folder_name, sizeof( folder_name ), "%d.%m_%H:%M:%S", local_time );

        char buffer[ MAX_LEN_PATH ] = "";

        snprintf( buffer, MAX_LEN_PATH, "logs/%s", folder_name );
        list->logging.common_log_directory = strdup( buffer );

        snprintf( buffer, MAX_LEN_PATH, "%s/images", list->logging.common_log_directory );
        list->logging.log_img_directory = strdup( buffer );

        snprintf( buffer, MAX_LEN_PATH, "%s/image_sources", list->logging.common_log_directory );
        list->logging.log_img_src_directory = strdup( buffer );

        snprintf( buffer, MAX_LEN_PATH, "%s/LOGFILE.htm", list->logging.common_log_directory );
        list->logging.log_file_path = strdup( buffer );

        CommandExecute( "mkdir -p %s", list->logging.log_img_directory );
        CommandExecute( "mkdir -p %s", list->logging.log_img_src_directory );
    }

    static void ListPrintStats( const List_t* list ) {
        my_assert( list, "Null pointer on `list`" );

        PRINT_IN_LOG( 
            "List `%s` { %s:%lu } \n", 
            list->var_info.name,
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
    }


    static void ListDump( const List_t* list, const char* service_message, va_list args ) {
        my_assert( list, "Null pointer to `list`" );

        PRINT_IN_LOG      ( "<h3> DUMP " );
        PRINT_IN_LOG_VAARG( service_message, args );
        PRINT_IN_LOG      ( " </h3> \n" );

        uint error_code = list->var_info.error_code;

        if ( error_code != LIST_ERR_NONE ) {
            if ( error_code & LIST_ERR_SIZE_CAPACITY_CORRUPTION ) {
                PRINT( COLOR_BRIGHT_RED "Size/capacity corruption!!!" );

                return;
            }

            const char* error_message[] = {
                "Size/capacity corruption",
                "Poison corruption",
                "Straight loop",
                "Reverse loop",
                "Free loop",
                "Lack of elements in straight passage",
                "Lack of elements in reverse passage",
                "Lack of elements in passage of free elements"
            };

            for ( int n = 0; n < 8; n++ ) {
                if ( list->var_info.error_code & ( 1 << n) ) {
                    PRINT_IN_LOG( "<font color=\"red\">%s</font> \n", error_message[ n ] );
                }
            }
        }
        else {
            PRINT_IN_LOG( "<font color=\"green\">No error</font>\n" );
        }

        
        PRINT_IN_LOG( "\n\n" );

        ListPrintStats( list );

        PRINT_IN_LOG( 
            "\n<img src=\"images/image%lu.png\" height=\"200px\">\n",
            list->logging.image_number
        );

        PRINT_IN_LOG( "\n\n" );
    }

    static void ProcessConnection( FILE* graphic_file, List_t* list, int source_idx ) {
        my_assert( graphic_file, "Null pointer on `graphic file" );
        my_assert( list, "Null pointer on `list`" );

        int next_idx = list->elements[ source_idx ].next;
        int prev_idx = list->elements[ source_idx ].prev;

        if ( prev_idx == LIST_FREE_PREV_IDX ) {
            if ( next_idx == 0 ) return;

            PRINT_IN_GRAPHIC(
                "\tnode%d -> node%d [constraint=false, color=\"#643a6fff\", style=dashed, arrowsize=0.6];\n",
                source_idx, next_idx  
            );
            
            return;
        }

        if ( next_idx <= list->capacity ) {
            PRINT_IN_GRAPHIC( 
                "\tnode%d -> node%d [%s]; \n",
                source_idx, next_idx,
                ( source_idx == list->elements[ next_idx ].prev ) ? ( "constraint=false, color=black, dir=both, arrowhead=vee, arrowtail=vee, arrowsize=0.5" ) : ( "constraint=false, color=blue, penwidth=3.0" )
            );
        }
        else {
            PRINT_IN_GRAPHIC(
                "\tnode%d -> node%d[%s]\n",
                source_idx, next_idx,
                "constraint=false, color=red, penwidth=4.0"
            );
        }

        if ( prev_idx <= list->capacity ) {
            if ( source_idx == list->elements[ prev_idx ].next ) {
                PASS;
            }
            else {
                PRINT_IN_GRAPHIC(
                    "\tnode%d -> node%d[%s];\n",
                    source_idx, prev_idx, 
                    "constraint=false, color=cyan, penwidth=3.0"
                );
            }
        }
        else {
            PRINT_IN_GRAPHIC(
                "\tnode%d -> node%d[%s]\n",
                source_idx, prev_idx,
                "constraint=false, color=red, penwidth=5.0"
            );
        }        
    }

    static void GraphicPrintoutDraw( List_t* list ) {
        my_assert( list, "Null pointer on `list`" );

        char graphic_source_address[ MAX_LEN_PATH ] = "";

        snprintf( 
            graphic_source_address, MAX_LEN_PATH, 
            "%s/image%lu.dot", 
            list->logging.log_img_src_directory, list->logging.image_number
        );

        FILE* graphic_file = fopen( graphic_source_address, "w" );
        assert( graphic_file && "File opening error" );

        PRINT_IN_GRAPHIC( "digraph {\n\trankdir=LR;\tsplines=ortho;\n\tnodesep=1.5;\n\n\tnode[shape=doubleoctagon; color=\"red\"];\n\n" );
        PRINT_IN_GRAPHIC( 
            "\tnode0 [shape=Mrecord; style=filled; color=black; fillcolor=orange; label = \"prev=%d | next=%d\" ]; \n", 
            list->elements[0].prev,
            list->elements[0].next
        );

        for ( int idx = 1; idx <= list->capacity; idx++ ) {
            PRINT_IN_GRAPHIC( 
                "\tnode%d [shape=Mrecord; style=filled; color=black; fillcolor=\"#%X\"; label=\"idx=%d | value=%lg | { prev=%d | next=%d }\" ]; \n", 
                idx, 
                ( uint32_t ) ( ( list->elements[ idx ].prev == LIST_FREE_PREV_IDX ) ? COLOR_FREE : COLOR_OCCUPIED ),
                idx,
                list->elements[ idx ].value,
                list->elements[ idx ].prev,
                list->elements[ idx ].next
            );
            // PRINT_IN_GRAPHIC( "\t{ rank=same; node%d };\n", idx );
        }

        PRINT_IN_GRAPHIC( "\n\n" );

        for ( int idx = 0; idx < list->capacity; idx++ ) {
            PRINT_IN_GRAPHIC( 
                "\tnode%d -> node%d [style=invis];\n", 
                idx, 
                idx + 1
            );
        }

        PRINT_IN_GRAPHIC( "\n\n")

        for ( int idx = 0; idx <= list->capacity; idx++ ) {
            ProcessConnection( graphic_file, list, idx );
        }

        #define ANNOUNCE( string ) \
            PRINT_IN_GRAPHIC( \
                "\t%s [shape=note; style=filled; color=black; fillcolor=\"#FFFF99\"; label=\"%s\\n %d\"];\n", \
                #string, #string, ListGet##string( list ) \
            ); \
            PRINT_IN_GRAPHIC( "\t{ rank=same; %s; node%d };\n", #string, ListGet##string ( list ) ); \
            PRINT_IN_GRAPHIC( "\t%s -> node%d[arrowsize=0.7];\n\n", #string, ListGet##string ( list ) );

        ANNOUNCE( Head );
        ANNOUNCE( Tail );
        ANNOUNCE( Free );

        #undef ANNOUNCE

        PRINT_IN_GRAPHIC( "}" );

        int close_result = fclose( graphic_file );
        assert( close_result == 0 && "File closing error" );

        CommandExecute( 
            "dot %s -Tpng -o %s/image%lu.png", 
            graphic_source_address, list->logging.log_img_directory, list->logging.image_number 
        );

        list->logging.image_number++;
    }


    void ListLog( List_t* list, const LogModes mode, const char* service_message, ... ) { 
        switch ( mode ) {   
            case BEGIN_OF_PROGRAMM: {
                FolderCreate( list );

                list->logging.log_file = fopen( list->logging.log_file_path, "w" );
                assert( list->logging.log_file && "File opening error" );

                fprintf( list->logging.log_file, "<pre>\n" );

                break;
            }
            case DUMP: {
                my_assert( list, "Null pointer on `list`" );

                va_list args;
                va_start( args, service_message );
                ListDump( list, service_message, args );
                va_end( args );
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
#endif