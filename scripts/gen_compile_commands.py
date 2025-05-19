#!/usr/bin/env python3
import json
import os
import sys


def create_compile_commands():
    # Read make output from stdin
    make_output = sys.stdin.read()

    commands = []
    for line in make_output.split('\n'):
        if 'gcc' in line:
            # Find the source file
            src_files = [f for f in line.split() if f.endswith('.c')]
            if src_files:
                entry = {
                    "directory": os.getcwd(),
                    "command": line,
                    "file": src_files[0]
                }
                commands.append(entry)

    # Write the compilation database
    with open('compile_commands.json', 'w') as f:
        json.dump(commands, f, indent=2)


if __name__ == '__main__':
    create_compile_commands()
