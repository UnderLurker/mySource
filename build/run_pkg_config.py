#!/usr/bin/env python
import sys
import subprocess

args = sys.argv[1:]
output = subprocess.check_output(["pkg-config"] + args).decode("utf-8").strip()

for item in output.split():
    print(item)
