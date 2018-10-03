import re
import sys
import os

def escape_file(path):
    paths, filename = os.path.split(path)
    with open(path, 'r') as f1, open(os.path.join(paths, "escaped_"+filename), 'w') as f2:
        content = f1.read()
        escaped = re.sub(r'\\u00(\d+)', r'%\1', content)
        f2.write(escaped)

escape_file(sys.argv[1])
