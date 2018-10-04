./gradlew distTar
mkdir tmp
mv driver/build/distributions/driver.tar tmp/driver.tar
cd tmp
tar vxf driver.tar
./bin/driver -action extractTriple -triple_file ../../../yagodump/escaped_yagoFacts.ttl -triple_output=../triples.txt"
