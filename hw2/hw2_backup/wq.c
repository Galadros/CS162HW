#include <stdlib.h>
#include "wq.h"
#include "utlist.h"

/* Initializes a work queue WQ. */
void wq_init(wq_t *wq) {

  /* TODO: Make me thread-safe! */
  pthread_mutex_init(&(wq->wq_empty_mutex), NULL);
  pthread_mutex_init(&(wq->wq_modification_mutex), NULL);
  pthread_cond_init(&(wq->wq_empty_condition), NULL);
  pthread_mutex_lock(&(wq->wq_modification_mutex));
  wq->size = 0;
  wq->head = NULL;
  pthread_mutex_unlock(&(wq->wq_modification_mutex));
}

/* Remove an item from the WQ. This function should block until there
 * is at least one item on the queue. */
int wq_pop(wq_t *wq) {

  /* TODO: Make me blocking and thread-safe! */
  pthread_cond_wait(&(wq->wq_empty_condition), &(wq->wq_empty_mutex));
  pthread_mutex_lock(&(wq->wq_modification_mutex));

  wq_item_t *wq_item = wq->head;
  int client_socket_fd = wq->head->client_socket_fd;
  wq->size--;
  DL_DELETE(wq->head, wq->head);

  free(wq_item);
  
  // I think this is the correct order to release locks in
  pthread_mutex_unlock(&(wq->wq_modification_mutex));  
  pthread_mutex_unlock(&(wq->wq_empty_mutex));

  return client_socket_fd;
}

/* Add ITEM to WQ. */
void wq_push(wq_t *wq, int client_socket_fd) {

  /* TODO: Make me thread-safe! */

  // ...there's a remote chance I need to check the wq_empty_condition lock here?
  pthread_mutex_lock(&(wq->wq_modification_mutex));
  wq_item_t *wq_item = calloc(1, sizeof(wq_item_t));
  wq_item->client_socket_fd = client_socket_fd;
  DL_APPEND(wq->head, wq_item);
  wq->size++;
  pthread_cond_broadcast(&(wq->wq_empty_condition));
  pthread_mutex_unlock(&(wq->wq_modification_mutex));
}
