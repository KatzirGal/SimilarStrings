# SimilarStrings

SimilarStrings is an application that provides a simple API to query similar words and view application statistics. Follow the instructions below to clone the repository, build the server, and run the application.

## PREREQUISITES
Before you begin, ensure your system has the following installed:
- libssl-dev
- cmake
- g++ (13 or higher)
- Python3

## CLONING THE REPOSITORY
Clone the repository from GitHub by running:

```bash
git clone https://github.com/KatzirGal/SimilarStrings.git
```

## BUILDING THE APPLICATION
The repository includes a Python script (`build.py`) that automates the build process. This script will:
- Clone the POCO library as part of the build process (this may take a few minutes).
- Run CMake to configure the project.
- Compile the server.

To build the server, navigate to the repository's root directory and execute:

```bash
python3 build.py
```

## RUNNING THE SERVER
After the build completes, start the server by executing the compiled binary with the provided data file:

```bash
./SimilarStrings words_clean.txt
```

## ACCESSING THE API ENDPOINTS
Once the server is running, open your web browser and use one of the following URLs to interact with the API:

### Similar Words Endpoints
- [http://localhost:8000/api/v1/similar?word=aerssw](http://localhost:8000/api/v1/similar?word=aerssw)
- [http://localhost:8000/api/v1/similar?word=apple](http://localhost:8000/api/v1/similar?word=apple)
- [http://localhost:8000/api/v1/similar?word=erssw](http://localhost:8000/api/v1/similar?word=erssw)

### Statistics Endpoint
- [http://localhost:8000/api/v1/stats](http://localhost:8000/api/v1/stats)

## SUMMARY OF STEPS
1. Clone the repository:
   ```bash
   git clone https://github.com/KatzirGal/SimilarStrings.git
   ```
2. Ensure the prerequisites (`libssl-dev`, `g++`, `Python3`) are installed.
3. Build the server by running:
   ```bash
   python3 build.py
   ```
   (This process may take a few minutes as it will clone the POCO library.)
4. Run the server:
   ```bash
   ./SimilarStrings words_clean.txt
   ```
5. Access the API endpoints using your web browser.

## PERMUTATION TRIE ALGORITHM

### Problem Statement
Given a dictionary of words and an input word (not necessarily present in the dictionary), return all dictionary words that are permutations of the input word.

### Key Concept
The algorithm uses a specialized Trie structure, called **PermutatedTrie**, designed to store words in a normalized format. Each word is sorted alphabetically before insertion, ensuring that all permutations of the same word are stored under the same Trie node.

### Algorithm Overview

#### Building the Trie
- For each word in the dictionary:
  - Sort the characters of the word alphabetically (e.g., *"cat"* → *"act"*, *"tac"* → *"act"*).
  - Insert this sorted word into the Trie.
    - Traverse the Trie character-by-character.
    - If a node for a character doesn’t exist, create it.
    - At the end node, store the original dictionary word in a list.

#### Querying the Trie
- Given an input word, sort its characters alphabetically.
- Traverse the Trie using this sorted sequence.
- When reaching the final node, return the list of all words stored at that node.
  - This list contains all dictionary words that are permutations of the input word.

### Data Structures Used
- **Trie Node:**
  - A map/dictionary of child nodes (character → node)
  - A list to store words at the terminal node

### Pseudocode

#### Insertion
```pseudo
function insert(word):
    sortedWord = sortCharacters(word)
    node = root
    for char in sortedWord:
        if char not in node.children:
            node.children[char] = new TrieNode()
        node = node.children[char]
    node.words.append(word)
```

#### Query
```pseudo
function getPermutations(word):
    sortedWord = sortCharacters(word)
    node = root
    for char in sortedWord:
        if char not in node.children:
            return []
        node = node.children[char]
    return node.words
```

### Example
**Dictionary:** `["cat", "tac", "act", "dog", "god"]`

**Trie Structure:**  
- Root  
  - 'a' → 'c' → 't' → ["cat", "tac", "act"]  
  - 'd' → 'g' → 'o' → ["dog", "god"]  

**Query:**  
Input: `"cta"`  
- Sort: `"act"`
- Traverse: Root → 'a' → 'c' → 't'
- Result: `["cat", "tac", "act"]`

## LICENSE
This project is licensed under the MIT License.

