#include <elf.h>
#include <fs.h>
#include <printk.h>
#include <proc.h>
#include <slob.h>
#include <vm.h>

int execv(const char *file, char const **argv) {
    struct inode *ip;
    Elf64_Ehdr ehdr;
    struct proc *rp;
    char *page;
    Elf64_Phdr *phdr = NULL;

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
    rp->tf->sp = USTACK;

    return 0;
}
