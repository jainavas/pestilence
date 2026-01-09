/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   anti_process.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jainavas <jainavas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/08 18:08:56 by jainavas          #+#    #+#             */
/*   Updated: 2026/01/09 17:57:41 by jainavas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/pestilence.h"

// Verificar si existe un proceso con el nombre dado
bool is_process_running(const char *process_name)
{
    DIR *proc_dir;
    struct dirent *entry;
    char cmdline_path[512];
    char cmdline[256];
    FILE *fp;
    
    proc_dir = opendir("/proc");
    if (!proc_dir)
        return false;
    
    // Iterar sobre /proc/[pid]/
    while ((entry = readdir(proc_dir)) != NULL) {
        // Verificar que el nombre del directorio sea un número (PID)
        char *endptr;
        strtol(entry->d_name, &endptr, 10);
        if (*endptr != '\0')
            continue;
        
        // Leer /proc/[pid]/comm (nombre del proceso)
        snprintf(cmdline_path, sizeof(cmdline_path), 
                 "/proc/%s/comm", entry->d_name);
        
        fp = fopen(cmdline_path, "r");
        if (!fp)
            continue;
        
        if (fgets(cmdline, sizeof(cmdline), fp)) {
            // Quitar newline
            cmdline[strcspn(cmdline, "\n")] = '\0';
            
            // Comparar
            if (strcmp(cmdline, process_name) == 0) {
                fclose(fp);
                closedir(proc_dir);
                return true;
            }
        }
        
        fclose(fp);
    }
    
    closedir(proc_dir);
    return false;
}