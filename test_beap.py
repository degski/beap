
# Run with nosetests3

import random
import math

from beap import Beap, VerifiedBeap


# Data from Ian Munro's "ImpSODA06.ppt" presentation, Slide 3,
# with mistake corrected (21 and 22 not in beap order).
BEAP_DATA = [
    72,
    68, 63,
    44, 62, 55,
    33, 22, 32, 51,
    13, 18, 21, 19, 22,
    11, 12, 14, 17, 9, 13,
     3,  2, 10
]

def print_spans():
    for i in range(100):
        print(i, " ", Beap.span(i))
        
def test_span():
    assert Beap.span(0) == (0, 0)
    assert Beap.span(1) == (1, 2)
    assert Beap.span(2) == (3, 5)
    assert Beap.span(3) == (6, 9)
    assert Beap.span(4) == (10, 14)    
    assert Beap.span(5) == (15, 20)    
    assert Beap.span(100) == (5050, 5150)
    # Size of span of height i is i + 1
    for i in range(100):
        start, end = Beap.span(i)
        assert end - start + 1 == i + 1


def test_invariants():
    beap = VerifiedBeap()
    beap.arr = BEAP_DATA.copy()
    beap.height = 6
    beap.check_invariants()

    beap.arr[0], beap.arr[1] = beap.arr[1], beap.arr[0]
    try:
        beap.check_invariants()
        failed = True
    except AssertionError:
        failed = False
    assert not failed, "Invariant checks don't work"

    beap.arr = BEAP_DATA.copy()
    beap.arr[0], beap.arr[len(beap.arr) - 1] = beap.arr[len(beap.arr) - 1], beap.arr[0]
    try:
        beap.check_invariants()
        failed = True
    except AssertionError:
        failed = False
    assert not failed, "Invariant checks don't work"


def test_search_once_off():
    beap = VerifiedBeap()
    beap.arr = BEAP_DATA.copy()
    beap.height = 6

    assert beap.search(51) == (9, 3)
    assert beap.search(53) == None


def test_search():
    beap = VerifiedBeap()
    beap.arr = BEAP_DATA.copy()
    beap.height = 6

    for i in range(101):
        res = beap.search(i)
        if i not in BEAP_DATA:
            assert res is None
        else:
            idx, height = res
            assert beap.arr[idx] == i
            assert height <= beap.height


def test_insert_trace_few():
    beap = VerifiedBeap()

    beap.insert(1)
    assert beap.height == 0
    assert beap.arr == [1]
    beap.check_invariants()

    beap.insert(2)
    assert beap.height == 1
    assert beap.arr == [2, 1]
    beap.check_invariants()

    beap.insert(3)
    assert beap.height == 1
    assert beap.arr == [3, 1, 2]
    beap.check_invariants()

    beap.insert(4)
    assert beap.height == 2
    assert beap.arr == [4, 3, 2, 1]
    beap.check_invariants()

    beap.insert(5)
    assert beap.height == 2
    assert beap.arr == [5, 3, 4, 1, 2]
    beap.check_invariants()

    beap.insert(6)
    assert beap.height == 2
    assert beap.arr == [6, 3, 5, 1, 2, 4]
    beap.check_invariants()

    beap.insert(7)
    assert beap.height == 3
    assert beap.arr == [7, 6, 5, 3, 2, 4, 1]
    beap.check_invariants()    
def test_insert():
    beap = VerifiedBeap()
    data = sorted(BEAP_DATA)
    for v in data:
        beap.insert(v)
        beap.check_invariants()
    # Doesn't work and doesn't have to work, as there're many different
    # partial orderings compatible with beap.
    #assert beap.arr == BEAP_DATA

    beap2 = VerifiedBeap()
    data = sorted(BEAP_DATA, reverse=True)
    for v in data:
        beap2.insert(v)
        beap2.check_invariants()

    for i in range(101):
        res1 = bool(beap.search(i))
        res2 = bool(beap2.search(i))
        assert res1 == res2


def test_delete():
    beap = VerifiedBeap()
    beap.arr = BEAP_DATA.copy()
    beap.height = 6

    res = []
    while beap.arr:
        res.append(beap.arr[0])
        beap.delete(0, 0)
        beap.check_invariants()

    assert res == sorted(res, reverse=True)


def test_insert_remove_random():
    beap = VerifiedBeap()
    items = []
    res = []

    RANGE = 100
    for i in range(RANGE):
        v = random.randrange(RANGE * 2)
        items.append(v)
        beap.insert(v)
        beap.check_invariants()

    while items:
        idx = random.randrange(len(items))
        v = items.pop(idx)
        beap.remove(v)
        beap.check_invariants()

    assert len(beap.arr) == 0


def test_op_complexity():
    beap = VerifiedBeap()
    items = []
    res = []

    RANGE = 100
    for i in range(RANGE):
        v = random.randrange(RANGE * 2)
        items.append(v)
        max_iters = math.ceil(math.sqrt(2 * len(beap.arr)))
        beap.iters = 0
        beap.insert(v)
        assert beap.iters <= max_iters

    max_iters = 2 * math.ceil(math.sqrt(2 * len(beap.arr)))

    for v in items:
        beap.iters = 0
        beap.search(v)
        assert beap.iters <= max_iters

    while beap.arr:
        max_iters = math.ceil(math.sqrt(2 * len(beap.arr)))
        beap.iters = 0
        beap.delete(0, 0)
        assert beap.iters <= max_iters

def main():
    beap = VerifiedBeap()
    beap.arr = BEAP_DATA.copy()
    beap.height = 6
    beap.insert(54)

if __name__ == "__main__": 
    main ( )
