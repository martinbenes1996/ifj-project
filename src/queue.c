
#include <stdbool.h>
#include <stdlib.h>

#include "queue.h"

QueueHead InitQueue()
{
  QueueHead h;
  h = malloc(sizeof(struct queue_head));
  h->first = NULL;
  /* implementation */
  return h;
}

bool AddToQueue(QueueTail q, Phrasem data)
{
	if(q == NULL)
		return false;
	QueueItem i = malloc(sizeof(struct queue_item));
	if(q->last == NULL) {
		q->last = i;
		q->last->next = NULL;

	}


	else {
		q->last->next = i;
		i->next = NULL;
		q->last = q->last->next;

	}

	q->last->data = data;
		


  /* implementation */
  return false;
	
	
}

Phrasem RemoveFromQueue(QueueHead q)
{
	if(q == NULL || q->first == NULL) return NULL;
	QueueItem p;
	Phrasem d;
	d = q->first->data;
	p = q->first;
	q->first = q->first->next;
	free(p);

  return d;
}

void ClearQueue(QueueHead q)
{	
	QueueItem pointer;
	pointer = q->first;
	if(q == NULL) return;
	while(pointer != NULL) {
		q->first = q->first->next;
		free(pointer);
		pointer = q->first;
	}

}
