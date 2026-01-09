/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf_parser.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jainavas <jainavas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 17:54:11 by jainavas          #+#    #+#             */
/*   Updated: 2026/01/09 22:39:16 by jainavas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/pestilence.h"

int parse_elf(const char *filename, t_elf *elf)
{
    int fd = custom_open(filename, O_RDONLY);
    if (fd < 0)
        return -1;
    
    // Obtener tamaño del archivo
    struct stat st;
    if (fstat(fd, &st) < 0) {
        custom_close(fd);
        return -1;
    }
    elf->size = st.st_size;
    
    // Mapear archivo en memoria (más eficiente que read)
    elf->data = mmap(NULL, elf->size, PROT_READ, MAP_PRIVATE, fd, 0);
    custom_close(fd);
    
    if (elf->data == MAP_FAILED) {
        return -1;
    }
    
    // Verificar magic number (los primeros 4 bytes deben ser 0x7f 'E' 'L' 'F')
    if (memcmp(elf->data, ELFMAG, SELFMAG) != 0) {
        munmap(elf->data, elf->size);
        return -1;
    }
    
    // Inicializar punteros a las estructuras
    elf->ehdr = (Elf64_Ehdr *)elf->data;
    
    // Verificar que sea 64-bit
    if (elf->ehdr->e_ident[EI_CLASS] != ELFCLASS64) {
        munmap(elf->data, elf->size);
        return -1;
    }
    
    // Los program headers empiezan en e_phoff
    elf->phdr = (Elf64_Phdr *)(elf->data + elf->ehdr->e_phoff);
    
    // Los section headers empiezan en e_shoff
    elf->shdr = (Elf64_Shdr *)(elf->data + elf->ehdr->e_shoff);
    
    return 0;
}

void cleanup_elf(t_elf *elf)
{
    if (elf->data)
        munmap(elf->data, elf->size);
}

// Busca una sección por nombre (ej: ".text")
Elf64_Shdr *find_section(t_elf *elf, const char *name)
{
    // La tabla de strings de nombres de secciones está en e_shstrndx
    Elf64_Shdr *shstrtab = &elf->shdr[elf->ehdr->e_shstrndx];
    char *string_table = elf->data + shstrtab->sh_offset;
    
    for (int i = 0; i < elf->ehdr->e_shnum; i++) {
        char *section_name = string_table + elf->shdr[i].sh_name;
        if (strcmp(section_name, name) == 0)
            return &elf->shdr[i];
    }
    
    return NULL;
}