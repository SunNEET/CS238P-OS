#include "types.h"
#include "stat.h"
#include "user.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"

int
main(int argc, char *argv[])
{
    int i;
    struct uproc p[1];
    struct uproc* up;
    for(i=0; i<NPROC; i++) {
        if(getprocinfo(i, &p) == -1) 
            continue;
        up = p;
        printf(1, "Name = %s\t", up->name);
        printf(1, ", PID = %d\t",  up->pid);
        if(up->pid != 1)
            printf(1, ", PPID = %d\t", up->ppid);
        else
            printf(1, ", PPID = N/A\t");
        printf(1, ", Size = %d\t", up->sz);
        printf(1, ", State = ");
        switch(up->state){
            case UNUSED:
                printf(1,"%s\t", "UNUSED  ");
                break;
            case EMBRYO:
                printf(1,"%s\t", "EMBRYO  ");
                break;
            case SLEEPING:
                printf(1,"%s\t", "SLEEPING");
                break;
            case RUNNABLE:
                printf(1,"%s\t", "RUNNABLE");
                break;
            case RUNNING:
                printf(1,"%s\t", "RUNNING ");
                break;
            case ZOMBIE:
                printf(1,"%s\t", "ZOMBIE  ");
                break;
        }
        if(up->killed == 0)
            printf(1,", Killed = FALSE\t");
        else
            printf(1,", Killed = TRUE\t");
        if(up->chan != 0)
            printf(1,", Channel = %d\t", (int)up->chan);
        else
            printf(1,", Channel = NOT WAITING\t");
        printf(1, "\n");
    }
    exit();
}
