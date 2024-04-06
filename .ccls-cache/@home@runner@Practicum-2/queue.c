#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

// Function to initialize the queue
void init_queue(Queue_t *queue) {
  queue->front = NULL;
  queue->rear = NULL;
}

// Function to enqueue an element into the queue
void enqueue(Queue_t *queue, void *data) {
  // Create a new node
  Node *newNode = (Node *)malloc(sizeof(Node));
  if (newNode == NULL) {
    printf("Memory allocation failed.\n");
    return;
  }
  newNode->data = data;
  newNode->next = NULL;

  // If the queue is empty, set both front and rear to the new node
  if (queue->rear == NULL) {
    queue->front = newNode;
    queue->rear = newNode;
  } else {
    // Otherwise, add the new node to the end of the queue
    queue->rear->next = newNode;
    queue->rear = newNode;
  }
}

// Function to dequeue an element from the queue
void *dequeue(Queue_t *queue) {
  // If the queue is empty, return NULL
  if (queue->front == NULL) {
    printf("Queue is empty.\n");
    return NULL;
  }

  // Store the data of the front node and move front to the next node
  void *data = queue->front->data;
  Node *temp = queue->front;
  queue->front = queue->front->next;

  // If the front becomes NULL, set rear to NULL as well
  if (queue->front == NULL) {
    queue->rear = NULL;
  }

  // Free the memory of the dequeued node
  free(temp);

  return data;
}

// Function to free the memory allocated for the queue
void free_queue(Queue_t *queue) {
  while (queue->front != NULL) {
    Node *temp = queue->front;
    queue->front = queue->front->next;
    free(temp);
  }
  queue->rear = NULL;
}
