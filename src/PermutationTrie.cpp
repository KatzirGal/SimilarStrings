#include "PermutationTrie.h"

#include <algorithm>

PermutationTrie::PermutationTrie()
    : m_root(std::make_shared<TrieNode>()), m_totalWords(0), m_maxWord(0)
{

}


void PermutationTrie::Insert(const std::string& word)
{
    std::string sortedWord = word;
    std::sort(sortedWord.begin(), sortedWord.end());

    std::shared_ptr<TrieNode> node = m_root;
    for (char c : sortedWord) {
        if (!node->children.count(c))
            node->children[c] = std::make_shared<TrieNode>();
        node = node->children[c];
    }

    node->permutatedWords.push_back(word);

    ++m_totalWords;
    m_maxWord = std::max(m_maxWord, sortedWord.size());
}

void PermutationTrie::GetPermutatedWords(const std::string& word, std::vector<std::string>& permutatedWords) const
{
    if (word.size() > m_maxWord)
    {
        return;
    }

    std::string sortedWord = word;
    std::sort(sortedWord.begin(), sortedWord.end());

    std::shared_ptr<TrieNode> node = m_root;
    for (char c : sortedWord) {
        if (!node->children.count(c)) return;
        node = node->children[c];
    }

    permutatedWords = node->permutatedWords;
}

int PermutationTrie::GetTotalWords() const
{
    return m_totalWords;
}
