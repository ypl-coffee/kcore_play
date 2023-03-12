#define _LARGEFILE64_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <libelf.h>

const unsigned long long vaddr_text = 0xffffffffbcc00000;	/* $ grep " \_text" /proc/kallsyms */
const unsigned long long vaddr_nr_cpu_ids = 0xffffffffbe3af604;	/* $ grep " nr_cpu_ids" /proc/kallsyms */

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

	int nr_cpu_ids;

	lseek64(fd, phdr.p_offset + vaddr_nr_cpu_ids - vaddr_text, SEEK_SET);
	read(fd, &nr_cpu_ids, sizeof(int));

	printf("%d\n", nr_cpu_ids);

	elf_end(elf);
	close(fd);

	return 0;
}
