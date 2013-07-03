// Copyright (c) 2012- PPSSPP Project.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2.0 or later versions.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License 2.0 for more details.

// A copy of the GPL 2.0 should have been included with the program.
// If not, see http://www.gnu.org/licenses/

// Official git repository and contact information can be found at
// https://github.com/hrydgard/ppsspp and http://www.ppsspp.org/.

#pragma once

#include "../../Globals.h"

#include "ElfTypes.h"

enum {
	R_MIPS_NONE,
	R_MIPS_16,
	R_MIPS_32,
	R_MIPS_REL32,
	R_MIPS_26,
	R_MIPS_HI16,
	R_MIPS_LO16,
	R_MIPS_GPREL16,
	R_MIPS_LITERAL,
	R_MIPS_GOT16,
	R_MIPS_PC16,
	R_MIPS_CALL16,
	R_MIPS_GPREL32
};

enum KnownElfTypes
{
	KNOWNELF_PSP = 0,
	KNOWNELF_DS = 1,
	KNOWNELF_GBA = 2,
};

typedef int SectionID;

class ElfReader
{
protected:
	static inline unsigned int bswap32(unsigned int x) { return _byteswap_ulong(x); }
	static inline unsigned int bswap16(unsigned int x) { return _byteswap_ushort(x); }

	static inline void swap_elf(Elf32_Ehdr * header) {
	#ifdef PPC
		header->e_type =		bswap16(header->e_type);
		header->e_machine =		bswap16(header->e_machine);
		header->e_version =		bswap32(header->e_version);
		header->e_entry =		bswap32(header->e_entry);
		header->e_phoff =		bswap32(header->e_phoff);
		header->e_shoff =		bswap32(header->e_shoff);
		header->e_flags =		bswap32(header->e_flags);
		header->e_ehsize =		bswap16(header->e_ehsize);
		header->e_phentsize =	bswap16(header->e_phentsize);
		header->e_phnum =		bswap16(header->e_phnum);
		header->e_shentsize =	bswap16(header->e_shentsize);
		header->e_shnum =		bswap16(header->e_shnum);
		header->e_shstrndx =	bswap16(header->e_shstrndx);
	#endif
	}
	
	static inline void swap_elf(Elf32_Shdr * header) {
	#ifdef PPC
		header->sh_name =		bswap32(header->sh_name);
		header->sh_type =		bswap32(header->sh_type);
		header->sh_flags =		bswap32(header->sh_flags);
		header->sh_addr =		bswap32(header->sh_addr);
		header->sh_offset =		bswap32(header->sh_offset);
		header->sh_size =		bswap32(header->sh_size);
		header->sh_link =		bswap32(header->sh_link);
		header->sh_info =		bswap32(header->sh_info);
		header->sh_addralign =	bswap32(header->sh_addralign);
		header->sh_entsize =	bswap32(header->sh_entsize);
	#endif
	}

	
	static inline void swap_elf(Elf32_Phdr * header) {
	#ifdef PPC
		header->p_type =		bswap32(header->p_type);
		header->p_offset =		bswap32(header->p_offset);
		header->p_vaddr =		bswap32(header->p_vaddr);
		header->p_paddr =		bswap32(header->p_paddr);
		header->p_filesz =		bswap32(header->p_filesz);
		header->p_memsz =		bswap32(header->p_memsz);
		header->p_flags =		bswap32(header->p_flags);
		header->p_align =		bswap32(header->p_align);
	#endif
	}

	static inline void swap_elf(Elf32_Sym * header) {
	#ifdef PPC
		header->st_name =		bswap32(header->st_name);
		header->st_value =		bswap32(header->st_value);
		header->st_size =		bswap32(header->st_size);
		header->st_shndx =		bswap16(header->st_shndx);
	#endif
	}

	static inline void swap_elf(Elf32_Rel * header) {
	#ifdef PPC
		header->r_offset =		bswap32(header->r_offset);
		header->r_info =		bswap32(header->r_info);
	#endif
	}
public:
	ElfReader(void *ptr) :
		sectionOffsets(0),
		sectionAddrs(0),
		bRelocate(false),
		entryPoint(0),
		vaddr(0) {
		INFO_LOG(LOADER, "ElfReader: %p", ptr);
		base = (char*)ptr;
		base32 = (u32 *)ptr;
		header = (Elf32_Ehdr*)ptr;		
		swap_elf(header);
		segments = (Elf32_Phdr *)(base + header->e_phoff);
		sections = (Elf32_Shdr *)(base + header->e_shoff);
	}

	~ElfReader() {
		delete [] sectionOffsets;
		delete [] sectionAddrs;
	}

	u32 Read32(int off) {
		return base32[off>>2];
	}

	// Quick accessors
	ElfType GetType() { return (ElfType)(header->e_type); }
	ElfMachine GetMachine() { return (ElfMachine)(header->e_machine); }
	u32 GetEntryPoint() { return entryPoint; }
	u32 GetFlags() { return (u32)(header->e_flags); }

	int GetNumSegments() { return (int)(header->e_phnum); }
	int GetNumSections() { return (int)(header->e_shnum); }
	const char *GetSectionName(int section);
	u8 *GetPtr(int offset)
	{
		return (u8*)base + offset;
	}
	u8 *GetSectionDataPtr(int section)
	{
		if (section < 0 || section >= header->e_shnum)
			return 0;
		if (sections[section].sh_type != SHT_NOBITS)
			return GetPtr(sections[section].sh_offset);
		else
			return 0;
	}
	u8 *GetSegmentPtr(int segment)
	{
		return GetPtr(segments[segment].p_offset);
	}
	u32 GetSectionAddr(SectionID section) {return sectionAddrs[section];}
	int GetSectionSize(SectionID section)
	{
		return sections[section].sh_size;
	}
	SectionID GetSectionByName(const char *name, int firstSection=0); //-1 for not found

	u32 GetSegmentPaddr(int segment)
	{
	    return segments[segment].p_paddr;
	}
	u32 GetSegmentOffset(int segment)
	{
	    return segments[segment].p_offset;
	}
	u32 GetSegmentVaddr(int segment)
	{
		return segmentVAddr[segment];
	}

	bool DidRelocate() {
		return bRelocate;
	}

	u32 GetVaddr()
	{
		return vaddr;
	}

	u32 GetTotalSize()
	{
		return totalSize;
	}

	// More indepth stuff:)
	bool LoadInto(u32 vaddr);
	bool LoadSymbols();
	void LoadRelocations(Elf32_Rel *rels, int numRelocs);
	void LoadRelocations2(int rel_seg);


private:
	char *base;
	u32 *base32;
	Elf32_Ehdr *header;
	Elf32_Phdr *segments;
	Elf32_Shdr *sections;
	u32 *sectionOffsets;
	u32 *sectionAddrs;
	bool bRelocate;
	u32 entryPoint;
	u32 totalSize;
	u32 vaddr;
	u32 segmentVAddr[32];
};
