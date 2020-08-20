#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/tty.h>
#include <linux/cred.h>
#include <linux/uidgid.h>


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

void audit_put_tty(struct tty_struct *tty)
{
        tty_kref_put(tty);
}

static int hello_init(void)
{
        struct task_struct *task_it = NULL;
        struct tty_struct *tty = NULL;
        pr_info("tty_snoop installed\n");
                
        for_each_process(task_it) {
                tty =  audit_get_tty(task_it);
                pr_info("pid: %u, tty name: %s uid: %u\n", task_it->pid, tty->name, task_it->real_cred->uid.val);
		audit_put_tty(tty);
        }

        return 0;
}

static void hello_exit(void)
{
        pr_info("tty_snoop removed\n");
}

module_init(hello_init);
module_exit(hello_exit);
