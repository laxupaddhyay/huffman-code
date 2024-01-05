#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <vector>

using namespace std;

// Node structure for Huffman Tree
struct HuffmanNode {
    char data;
    int frequency;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(char data, int frequency) : data(data), frequency(frequency), left(nullptr), right(nullptr) {}
};

// Comparison struct for priority queue
struct CompareNodes {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->frequency > b->frequency;
    }
};

// Traverse the Huffman Tree and generate codes
void generateCodes(HuffmanNode* root, string code, unordered_map<char, string>& codes) {
    if (root == nullptr) {
        return;
    }

    if (!root->left && !root->right) {
        codes[root->data] = code;
    }

    generateCodes(root->left, code + "0", codes);
    generateCodes(root->right, code + "1", codes);
}

// Build Huffman Tree and return the root
HuffmanNode* buildHuffmanTree(const unordered_map<char, int>& frequencies) {
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, CompareNodes> pq;

    for (const auto& entry : frequencies) {
        pq.push(new HuffmanNode(entry.first, entry.second));
    }

    while (pq.size() > 1) {
        HuffmanNode* left = pq.top(); pq.pop();
        HuffmanNode* right = pq.top(); pq.pop();

        HuffmanNode* merged = new HuffmanNode('\0', left->frequency + right->frequency);
        merged->left = left;
        merged->right = right;

        pq.push(merged);
    }

    return pq.top();
}

// Encode the input file using Huffman codes and write to output file
void compressFile(const string& inputFileName, const string& outputFileName) {
    ifstream inputFile(inputFileName, ios::binary);
    ofstream outputFile(outputFileName, ios::binary | ios::trunc);

    if (!inputFile.is_open() || !outputFile.is_open()) {
        cerr << "Error opening files." << endl;
        return;
    }

    unordered_map<char, int> frequencies;

    // Count frequencies of each character in the input file
    char ch;
    while (inputFile.get(ch)) {
        frequencies[ch]++;
    }

    // Build Huffman Tree
    HuffmanNode* root = buildHuffmanTree(frequencies);

    // Generate Huffman codes
    unordered_map<char, string> codes;
    generateCodes(root, "", codes);

    // Write Huffman codes to the output file
    for (const auto& entry : codes) {
        outputFile << entry.first << entry.second << ' ';
    }
    outputFile << '\n';

    // Reset file pointer to the beginning of the input file
    inputFile.clear();
    inputFile.seekg(0);

    // Encode the input file and write to the output file
    string encodedBits;
    while (inputFile.get(ch)) {
        encodedBits += codes[ch];
    }

    // Convert the string of bits to bytes and write to the output file
    for (size_t i = 0; i < encodedBits.length(); i += 8) {
        char byte = 0;
        for (size_t j = 0; j < 8 && i + j < encodedBits.length(); ++j) {
            if (encodedBits[i + j] == '1') {
                byte |= (1 << (7 - j));
            }
        }
        outputFile.put(byte);
    }

    inputFile.close();
    outputFile.close();
}

int main() {
    string inputFileName = "input.txt";
    string outputFileName = "compressed.bin";

    compressFile(inputFileName, outputFileName);

    cout << "File compressed successfully!" << endl;

    return 0;
}
