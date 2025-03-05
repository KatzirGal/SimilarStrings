#pragma once

#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

struct TrieNode {
public:
    std::unordered_map<char, std::shared_ptr<TrieNode>> children;

    std::vector<std::string> permutatedWords;

    TrieNode() {}
};

class PermutationTrie {
private:
    std::shared_ptr<TrieNode> m_root;
    size_t m_totalWords;
    size_t m_maxWord;

public:
    PermutationTrie();

    void Insert(const std::string& word);
    void GetPermutatedWords(const std::string& word, std::vector<std::string>& permutatedWords) const;
    int GetTotalWords() const;
};
