#define _LARGEFILE64_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <libelf.h>

const unsigned long long vaddr_text = 0xffffffffbcc00000;		/* $ grep " \_text" /proc/kallsyms */
const unsigned long long vaddr_boot_cpu_data = 0xffffffffbe3aea80;	/* $ grep " boot_cpu_data" /proc/kallsyms */

struct cpuinfo_x86 {
	unsigned char family, vendor, model;
};

int main(void)
{
	int fd = open("/proc/kcore", O_RDONLY);
	Elf *elf = elf_begin(fd, ELF_C_READ, NULL);

	Elf64_Ehdr ehdr;	/* ELF Executable Header */
	lseek64(fd, 0, SEEK_SET);
	read(fd, &ehdr, sizeof(ehdr));

	Elf64_Phdr phdr;	/* ELF Program Header(s) */
	for (int i = 0; i < ehdr.e_phnum; i++) {
		lseek64(fd, ehdr.e_phoff + (i * sizeof(phdr)), SEEK_SET);
		read(fd, &phdr, sizeof(phdr));

		if (phdr.p_vaddr == vaddr_text)
			break;
	}

	struct cpuinfo_x86 boot_cpu_data;

	lseek64(fd, phdr.p_offset + vaddr_boot_cpu_data - vaddr_text, SEEK_SET);
	read(fd, &boot_cpu_data, sizeof(boot_cpu_data));

	printf("CPU family:\t%d\n", boot_cpu_data.family);	/* 0x06, hard-coded in arch/x86/include/asm/cpu_device_id.h ...? */
	printf("CPU vendor:\t%d\n", boot_cpu_data.vendor);	/* 0x00, X86_VENDOR_INTEL */
	printf("CPU model:\t%d\n", boot_cpu_data.model);	/* 0x4f, INTEL_FAM6_BROADWELL_X */

	elf_end(elf);
	close(fd);

	return 0;
}
