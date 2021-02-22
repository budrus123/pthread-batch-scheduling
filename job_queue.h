#include <stdlib.h>
#include <stdio.h>
// #include "job.h"

/* Link list node */
// typedef struct Node* job_queue;


void sortedInsert(struct Node** head_ref, struct Node* new_node);
void insertionSort(struct Node **head_ref);
void printList(struct Node *head);
void push(struct Node** head_ref, int new_data);