#!/bin/bash
set -ex

run=( 300 320 340 360 380 400 420 440 460 480 500 )
for i in "${run[@]}"
do
    name=dataset$i
    echo $name
    dir=/data/wikidata/$name
    mkdir -p $dir
    cd $dir
    python3 /MIUN/kgekit/processing_scripts/build_dataset.py ../wikidata-triples.txt 1000 $i

    /MIUN/kgekit/processing_scripts/basic_stat.sh | tee stats.txt
done