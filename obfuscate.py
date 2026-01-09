#!/usr/bin/env python3

import os
import re
import base64
import argparse
from pathlib import Path

# -------------------------------------------------
# Configuración
# -------------------------------------------------

C_KEYWORDS = {
    "auto", "break", "case", "char", "const", "continue", "default", "do",
    "double", "else", "enum", "extern", "float", "for", "goto", "if",
    "inline", "int", "long", "register", "restrict", "return", "short",
    "signed", "sizeof", "static", "struct", "switch", "typedef", "union",
    "unsigned", "void", "volatile", "while", "_Bool", "_Complex", "_Imaginary"
}

IDENTIFIER_REGEX = re.compile(r'\b[a-zA-Z_][a-zA-Z0-9_]*\b')
OBF_REGEX = re.compile(r'\bobf_[A-Za-z0-9_\-]+\b')

VALID_EXTENSIONS = {".c", ".h"}

# -------------------------------------------------
# Criptografía simple reversible
# -------------------------------------------------

def xor_crypt(data: bytes, key: bytes) -> bytes:
    return bytes(b ^ key[i % len(key)] for i, b in enumerate(data))


def obfuscate_name(name: str, key: str) -> str:
    enc = xor_crypt(name.encode(), key.encode())
    return "obf_" + base64.urlsafe_b64encode(enc).decode().rstrip("=")


def deobfuscate_name(obf: str, key: str) -> str:
    raw = obf[4:]
    padding = "=" * (-len(raw) % 4)
    dec = base64.urlsafe_b64decode(raw + padding)
    return xor_crypt(dec, key.encode()).decode()

# -------------------------------------------------
# Utilidades de archivos
# -------------------------------------------------

def collect_files(root: Path):
    files = []
    for path in root.rglob("*"):
        if path.suffix in VALID_EXTENSIONS:
            files.append(path)
    return files


def read_files(files):
    return {f: f.read_text(encoding="utf-8") for f in files}


def write_files(file_map):
    for path, content in file_map.items():
        path.write_text(content, encoding="utf-8")

# -------------------------------------------------
# Análisis global de símbolos
# -------------------------------------------------

def extract_identifiers(code: str) -> set:
    return set(IDENTIFIER_REGEX.findall(code))


def build_global_symbol_table(files_content: dict, key: str) -> dict:
    symbols = set()

    for code in files_content.values():
        for ident in extract_identifiers(code):
            if ident in C_KEYWORDS:
                continue
            if ident.startswith("obf_"):
                continue
            symbols.add(ident)

    table = {sym: obfuscate_name(sym, key) for sym in symbols}
    return table


def build_reverse_symbol_table(files_content: dict, key: str) -> dict:
    symbols = set()

    for code in files_content.values():
        symbols |= set(OBF_REGEX.findall(code))

    table = {}
    for obf in symbols:
        try:
            table[obf] = deobfuscate_name(obf, key)
        except Exception:
            pass

    return table

# -------------------------------------------------
# Sustitución segura
# -------------------------------------------------

def replace_symbols(code: str, table: dict) -> str:
    def repl(match):
        word = match.group(0)
        return table.get(word, word)

    return IDENTIFIER_REGEX.sub(repl, code)

# -------------------------------------------------
# Pipeline principal
# -------------------------------------------------

def obfuscate_project(root: Path, key: str):
    files = collect_files(root)
    content = read_files(files)

    table = build_global_symbol_table(content, key)

    for path in content:
        content[path] = replace_symbols(content[path], table)

    write_files(content)


def deobfuscate_project(root: Path, key: str):
    files = collect_files(root)
    content = read_files(files)

    table = build_reverse_symbol_table(content, key)

    for path in content:
        content[path] = replace_symbols(content[path], table)

    write_files(content)

# -------------------------------------------------
# CLI
# -------------------------------------------------

def main():
    parser = argparse.ArgumentParser(
        description="Ofuscador/desofuscador de proyectos C (src + include)"
    )
    parser.add_argument("mode", choices=["obfuscate", "deobfuscate"])
    parser.add_argument("project_root", help="Raíz del proyecto C")
    parser.add_argument("--key", required=True)

    args = parser.parse_args()
    root = Path(args.project_root)

    if not root.exists():
        raise SystemExit("Ruta del proyecto no existe")

    if args.mode == "obfuscate":
        obfuscate_project(root, args.key)
    else:
        deobfuscate_project(root, args.key)

if __name__ == "__main__":
    main()
