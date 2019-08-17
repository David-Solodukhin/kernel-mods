
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

enum { BUFFER_SIZE = 4 };

struct mmap_info {
	char *data;
};

/* After unmap. */
static void vm_close(struct vm_area_struct *vma)
{
	pr_info("vm_close\n");
}

/* First page access. */
	int (*fault)(struct vm_fault *vmf);
static int vm_fault(struct vm_fault *vmf)
{
	struct page *page;
	struct mmap_info *info;

	pr_info("vm_fault\n");
	info = (struct mmap_info *)vmf->vma->vm_private_data;
	if (info->data) {
		page = virt_to_page(info->data);
		get_page(page);
		vmf->page = page;
	}
	return 0;
}

/* Aftr mmap. TODO vs mmap, when can this happen at a different time than mmap? */
static void vm_open(struct vm_area_struct *vma)
{
	pr_info("vm_open\n");
}

static struct vm_operations_struct vm_ops =
{
	.close = vm_close,
	.fault = vm_fault,
	.open = vm_open,
};

static int mmap(struct file *filp, struct vm_area_struct *vma)
{
	pr_info("mmap\n");
	vma->vm_ops = &vm_ops;
	vma->vm_flags |= VM_DONTEXPAND | VM_DONTDUMP;
	vma->vm_private_data = filp->private_data;
	vm_open(vma);
	return 0;
}

static int open(struct inode *inode, struct file *filp)
{
	struct mmap_info *info;

	pr_info("open\n");
	info = kmalloc(sizeof(struct mmap_info), GFP_KERNEL);
	pr_info("virt_to_phys = 0x%llx\n", (unsigned long long)virt_to_phys((void *)info));
	info->data = (char *)get_zeroed_page(GFP_KERNEL);
	memcpy(info->data, "asdf", BUFFER_SIZE);
	filp->private_data = info;
	return 0;
}

static ssize_t read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	struct mmap_info *info;
    int ret;

	pr_info("read\n");
	info = filp->private_data;
    ret = min(len, (size_t)BUFFER_SIZE);
    if (copy_to_user(buf, info->data, ret)) {
        ret = -EFAULT;
	}
	return ret;
}

static ssize_t write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	struct mmap_info *info;

	pr_info("write\n");
	info = filp->private_data;
    if (copy_from_user(info->data, buf, min(len, (size_t)BUFFER_SIZE))) {
        return -EFAULT;
    } else {
        return len;
    }
}

static int release(struct inode *inode, struct file *filp)
{
	struct mmap_info *info;

	pr_info("release\n");
	info = filp->private_data;
	free_page((unsigned long)info->data);
	kfree(info);
	filp->private_data = NULL;
	return 0;
}

static const struct file_operations fops = {
	.mmap = mmap,
	.open = open,
	.release = release,
	.read = read,
	.write = write,
};
static int ex_Pid = 1578;
module_param(ex_Pid, int, S_IRUGO);
MODULE_PARM_DESC(ex_Pid, "pid to signal");

static int sig_num = 15;
module_param(sig_num, int, S_IRUGO);
MODULE_PARM_DESC(sig_num, "signal");
static int myinit(void)
{
       struct siginfo info; //on ks
       memset(&info, 0, sizeof(struct siginfo));
       info.si_signo = sig_num;
       info.si_code = 0;
       info.si_int = 4204; //irrelevant
struct task_struct *p_sig = NULL;
	
       if (p_sig == NULL){
//https://www.kernel.org/doc/Documentation/RCU/whatisRCU.txt
printk(KERN_INFO "MADE IT HERE-------------------\n");
               rcu_read_lock();
               p_sig = pid_task(find_vpid(ex_Pid), PIDTYPE_PID);
               rcu_read_unlock();
       }

	if (p_sig) {
		printk("process found and set as current");
	}
	
       int ret = send_sig_info(sig_num, &info, p_sig);
       if (ret < 0) {
               printk(KERN_INFO "error sending signal\n");
       }


	return 0;
}

static void myexit(void)
{
	//remove_proc_entry(filename, NULL);
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
