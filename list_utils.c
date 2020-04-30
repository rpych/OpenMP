#include "list_utils.h"
#include <stdio.h>
#include <stdlib.h>

void push(struct Node** head_ref, int new_data)
{
    struct Node* new_node = malloc( sizeof( Node ) );
    new_node->data = new_data;
    new_node->next = (*head_ref);
    (*head_ref) = new_node;
}

void printList(struct Node *node)
{
    while (node != NULL)
    {
        printf("%d ", node->data);
        node = node->next;
    }
    printf("\n");
}

// struct Node *getTail(struct Node *cur)
// {
//     while (cur != NULL && cur->next != NULL)
//         cur = cur->next;
//     return cur;
// }

// struct Node *partition(struct Node *head, struct Node *end,
//                     struct Node **newHead, struct Node **newEnd)
// {
//     struct Node *pivot = end;
//     struct Node *prev = NULL, *cur = head, *tail = pivot;

//     while (cur != pivot)
//     {
//         if (cur->data < pivot->data)
//         {
//             if ((*newHead) == NULL)
//                 (*newHead) = cur;

//             prev = cur;
//             cur = cur->next;
//         }
//         else
//         {
//             if (prev)
//                 prev->next = cur->next;
//             struct Node *tmp = cur->next;
//             cur->next = NULL;
//             tail->next = cur;
//             tail = cur;
//             cur = tmp;
//         }
//     }
//     if ((*newHead) == NULL)
//         (*newHead) = pivot;

//     (*newEnd) = tail;
//     return pivot;
// }


// struct Node *quickSortRecur(struct Node *head, struct Node *end)
// {
//     if (!head || head == end)
//         return head;

//     Node *newHead = NULL, *newEnd = NULL;
//     struct Node *pivot = partition(head, end, &newHead, &newEnd);

//     if (newHead != pivot)
//     {
//         struct Node *tmp = newHead;
//         while (tmp->next != pivot)
//             tmp = tmp->next;
//         tmp->next = NULL;
//         newHead = quickSortRecur(newHead, tmp);
//         tmp = getTail(newHead);
//         tmp->next = pivot;
//     }
//     pivot->next = quickSortRecur(pivot->next, newEnd);
//     return newHead;
// }


// void quickSort(struct Node **headRef)
// {
//     (*headRef) = quickSortRecur(*headRef, getTail(*headRef));
//     return;
// }


Node* partition(Node** head, Node* start, Node* end)
{
    Node* left = NULL;
    Node* right = NULL;
    Node* prev_left = NULL;
    Node* prev_right = NULL;
    Node* pivot = start;
    //pivotRet = pivot;
    Node* it = (start)->next;
    int c = 0;
    if((end)->next == NULL)
        printf("End is NULL\n");
    while(it != (end)->next)
    {
        // if(it == NULL){
        //     printf("IT is NULL\n");
        //     break;
        // }
        if(it->data <= pivot->data)
        {
            if(left==NULL)
            {
                left = it;
                prev_left = left;
                it = it->next;
                prev_left->next = NULL;
                continue;
            }
            Node* tmp = prev_left;    
            prev_left = it;    
            it = it->next; 
            prev_left->next = NULL;
            tmp->next = prev_left; 
            printf("partition left %d\n", prev_left->data);
        }
        else
        {
            if(right==NULL)
            {
                right = it;
                prev_right = right;
                it = it->next;
                prev_right->next = NULL;
                continue;
            }
            Node* tmp = prev_right;    
            prev_right = it;    
            it = it->next; 
            prev_right->next = NULL;
            tmp->next = prev_right; 
            printf("partition right %d\n", prev_right->data);
        }
        
    }
    //Node* tmpl = *start;
    //printf("LEFT \n");
    //printList(left);
    //printf("RIGHT\n");
    //Node* tmpr = *end;
    //printList(right);
    pivot->next = right;
    if(left!=NULL)
    {
        prev_left->next = pivot;
        (*head) = left;
    }
    else
    {
        (*head) = pivot;
    }
    
    printf("partition pivot %d, head = %d\n", pivot->data, (*head)->data);
    return pivot;
}

void quicksortRec(Node** head, Node* left, Node* right)
{
    if((*head)==NULL || left == right ) //|| right == NULL
    {
        printf("KONIEC REKURENCJI HEAD = %d oraz left==right %d\n", (*head)->data, (left==right));
        return;
    }    
    
    Node* pivot = NULL;

    pivot = partition(head, left, right);
    printf("After partition\n");
    
    //printList(*head);
    if((*head)!=NULL && (*head) != pivot)
    {
        Node* tmp = (*head);
        while(tmp->next!=pivot)
        {
            tmp = tmp->next;
        }
        tmp->next = NULL;
        quicksortRec(head, *head, tmp);
        tmp = getTail(*head);
        tmp->next = pivot;
        printf("HEAD po QUICKSORCIE %d\n", (*head)->data);
    }
    
    Node* tmp_right = getTail(pivot);
    Node* new_left = pivot->next;
    //printf("PIVOT = %d, NEW_LEFT = %d, TMP_RIGHT=%d\n", pivot->data, new_left->data, tmp_right->data);
    if(new_left != NULL)
        quicksortRec(&new_left, new_left, tmp_right);
    if(pivot != new_left && new_left != NULL)
        pivot->next = new_left;
    

}

Node* getTail(Node* head)
{
    Node* it = head;
    while(it->next!=NULL)
    {
        it = it->next;
    }
    return it;
}

void quickSort(Node** head)
{
    Node* tail = getTail(*head);
    printf("TAIL = %d\n", tail->data);
    quicksortRec(head, *head, tail);
}



























