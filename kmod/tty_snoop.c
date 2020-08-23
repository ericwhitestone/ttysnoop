#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/tty.h>
#include <linux/cred.h>
#include <linux/uidgid.h>


void ttysnoop_release(struct kobject *kobj) {
       pr_info("ttysnoop releasing\n"); 
}

static struct kobj_type ttysnoop_ktype = { 
    .release = ttysnoop_release
};

static struct ttysnoop_state {
        /**
         * kobject for the data structure maintained by this module 
         * that will keep sysfs up to date
         */
        struct kobject ttysnoop_kobj;
} ttysnoop_state;

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
        int kobj_add_err;

        pr_info("tty_snoop installed\n");
        kobject_init(&ttysnoop_state.ttysnoop_kobj, &ttysnoop_ktype);
        kobj_add_err = kobject_add(&ttysnoop_state.ttysnoop_kobj, kernel_kobj, "ttysnoop");
        if (kobj_add_err)
        {
                kobject_put(&ttysnoop_state.ttysnoop_kobj);
                return kobj_add_err;
        }
        for_each_process(task_it) {
                tty =  audit_get_tty(task_it);
                pr_info("pid: %u, tty name: %s uid: %u\n", task_it->pid, tty->name, task_it->real_cred->uid.val);
		audit_put_tty(tty);
        }

        return 0;
}

static void ttysnoop_exit(void)
{
        kobject_put(&ttysnoop_state.ttysnoop_kobj);
        pr_info("tty_snoop removed\n");
}
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eric Whitestone <eric.whitestone@gmail.com>");
module_init(ttysnoop_init);
module_exit(ttysnoop_exit);
