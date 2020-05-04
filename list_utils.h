#ifndef LIST_UTILS_H_INCLUDED
#define LIST_UTILS_H_INCLUDED

typedef struct Node
{
    int data;
    struct Node *next;
} Node;


void push(struct Node** head_ref, int new_data);
void printList(Node *node);
Node *getTail( Node *cur);
Node* partition(Node** head, Node* start, Node* end);
void quicksortRec(Node** head, Node* left, Node* right);
void quickSort(Node **headRef);

#endif