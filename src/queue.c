#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>

#include "io.h"
#include "queue.h"

/*--------------------------------------------------*/
/** @addtogroup Queue_types
 * Types used in Queue module.
 * @{
 */

/**
 * @brief   One item in queue.
 *
 * Queue is made by head (to remove from), tail (to add to) and linked
 * list of items, containing data and pointer to next item.
 * First item is addressed by pointer from head, last from tail and
 * the next element is set to NULL (nothing).
 */
typedef struct queue_item {
  Phrasem data; /**< Data of item. */
  struct queue_item * next; /**< Pointer to next item. */
} * volatile QueueItem;

typedef QueueItem QueueHead;
typedef QueueItem QueueTail;

Phrasem mem;

/** @} */
/*--------------------------------------------------*/

volatile QueueHead head = NULL; /**< Head of the queue. */
volatile QueueTail tail = NULL; /**< Tail of the queue. */
bool isValid() { return (head == NULL && tail == NULL)
                     || (head != NULL && tail != NULL); }
bool isEmpty() { return head == NULL && tail == NULL; }
extern bool ScannerIsScanning();

bool finished = false; /**< True, if scanner ended. */
pthread_mutex_t ReadEnabled;
pthread_mutex_t QueueEdit;

/**
 * @brief   Queue is filled.
 */
void FinishConnectionToQueue()
{
  pthread_mutex_unlock(&ReadEnabled);
  #ifdef QUEUE_DEBUG
    debug("Connection to Queue finished.");
  #endif
}

void InitQueue()
{
  #ifdef QUEUE_DEBUG
    debug("Queue init.");
  #endif

  // init lock
  pthread_mutex_init(&ReadEnabled, NULL);
  pthread_mutex_lock(&ReadEnabled);
  pthread_mutex_init(&QueueEdit, NULL);

  // fill
  finished = false;
  head = NULL;
  tail = NULL;
}

bool AddToQueue(Phrasem data)
{
  //pthread_mutex_lock(&QueueEdit);

  // control
	if(data == NULL) return false;
  if(!isValid()) return false;

  // allocating a new item
  QueueItem i = malloc(sizeof(struct queue_item));
  if(i == NULL) return false;

  // empty
  if(isEmpty())
  {
    head = i;
    tail = i;
    tail->data = data;
    #ifdef QUEUE_DEBUG
      debug("Queue: adding to empty queue.");
      PrintPhrasem(data);
    #endif
    pthread_mutex_unlock(&ReadEnabled);
  }
  else
  {
    tail->next = i;
    tail = i;
    tail->data = data;

    #ifdef QUEUE_DEBUG
      debug("Queue: adding to queue.");
      PrintPhrasem(data);
    #endif
  }

  //pthread_mutex_unlock(&QueueEdit);
  return true;
}

Phrasem RemoveFromQueue()
{
  if(mem != NULL)
  {
    Phrasem p = mem;
    mem = NULL;
    return p;
  }

  //pthread_mutex_lock(&QueueEdit);
  // control
  if(isEmpty())
  {
    if(!ScannerIsScanning()) return NULL;
    #ifdef QUEUE_DEBUG
      debug("Queue: waiting for data.");
    #endif
    //pthread_mutex_unlock(&QueueEdit);
    pthread_mutex_lock(&ReadEnabled);
    if(!ScannerIsScanning()) return NULL;
    //pthread_mutex_lock(&QueueEdit);
  }

  if(!isValid()) return NULL;
  if(isEmpty()) return NULL;

  // copy
  if(head == NULL) return NULL;
	QueueItem p = head;
	Phrasem d = head->data;

  // set pointer
	if(head != NULL) head = head->next;
  if(head == NULL) tail = NULL;

  #ifdef QUEUE_DEBUG
    debug("Queue: removing from queue.");
    PrintPhrasem(d);
  #endif

  // dealloc memory and return data
	free(p);
  //pthread_mutex_unlock(&QueueEdit);
  return d;
}

bool ReturnToQueue(Phrasem p)
{
  if(mem != NULL) return false;

  mem = p;
  return true;
}

void ClearQueue()
{
  //pthread_mutex_lock(&QueueEdit);
  #ifdef QUEUE_DEBUG
    debug("Queue: clearing the queue.");
  #endif

  // control
  if(!isValid()) return;
  if(isEmpty()) return;

  // clear cycle
	while(head != NULL) {

    free(head->data);
    QueueItem qi = head;

    // moving head
		head = head->next;

    free(qi);
	}
  tail = NULL;
  //pthread_mutex_unlock(&QueueEdit);
}

void PrintQueue()
{
  // control
  if(!isValid()) return;
  if(isEmpty()) return;

  QueueItem q = head;

  // clear cycle
	while(q != NULL) {

    PrintPhrasem(q->data);
    q = q->next;

	}

}
