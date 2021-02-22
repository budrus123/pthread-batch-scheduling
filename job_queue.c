#include <stdlib.h>
#include <stdio.h>
#include "job.h"

struct Node 
{ 
	struct Job* job; 
	struct Node* next; 
}; 

// typedef struct Node* job_queue;

// Helper function to print a given linked list
void printList(struct Node* head)
{
    struct Node* ptr = head;
    while (ptr)
    {
        printf("%d —> ", ptr->job->id);
        ptr = ptr->next;
    }
 
    printf("null");
}
 
// Helper function to insert a new node at the beginning of the linked list
void push(struct Node** head, int data)
{
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->job->id = data;
    newNode->next = *head;
    *head = newNode;
}
 
// Function to insert a given node at its correct sorted position into a given
// list sorted in increasing order
void sortedInsert(struct Node** head, struct Node* newNode)
{
    struct Node dummy;
    struct Node* current = &dummy;
    dummy.next = *head;
 
    while (current->next != NULL && current->next->job->id < newNode->job->id) {
        current = current->next;
    }
 
    newNode->next = current->next;
    current->next = newNode;
    *head = dummy.next;
}
 
// Given a list, change it to be in sorted order (using `sortedInsert()`).
void InsertSort(struct Node** head)
{
    struct Node* result = NULL;     // build the answer here
    struct Node* current = *head;   // iterate over the original list
    struct Node* next;
 
    while (current != NULL)
    {
        // tricky: note the next pointer before we change it
        next = current->next;
 
        sortedInsert(&result, current);
        current = next;
    }
 
    *head = result;
}
 
// int main(void)
// {
//     // input keys
//     int keys[] = {6, 3, 4, 8, 2, 9};
//     int n = sizeof(keys)/sizeof(keys[0]);
 
//     // points to the head node of the linked list
//     struct Node* head = NULL;
 
//     // construct a linked list
//     for (int i = n-1; i >= 0; i--) {
//         push(&head, keys[i]);
//     }
 
//     InsertSort(&head);
 
//     // print linked list
//     printList(head);
 
//     return 0;
// }