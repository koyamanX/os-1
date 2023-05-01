#ifndef ELF_H
#define ELF_H

#include <riscv.h>

typedef u64 Elf64_Addr;
typedef u64 Elf64_Off;
typedef u8 Elf64_Byte;
typedef u16 Elf64_Half;
typedef u64 Elf64_Sword;
typedef u32 Elf64_Word;
typedef i64 Elf64_Sxword;
typedef u64 Elf64_Xword;

#define EI_MAG0 0
#define EI_MAG1 1
#define EI_MAG2 2
#define EI_MAG3 3
#define EI_CLASS 4
#define EI_DATA 5
#define EI_VERSION 6
#define EI_OSABI 7
#define EI_ABIVERSION 8
#define EI_PAD 9
#define EI_NIDENT 16

#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

#define EV_NONE 0
#define EV_CURRENT 1

#define ELFCLASSNONE 0
#define ELFCLASS32 1
#define ELFCLASS64 2

#define ELFDATANONE 0
#define ELFDATA2LSB 1
#define ELFDATA2MSB 2

#define ELFOSABI_NONE 0
#define ELFOSABI_SYSV ELFOSABI_NONE

#define ET_NONE 0
#define ET_EXEC 2

#define EM_RISCV 243

#define IS_RISCV_ELF(ehdr)                        \
    ((ehdr).e_ident[EI_MAG0] == ELFMAG0 &&        \
     (ehdr).e_ident[EI_MAG1] == ELFMAG1 &&        \
     (ehdr).e_ident[EI_MAG2] == ELFMAG2 &&        \
     (ehdr).e_ident[EI_MAG3] == ELFMAG3 &&        \
     (ehdr).e_ident[EI_CLASS] == ELFCLASS64 &&    \
     (ehdr).e_ident[EI_DATA] == ELFDATA2LSB &&    \
     (ehdr).e_ident[EI_VERSION] == EV_CURRENT &&  \
     (ehdr).e_ident[EI_OSABI] == ELFOSABI_SYSV && \
     (ehdr).e_machine == EM_RISCV)

#define PT_NULL 0
#define PT_LOAD 1

#define PF_X 1
#define PF_W 2
#define PF_R 4

typedef struct elf64_hdr {
    Elf64_Byte e_ident[EI_NIDENT];
    Elf64_Half e_type;
    Elf64_Half e_machine;
    Elf64_Word e_version;
    Elf64_Addr e_entry;
    Elf64_Off e_phoff;
    Elf64_Off e_shoff;
    Elf64_Word e_flags;
    Elf64_Half e_ehsize;
    Elf64_Half e_phentsize;
    Elf64_Half e_phnum;
    Elf64_Half e_shentsie;
    Elf64_Half e_shnum;
    Elf64_Half e_shstrndx;
} Elf64_Ehdr;

typedef struct elf64_phdr {
    Elf64_Word p_type;
    Elf64_Word p_flags;
    Elf64_Off p_offset;
    Elf64_Addr p_vaddr;
    Elf64_Addr p_paddr;
    Elf64_Xword p_filesz;
    Elf64_Xword p_memsz;
    Elf64_Xword p_align;
} Elf64_Phdr;

typedef struct elf64_shdr {
    Elf64_Word sh_name;
    Elf64_Word sh_type;
    Elf64_Xword sh_flags;
    Elf64_Addr sh_addr;
    Elf64_Off sh_offset;
    Elf64_Xword sh_size;
    Elf64_Word sh_link;
    Elf64_Word sh_info;
    Elf64_Xword sh_addralign;
    Elf64_Xword sh_entsize;
} ELF64_Shdr;
#endif
