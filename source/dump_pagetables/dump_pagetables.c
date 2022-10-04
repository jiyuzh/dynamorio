/*
 * Debug helper to dump the current kernel pagetables of the system
 * so that we can see what the various memory ranges are set to.
 *
 * (C) Copyright 2022 Jiyuan Zhang
 *
 * Author: Jiyuan Zhang <jiyuanz3@illinois.edu>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 */

/*
 * PTE ID 031 VA ffffffffc3e00000 PFN 184f17 TYP T
 * PTE: This entry represents a PTE table/PMD huge page
 * ID: Offset of this entry in the current PMD table
 * VA: Virtual address prefix of this entry
 * PFN: Physical frame number of the target
 * TYP: Type of the target (T for table page, P for leaf page)
 */

#include <linux/proc_fs.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/seq_file.h>
#include <linux/sort.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/sched/mm.h>
#include <linux/security.h>

#include <asm/pgtable.h>
#include <asm/pgtable_64.h>

static int selected_pid = 1;

// @return: may be null
struct mm_struct *get_mm_from_pid(struct seq_file *s, pid_t pid, int checked)
{
	struct task_struct *task;
	struct mm_struct *mm;

	task = pid_task(find_vpid(selected_pid), PIDTYPE_PID);
	if (!task) {
		return NULL;
	}

	mm = get_task_mm(task);

	if (!mm) {
		seq_printf(s, "Leave get_mm_from_pid, mm_struct not found\n");
		return NULL;
	}

	return mm;
}

static void print_pte_tree(struct seq_file *s, pte_t *pte0, unsigned long upper)
{
	unsigned long i;
	unsigned long va;

	for (i = 0; i < PTRS_PER_PTE; i++) {
		pte_t *pte = pte0 + i;
		unsigned long pfn;

		if (pte_none(*pte))
			continue;

		pfn = pte_pfn(*pte);

		va = upper + (i << PAGE_SHIFT);

		if (pfn)
			seq_printf(s, "\t\t\t\tPAGE ID %03ld VA %016lx PFN %lx TYP %s\n", i, va, pfn, "P");
	}
}

static void print_pmd_tree(struct seq_file *s, pmd_t *pmd0, unsigned long upper)
{
	unsigned long i;
	unsigned long va;
	bool leaf;

	for (i = 0; i < PTRS_PER_PMD; i++) {
		pmd_t *pmd = pmd0 + i;
		unsigned long pfn;

		if (pmd_none(*pmd) || unlikely(pmd_bad(*pmd)))
			continue;

		pfn = pmd_pfn(*pmd);
		leaf = pmd_large(*pmd);

		va = upper + (i << PMD_SHIFT);

		if (pfn)
			seq_printf(s, "\t\t\tPTE ID %03ld VA %016lx PFN %lx TYP %s\n", i, va, pfn, leaf ? "P" : "T");

		if (leaf)
			continue;

		print_pte_tree(s, (pte_t *) pmd_page_vaddr(*pmd), va);
	}
}

static void print_pud_tree(struct seq_file *s, pud_t *pud0, unsigned long upper, loff_t off)
{
	unsigned long i = off / (1);
	unsigned long j = off % (1);
	unsigned long va;
	bool leaf;

	i = off / (1);
	j = off % (1);

	pud_t *pud = pud0 + i;
	unsigned long pfn;

	if (pud_none(*pud) || unlikely(pud_bad(*pud)))
		return;

	pfn = pud_pfn(*pud);
	leaf = pud_large(*pud);

	va = upper + (i << PUD_SHIFT);

	if (pfn && !j)
		seq_printf(s, "\t\tPMD ID %03ld VA %016lx PFN %lx TYP %s\n", i, va, pfn, leaf ? "P" : "T");

	if (leaf)
		return;

	print_pmd_tree(s, (pmd_t *) pud_page_vaddr(*pud), va);
}

static void print_p4d_tree(struct seq_file *s, p4d_t *p4d0, unsigned long upper, loff_t off)
{
	unsigned long i = off / (PTRS_PER_PUD);
	unsigned long j = off % (PTRS_PER_PUD);
	unsigned long va;
	bool leaf;

	p4d_t *p4d = p4d0 + i;
	unsigned long pfn;

	if (p4d_none(*p4d) || unlikely(p4d_bad(*p4d)))
		return;

	pfn = p4d_pfn(*p4d);
	leaf = p4d_large(*p4d);

	va = upper + (i << P4D_SHIFT);

	if (pfn && !j)
		seq_printf(s, "\tPUD ID %03ld VA %016lx PFN %lx TYP %s\n", i, va, pfn, leaf ? "P" : "T");

	if (leaf)
		return;

	print_pud_tree(s, (pud_t *) p4d_page_vaddr(*p4d), va, j);
}

static void print_pgd_tree(struct seq_file *s, pgd_t *pgd0, bool include_kernel, loff_t off)
{
	unsigned long upper = 0;

	unsigned long i = off / (PTRS_PER_P4D * PTRS_PER_PUD);
	unsigned long j = off % (PTRS_PER_P4D * PTRS_PER_PUD);
	unsigned long va;
	bool leaf;

	pgd_t *pgd = pgd0 + i;
	unsigned long pfn;

	if (pgd_none(*pgd) || unlikely(pgd_bad(*pgd)))
		return;

	pfn = pgd_pfn(*pgd);
	leaf = pgd_large(*pgd);

	if (i >= 256) {
		if (!include_kernel)
			return;

		upper = PGDIR_MASK << 9;
	}

	va = upper + (i << PGDIR_SHIFT);

	if (pfn && !j)
		seq_printf(s, "P4D ID %03ld VA %016lx PFN %lx TYP %s\n", i, va, pfn, leaf ? "P" : "T");

	if (leaf)
		return;

	if (!pgtable_l5_enabled())
		print_p4d_tree(s, (p4d_t *) pgd, va, j);
	else
		print_p4d_tree(s, (p4d_t *) pgd_page_vaddr(*pgd), va, j);

}

long unsigned int extract_cr3(struct mm_struct *mm)
{
	void *vcr3 = mm->pgd;
	return __pa(vcr3) >> PAGE_SHIFT;
}

static int pt_seq_show(struct seq_file *s, void *v)
{
	struct mm_struct *mm = NULL;
	loff_t *spos = v;

	if (selected_pid != -1) {
		mm = get_mm_from_pid(s, selected_pid, false);
	}

	if (!mm) {
		seq_printf(s, "Invalid PID %d, fallback to %d\n", selected_pid, current->pid);

		mm = get_task_mm(current);
		selected_pid = current->pid;
	}

	if (!mm) {
		seq_printf(s, "Invalid fallback PID %d, exitn", selected_pid); 
		return 0;
	}

	if (*spos == 0) {
		seq_printf(s, "CR3 PID %d PFN %lx\n", selected_pid, extract_cr3(mm));
		seq_printf(s, "\n");
	}

	print_pgd_tree(s, mm->pgd, true, *spos);

	mmput(mm);

	return 0;
}

static void *pt_seq_start(struct seq_file *s, loff_t *pos)
{
	loff_t *spos;

	spos = kmalloc(sizeof(loff_t), GFP_KERNEL);
	if (!spos)
		return NULL;

	*spos = *pos;
	return spos;
}

static void *pt_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	loff_t *spos = v;
	*pos = ++*spos;

	if (*spos >= PTRS_PER_PGD * PTRS_PER_P4D * PTRS_PER_PUD)
		return NULL;

	return spos;
}

static void pt_seq_stop(struct seq_file *s, void *v)
{
	kfree(v);
}

ssize_t pt_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	char mybuf[64];

	if (count > sizeof(mybuf)) {
		count = sizeof(mybuf);
	}

	if (kstrtoint_from_user(buf, count, 10, &selected_pid)) {
		selected_pid = -1;
	}

	return count;
}

static struct seq_operations pt_seq_ops = {
	.start = pt_seq_start,
	.next  = pt_seq_next,
	.stop  = pt_seq_stop,
	.show  = pt_seq_show,
};

static int pt_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &pt_seq_ops);
};

static struct proc_ops pt_file_ops = {
	.proc_open    = pt_open,
	.proc_read    = seq_read,
	.proc_write   = pt_write,
	.proc_lseek   = seq_lseek,
	.proc_release = seq_release
};

static struct proc_dir_entry *pt_file;

int init_module(void)
{
	pt_file = proc_create("page_tables", 0666, NULL, &pt_file_ops);
	if (!pt_file)
		return -ENOMEM;

	return 0;
}

void cleanup_module(void)
{
	remove_proc_entry("page_tables", NULL);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jiyuan Zhang <jiyuanz3@illinois.edu>");
MODULE_DESCRIPTION("Kernel debugging helper that dumps pagetables");
