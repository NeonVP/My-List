#include <stdio.h>
#include <stdlib.h>

#ifndef LIST_H
#define LIST_H

#include "DebugUtils.h"


#define INIT( name ) ON_DEBUG( .varInfo={ 0, #name, __func__, __FILE__, __LINE__ } )

const size_t MAX_LEN_PATH = 100;

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
#endif

struct List_t {
    Element_t* head;
    Element_t* tail;
    int free;

    Element_t* data;
    
    size_t size;
    size_t capacity;

    #ifdef _DEBUG
        VarInfo_t varInfo;
        char log_file_address[ MAX_LEN_PATH ];
        char log_file_folder [ MAX_LEN_PATH ];
        FILE* log_file;
    #endif
};


ListStatus_t ListCtor( List_t* list, const size_t data_capacity );
ListStatus_t ListDtor( List_t* lsit );

ListStatus_t ListInsert( List_t* list, const int index, const ElementData_t number  );
ListStatus_t ListDelete( List_t* list, const int index );

void ListVerify( List_t* list );

void ListLog( List_t* list, LogModes mode );
void FileAndFolderCreate( List_t* list);
void ListDump( const List_t* list );


#endif // LIST_H