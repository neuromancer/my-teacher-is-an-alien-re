#!/usr/bin/env python3
"""Compare a single function without rebuilding."""
import sys
sys.path.insert(0, 'bin')
from compileAndCompare import get_similarity

fn = sys.argv[1]
dis = sys.argv[2]

sim, txt, seh = get_similarity(fn, dis, clean_build=False)
if sim is None:
    print(f'{fn}: {txt}')
else:
    print(txt)
    print(f'\nSimilarity: {sim:.2f}%')
    if seh and seh.strip():
        print(f'\nSEH code:')
        print(seh)
