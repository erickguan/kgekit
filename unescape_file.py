import re
import sys
import os
import binascii

def unescape_pecentage_encoded(matchobj):
    if matchobj:
        return binascii.unhexlify(match.group(1)).decode('UTF-8')
    else:
        return ''

def unescape_file(path):
    paths, filename = os.path.split(path)
    with open(path, 'r') as f1, open(os.path.join(paths, "processed_"+filename), 'w') as f2:
        content = f1.read()
        escaped = re.sub(r'%(\d+)', unescape_pecentage_encoded, content)
        f2.write(escaped)

unescape_file(sys.argv[1])
