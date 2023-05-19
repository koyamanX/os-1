#include <elf.h>
#include <fs.h>
#include <kmalloc.h>
#include <printk.h>
#include <proc.h>
#include <string.h>
#include <vm.h>

#define MAX_ARG 8
#define MAX_ARG_LEN 128
int execv(const char *file, char const **argv) {
    struct inode *ip;
    Elf64_Ehdr ehdr;
    struct proc *rp;
    char *page;
    Elf64_Phdr *phdr = NULL;
    u64 sp = USTACK;
    int argc = 0;

    ip = namei((char *)file);
    // TODO: check permission for executable file (rx)
    if (ip == NULL) {
        return -1;
    }
    readi(ip, (char *)&ehdr, 0, sizeof(ehdr));

    if (IS_RISCV_ELF(ehdr) && ehdr.e_type == ET_EXEC) {
        DEBUG_PRINTK("Valid ELF\n");
    } else {
        DEBUG_PRINTK("Invalid ELF\n");
    }

    phdr = kmalloc(sizeof(Elf64_Phdr));

    // Extract argv from caller's memory space.
    if (argv != NULL) {
        u64 pa;
        size_t n;
        char str[MAX_ARG_LEN + 1];
        char *args[MAX_ARG + 1];
        memset(args, 0, sizeof(args));

        for (int i = 0; i < MAX_ARG; i++) {
            copyin((void *)argv, &pa, sizeof(char *));
            if (pa == 0)
                break;
            memset(str, 0, sizeof(str));
            copyinstr((void *)pa, str, MAX_ARG_LEN, &n);

            n = ALIGN_SP(n);
            sp = sp - n;
            args[i] = (void *)sp;
            copyout(str, (void *)sp, n);

            argc++;
            argv++;
        }
        sp = sp - (sizeof(char *) * (MAX_ARG + 1));
        copyout(args, (void *)sp, sizeof(char *) * (MAX_ARG + 1));
    }

    rp = this_proc();
    readi(ip, (char *)phdr, ehdr.e_phoff, sizeof(Elf64_Phdr) * ehdr.e_phnum);

    u64 prot = PTE_V | PTE_U;
    for (int i = 0; i < ehdr.e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD) {
            u64 off = phdr[i].p_offset;

            VERBOSE_PRINTK(
                "PT_LOAD: p_offset: %x, p_vaddr: %x, p_paddr: %x, p_filesz: "
                "%x,p_memsz: %x, p_align: %x\n",
                phdr[i].p_offset, phdr[i].p_vaddr, phdr[i].p_paddr,
                phdr[i].p_filesz, phdr[i].p_memsz, phdr[i].p_align);

            if (phdr[i].p_flags & PF_X) {
                prot |= PTE_X;
            }
            if (phdr[i].p_flags & PF_R) {
                prot |= PTE_R;
            }
            if (phdr[i].p_flags & PF_W) {
                prot |= PTE_W;
            }
            for (u64 va = phdr[i].p_vaddr;
                 va < phdr[i].p_vaddr + phdr[i].p_memsz; va += PAGE_SIZE) {
                page = (char *)va2pa(rp->pgtbl, va);
                if (page == NULL) {
                    page = alloc_page();
                    kvmmap(rp->pgtbl, va, (u64)page, PAGE_SIZE, prot);
                }
                kvmdump(rp->pgtbl, va);
                readi(ip, (char *)page, off, PAGE_SIZE);
                off += PAGE_SIZE;
            }
            // TODO: If memsz is grater than filesz, zero fill.
        }
    }
    kfree(phdr);

    rp->tf->sepc = ehdr.e_entry;
    rp->tf->sp = sp;
    rp->tf->a0 = argc;
    rp->tf->a1 = sp;

    return 0;
}
