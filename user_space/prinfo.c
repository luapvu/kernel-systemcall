include <stdio.h>
#include <linux/unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

#define _prinfo_TEST_ 328

struct prinfo {
long state; /* current state of process */
long nice; /* process nice value */
pid_t pid; /* process id (input) */
pid_t parent_pid; /* process id of parent */
pid_t youngest_child_pid; /* process id of youngest child */
pid_t younger_sibling_pid; /* pid of the oldest among younger siblings */
pid_t older_sibling_pid; /* pid of the youngest among older siblings */
unsigned long start_time; /* process start time */
long user_time; /* CPU time spent in user mode */
long sys_time; /* CPU time spent in system mode */
long cutime; /* total user time of children */
long cstime; /* total system time of children */
long uid; /* user id of process owner */
char comm[16]; /* name of program executed */
};


int main(int argc, char *argv[])
{
        struct prinfo myPrinfo;
        struct prinfo* info = &myPrinfo;
        info->pid = getpid();

        struct prinfo myPrinfo1;
        struct prinfo* info1 = &myPrinfo1;
        info1->pid = 1;

        struct prinfo* infoNull = NULL;

        printf("\n----------------------------------\n");
        printf("PID = getpid()\n\n");

        syscall(_prinfo_TEST_ , info );
        printf("\n---------------------------------\n");

//      printf("\n----------------------------------\n");
//      printf("PID = 1\n\n");

//      syscall(_prinfo_TEST_ , info1 );
//      printf("\n---------------------------------\n");

//      printf("\n----------------------------------\n");
//      printf("PID = NULL\n\n");

//      syscall(_prinfo_TEST_ , infoNull );
//      printf("\n---------------------------------\n");



        return 0;
}
