"""YAGO3's ttle file contain escaped unicode symbol like `\u0022`. This file
percentage encoded the ttl files.
"""
import re
import sys
import os

def escape_file(path):
    paths, filename = os.path.split(path)
    with open(path, 'r') as f1, open(os.path.join(paths, "escaped_"+filename), 'w') as f2:
        content = f1.read()
        escaped = re.sub(r'\\u00(\d+)', r'%\1', content)
        escaped = re.sub(r'\\\\', r'%92', escaped)
        f2.write(escaped)

escape_file(sys.argv[1])
