/*
 * elf_def.h
 *
 *  Created on: 2016-4-26
 *      Author: Administrator
 *      目前只考虑32位与64位兼容的方案.即默认so都是32位版本.
 */

#ifndef ELF_DEF_H_
#define ELF_DEF_H_

#ifndef ARCH_ELFSIZE
#define ARCH_ELFSIZE 32
#endif
#include <elf.h>

#if !defined(ELFSIZE) && defined(ARCH_ELFSIZE)
#define ELFSIZE ARCH_ELFSIZE
#endif

#if defined(ELFSIZE)
#define CONCAT(x,y)	__CONCAT(x,y)
#define ELFNAME(x)	CONCAT(elf,CONCAT(ELFSIZE,CONCAT(_,x)))
#define ELFNAME2(x,y)	CONCAT(x,CONCAT(_elf,CONCAT(ELFSIZE,CONCAT(_,y))))
#define ELFNAMEEND(x)	CONCAT(x,CONCAT(_elf,ELFSIZE))
#define ELFDEFNNAME(x)	CONCAT(ELF,CONCAT(ELFSIZE,CONCAT(_,x)))
#endif

#if defined(ELFSIZE) && (ELFSIZE == 32)
#define Elf_Ehdr	Elf32_Ehdr
#define Elf_Phdr	Elf32_Phdr
#define Elf_Shdr	Elf32_Shdr
#define Elf_Sym		Elf32_Sym
#define Elf_Rel		Elf32_Rel
#define Elf_RelA	Elf32_Rela
#define Elf_Dyn		Elf32_Dyn
#define Elf_Half	Elf32_Half
#define Elf_Word	Elf32_Word
#define Elf_Sword	Elf32_Sword
#define Elf_Addr	Elf32_Addr
#define Elf_Off		Elf32_Off
#define Elf_Nhdr	Elf32_Nhdr
#define Elf_Note	Elf32_Note

#define ELF_R_SYM	ELF32_R_SYM
#define ELF_R_TYPE	ELF32_R_TYPE
#define ELF_R_INFO	ELF32_R_INFO
#define ELFCLASS	ELFCLASS32

#define ELF_ST_BIND	ELF32_ST_BIND
#define ELF_ST_TYPE	ELF32_ST_TYPE
#define ELF_ST_INFO	ELF32_ST_INFO

#define AuxInfo		Aux32Info
#elif defined(ELFSIZE) && (ELFSIZE == 64)
#define Elf_Ehdr	Elf64_Ehdr
#define Elf_Phdr	Elf64_Phdr
#define Elf_Shdr	Elf64_Shdr
#define Elf_Sym		Elf64_Sym
#define Elf_Rel		Elf64_Rel
#define Elf_RelA	Elf64_Rela
#define Elf_Dyn		Elf64_Dyn
#define Elf_Half	Elf64_Half
#define Elf_Word	Elf64_Word
#define Elf_Sword	Elf64_Sword
#define Elf_Addr	Elf64_Addr
#define Elf_Off		Elf64_Off
#define Elf_Nhdr	Elf64_Nhdr
#define Elf_Note	Elf64_Note

#define ELF_R_SYM	ELF64_R_SYM
#define ELF_R_TYPE	ELF64_R_TYPE
#define ELF_R_INFO	ELF64_R_INFO
#define ELFCLASS	ELFCLASS64

#define ELF_ST_BIND	ELF64_ST_BIND
#define ELF_ST_TYPE	ELF64_ST_TYPE
#define ELF_ST_INFO	ELF64_ST_INFO

#define AuxInfo		Aux64Info
#endif

#endif /* ELF_DEF_H_ */
