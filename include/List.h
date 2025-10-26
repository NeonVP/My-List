#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#ifndef LIST_H
#define LIST_H

#include "DebugUtils.h"


#define INIT( name ) ON_DEBUG( .var_info={ 0, #name, __func__, __FILE__, __LINE__ } )
#define CHECK_STATUS( command )                                                       \
    if ( command != SUCCESS ) {                                                       \
        fprintf( stderr, COLOR_BRIGHT_RED "Incorrect command `%s` \n", #command );    \
        return EXIT_FAILURE;                                                          \
    }

const size_t MAX_LEN_PATH = 50;

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

#ifdef _DEBUG
    struct VarInfo_t {
        long        error_code;
        const char* name;
        const char* func;
        const char* file;
        size_t      line;
    };

    struct LogStat_t {
        char log_file_folder [ MAX_LEN_PATH ];
        char log_file_address[ MAX_LEN_PATH + 12 ];     
        /* 
        The buffer needs extra 12 bytes to safely store the appended "/LOGFILE.htm" string. 
        This prevents truncation when combining the folder path with the fixed filename. 
        */
        FILE* log_file;

        size_t image_number;
    };

    enum ListErrorCodes {
        LIST_ERROR_NONE               = 1 << 0,
        LIST_ERROR_SIZE_OVER_CAPACITY = 1 << 1,
        LIST_ERROR_ISNT_POISON        = 1 << 2,
        LIST_ERROR_NULL_CAPACITY      = 1 << 3
    };

    enum LogModes {
        BEGIN_OF_PROGRAMM = 0,
        DUMP              = 1,
        END_OF_PROGRAM    = 2
    };

    enum FillColors {
        COLOR_FREE = 0xEAB56F,
        COLOR_OCCUPIED = 0xE37239
    };
#endif

struct List_t {
    int head;
    int tail;
    int free;

    Element_t* data;
    
    int size;
    int capacity;

    #ifdef _DEBUG
        VarInfo_t var_info;
        LogStat_t logging;
    #endif
};


ListStatus_t ListCtor( List_t* list, const int data_capacity );
ListStatus_t ListDtor( List_t* lsit );

ListStatus_t ListInsert( List_t* list, const int index, const ElementData_t number  );
ListStatus_t ListDelete( List_t* list, const int index );

void ListVerify( List_t* list );

void ListLog( List_t* list, LogModes mode );
void FileAndFolderCreate( List_t* list);
void ListDump( const List_t* list );
void GraphicPrintoutDraw( List_t* list );


#endif // LIST_H