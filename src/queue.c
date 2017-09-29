#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>

#include "io.h"
#include "queue.h"

pthread_mutex_t ReadEnabled;

QueueHead InitQueue()
{
  // alloc head
  QueueHead h = malloc(sizeof(struct queue_head));
  if(h == NULL) return NULL;

  #ifdef QUEUE_DEBUG
    debug("Queue init.");
  #endif

  // init lock
  pthread_mutex_init(&ReadEnabled, NULL);
  pthread_mutex_lock(&ReadEnabled);

  #ifdef QUEUE_DEBUG
    debug("Queue Read locked.");
  #endif

  // fill and return
  h->first = NULL;
  return h;
}

extern void ResetParserQueue(QueueItem);
bool AddToQueue(QueueTail q, Phrasem data)
{
  // control
	if(q == NULL) return false;

  // allocating a new item
  QueueItem i = malloc(sizeof(struct queue_item));

  // empty queue
  if(q->last == NULL) {

    #ifdef QUEUE_DEBUG
      debug("Queue: adding to empty queue.");
    #endif

    // set the pointers
		q->last = i;
		q->last->next = NULL;

    // reset the Parser connection
    ResetParserQueue(i);

    // let the parser
    pthread_mutex_unlock(&ReadEnabled);
  }

  // not empty queue
	else {

    #ifdef QUEUE_DEBUG
      debug("Queue: adding to queue.");
    #endif

    // set the pointers
  	q->last->next = i;
		i->next = NULL;
		q->last = q->last->next;

	}

  // copy data
	q->last->data = data;
  return true;
}

Phrasem RemoveFromQueue(QueueHead q)
{
  // control
	if(q == NULL) return NULL;
  if(q->first == NULL) pthread_mutex_lock(&ReadEnabled);

  #ifdef QUEUE_DEBUG
    debug("Queue: removing queue.");
  #endif

  // copy
	QueueItem p = q->first;
	Phrasem d = q->first->data;

  // set pointer
	q->first = q->first->next;

  // dealloc memory and return data
	free(p);
  return d;
}

void ClearQueue(QueueHead q)
{
  #ifdef QUEUE_DEBUG
    debug("Queue: clearing the queue.");
  #endif

  // control
	if(q == NULL) return;

  // help pointer
	QueueItem qi = q->first;
  Phrasem ph = q->first->data;

  // clear cycle
	while(qi != NULL) {

    // moving head
		q->first = q->first->next;

    // clear item
    //free(ph);
		free(qi);

    // reset help pointer
		qi = q->first;
    ph = q->first->data;
	}
}

void PrintQueue(QueueHead q)
{
  // control
	if(q == NULL) return;

  // help pointer
	QueueItem pointer = q->first;

  // clear cycle
	while(pointer != NULL) {

    // moving head
		q->first = q->first->next;

    // clear item
		fprintf(stderr, "%d[%c] ", pointer->data->id, pointer->data->id);

    // reset help pointer
		pointer = q->first;
	}
  fprintf(stderr, "\n");
}
