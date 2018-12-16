"""Collect dataset statistics."""

from itertools import groupby
import kgekit.io

ONE_TO_ONE_MARGIN_LINE = 1.5
# Check this when modify:
# https://gist.github.com/fantasticfears/f8053480ff1db6da83cb30e446ab2181
def calculate_one_or_many(translation_filename, *triple_filenames):
    _, relations = kgekit.io.relationead_translation(translation_filename)
    relations = dict(relations)
    triples = []
    for filename in triple_filenames:
        tmp, _ = kgekit.io.read_triple_indexes(filename)
        triples += tmp

    triples.sort(key=lambda x: (x.relation, x.head, x.tail))
    grouped = [(k, list(g)) for k, g in groupby(triples, key=lambda x: x.relation)]
    num_of_relations = len(grouped)
    head_per_tail_list = [0] * num_of_relations
    tail_per_head_list = [0] * num_of_relations

    one_to_one = []
    one_to_many = []
    many_to_one = []
    many_to_many = []

    for relation, triples in grouped:
        head_list = []
        tail_list = []
        for triple in triples:
            head_list.append(triple.head)
            tail_list.append(triple.tail)
        head_set = set(head_list)
        tail_set = set(tail_list)
        head_per_tail = len(head_list) / len(tail_set)
        tail_per_head = len(tail_list) / len(head_set)
        head_per_tail_list[relation] = head_per_tail
        tail_per_head_list[relation] = tail_per_head
        if head_per_tail < ONE_TO_ONE_MARGIN_LINE and tail_per_head < ONE_TO_ONE_MARGIN_LINE:
            one_to_one.append(relation)
        elif head_per_tail >= ONE_TO_ONE_MARGIN_LINE and tail_per_head < ONE_TO_ONE_MARGIN_LINE:
            many_to_one.append(relation)
        elif head_per_tail < ONE_TO_ONE_MARGIN_LINE and tail_per_head >= ONE_TO_ONE_MARGIN_LINE:
            one_to_many.append(relation)
        else:
            many_to_many.append(relation)

    return relations, (one_to_one, one_to_many, many_to_one, many_to_many), (head_per_tail_list, tail_per_head_list)

