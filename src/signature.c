/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signature.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jainavas <jainavas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 23:04:13 by jainavas          #+#    #+#             */
/*   Updated: 2026/01/08 17:59:44 by jainavas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/pestilence.h"

const char *get_signature(void)
{
    static char signature[MAX_SIGNATURE_LEN] = {0};
    
    if (signature[0] == '\0') {
        snprintf(signature, sizeof(signature),
                 "%s%s-%s",
                 SIGNATURE_PREFIX,
                 "jainavas",
                 "jvidal-t");
    }
    
    return signature;
}

// Detectar si el binario ya está infectado
bool is_infected(t_elf *elf)
{
    const char *sig = get_signature();
    size_t sig_len = strlen(sig);
    
    // Buscar la firma en TODO el archivo
    for (size_t i = 0; i < elf->size - sig_len; i++) {
        if (memcmp(elf->data + i, sig, sig_len) == 0)
            return true;
    }
    
    return false;
}
