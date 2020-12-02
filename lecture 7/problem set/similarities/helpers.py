from nltk.tokenize import sent_tokenize


def lines(a, b):
    # split a and b by their line breaks "\n"
    a_split = a.split("\n")
    b_split = b.split("\n")

    # list comprehension by enumerating each and comparing to see if each value in the list
    # of splits for a exists in the list of splits for b
    results = [str for str in a_split if (str in b_split) or (not str and "" in b_split)]

    # set makes sure there are no duplicates
    return set(results)


def sentences(a, b):
    # split a and b by their sentence breaks as defined by Natural Language Toolkit
    a_split = sent_tokenize(a)
    b_split = sent_tokenize(b)

    # list comprehension by enumerating each and comparing to see if each value in the list
    # of splits for a exists in the list of splits for b
    results = [str for str in a_split if (str in b_split) or (not str and "" in b_split)]

    # set makes sure there are no duplicates
    return set(results)


def substrings(a, b, n):
    # TODO probably a faster way to do this, but just did it in the cleanest way first
    # list comprhension to splice out each substring of length n from a string
    # for both a and b
    a_split = [a[ix:ix + n] for ix, str in enumerate(a) if ix < len(a) - n + 1]
    b_split = [b[ix:ix + n] for ix, str in enumerate(b) if ix < len(b) - n + 1]

    # list comprehension by enumerating each and comparing to see if each value in the list
    # of splits for a exists in the list of splits for b
    results = [str for str in a_split if (str in b_split) or (not str and "" in b_split)]

    # set makes sure there are no duplicates
    return set(results)
