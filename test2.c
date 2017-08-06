#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int something_worked(void) {
    /* thread operation might fail, so here's a silly example */
    void *p = malloc(10);
    free(p);
    return p ? 1 : 0;
}

void *myThread(void *result)
{
   if (something_worked()) {
       *((int*)result) = 42;
       pthread_exit(result);
   } else {
       pthread_exit(0);
   }
}

int main()
{
   pthread_t tid;
   void *status = 0;
   int result;

   pthread_create(&tid, NULL, myThread, &result);
   pthread_join(tid, &status);

   if (status != 0) {
       printf("%d\n",result);
   } else {
       printf("thread failed\n");
   }

   return 0;
}
