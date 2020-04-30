#ifndef LIST_UTILS_H_INCLUDED
#define LIST_UTILS_H_INCLUDED

typedef struct Node
{
    int data;
    struct Node *next;
} Node;

// void push(struct Node** head_ref, int new_data);
// void printList(struct Node *node);
// struct Node *getTail(struct Node *cur);
// struct Node *partition(struct Node *head, struct Node *end,
//                     struct Node **newHead, struct Node **newEnd);
// struct Node *quickSortRecur(struct Node *head, struct Node *end);
// void quickSort(struct Node **headRef);


void push(struct Node** head_ref, int new_data);
void printList(Node *node);
Node *getTail( Node *cur);
Node* partition(Node** head, Node* start, Node* end);
void quicksortRec(Node** head, Node* left, Node* right);
void quickSort(Node **headRef);

#endif