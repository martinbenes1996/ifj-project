
#include <stdbool.h>
#include <stdlib.h>

#include "stack.h"
#include "types.h"

typedef struct stack_item
{
  Phrasem data;
  struct stack_item * next;
} * StackItem;

typedef struct stack_head
{
  StackItem * first;
} Stack;

static Stack mstack;

bool PushOntoStack(Phrasem data)
{
  StackItem it = malloc(sizeof(struct stack_item));
  if(it == NULL)
  {
    // error catch missing
    return false;
  }

  // filling up
  it->data = data;
  it->next = mstack.first;

  // moving head
  mstack.first = it;
  return true;
}

Phrasem PopFromStack()
{
  StackItem * it = mstack.first;
  if(it == NULL) return END_PTR;

  mstack.first = it;

  Phrasem p = it->data;
  free(it);
  return p;

}
