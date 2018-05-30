#include <linux/module.h>    // included for all kernel modules
#include <linux/kernel.h>    // included for KERN_INFO
#include <linux/init.h>      // included for __init and __exit macros
#include <linux/netfilter.h>
#include <linux/vmalloc.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/refcount.h>
//#undef __KERNEL__
#include <linux/netfilter_ipv4.h>
//#define __KERNEL__
#define LATENCY_DELAY 1
#define LATENCY_REPEAT 2
#define DROP_NORMAL 3
#define DROP_ERASE 4


MODULE_LICENSE("GPL");
int ct = 0;
int l = 3, d = 0, dt=0, lt=LATENCY_REPEAT;

module_param(l, int, 0);
module_param(d, int, 0);
module_param(lt, int, 0);
module_param(dt, int, 0);



static struct nf_hook_ops nfho;   //net filter hook option struct
char * last_packet;
size_t last_packet_size;
u32 * packet_sync;
unsigned int my_hook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)  {


  if (!skb->data_len) {
	return NF_ACCEPT;
	}


  printk(KERN_INFO "--hooked packet \n %d, %d\n", skb->data_len, skb->protocol);
    memcpy(last_packet, skb->data, min((size_t)skb->data_len, (size_t)1000)); //copy to intermediate location
    last_packet_size = min((size_t)skb->data_len, (size_t)1000);


    switch(dt) {
      case DROP_ERASE: {
          memset(skb->data, '\0', skb->data_len);
          printk("logged as accepted, but dropped\n");
          return NF_ACCEPT;
      }
      case DROP_NORMAL: {
        unsigned int i = 0;
        get_random_bytes(&i, sizeof(unsigned int));
        i = i % 100;
        if (i <= d) {
          printk("packet normal dropped\n");
          return NF_DROP;
        }

      }
    }


    switch(lt) {
      case LATENCY_DELAY: {
        mdelay(l % 200);
        break;
      }
      case LATENCY_REPEAT: {
        if (ct == 0) {
          ct = l;
          printk("slowed down packet\n");
          return NF_REPEAT;
        }
        ct--;
        break;
      }

  }

      return NF_ACCEPT;

    /*if(refcount_read(&skb->users) == 1) {
      printk(KERN_INFO "packet was not accepted\n");
      refcount_set(&skb->users, 2);
      return NF_REPEAT;
    }
    printk(KERN_INFO "packet was accepted\n");
    //skb->users = (refcount_t)1;
    refcount_set(&skb->users, 1);*/
    //return NF_ACCEPT;
}

static int init_filter_if(void)
{

  /*
  NF_INET_PRE_ROUTING,
	NF_INET_LOCAL_IN,
	NF_INET_FORWARD,
	NF_INET_LOCAL_OUT,
	NF_INET_POST_ROUTING,
	NF_INET_NUMHOOKS
  */
  nfho.hook = my_hook;
  nfho.hooknum = 0 ;
  nfho.pf = PF_INET;
  nfho.priority = NF_IP_PRI_FIRST;

  nf_register_net_hook(&init_net, &nfho);

  return 0;
}
static int open(struct inode *inode, struct file *filp)
{
	printk("file opened\n");
return 0;

}


static ssize_t read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	int ret = min(len, (size_t)last_packet_size);
    if (copy_to_user(buf, last_packet, ret)) {
printk("failed to copy\n");
        ret = -EFAULT;
	}
printk("copied successfully\n");
	return ret;
}

static int release(struct inode *inode, struct file *filp)
{

	return 0;
}

static const struct file_operations proc_fops = {
  .owner = THIS_MODULE,
  .open = open,
  .read = read,
  .release = release,
};




static int __init mod_init(void)
{
    printk(KERN_INFO "INSERTING HOOK-------------------------------\n");
    last_packet = kmalloc(1000, GFP_KERNEL); //what
    proc_create("p_data", 0, NULL, &proc_fops);
    ct = l;
    init_filter_if();
    return 0;    // Non-zero return means that the module couldn't be loaded.
}

static void __exit mod_cleanup(void)
{
  kfree(last_packet);
  nf_unregister_net_hook(&init_net, &nfho);
  remove_proc_entry("p_data", NULL);
  printk(KERN_INFO "Cleaning up module!.\n");
}

module_init(mod_init);
module_exit(mod_cleanup);
