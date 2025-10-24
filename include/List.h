#ifndef LIST_H
#define LIST_H


enum ListStatus_t {
    SUCCESS = 0,
    FAIL    = 1
};

typedef int ElementData_t;

struct Element_t {
    Element_t*    prev;
    ElementData_t data;
    Element_t*    next;
};

struct List_t {
    Element_t* head;
    Element_t* tail;
};


ListStatus_t ListCtor( List_t* list );
ListStatus_t ListDtor( List_t* lsit );

ListStatus_t ListInsert( List_t* list );
ListStatus_t ListDelete( List_t* list );


void ListDump  ( const List_t* list );
void ListVerify( const List_t* list );


#endif LIST_H