#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace project31::algorithms {

class HuffmanCodec {
public:
    [[nodiscard]] static HuffmanCodec fromFrequencies(
        const std::unordered_map<char, std::uint64_t>& frequencies
    );
    [[nodiscard]] static HuffmanCodec fromText(const std::string& text);

    [[nodiscard]] const std::map<char, std::string>& codes() const noexcept;
    [[nodiscard]] std::string encode(const std::string& text) const;
    [[nodiscard]] std::string decode(const std::string& bits) const;

private:
    struct Node {
        std::uint64_t frequency;
        char symbol;
        unsigned char minimumSymbol;
        bool isLeaf;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
    };

    [[nodiscard]] static bool before(const Node* left, const Node* right);
    static void heapPush(std::vector<std::unique_ptr<Node>>& heap, std::unique_ptr<Node> node);
    [[nodiscard]] static std::unique_ptr<Node> heapPop(
        std::vector<std::unique_ptr<Node>>& heap
    );
    static void assignCodes(
        const Node* node,
        std::string& path,
        std::map<char, std::string>& codes
    );

    std::unique_ptr<Node> root_;
    std::map<char, std::string> codes_;
};

} // namespace project31::algorithms
