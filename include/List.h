#include <stdlib.h>
#include <sys/types.h>

#ifdef _LINUX
#include <linux/limits.h>
const size_t MAX_LEN_PATH = PATH_MAX;
#else
const size_t MAX_LEN_PATH = 128;
#endif

#ifndef LIST_H
#define LIST_H

#include "DebugUtils.h"


#define INIT( name ) ON_DEBUG( .var_info={ LIST_ERR_NONE, #name, __func__, __FILE__, __LINE__ } )
#define CHECK_STATUS( command )                                                       \
    if ( command != SUCCESS ) {                                                       \
        fprintf( stderr, COLOR_BRIGHT_RED "Incorrect command `%s` \n", #command );    \
        ListDtor( &list );                                                             \
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

#ifdef _DEBUG
    struct VarInfo_t {
        uint        error_code;
        const char* name;
        const char* func;
        const char* file;
        size_t      line;
    };

    struct LogStat {
        char* common_log_directory;
        char* log_file_path;
        char* log_img_directory;
        char* log_img_src_directory;

        FILE* log_file;

        size_t image_number;
    };

    enum ListErrorCodes {
        LIST_ERR_NONE                     = 0,
        LIST_ERR_SIZE_CAPACITY_CORRUPTION = 1 << 0,
        LIST_ERR_POISON_CORRUPTION        = 1 << 1,
        LIST_ERR_STRAIGHT_LOOP            = 1 << 2,
        LIST_ERR_REVERSE_LOOP             = 1 << 3,
        LIST_ERR_FREE_LOOP                = 1 << 4,
        LIST_ERR_LACK_OF_ELEMENTS         = 1 << 5
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

// TODO: hide struct fields from users: transfer struct fo .cpp and add typedef
// struct list;
// typedef list List_t;

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

    #ifdef _DEBUG
        VarInfo_t var_info;
        LogStat logging;
    #endif
};


ListStatus_t ListCtor( List_t* list, const int data_capacity );
ListStatus_t ListDtor( List_t* lsit );

void ListRealloc( List_t* list, const int new_capacity );

ListStatus_t ListInsertAfter ( List_t* list, const int index, const ElementData_t number  );
ListStatus_t ListInsertBefore( List_t* list, const int index, const ElementData_t number );
ListStatus_t ListDelete      ( List_t* list, const int index );

int ListGetHead( List_t* list );
int ListGetTail( List_t* list );

int ListGetHead    ( const List_t* list );
int ListGetTail    ( const List_t* list );
int ListGetFree    ( const List_t* list );
int ListGetSize    ( const List_t* list );
int ListGetCapacity( const List_t* list );

int           ListGetNext   ( const List_t* list, const int index );
int           ListGetPrev   ( const List_t* list, const int index );
ElementData_t ListGetElement( const List_t* list, const int index );

#ifdef _DEBUG
    ListStatus_t ListVerify( List_t* list );

    LogStat* ListGetLogging( const List_t* list );

    void ListLog( List_t* list, LogModes mode, const char* service_message, ... );

    void FolderCreate( List_t* list );

    void ListDump      ( const List_t* list, const char* service_message, va_list args );
    void ListPrintStats( const List_t* list );

    void GraphicPrintoutDraw( List_t* list );
#endif

// за size  проверять, что мы возвращаемся к нулевому элементу

#endif // LIST_H