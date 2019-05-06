#! /bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )/.." >/dev/null 2>&1 && pwd )"
VERSION=`python3 -c "import kgekit; print(kgekit.__version__)" | tr -d '\n'`
echo "Entering $DIR for uploading new package version $VERSION"
cd $DIR
git add kgekit/__init__.py
git commit -m "version bump"
python3 setup.py sdist bdist_wheel
twine upload --verbose -s -i 76407351 -u fantasticfears dist/kgekit-$VERSION.tar.gz
