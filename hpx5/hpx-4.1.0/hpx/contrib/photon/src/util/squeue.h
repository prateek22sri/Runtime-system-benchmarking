/*
 * This is a redefinition of sys/queue.h to provide thread-safety.
 * See sys/queue.h for usage, copyright and other information.
 *
 * Only LIST and SLIST are supported.
 *
 * SAFE_ variations are provided for some list operations; they all begin
 * with a lock on the list and end with an unlock. You don't want to use
 * these in for loops or blocks with multiple list operations. Use LIST_LOCK
 * and LIST_UNLOCK for those.
 *
 */

#ifndef PHOTON_SQUEUE_H_
#define PHOTON_SQUEUE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/queue.h>
#include <pthread.h>

/*
 * List definitions.
 */
#undef LIST_HEAD
#define LIST_HEAD(name, type)                       \
struct name {                               \
    struct type *lh_first;  /* first element */         \
    pthread_mutex_t mtx;    \
}

#undef LIST_HEAD_INITIALIZER
#define LIST_HEAD_INITIALIZER(head)                 \
    { NULL, PTHREAD_MUTEX_INITIALIZER }

#undef LIST_ENTRY
#define LIST_ENTRY(type)                        \
struct {                                \
    struct type *le_next;   /* next element */          \
    struct type **le_prev;  /* address of previous next element */  \
    pthread_mutex_t *mtx;   /* points to mtx in head */ \
}
/*
 * List functions.
 */
#undef LIST_INIT
#define LIST_INIT(head) do {                        \
    (head)->lh_first = NULL;                    \
    pthread_mutex_init(&(head)->mtx, NULL);     \
} while (/*CONSTCOND*/0)

#undef LIST_INSERT_AFTER
#define LIST_INSERT_AFTER(listelm, elm, field) do {         \
    if (((elm)->field.le_next = (listelm)->field.le_next) != NULL)  \
        (listelm)->field.le_next->field.le_prev =       \
            &(elm)->field.le_next;              \
    (listelm)->field.le_next = (elm);               \
    (elm)->field.le_prev = &(listelm)->field.le_next;       \
    (elm)->field.mtx = &(listelm)->field.mtx;       \
} while (/*CONSTCOND*/0)

#define SAFE_LIST_INSERT_AFTER(listelm, elm, field) do {         \
    pthread_mutex_lock((listelm)->field.mtx); \
    LIST_INSERT_AFTER(listelm, elm, field); \
    pthread_mutex_unlock((listelm)->field.mtx); \
} while (/*CONSTCOND*/0)


#undef LIST_INSERT_BEFORE
#define LIST_INSERT_BEFORE(listelm, elm, field) do {            \
    (elm)->field.le_prev = (listelm)->field.le_prev;        \
    (elm)->field.le_next = (listelm);               \
    (elm)->field.mtx = (listelm)->field.mtx;        \
    *(listelm)->field.le_prev = (elm);              \
    (listelm)->field.le_prev = &(elm)->field.le_next;       \
} while (/*CONSTCOND*/0)

#define SAFE_LIST_INSERT_BEFORE(listelm, elm, field) do {         \
    pthread_mutex_lock((listelm)->field.mtx); \
    LIST_INSERT_BEFORE(listelm, elm, field); \
    pthread_mutex_unlock((listelm)->field.mtx); \
} while (/*CONSTCOND*/0)

#undef LIST_INSERT_HEAD
#define LIST_INSERT_HEAD(head, elm, field) do {             \
    if (((elm)->field.le_next = (head)->lh_first) != NULL)      \
        (head)->lh_first->field.le_prev = &(elm)->field.le_next;\
    (head)->lh_first = (elm);                   \
    (elm)->field.le_prev = &(head)->lh_first;           \
    (elm)->field.mtx = &(head)->mtx;            \
} while (/*CONSTCOND*/0)

#define SAFE_LIST_INSERT_HEAD(head, elm, field) do {             \
    pthread_mutex_lock(&(head)->mtx); \
    LIST_INSERT_HEAD(head, elm, field); \
    pthread_mutex_unlock(&(head)->mtx); \
} while (/*CONSTCOND*/0)


#define SAFE_LIST_REMOVE(elm, field) do {                    \
    pthread_mutex_lock((elm)->field.mtx); \
    LIST_REMOVE(elm, field);                \
    pthread_mutex_unlock((elm)->field.mtx); \
} while (/*CONSTCOND*/0)

/*
 * List locking functions.
 */
#define LIST_LOCK(head) do {                \
        pthread_mutex_lock(&(head)->mtx);     \
} while (/*CONSTCOND*/0)

#define LIST_LOCK_ELM(listelm, field) do {                \
        pthread_mutex_lock((listelm)->field.mtx); \
} while (/*CONSTCOND*/0)

#define LIST_UNLOCK(head) do {                \
        pthread_mutex_unlock(&(head)->mtx);     \
} while (/*CONSTCOND*/0)

#define LIST_UNLOCK_ELM(listelm, field) do {                \
        pthread_mutex_unlock((listelm)->field.mtx); \
} while (/*CONSTCOND*/0)

/*
 * Singly-linked List definitions.
 */
#undef SLIST_HEAD
#define SLIST_HEAD(name, type)                      \
struct name {                               \
    struct type *slh_first; /* first element */         \
    pthread_mutex_t mtx;    \
}

#undef SLIST_HEAD_INITIALIZER
#define SLIST_HEAD_INITIALIZER(head)                    \
    { NULL, PTHREAD_MUTEX_INITIALIZER }

#undef SLIST_ENTRY
#define SLIST_ENTRY(type)                       \
struct {                                \
    struct type *sle_next;  /* next element */          \
    pthread_mutex_t *mtx;   /* points to mtx in head */ \
}

/*
 * Singly-linked List functions.
 */
#undef SLIST_INIT
#define SLIST_INIT(head) do {                       \
    (head)->slh_first = NULL;                   \
    pthread_mutex_init(&(head)->mtx, NULL);     \
} while (/*CONSTCOND*/0)

#undef SLIST_INSERT_AFTER
#define SLIST_INSERT_AFTER(slistelm, elm, field) do {           \
    (elm)->field.sle_next = (slistelm)->field.sle_next;     \
    (elm)->field.mtx = (slistelm)->field.mtx;       \
    (slistelm)->field.sle_next = (elm);             \
} while (/*CONSTCOND*/0)

#define SAFE_SLIST_INSERT_AFTER(head, elm, field) do {            \
    pthread_mutex_lock(&(head)->mtx);       \
    SLIST_INSERT_AFTER(head, elm, field);    \
    pthread_mutex_unlock(&(head)->mtx);     \
} while (/*CONSTCOND*/0)


#undef SLIST_INSERT_HEAD
#define SLIST_INSERT_HEAD(head, elm, field) do {            \
    (elm)->field.sle_next = (head)->slh_first;          \
    (elm)->field.mtx =  &(head)->mtx;           \
    (head)->slh_first = (elm);                  \
} while (/*CONSTCOND*/0)

#define SAFE_SLIST_INSERT_HEAD(head, elm, field) do {            \
    pthread_mutex_lock(&(head)->mtx);       \
    SLIST_INSERT_HEAD(head, elm, field);    \
    pthread_mutex_unlock(&(head)->mtx);     \
} while (/*CONSTCOND*/0)


#define SAFE_SLIST_REMOVE_HEAD(head, field) do {             \
    pthread_mutex_lock(&(head)->mtx); \
    (head)->slh_first = (head)->slh_first->field.sle_next;      \
    pthread_mutex_unlock(&(head)->mtx); \
} while (/*CONSTCOND*/0)


#define SAFE_SLIST_REMOVE(head, elm, type, field) do {           \
    pthread_mutex_lock(&(head)->mtx);       \
    SLIST_REMOVE(head, elm, type, field);   \
    pthread_mutex_unlock(&(head)->mtx);     \
} while (/*CONSTCOND*/0)

/*
 * Singly-linked list locking functions.
 */

#define SLIST_LOCK(head) do {                \
        pthread_mutex_lock(&(head)->mtx);     \
} while (/*CONSTCOND*/0)

#define SLIST_LOCK_ELM(slistelm, field) do {                \
        pthread_mutex_lock((slistelm)->field.mtx);     \
} while (/*CONSTCOND*/0)

#define SLIST_UNLOCK(head) do {                \
        pthread_mutex_unlock(&(head)->mtx);     \
} while (/*CONSTCOND*/0)

#define SLIST_UNLOCK_ELM(slistelm, field) do {                \
        pthread_mutex_unlock((slistelm)->field.mtx);     \
} while (/*CONSTCOND*/0)

#ifdef __cplusplus
}
#endif

#endif /* PHOTON_SQUEUE_H_ */
