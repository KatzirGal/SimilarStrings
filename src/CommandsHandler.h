#pragma once

#include "PermutationTrie.h"

#include <Poco/JSON/Object.h>

class CommandsHandler
{
public:
    CommandsHandler();
    int InitPermutationTrie(std::string filepath);

public:
    bool OnSimilar(const std::string& word, std::vector<std::string>& similarWords);
    bool OnStats(Poco::JSON::Object& responseObj);

private:
    PermutationTrie m_permutationTrie;
};