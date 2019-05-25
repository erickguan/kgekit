cd $(pwd) && echo $(pwd) && wc -l *.txt
python3 -c "import kgekit.io; e, r = kgekit.io.read_translation('translation.protobuf'); print(len(list(e)), len(list(r)))"