#include "list.h"

Node *createnode(int data) {
  Node *newNode = malloc(sizeof(Node));
  newNode->data = data;
  newNode->next = NULL;
  return newNode;
}

List *emptylist() {
  List *list = malloc(sizeof(List));
  list->head = NULL;
  return list;
}

void display(List *list) {
  Node *current = list->head;
  if (list->head == NULL)
    return;
  while (current->next != NULL) {
    printf("%d,", current->data);
    current = current->next;
  }
  printf("%d\n", current->data);
}

void add(int data, List *list) {
  Node *current = NULL;
  if (list->head == NULL) {
    list->head = createnode(data);
  } else {
    current = list->head;
    while (current->next != NULL) {
      current = current->next;
    }
    current->next = createnode(data);
  }
}

int delete (int data, List *list) {
  Node *current = list->head;
  Node *previous = current;
  while (current != NULL) {
    if (current->data == data) {
      previous->next = current->next;
      if (current == list->head)
        list->head = current->next;
      free(current);
      return 1;
    }
    previous = current;
    current = current->next;
  }
  return 0;
}

void reverse(List *list) {
  Node *reversed = NULL;
  Node *current = list->head;
  Node *temp = NULL;
  while (current != NULL) {
    temp = current;
    current = current->next;
    temp->next = reversed;
    reversed = temp;
  }
  list->head = reversed;
}

void destroy(List *list) {
  Node *current = list->head;
  Node *next = current;
  while (current != NULL) {
    next = current->next;
    free(current);
    current = next;
  }
  free(list);
}
