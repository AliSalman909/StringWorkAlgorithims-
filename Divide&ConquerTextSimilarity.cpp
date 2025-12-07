#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map> 
#include <cmath>
#include <algorithm>

using namespace std;

class RabinKarpChecker {
private:
    long long d = 256;      // Base 
    long long q = 1000000007; // A large prime modulus (restored from 13)
    string A, B;

public:
    RabinKarpChecker(const string& docA, const string& docB) : A(docA), B(docB) {}

    // The Check(K) subproblem
    bool check(int K) {
        if (K == 0)
            return true;
        // Check for bounds early
        if (K > A.length() || K > B.length())
            return false;

        // 1. Hashing Document A
        unordered_map<long long, vector<int>> hashesA;

        long long h = 1; // d^(K-1) % q
        for (int i = 0; i < K - 1; ++i) {
            h = (h * d) % q;
        }

        long long currentHashA = 0;
        for (int i = 0; i < K; ++i) {
            currentHashA = (d * currentHashA + A[i]) % q;
        }
        
        hashesA[currentHashA].push_back(0);


        // Compute rolling hashes for the rest of A
        for (int i = 0; i <= (int)A.length() - K - 1; ++i) {
            currentHashA = (d * (currentHashA - A[i] * h) + A[i + K]) % q;
            if (currentHashA < 0) currentHashA += q;

            // Store the hash at index i+1
            hashesA[currentHashA].push_back(i + 1);
        }

        // 2. Checking Document B
        long long currentHashB = 0;
        for (int i = 0; i < K; ++i) {
            currentHashB = (d * currentHashB + B[i]) % q;
        }
        if (hashesA.count(currentHashB)) {
            string subB = B.substr(0, K); // Get B's substring once
            for (int indexA : hashesA[currentHashB]) { // Iterate all A's indices with this hash
                if (A.substr(indexA, K) == subB) {
                    return true; // Found a true match
                }
            }
        }

        // Check rolling hashes for the rest of B
        for (int i = 0; i <= (int)B.length() - K - 1; ++i) {
            currentHashB = (d * (currentHashB - B[i] * h) + B[i + K]) % q;
            if (currentHashB < 0) currentHashB += q;

            if (hashesA.count(currentHashB)) {
                string subB = B.substr(i + 1, K); // Get B's substring
                for (int indexA : hashesA[currentHashB]) { // Iterate all A's indices
                    if (A.substr(indexA, K) == subB) {
                        return true; // Found a true match
                    }
                }
            }
        }

        // 4. No match
        return false;
    }
};


int findLongestMatch(const string& docA, const string& docB) {
    int n = docA.length();
    int m = docB.length();
    int low = 0;
    int high = min(n, m);
    int maxLen = 0;

    RabinKarpChecker checker(docA, docB);

    cout << "Starting D&C Binary Search on length..." << endl;

    while (low <= high) {
        int K = low + (high - low) / 2;
        cout << "  Checking K = " << K << "...";

        if (checker.check(K)) {
            cout << "  Result: Found." << endl;
            maxLen = K;       // This is a possible answer
            low = K + 1;      // Try to find a longer one 
        }
        else {
            cout << "  Result: Not Found." << endl;
            high = K - 1;     // This length is too long
        }
    }

    return maxLen;
}

int main() {
    string docA, docB;

    cout << "Enter Document A: ";
    getline(cin, docA); // Use getline to allow spaces

    cout << "Enter Document B: ";
    getline(cin, docB);

    cout << "\nDocument A: " << docA << endl;
    cout << "Document B: " << docB << endl;

    // --- Algorithm Execution ---
    int resultLength = findLongestMatch(docA, docB);

    cout << "\nAlgorithm complete." << endl;
    cout << "Maximum Common Substring Length: " << resultLength << endl;

    return 0;
}