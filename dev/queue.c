#include <stdio.h>
#include <sys/queue.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    TAILQ_HEAD(tailhead, entry) head;
    struct tailhead *headp;                 /* Tail queue head. */
    struct entry
    {
        char name[10];
        TAILQ_ENTRY(entry) entries;         /* Tail queue. */
    } *n1, *n2, *n3, *np;

    TAILQ_INIT(&head);                      /* Initialize the queue. */

    n1 = malloc(sizeof(struct entry));      /* Insert at the head. */
    strcpy(n1->name, "Francis");
    TAILQ_INSERT_TAIL(&head, n1, entries);

    n1 = malloc(sizeof(struct entry));      /* Insert at the tail. */
    strcpy(n1->name, "Victor");
    TAILQ_INSERT_TAIL(&head, n1, entries);

    n2 = malloc(sizeof(struct entry));      /* Insert after. */
    strcpy(n2->name, "Grace");
    TAILQ_INSERT_AFTER(&head, n1, n2, entries);

    /* Forward traversal. */
    for (np = head.tqh_first; np != NULL; np = np->entries.tqe_next)
        printf("%s\n", np->name);

    /* Delete. */
    while (head.tqh_first != NULL)
        

}
