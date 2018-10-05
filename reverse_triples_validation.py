import sys
import kgekit

def validate(triples):
    pass

if __name__ == '__main__':
    with open(sys.argv[1]) as f:
        validate(f.read())
