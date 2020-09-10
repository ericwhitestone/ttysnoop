#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/tty.h>
#include <linux/cred.h>
#include <linux/uidgid.h>
#include <linux/slab.h>

static struct kobject *ttysnoop_kobj;

static ssize_t ttysnoop_show (struct kobject *kobj, struct kobj_attribute *attr, char *buf) 
{
        return sprintf(buf, "%s", "Hello!\n");
}

static ssize_t ttysnoop_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) 
{
        return 0;
}

static struct kobj_attribute ttysnoop_attr = 
       __ATTR(ttystate, 0444,  ttysnoop_show, ttysnoop_store);


/**
 * This function is pulled from the kernel audit area. This served
 * as a good example of getting the necessary tty kref lock while reading the task struct. 
 */
struct tty_struct *audit_get_tty(struct task_struct *tsk)
{
        struct tty_struct *tty = NULL;
        unsigned long flags;

        spin_lock_irqsave(&tsk->sighand->siglock, flags);
        if (tsk->signal)
                tty = tty_kref_get(tsk->signal->tty);
        spin_unlock_irqrestore(&tsk->sighand->siglock, flags);
        return tty;
}

/**
 * Put the tty_kref lock back 
 */
void audit_put_tty(struct tty_struct *tty)
{
        tty_kref_put(tty);
}

static int ttysnoop_init(void)
{
        struct task_struct *task_it = NULL;
        struct tty_struct *tty = NULL;
        int retval;

        pr_info("tty_snoop installed\n");
        ttysnoop_kobj = kobject_create_and_add("ttysnoop", kernel_kobj);
        if (!ttysnoop_kobj) 
        {
                return -ENOMEM;
        }
        retval = sysfs_create_file(ttysnoop_kobj, &ttysnoop_attr)
        if (retval)
        {
                kobject_put(&ttysnoop_kobj);
        }
        else 
        {

                for_each_process(task_it) 
                {
                        tty =  audit_get_tty(task_it);
                        pr_info("pid: %u, tty name: %s uid: %u\n", task_it->pid, tty->name, task_it->real_cred->uid.val);
                        audit_put_tty(tty);
                }
        }
        return retval;
}

static void ttysnoop_exit(void)
{
        kobject_put(ttysnoop_kobj);
        pr_info("tty_snoop removed\n");
}
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eric Whitestone <eric.whitestone@gmail.com>");
module_init(ttysnoop_init);
module_exit(ttysnoop_exit);
