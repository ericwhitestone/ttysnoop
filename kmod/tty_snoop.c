#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>


static int hello_init(void)
{
	struct task_struct *task_it;

    pr_info("tty_snoop installed\n");
	
	for_each_process(task_it){
		pr_info("pid: %u \n", task_it->pid);
	}

    return 0;
}

static void hello_exit(void)
{
    pr_info("tty_snoop removed\n");
}

module_init(hello_init);
module_exit(hello_exit);
