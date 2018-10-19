"""YAGO3's ttle file contain escaped unicode symbol like `\u0022`. They are
escaped by `escape_file.py`. This file reverse the symbol of percentage encoded
symbols to UTF-8 string.
"""
import re
import sys
import os
import binascii

def unescape_pecentage_encoded(matchobj):
    if matchobj:
        return binascii.unhexlify(matchobj.group(1)).decode('charmap')
    else:
        return ''

def unescape_file(path):
    paths, filename = os.path.split(path)
    with open(path, 'r') as f1, open(os.path.join(paths, "processed_"+filename), 'w') as f2:
        content = f1.read()
        escaped = re.sub(r'%(\d\d)', unescape_pecentage_encoded, content)
        f2.write(escaped)

unescape_file(sys.argv[1])
