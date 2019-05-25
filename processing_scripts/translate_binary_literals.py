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

def translate_literals(translation_filename, literal_filename):
    ents_translation, rels_translation = kgekit.io.read_translation(translation_filename)
    ents = {}
    rels = {}
    for element, index in ents_translation:
        ents[element] = index
    for element, index in rels_translation:
        rels[element] = index

    labels = kgekit.io.read_labels(literal_filename, '\t')
    ent_res = []
    rel_res = []
    for item_id, text, lang in labels:
        if item_id in ents:
            ent_res.append(f"{ents[item_id]}\t{text}\t{lang}")
        elif item_id in rels:
            rel_res.append(f"{rels[item_id]}\t{text}\t{lang}")
        else:
            print(f"Unknown id: {item_id}")

    with open('literal_entities.txt', 'w') as f:
        for l in ent_res:
            f.write(l+"\n")
    
    with open('literal_relations.txt', 'w') as f:
        for l in rel_res:
            f.write(l+"\n")

if __name__ == '__main__':
    translate_literals(sys.argv[1], sys.argv[2])