#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#ifndef LIST_H
#define LIST_H

#include "DebugUtils.h"


#define INIT( name ) ON_DEBUG( .var_info={ LIST_ERR_NONE, #name, __func__, __FILE__, __LINE__ } )
#define CHECK_STATUS( command )                                                       \
    if ( command != SUCCESS ) {                                                       \
        fprintf( stderr, COLOR_BRIGHT_RED "Incorrect command `%s` \n", #command );    \
        return EXIT_FAILURE;                                                          \
    }

#ifdef _DEBUG
#define VERIFY( ... )                                     \
    if ( ListVerify( list ) != SUCCESS ) {                \
        PRINT_STATUS_FAIL;                                \
        return FAIL;                                      \
    }                                                     \
    __VA_ARGS__                                           \
    if ( ListVerify( list ) != SUCCESS ) {                \
        PRINT_STATUS_FAIL;                                \
        return FAIL;                                      \
    }
#else
#define VERIFY( list ) __VA_ARGS__
#endif

const size_t MAX_LEN_PATH = 50;

#ifdef _DEBUG
    struct VarInfo_t {
        long        error_code;
        const char* name;
        const char* func;
        const char* file;
        size_t      line;
    };

    struct LogStat {
        char log_directory [ MAX_LEN_PATH ];
        char log_file_path [ MAX_LEN_PATH + 12 ];     
        /* 
        The buffer needs extra 12 bytes to safely store the appended "/LOGFILE.htm" string. 
        This prevents truncation when combining the folder path with the fixed filename. 
        */

        FILE* log_file;

        size_t image_number;
    };

    enum ListErrorCodes {
        LIST_ERR_NONE                   = 1 << 0,
        LIST_ERR_SIZE_OVER_CAPACITY     = 1 << 1,
        LIST_ERR_MISSING_NODE_DATA      = 1 << 2,
        LIST_ERR_NULL_CAPACITY          = 1 << 3,
    };

    enum LogModes {
        BEGIN_OF_PROGRAMM = 0,
        DUMP              = 1,
        END_OF_PROGRAM    = 2
    };

    enum FillColors {
        COLOR_FREE     = 0xE6F3FF,
        COLOR_OCCUPIED = 0xB3D9FF
    };
#endif

enum ListStatus_t {
    SUCCESS = 0,
    FAIL    = 1
};

typedef double ElementData_t;

const ElementData_t poison = 777;

struct Element_t {
    int           prev;
    ElementData_t value;
    int           next;
};

struct List_t {    
    int size;
    int capacity;
    
    int free;

    Element_t* elements;

    #ifdef _DEBUGlist->elements[0].next );
    PRINT_IN_GRAPHIC( "tail -> node%d [arrowhead=vee];\n\t", list->elements[0].prev );
    PRINT_IN_GRAPHIC( "free -> node%d [arrowhead=vee];\n\t", list->free );
        VarInfo_t var_info;
        LogStat logging;
    #endif
};


ListStatus_t ListCtor( List_t* list, const int data_capacity );
ListStatus_t ListDtor( List_t* lsit );
void ListRealloc( List_t* list, const int new_capacity );

ListStatus_t ListInsert( List_t* list, const int index, const ElementData_t number  );
ListStatus_t ListDelete( List_t* list, const int index );

#ifdef _DEBUG
    ListStatus_t ListVerify( List_t* list );

    void ListLog            ( List_t* list, LogModes mode, const char* service_message );
    void FileAndFolderCreate( List_t* list );
    void ListDump           ( const List_t* list, const char* service_message );
    void GraphicPrintoutDraw( List_t* list );
#endif


#endif // LIST_H