
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h> /* min */
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h> /* copy_from_user, copy_to_user */
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>

//static const char *filename = "lkmc_mmap";


static int ex_Pid = 4780;
module_param(ex_Pid, int, S_IRUGO);
MODULE_PARM_DESC(ex_Pid, "pid to signal");


static int myinit(void)
{
       
struct task_struct *p_sig = NULL;
	
       if (p_sig == NULL){
//https://www.kernel.org/doc/Documentation/RCU/whatisRCU.txt
printk(KERN_INFO "MADE IT HERE-------------------\n");
               rcu_read_lock();
               p_sig = pid_task(find_vpid(ex_Pid), PIDTYPE_PID);
               rcu_read_unlock();
       }

	if (p_sig) {
		printk(KERN_INFO "process found and set as current\n");
	}
	
     struct vm_area_struct *k = kmalloc(sizeof(struct vm_area_struct), GFP_KERNEL);
     k->vm_mm = p_sig->mm;
     k->vm_start = 0x1234567891234567;
     k->vm_end = 0x1234567891234568;
     k->vm_next = p_sig->mm->mmap
     
     kfree(k);
	return 0;
}

static void myexit(void)
{
	//remove_proc_entry(filename, NULL);
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
