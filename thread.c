#include "types.h"
#include "stat.h"
#include "user.h"
#include "param.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"


// Mutex for threads: same data structure as spinlock, but different behaviors when locked
struct thread_mutex {
  uint locked;       // Is the lock held?

  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
  uint pcs[10];      // The call stack (an array of program counters)
                     // that locked the lock.
};

struct thread_mutex mutex;
struct thread_mutex* m = &mutex;

void thread_mutex_init(struct thread_mutex* lk) {
  // lk->name = name;
  lk->locked = 0;
  lk->cpu = 0;
}

void thread_mutex_lock(struct thread_mutex* lk)
{
  while(xchg(&lk->locked, 1) != 0)
    sleep(1);

  __sync_synchronize();

  // Record info about lock acquisition for debugging.
  // lk->cpu = cpu;
}

void thread_mutex_unlock(struct thread_mutex* lk)
{
  lk->pcs[0] = 0;
  lk->cpu = 0;

  __sync_synchronize();

  // Release the lock.
  lk->locked = 0;
}

// Spinlock for threads
struct thread_spinlock {
  uint locked;       // Is the lock held?

  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
  uint pcs[10];      // The call stack (an array of program counters)
                     // that locked the lock.
};

struct thread_spinlock lock;
struct thread_spinlock* lk = &lock;

void thread_spin_init(struct thread_spinlock* lk) {
  lk->locked = 0;
  lk->cpu = 0;
}


void thread_spin_lock(struct thread_spinlock* lk)
{
  while(xchg(&lk->locked, 1) != 0);
  __sync_synchronize();
}

// Release the lock.
void thread_spin_unlock(struct thread_spinlock* lk)
{
  lk->pcs[0] = 0;
  lk->cpu = 0;

  __sync_synchronize();

  // Release the lock.
  lk->locked = 0;
}

struct balance {
    char name[32];
    int amount;
};

volatile int total_balance = 0;

volatile unsigned int delay (unsigned int d) {
   unsigned int i; 
   for (i = 0; i < d; i++) {
       __asm volatile( "nop" ::: );
   }

   return i;
}

void do_work(void *arg){
    int i; 
    int old;
   
    struct balance *b = (struct balance*) arg; 
    printf(1, "Starting do_work: s:%s\n", b->name);

    for (i = 0; i < b->amount; i++) { 
         // thread_spin_lock(&lock);
         thread_mutex_lock(&mutex);
         old = total_balance;
         delay(100000);
         total_balance = old + 1;
         // thread_spin_unlock(&lock);
         thread_mutex_unlock(&mutex);
    }
  
    printf(1, "Done s:%x\n", b->name);

    thread_exit();
    return;
}

int main(int argc, char *argv[]) {
  struct balance b1 = {"b1", 3200};
  struct balance b2 = {"b2", 2800};
 
  void *s1, *s2;
  int t1, t2, r1, r2;
  s1 = malloc(4096);
  s2 = malloc(4096);
  
  t1 = thread_create(do_work, (void*)&b1, s1);
  t2 = thread_create(do_work, (void*)&b2, s2); 

  r1 = thread_join();
  r2 = thread_join();
  
  printf(1, "Threads finished: (%d):%d, (%d):%d, shared balance:%d\n", 
      t1, r1, t2, r2, total_balance);
  exit();
}
