#ifndef ERRORS_H
#define ERRORS_H

#include <stdio.h>
#include "Colors.h"

#ifdef _DEBUG
    #define my_assert(arg, message)                                                                       \
        if ( !( arg ) ) {                                                                                     \
            fprintf( stderr, COLOR_RED "Error in function `%s` %s:%d: %s \n" COLOR_RESET,                 \
                    __func__, __FILE__, __LINE__, message );                                              \
            abort();                                                                                      \
        }

    #define PRINT(str, ...) fprintf( stderr, str COLOR_RESET, ##__VA_ARGS__ );
    
    #define PRINT_IN  PRINT( GRID COLOR_BRIGHT_YELLOW "In `%s` \n", __func__ );
    #define PRINT_OUT PRINT( GRID COLOR_BRIGHT_YELLOW "OUT `%s` \n", __func__ );

    #define ON_DEBUG(...) __VA_ARGS__
#else
    #define my_assert(arg, message) ((void) (arg));

    #define PRINT(str, ...)

    #define PRINT_IN  fprintf( stderr, GRID COLOR_BRIGHT_YELLOW "In %s \n", __func__ );
    #define PRINT_OUT fprintf( stderr, GRID COLOR_BRIGHT_YELLOW "OUT %s \n", __func__ );

    #define ON_DEBUG(...)
#endif //_DEBUG

#endif //ERRORS_H