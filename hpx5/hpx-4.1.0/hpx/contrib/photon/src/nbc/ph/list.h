#ifndef _LIST_H_
#define _LIST_H_

#include <stdio.h>
#include <stdlib.h>
typedef struct node {
  int data;
  struct node *next;
} Node;

typedef struct list { Node *head; } List;

List *emptylist();
void add(int data, List *list);
int delete (int data, List *list);
void display(List *list);
void reverse(List *list);
void destroy(List *list);

Node *createnode(int data);
#endif
