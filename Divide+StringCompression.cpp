#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// --- Helper: Compare two suffixes explicitly ---
// Returns true if suffix at index i is lexicographically smaller than suffix at index j
bool isSmaller(const string& text, int i, int j) {
    int n = text.length();
    while (i < n && j < n) {
        if (text[i] < text[j]) return true;
        if (text[i] > text[j]) return false;
        i++;
        j++;
    }
    // If one runs out, the shorter one is "smaller"
    return i == n && j < n;
}

// --- PART 1: DIVIDE AND CONQUER (Merge Sort) ---

// The "Merge" step of Divide and Conquer
void merge(const string& text, vector<int>& suffixes, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    // Create temp arrays
    vector<int> L(n1), R(n2);

    for (int i = 0; i < n1; i++)
        L[i] = suffixes[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = suffixes[mid + 1 + j];

    // Merge the temp arrays back into suffixes[left..right]
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        // CRITICAL: String comparison happens here
        if (isSmaller(text, L[i], R[j])) {
            suffixes[k] = L[i];
            i++;
        }
        else {
            suffixes[k] = R[j];
            j++;
        }
        k++;
    }

    // Copy remaining elements
    while (i < n1) {
        suffixes[k] = L[i];
        i++;
        k++;
    }
    while (j < n2) {
        suffixes[k] = R[j];
        j++;
        k++;
    }
}

// The Recursive "Divide" step
void mergeSortSuffixes(const string& text, vector<int>& suffixes, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        // DIVIDE: Recursively sort first and second halves
        mergeSortSuffixes(text, suffixes, left, mid);
        mergeSortSuffixes(text, suffixes, mid + 1, right);

        // COMBINE: Merge the sorted halves
        merge(text, suffixes, left, mid, right);
    }
}

// Wrapper function to start the D&C process
vector<int> buildSuffixArrayDC(const string& text) {
    int n = text.length();
    vector<int> suffixes(n);
    // Initialize with indices 0 to n-1
    for (int i = 0; i < n; i++) suffixes[i] = i;

    // Start Divide and Conquer
    mergeSortSuffixes(text, suffixes, 0, n - 1);
    return suffixes;
}

// --- PART 2: FIND REPETITIONS (LCP Calculation) ---

// Calculates Longest Common Prefix between two specific suffixes
int computeLCP(const string& text, int i, int j) {
    int n = text.length();
    int len = 0;
    while (i + len < n && j + len < n && text[i + len] == text[j + len]) {
        len++;
    }
    return len;
}

// Builds the LCP array based on the sorted Suffix Array
vector<int> buildLCPArray(const string& text, const vector<int>& suffixArr) {
    int n = text.length();
    vector<int> lcp(n, 0);

    // Compare adjacent suffixes in the sorted list
    for (int i = 1; i < n; i++) {
        lcp[i] = computeLCP(text, suffixArr[i - 1], suffixArr[i]);
    }
    return lcp;
}

// --- PART 3: COMPRESSION OUTPUTS ---

string buildBWT(const string& text, const vector<int>& suffixArr) {
    int n = text.length();
    string bwt = "";
    for (int i = 0; i < n; i++) {
        if (suffixArr[i] == 0) bwt += text[n - 1];
        else bwt += text[suffixArr[i] - 1];
    }
    return bwt;
}

int main() {
    // 1. INPUT
    string text;
    cout << "--- Divide & Conquer String Processing ---" << endl;
    cout << "Enter the string to process: ";
    getline(cin, text); // Using getline to allow spaces in input

    // Automatically append sentinel if user didn't type it
    if (text.empty() || text.back() != '$') {
        text += "$";
        cout << "(Auto-appended '$' terminator for algorithm correctness)" << endl;
    }

    int n = text.length();
    cout << "Processing: " << text << endl;

    // 2. Build Suffix Array using Divide & Conquer
    vector<int> suffixArr = buildSuffixArrayDC(text);

    // 3. Build LCP Array
    vector<int> lcpArr = buildLCPArray(text, suffixArr);

    // 4. Output Analysis
    int maxLCP = 0;
    int indexMax = 0;

    cout << "\nIndex\tSA\tLCP\tSuffix" << endl;
    for (int i = 0; i < n; i++) {
        // Printing only first 20 chars of suffix to keep console clean if input is long
        string suffixDisplay = text.substr(suffixArr[i]);
        if (suffixDisplay.length() > 20) suffixDisplay = suffixDisplay.substr(0, 20) + "...";

        cout << i << "\t" << suffixArr[i] << "\t" << lcpArr[i] << "\t" << suffixDisplay << endl;

        if (lcpArr[i] > maxLCP) {
            maxLCP = lcpArr[i];
            indexMax = i;
        }
    }

    // 5. Task Findings
    cout << "\n--- Task 2: Longest Repeated Substring ---" << endl;
    if (maxLCP > 0) {
        string lrs = text.substr(suffixArr[indexMax], maxLCP);
        cout << "Longest Repeated Substring: \"" << lrs << "\" (Length: " << maxLCP << ")" << endl;
    }
    else {
        cout << "No repeated substrings found." << endl;
    }

    cout << "\n--- Task 3: Compression Representation ---" << endl;
    string bwt = buildBWT(text, suffixArr);
    cout << "Burrows-Wheeler Transform: " << bwt << endl;

    return 0;
}