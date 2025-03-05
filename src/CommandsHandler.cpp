#include "CommandsHandler.h"

#include <chrono>
#include <fstream>
#include <iostream>

#include <Poco/JSON/Object.h>

CommandsHandler::CommandsHandler()
{
}

int CommandsHandler::InitPermutationTrie(std::string filepath)
{
    std::ifstream file(filepath);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file!" << std::endl;
        return 1;
    }

    std::cout << "Reading words from file" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    std::string line;
    while (std::getline(file, line))
    {
        m_permutationTrie.Insert(line);
    }

    auto stop = std::chrono::high_resolution_clock::now();

    std::cout << "Finished to read words from file" << std::endl;

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Time taken: " << duration.count() / 1e6 << " seconds" << std::endl;


    file.close();

    return 0;
}

bool CommandsHandler::OnSimilar(const std::string& word, std::vector<std::string>& similarWords)
{
    m_permutationTrie.GetPermutatedWords(word, similarWords);

    auto iter = find(similarWords.begin(), similarWords.end(), word);
    if(iter != similarWords.end())
        similarWords.erase(iter);

    return true;
}

bool CommandsHandler::OnStats(Poco::JSON::Object& responseObj)
{
    responseObj.set("totalWords", m_permutationTrie.GetTotalWords());
    return true;
}
