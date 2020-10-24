from nltk.tokenize import sent_tokenize


def lines(a, b):
    """Return lines in both a and b"""
    a_set = set(a.splitlines())
    b_set = set(b.splitlines())
    return list(a_set & b_set)


def sentences(a, b):
    """Return sentences in both a and b"""
    a_set = set(sent_tokenize(a))
    b_set = set(sent_tokenize(b))
    return list(a_set & b_set)


def get_substrs(a, n):
    # get all substrings from input string with lenght n
    substrs_list = []
    lenght = len(a)
    for i in range(lenght):
        end = i + n
        if end > lenght:
            break
        substrs_list.append(a[i:end])
    return substrs_list


def substrings(a, b, n):
    """Return substrings of length n in both a and b"""
    a_set = set(get_substrs(a, n))
    b_set = set(get_substrs(b, n))
    return list(a_set & b_set)