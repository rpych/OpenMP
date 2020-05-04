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


Node* partition(Node** head, Node* start, Node* end)
{
    Node* left = NULL;
    Node* right = NULL;
    Node* prev_left = NULL;
    Node* prev_right = NULL;
    Node* pivot = start;
    Node* it = (start)->next;
    
    while(it != (end)->next)
    {
        
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
            //printf("partition left %d\n", prev_left->data);
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
            //printf("partition right %d\n", prev_right->data);
        }
        
    }
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
    
    return pivot;
}

void quicksortRec(Node** head, Node* left, Node* right)
{
    if((*head)==NULL || left == right )
    {
        return;
    }    
    
    Node* pivot = NULL;
    pivot = partition(head, left, right);
    
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
    }
    
    Node* tmp_right = getTail(pivot);
    Node* new_left = pivot->next;
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
    quicksortRec(head, *head, tail);
}



























