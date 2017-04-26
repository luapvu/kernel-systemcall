#include <linux/kernel.h>
#include <linux/prinfo.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <asm-powerpc/cputime.h>
#include <asm/current.h>
#include <linux/list.h>

pid_t  process_PID;

asmlinkage int sys_prinfo(struct prinfo *info)
{

	if(info == NULL){//returns -22 if null is passed
	printk("EINVAL(-22) \n");
	return -22;
	}


//	struct task_struct *processInfo = current;
	process_PID = info->pid;
	struct task_struct *processInfo = find_task_by_pid(process_PID); //given the pid we find the task struct of this pid
	const cputime_t userTime = processInfo->utime; //user space time in ms
	const cputime_t systemTime = processInfo->stime; // kernal space system time  in ms
	long startTime = processInfo->start_time.tv_sec; //start time in seconds

	struct task_struct *myYoungestOlderSibling, *myOldestYoungerSibling; //the original process's sibling
	struct task_struct *myChild; //process's child to traverse
	pid_t oldestYoungerSibling_PID = -1; //will hold the PID of oldest younger sibling
	pid_t youngestOlderSibling_PID = -1; //will hold the PID of youngest older sibling
	pid_t youngestChild_PID = -1;
	long totalUserTime_children = 0;
	long totalSystemTime_children = 0;

	if(list_empty(&processInfo->sibling)){ //you have no siblings so you have no older or younger siblings
		youngestOlderSibling_PID = -1;
		oldestYoungerSibling_PID = -1;
	}else{// means you have at least 1 sibling

		myYoungestOlderSibling = list_entry(processInfo->sibling.prev, struct task_struct, sibling);

		if(myYoungestOlderSibling->start_time.tv_sec <= processInfo->start_time.tv_sec){ //if your prev sibling has a start time greater than yours, then you know it's a older sibling
			youngestOlderSibling_PID = myYoungestOlderSibling->pid;
		}else{ //if not, then you  don't have any older siblings
			youngestOlderSibling_PID = -2;
		}

		myOldestYoungerSibling = list_entry(processInfo->sibling.next, struct task_struct, sibling);

		if(myOldestYoungerSibling->start_time.tv_sec >= processInfo->start_time.tv_sec){ //if your next sibling has a start time younger than yours, then you know it's a younger sibling
			oldestYoungerSibling_PID = myOldestYoungerSibling->pid;
		}else{ //if not, then you don't have any younger siblings
			oldestYoungerSibling_PID = -2;
		}



	}




	//travers children list
	list_for_each_entry(myChild, &processInfo->children, sibling)
	{
	//	myChild = list_entry(c, struct task_struct, children);
		totalUserTime_children = cputime_to_msecs(myChild->utime) + totalUserTime_children;
		totalSystemTime_children = cputime_to_msecs(myChild->stime) + totalSystemTime_children;
		if(list_is_last(&myChild->sibling, &processInfo->children)){
			youngestChild_PID = myChild->pid;
		}
	}


	//print out everything for the user
	printk("[1] Current state of process: %lu \n", processInfo->state);
	printk("[2] Process nice value: %d \n", task_nice(processInfo));
	printk("[3] Process id: %d \n", processInfo->pid);
	printk("[4] Process id of parent: %d \n", processInfo->parent->pid);
	printk("[5] Process id of youngest child: %d \n", youngestChild_PID);
	printk("[6] Pid of the oldest among younger siblings: %d \n", oldestYoungerSibling_PID);
	printk("[7] Pid of the youngest among older siblings: %d \n", youngestOlderSibling_PID);
	printk("[8] Process start time: %li (secs) \n", startTime);
	printk("[9] CPU time spent in user mode: %u (ms) \n", cputime_to_msecs(userTime));
	printk("[10] CPU time spent in system mode: %u (ms)\n", cputime_to_msecs(systemTime));
	printk("[11] Total user time of children: %li \n", totalUserTime_children);
	printk("[12] Total system time of children: %li \n", totalSystemTime_children);
	printk("[13] User id of process owner: %d \n", processInfo->uid);
	printk("[14] Name of Program executed: %s \n", processInfo->comm );

	//allocate all values to the input structure
	info->state = processInfo->state;
	info->nice = task_nice(processInfo);
	info->parent_pid = processInfo->parent->pid;
	info->youngest_child_pid = youngestChild_PID;
	info->younger_sibling_pid = oldestYoungerSibling_PID;
	info->older_sibling_pid = youngestOlderSibling_PID;
	info->start_time = startTime;
	info->user_time = cputime_to_msecs(userTime);
	info->sys_time = cputime_to_msecs(systemTime);
	info->cutime = totalUserTime_children;
	info->cstime = totalSystemTime_children;
	info->uid = processInfo->uid;
//	info->comm = processInfo->comm;
	strcpy(info->comm, processInfo->comm);
	return 0;
}
