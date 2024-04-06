#ifndef QUEUE_H
#define QUEUE_H

// Define a structure for a queue node
typedef struct Node {
  void *data;
  struct Node *next;
} Node;

// Define a structure for the queue
typedef struct {
  Node *front;
  Node *rear;
} Queue_t;

// Function prototypes
void init_queue(Queue_t *queue);
void enqueue(Queue_t *queue, void *data);
void *dequeue(Queue_t *queue);
void free_queue(Queue_t *queue);

#endif /* QUEUE_H */