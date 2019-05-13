import sys
from collections import defaultdict
import kgekit.io
import kgekit.data
import random
from itertools import chain, dropwhile

def get_lang_item(lang, lang_options):
    def _get_item(item_dict):
        saved_text = None

        for lang in lang_options:
            try:
                saved_text = item_dict[lang]
                break
            except:
                pass
        
        if saved_text is not None:
            return saved_text, lang
        else:
            return None 
    return _get_item

get_zh_item = get_lang_item('zh', ["zh-hant", "zh-tw", "zh-hans", "zh-cn", "zh"])
get_en_item = get_lang_item('en', ["en", "en-gb", "en-ca"])
get_sv_item = get_lang_item('sv', ["sv"])

def match_triple_labels(label_filename):
    elements = defaultdict(dict)
    res = []
    labels = kgekit.io.read_labels(label_filename, '\t')
    for ele, text, lang in labels:
        elements[ele][lang] = text
    
    for ele, item in elements.items():
        zh = get_zh_item(item)
        if zh is not None:
            l = "\t".join(zh)
            res.append(f"{ele}\t{l}")
        
        en = get_en_item(item)
        if en is not None:
            l = "\t".join(en)
            res.append(f"{ele}\t{l}")

        sv = get_sv_item(item)
        if sv is not None:
            l = "\t".join(sv)
            res.append(f"{ele}\t{l}")

    return res

if __name__ == '__main__':
    labels = match_triple_labels(sys.argv[1])
    with open('matched_labels.txt', 'w') as f:
        for l in labels:
            f.write(l + "\n")
