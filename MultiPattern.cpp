#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm> // for std::transform
#include <map>         // To store matches
#include <cctype>      // for tolower

using namespace std;

const int K = 26;

struct Vertex {
    int next[K];
    bool output = false;
    int p = -1;
    char pch;
    int link = -1;
    int go[K];

    vector<int> pattern_indices;

    Vertex(int p = -1, char ch = '$') : p(p), pch(ch) {
        fill(begin(next), end(next), -1);
        fill(begin(go), end(go), -1);
    }
};

vector<Vertex> t(1); // trie
vector<string> patterns; // Store patterns for output

string to_lower(string s) {//normalize to lower case
    transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return static_cast<char>(tolower(c)); }
    );
    return s;
}

void add_string(string s, int pattern_index) {
    int v = 0;
    for (char ch : s) {
        int c = ch - 'a';
        if (c < 0 || c >= K) continue; // Ignore non-alphabet chars

        if (t[v].next[c] == -1) {
            t[v].next[c] = t.size();
            t.emplace_back(v, ch);
        }
        v = t[v].next[c];
    }
    t[v].output = true;
    t[v].pattern_indices.push_back(pattern_index); // Store which pattern ends here
}

int go(int v, char ch);

int get_link(int v) {
    if (t[v].link == -1) {
        if (v == 0 || t[v].p == 0)
            t[v].link = 0;
        else
            t[v].link = go(get_link(t[v].p), t[v].pch);
    }
    return t[v].link;
}

int go(int v, char ch) {
    int c = ch - 'a';
    if (c < 0 || c >= K) return 0; // Non-alphabet char goes to root

    if (t[v].go[c] == -1) {
        if (t[v].next[c] != -1)
            t[v].go[c] = t[v].next[c];
        else
            t[v].go[c] = v == 0 ? 0 : go(get_link(v), ch);
    }
    return t[v].go[c];
}


map<string, vector<int>> search(string text) {
    map<string, vector<int>> matches;
    int v = 0; // Start at the root

    for (int i = 0; i < static_cast<int>(text.length()); ++i) {
        char ch = text[i];
        v = go(v, ch); // Transition in the automaton

        // Check for outputs at this state
     
        int current_v = v;
        while (current_v != 0) {
            if (t[current_v].output) {
                for (int p_idx : t[current_v].pattern_indices) {
                    string pattern = patterns[p_idx];
                    // Add match, (i - pattern.length() + 1) is the start index
                    matches[pattern].push_back(i - static_cast<int>(pattern.length()) + 1);
                }
            }
            // Follow the failure link to check for suffixes
            current_v = get_link(current_v);
        }
    }
    return matches;
}

int main() {
    int n;
    cout << "Enter number of patterns: ";
    cin >> n;
    cin.ignore(); // consume newline

    cout << "Enter " << n << " patterns (one per line):" << endl;
    for (int i = 0; i < n; ++i) {
        string p;
        getline(cin, p);
        string normalized_p = to_lower(p);
        patterns.push_back(normalized_p); // Store the normalized pattern
        add_string(normalized_p, i);
    }


    queue<int> q;
    q.push(0);
    while (!q.empty()) {
        int v = q.front();
        q.pop();
        get_link(v); // Force calculation of link
        for (int i = 0; i < K; ++i)
            go(v, (char)('a' + i)); // Force calculation of go

        for (int i = 0; i < K; ++i) {
            if (t[v].next[i] != -1)
                q.push(t[v].next[i]);
        }
    }
   

    cout << "\nEnter text to search: ";
    string text;
    getline(cin, text);
    string normalized_text = to_lower(text);

    map<string, vector<int>> all_matches = search(normalized_text);

    cout << "\n--- Matches Found (Case-Insensitive) ---" << endl;
    if (all_matches.empty()) {
        cout << "No matches found." << endl;
    }
    for (const auto& kv : all_matches) {
        const string& pattern = kv.first;
        const vector<int>& indices = kv.second;
        cout << "Pattern \"" << pattern << "\" found at indices: ";
        for (int idx : indices) {
            cout << idx << " ";
        }
        cout << endl;
    }

    return 0;
}