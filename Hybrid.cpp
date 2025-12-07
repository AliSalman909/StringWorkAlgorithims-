#include <iostream>
#include <string>
#include <vector>
#include <unordered_set> // For Task 2
#include <cmath>         // For pow()
#include <limits>        // For cin.ignore

using namespace std;

// --- 1. Naïve Algorithm ---
vector<int> naiveSearch(const string& text, const string& pattern) {
    vector<int> matches;
    int n = text.length();
    int m = pattern.length();

    for (int i = 0; i <= n - m; i++) {
        int j;
        for (j = 0; j < m; ++j) {
            if (text[i + j] != pattern[j]) {
                break;
            }
        }
        if (j == m) {
            matches.push_back(i);
        }
    }
    return matches;
}

// --- 2. KMP Algorithm ---
vector<int> computeLPS(const string& pattern) {
    int m = pattern.length();
    vector<int> lps(m, 0);
    int length = 0; // Length of the previous longest prefix suffix
    int i = 1;

    while (i < m) {
        if (pattern[i] == pattern[length]) {
            length++;
            lps[i] = length;
            i++;
        }
        else {
            if (length != 0) {
                length = lps[length - 1];
            }
            else {
                lps[i] = 0;
                i++;
            }
        }
    }
    return lps;
}

vector<int> kmpSearch(const string& text, const string& pattern, int startIndex = 0) {
    vector<int> matches;
    int n = text.length();
    int m = pattern.length();

    if (m == 0) return matches;

    vector<int> lps = computeLPS(pattern);
    int i = startIndex; // text index
    int j = 0;          // pattern index

    while (i < n) {
        if (pattern[j] == text[i]) {
            i++;
            j++;
        }
        if (j == m) {
            matches.push_back(i - j);
            j = lps[j - 1];
        }
        else if (i < n && pattern[j] != text[i]) {
            if (j != 0) {
                j = lps[j - 1];
            }
            else {
                i++;
            }
        }
    }
    return matches;
}

// --- 3. Rabin-Karp Algorithm (with Adaptive Switching Logic) ---

// d = number of characters in alphabet (e.g., 256 for ASCII)
const long long d = 256;
const long long q = 101; // A small prime for simplicity


vector<int> rabinKarpSearch(const string& text, const string& pattern, int startIndex,
    int& lastCheckedIndex, bool& switchRequired) {
    vector<int> matches;
    int n = text.length();
    int m = pattern.length();
    long long pHash = 0; // hash value for pattern
    long long tHash = 0; // hash value for text window
    long long h = 1;     // d^(m-1) % q

    // *** CHANGE 2: Spurious hit limit changed from 5 to 3 ***
    int SPURIOUS_HIT_LIMIT = 3; // Adaptive switching threshold
    int spuriousHits = 0;
    switchRequired = false;
    lastCheckedIndex = startIndex;

    // Handle edge case where text is shorter than pattern
    if (n < m + startIndex) {
        return matches;
    }

    // Calculate h = d^(m-1) % q
    for (int i = 0; i < m - 1; ++i) {
        h = (h * d) % q;
    }

    // Calculate initial hashes
    for (int i = 0; i < m; ++i) {
        pHash = (d * pHash + pattern[i]) % q;
        tHash = (d * tHash + text[startIndex + i]) % q;
    }

    // Slide the pattern over the text
    for (int i = startIndex; i <= n - m; ++i) {
        lastCheckedIndex = i;
        // Check if hashes match
        if (pHash == tHash) {
            // Hashes match, now verify characters (this is the O(m) check)
            int j;
            for (j = 0; j < m; ++j) {
                if (text[i + j] != pattern[j]) {
                    break;
                }
            }

            if (j == m) {
                matches.push_back(i);
            }
            else {
                // Spurious Hit!
                spuriousHits++;
                if (spuriousHits > SPURIOUS_HIT_LIMIT) {
                    switchRequired = true;
                    return matches; // Give up and signal a switch
                }
            }
        }

        // Calculate hash for the next window
        if (i < n - m) {
            tHash = (d * (tHash - text[i] * h) + text[i + m]) % q;
            // We might get a negative tHash, convert it to positive
            if (tHash < 0) {
                tHash = (tHash + q);
            }
        }
    }
    return matches;
}


// --- 4. The Adaptive Algorithmic Framework ---

enum class Algorithm { NAIVE, KMP, RABIN_KARP };

Algorithm chooseAlgorithm(int m, int k) {
    // *** CHANGE 3: Naive threshold changed to m < 6 ***
    if (m < 6) {
        cout << "Decision: Tiny pattern (m=" << m << "). Using Naive." << endl;
        return Algorithm::NAIVE;
    }
    if (k <= 4) {
        cout << "Decision: Small alphabet (k=" << k << "). Using KMP." << endl;
        return Algorithm::KMP;
    }
    if (k >= 64) {
        cout << "Decision: Large alphabet (k=" << k << "). Trying Rabin-Karp." << endl;
        return Algorithm::RABIN_KARP;
    }
    cout << "Decision: Default case (m=" << m << ", k=" << k << "). Using KMP for safety." << endl;
    return Algorithm::KMP;
}

/**
 * Deliverable 1: The main adaptive strategy function.
 */
vector<int> adaptiveStringSearch(const string& text, const string& pattern, int alphabetSize) {
    cout << "--- Starting Adaptive Search ---" << endl;
    int m = pattern.length();
    // Handle empty pattern case
    if (m == 0) {
        cout << "--- Adaptive Search Complete ---" << endl;
        return {};
    }
    Algorithm choice = chooseAlgorithm(m, alphabetSize);

    if (choice == Algorithm::NAIVE) {
        return naiveSearch(text, pattern);
    }

    if (choice == Algorithm::KMP) {
        return kmpSearch(text, pattern);
    }

    if (choice == Algorithm::RABIN_KARP) {
        bool switchRequired = false;
        int lastCheckedIndex = 0;

        // 1. Try Rabin-Karp
        vector<int> matches = rabinKarpSearch(text, pattern, 0, lastCheckedIndex, switchRequired);

        // 2. Check for Task 3: Adaptive Switch
        if (switchRequired) {
            cout << "!!! Rabin-Karp hit threshold at index " << lastCheckedIndex << ". Switching to KMP. !!!" << endl;

            // 3. Run KMP on the *remainder* of the text
            // We start KMP just *after* where RK left off to avoid redundant checks
            vector<int> kmp_matches = kmpSearch(text, pattern, lastCheckedIndex + 1);

            // 4. Combine results
            matches.insert(matches.end(), kmp_matches.begin(), kmp_matches.end());
        }

        cout << "--- Adaptive Search Complete ---" << endl;
        return matches;
    }
    return {}; // Should not happen
}



vector<int> searchMultiplePatterns(const string& text, const vector<string>& patterns) {
    cout << "\n--- Task 2: Multi-Pattern Search (R-K method) ---" << endl;
    // This implementation assumes all patterns have the *same length* m
    // (A full Aho-Corasick would handle variable lengths)

    if (patterns.empty()) return {};
    int m = patterns[0].length();
    if (m == 0) return {};

    // Handle edge case where text is shorter than pattern
    if (text.length() < m) {
        return {};
    }

    // 1. Preprocessing: Hash all patterns and store in a set
    unordered_set<long long> patternHashes;
    for (const string& pattern : patterns) {
        long long pHash = 0;
        for (int i = 0; i < m; ++i) {
            pHash = (d * pHash + pattern[i]) % q;
        }
        patternHashes.insert(pHash);
    }
    cout << "Hashed " << patterns.size() << " patterns into a set." << endl;

    // 2. Search: Single pass over the text
    vector<int> matches;
    int n = text.length();
    long long tHash = 0;
    long long h = 1;
    for (int i = 0; i < m - 1; ++i) h = (h * d) % q;
    for (int i = 0; i < m; ++i) tHash = (d * tHash + text[i]) % q;

    for (int i = 0; i <= n - m; ++i) {
        // Check if text hash is in the set of pattern hashes
        if (patternHashes.count(tHash)) {
   
            matches.push_back(i);
        }

        // Roll the hash
        if (i < n - m) {
            tHash = (d * (tHash - text[i] * h) + text[i + m]) % q;
            if (tHash < 0) tHash = (tHash + q);
        }
    }
    return matches;
}


// *** CHANGE 1: Main function replaced with user-input logic ***
int main() {
    string text;
    int k;
    int choice;

    cout << "Enter the text to search in: ";
    getline(cin, text); // Use getline for text with spaces

    cout << "Enter the approximate alphabet size (k): ";
    cin >> k;
    // Clear the input buffer after reading an integer
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "\nSelect search type:" << endl;
    cout << "  1. Single Pattern (Adaptive Search)" << endl;
    cout << "  2. Multiple Patterns (Rabin-Karp Set)" << endl;
    cout << "Enter choice (1 or 2): ";
    cin >> choice;
    // Clear the input buffer again
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (choice == 1) {
        string pattern;
        cout << "Enter the pattern to search for: ";
        getline(cin, pattern); // Use getline for pattern

        vector<int> matches = adaptiveStringSearch(text, pattern, k);

        cout << "\n--- Single Pattern Results ---" << endl;
        if (matches.empty()) {
            cout << "No matches found." << endl;
        }
        else {
            cout << "Pattern found at indices: ";
            for (int index : matches) {
                cout << index << " ";
            }
            cout << endl;
        }
    }
    else if (choice == 2) {
        int numPatterns;
        cout << "How many patterns do you want to search for? ";
        cin >> numPatterns;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear buffer

        vector<string> patterns;
        int m = -1; // To store length of first pattern

        for (int i = 0; i < numPatterns; ++i) {
            string p;
            cout << "Enter pattern " << (i + 1) << ": ";
            getline(cin, p);

            if (i == 0) {
                m = p.length(); // Set the required length
                if (m == 0) {
                    cout << "Error: Empty patterns are not allowed." << endl;
                    return 1; // Exit
                }
            }

            if (p.length() != m) {
                cout << "\n*** Error ***" << endl;
                cout << "The provided 'searchMultiplePatterns' function requires all patterns to be the same length." << endl;
                cout << "Expected length " << m << ", but got " << p.length() << "." << endl;
                cout << "Aborting." << endl;
                return 1; // Exit with an error
            }
            patterns.push_back(p);
        }

        // Check if patterns were actually added (e.g., numPatterns > 0)
        if (!patterns.empty()) {
            vector<int> multiMatches = searchMultiplePatterns(text, patterns);

            cout << "\n--- Multi-Pattern Results ---" << endl;
            if (multiMatches.empty()) {
                cout << "No matches found for any pattern." << endl;
            }
            else {
                cout << "Matches found at indices: ";
                for (int index : multiMatches) {
                    cout << index << " ";
                }
                cout << endl;
                cout << "(Note: This shows the starting index of *any* matching pattern)." << endl;
            }
        }
        else {
            cout << "No patterns entered." << endl;
        }

    }
    else {
        cout << "Invalid choice. Exiting." << endl;
    }

    return 0;
}