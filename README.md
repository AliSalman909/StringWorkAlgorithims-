These are 4 questions from an Assignment that deal with creating new algorithims from taking parts of existing algorithims to solve various string related issues 
The hybrid solution implements an adaptive string matching system in C++ that dynamically selects between Naïve, KMP, and Rabin–Karp algorithms based on the pattern length and estimated alphabet size. For single-pattern search, the program:
-Uses Naïve for very short patterns,
-Prefers KMP for small/medium alphabets, and
-Starts with Rabin–Karp for large alphabets, automatically switching to KMP at runtime if too many spurious hash hits are detected.
-For multi-pattern search, it minimizes preprocessing by hashing all patterns of equal length into a set and scanning the text once with a rolling hash (Rabin–Karp style) to detect any matching pattern. The code provides a simple console interface where the user enters the text, alphabet size, and either a single pattern (adaptive search) or multiple patterns (multi-pattern Rabin–Karp).


### `Divide&ConquerTextSimilarity.cpp`

> This program computes the **length of the longest common substring** between two input documents using a binary search over substring length combined with a Rabin–Karp rolling hash checker. For each candidate length `K`, it hashes all substrings of length `K` in document A, then slides a rolling hash over document B and verifies candidate matches by direct substring comparison to avoid false positives. The “divide and conquer” aspect is in the **search over lengths** (using binary search), not in recursively splitting the documents themselves. The console interface asks for two lines of text and outputs only the maximum common substring length. 

###  `MultiPattern.cpp`

> This code implements a **classic Aho–Corasick multi-pattern matcher** for exact string search over lowercase English letters. It builds a trie of all input patterns, normalizes them and the text to lowercase, constructs failure links and `go` transitions, then scans the text once to report **all occurrences (including overlapping matches)** of every pattern. Matches are collected in a map from pattern → list of starting indices and printed in a readable format. The implementation focuses on case-insensitive exact matching with overlaps; it does **not** implement wildcard `?` handling or any special memory-optimized / partial-automaton design. 


###  `Divide+StringCompression.cpp`

> This program builds a **suffix array using a merge-sort–style divide and conquer algorithm** on suffix indices, then derives an LCP (Longest Common Prefix) array to find the **longest repeated substring** in a single input string. After sorting all suffixes lexicographically, it scans adjacent pairs to compute LCP values, reports the longest repeated substring and its length, and then generates the **Burrows–Wheeler Transform (BWT)** based on the suffix array. The input string is automatically given a `$` sentinel if missing, and detailed SA/LCP rows are printed for inspection. The implementation operates on one string (not explicit sub-blocks) and uses suffix-array logic rather than a separate suffix-array library. 


