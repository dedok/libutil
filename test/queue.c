#include "libutil/queue.h"
#include "libutil/log.h"
#include <unistd.h>


struct msg { int v; };

static const size_t max_threads = 4;
static size_t const max_messages = 100;
static struct lu_queue q;


void *
product(void * UNUSED_VAR(a))
{
    size_t count = 0;
    for (; count < max_messages / max_threads; ++count) {
        struct msg * m = MEM(malloc(sizeof(struct msg)));
        m->v = count;
        lu_queue_product(&q, (void *)m);
    }
    return NULL;
}


void
prod_consume_common_test()
{
    static const size_t max_message_per_it = 5;

    size_t total = 0;
    queue_data_t m[max_message_per_it];
    pthread_t prod_th[max_threads];

    lu_queue_init(&q);
    for (size_t pc = 0; pc < max_threads; ++pc)
        SYS(pthread_create(&prod_th[pc], NULL, product, NULL));

    for (size_t c = 0; c < max_messages; ++c) {
        memset(m, 0, max_message_per_it);
        size_t size =
            CHECK(lu_queue_timed_consume(&q, m, max_message_per_it, 1),
                  > max_message_per_it, DIE, "to many message came");
        for (size_t i = 0; i < size; ++i) {
            struct msg * m_ = (struct msg *)m[i].data;
            free(m_);
            ++total;
        }
    }

    for (size_t pc = 0; pc < max_threads; ++pc)
        SYS(pthread_join(prod_th[pc], NULL));
    CHECK(total, == max_messages - 1, DIE,
            "messages lost, expected '%i' got '%i'",
            (int)max_messages, (int)total);
    lu_queue_destroy(&q);
}


int main()
{
    prod_consume_common_test();
    return EXIT_SUCCESS;
}
