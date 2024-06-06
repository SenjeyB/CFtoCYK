#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <unordered_set>
#include <vector>
#include <string>

using namespace std;

vector<vector<int>> rules[8000];
vector<bool> check(26, false );
int rSize = 7000;
int st, mc;
void convertToCNF() {
    int newSymbolCounter = 0;
    vector<pair<int, vector<int>>> inserts;
    int addr = 0;
    for (int i = 7000; i < rSize; i++) {
        vector<vector<int>> newProductions;
        for (const auto& production : rules[i]) {
            vector<int> newProduction = production;
            while (newProduction.size() > 2) {
                int newNonTerminal = mc + 7000 + newSymbolCounter++;
                vector<int> newRule = {newProduction[newProduction.size() - 2], newProduction.back()};
                newProduction.pop_back();
                newProduction.pop_back();
                newProduction.push_back(newNonTerminal);
                inserts.push_back({newNonTerminal, newRule});
                addr++;
            }
            newProductions.push_back(newProduction);
        }
        rules[i] = newProductions;
    }
    rSize += addr;
    addr = 0;
    for (const auto& insert : inserts) {
        rules[insert.first].push_back(insert.second);
    }
    inserts.clear();
    
    unordered_set<int> eps;
    for (int i = 7000; i < rSize; i++) {
        for (const auto& production : rules[i]) {
            if (production.size() == 1 && production[0] == '_') {
                eps.insert(i);
                break;
            }
        }
    }

    bool isChanged = true;
    while (isChanged) {
        isChanged = false;
        for (int i = 7000; i < rSize; i++) {
            for (const auto& production : rules[i]) {
                bool allInEps = true;
                for (const auto& с : production) {
                    if (eps.count(с) == 0) {
                        allInEps = false;
                        break;
                    }
                }
                if (allInEps) {
                    if (eps.insert(i).second) {
                        isChanged = true;
                    }
                }
            }
        }
    }

    for (const auto& ep : eps) {
        if (ep == st + 7000) continue;
        for (auto it = rules[ep].begin(); it != rules[ep].end(); ) {
            if (it->size() == 1 && (*it)[0] == '_') {
                it = rules[ep].erase(it);
            } else {
                ++it;
            }
        }
    }

    
    for (int i = 7000; i < rSize; i++) {
        for (auto& production : rules[i]) {
            for (int j = 0; j < production.size(); j++) {
                if (eps.count(production[j])) {
                    vector<int> newProduction = production;
                    newProduction.erase(newProduction.begin() + j);
                    inserts.push_back({i, newProduction});
                }
            }
        }
    }

    for (const auto& insert : inserts) {
        rules[insert.first].push_back(insert.second);
    }
    inserts.clear();
    
    vector<unordered_set<int>> chainRules(8000);
    for (int i = 7000; i < rSize; i++) {
        chainRules[i].insert(i);
    }

    bool added = true;
    while (added) {
        added = false;
        for (int i = 7000; i < rSize; i++) {
            for (const auto& production : rules[i]) {
                if (production.size() == 1 && production[0] >= 7000) {
                    auto& from = chainRules[i];
                    auto& to = chainRules[production[0]];
                    int initialSize = from.size();
                    from.insert(to.begin(), to.end());
                    if (from.size() > initialSize) {
                        added = true;
                    }
                }
            }
        }
    } 

    for (int i = 7000; i < rSize; i++) {
        for (const auto& nonTerminal : chainRules[i]) {
            if (nonTerminal != i) {
                for (const auto& production : rules[nonTerminal]) {
                    if (!(production.size() == 1 && production[0] >= 7000)) {
                        inserts.push_back({i, production});
                    }
                }
            }
        }
    }

    for (const auto& insert : inserts) {
        rules[insert.first].push_back(insert.second);
    }
    inserts.clear();
    
    for (int i = 7000; i < rSize; i++) {
        auto& productions = rules[i];
        for (auto it = productions.begin(); it != productions.end(); ) {
            if (it->size() == 1 && (*it)[0] >= 7000) {
                it = productions.erase(it);
            } else {
                ++it;
            }
        }
    }
        
    
    for (int i = 7000; i < rSize; i++) {
        vector<vector<int>> newProductions;
        for (const auto& production : rules[i]) {
            if (production.size() == 2) {
                vector<int> newProduction;
                for (const auto& ch : production) {
                    if (ch < 7000) {
                        int newNonTerminal = mc + 7000 + newSymbolCounter++;
                        inserts.push_back({newNonTerminal, {ch}});
                        newProduction.push_back(newNonTerminal);
                        addr++;
                    } else {
                        newProduction.push_back(ch);
                    }
                }
                newProductions.push_back(newProduction);
            } else {
                newProductions.push_back(production);
            }
        }
        rules[i] = newProductions;
    }
    for (const auto& insert : inserts) {
        rules[insert.first].push_back(insert.second);
    }
    rSize += addr;
}


//Пить сок, писать CYK
bool CYK(const string& word) {
    int n = word.size();
    bool dp[201][201][40] = {{{false}}};
    for (int i = 1; i <= n; i++) {
        for (int j = 7000; j < rSize; j++) {
            for (const auto& production : rules[j]) {
                if (production.size() == 1 && production[0] == word[i - 1]) {
                    dp[1][i][j - 7000] = true;
                }
            }
        }
    }

    for (int i = 2; i <= n; i++) {
        for (int j = 1; j <= n - i + 1; j++) {
            for (int k = 1; k <= i - 1; k++) {
                for (int w = 7000; w < rSize; w++) {
                    for (const auto& production : rules[w]) {
                        if (production.size() == 2) {
                            if (dp[k][j][production[0] - 7000] && dp[i - k][k + j][production[1] - 7000]) {
                                dp[i][j][w - 7000] = true;
                            }
                        }
                    }
                }
            }
        }
    }
    return dp[n][1][st];
}
char c;
string s;
vector<int> ind(26, 0);
vector<pair<char, string>> temp;
string word;
int main() {
    freopen("cyk.in", "r", stdin);
    freopen("cyk.out", "w", stdout);
    
    cin >> word;
    
    while (cin >> c >> s) {
        temp.push_back({c, s});
        check[c - 'A'] = true;
        for(char ch : s) {
            if(isupper(ch)) check[ch - 'A'] = true;
        } 
    }
    int id = 0;
    for(int i = 0; i < 26; i++) {
        if(check[i]) {
            ind[i] = id++;
        }
        if(i == 18) st = ind[i];
    }
    mc = id;
    rSize += mc;
    for(auto& rul : temp) {
        c = rul.first;
        s = rul.second;
        vector<int> rule;
        for (auto ch : s) {
            if(!isupper(ch))
                rule.push_back(ch);
            else
                rule.push_back(ind[ch - 'A'] + 7000);
        }
        rules[ind[c - 'A'] + 7000].push_back(rule);
        
    }
    
    
    convertToCNF();

    if (CYK(word)) {
        cout << "YES";
    } else {
        cout << "NO";
    }
}