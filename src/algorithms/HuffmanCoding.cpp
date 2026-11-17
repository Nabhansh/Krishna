#include "algorithms/greedy/HuffmanCoding.hpp"

#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace project31::algorithms {

bool HuffmanCodec::before(const Node* left, const Node* right) {
    if (left->frequency != right->frequency) {
        return left->frequency < right->frequency;
    }
    return left->minimumSymbol < right->minimumSymbol;
}

void HuffmanCodec::heapPush(
    std::vector<std::unique_ptr<Node>>& heap,
    std::unique_ptr<Node> node
) {
    heap.push_back(std::move(node));
    std::size_t child = heap.size() - 1;
    while (child > 0) {
        const std::size_t parent = (child - 1) / 2;
        if (!before(heap[child].get(), heap[parent].get())) {
            break;
        }
        std::swap(heap[child], heap[parent]);
        child = parent;
    }
}

std::unique_ptr<HuffmanCodec::Node> HuffmanCodec::heapPop(
    std::vector<std::unique_ptr<Node>>& heap
) {
    std::unique_ptr<Node> result = std::move(heap.front());
    if (heap.size() > 1) {
        heap.front() = std::move(heap.back());
    }
    heap.pop_back();
    std::size_t parent = 0;
    while (parent < heap.size()) {
        const std::size_t left = parent * 2 + 1;
        if (left >= heap.size()) {
            break;
        }
        const std::size_t right = left + 1;
        std::size_t child = left;
        if (right < heap.size() && before(heap[right].get(), heap[left].get())) {
            child = right;
        }
        if (!before(heap[child].get(), heap[parent].get())) {
            break;
        }
        std::swap(heap[parent], heap[child]);
        parent = child;
    }
    return result;
}

void HuffmanCodec::assignCodes(
    const Node* node,
    std::string& path,
    std::map<char, std::string>& codes
) {
    if (node->isLeaf) {
        codes.emplace(node->symbol, path.empty() ? "0" : path);
        return;
    }
    path.push_back('0');
    assignCodes(node->left.get(), path, codes);
    path.back() = '1';
    assignCodes(node->right.get(), path, codes);
    path.pop_back();
}

HuffmanCodec HuffmanCodec::fromFrequencies(
    const std::unordered_map<char, std::uint64_t>& frequencies
) {
    HuffmanCodec codec;
    std::vector<std::unique_ptr<Node>> heap;
    heap.reserve(frequencies.size());
    for (const auto& [symbol, frequency] : frequencies) {
        if (frequency == 0) {
            throw std::invalid_argument("Huffman frequencies must be positive.");
        }
        auto node = std::make_unique<Node>(Node{
            frequency,
            symbol,
            static_cast<unsigned char>(symbol),
            true,
            nullptr,
            nullptr
        });
        heapPush(heap, std::move(node));
    }

    if (heap.empty()) {
        return codec;
    }
    while (heap.size() > 1) {
        std::unique_ptr<Node> left = heapPop(heap);
        std::unique_ptr<Node> right = heapPop(heap);
        if (left->frequency >
            std::numeric_limits<std::uint64_t>::max() - right->frequency) {
            throw std::overflow_error("Combined Huffman frequency exceeds uint64 range.");
        }
        auto parent = std::make_unique<Node>(Node{
            left->frequency + right->frequency,
            '\0',
            left->minimumSymbol < right->minimumSymbol
                ? left->minimumSymbol : right->minimumSymbol,
            false,
            std::move(left),
            std::move(right)
        });
        heapPush(heap, std::move(parent));
    }

    codec.root_ = heapPop(heap);
    std::string path;
    assignCodes(codec.root_.get(), path, codec.codes_);
    return codec;
}

HuffmanCodec HuffmanCodec::fromText(const std::string& text) {
    std::unordered_map<char, std::uint64_t> frequencies;
    for (char symbol : text) {
        std::uint64_t& count = frequencies[symbol];
        if (count == std::numeric_limits<std::uint64_t>::max()) {
            throw std::overflow_error("Text frequency exceeds uint64 range.");
        }
        ++count;
    }
    return fromFrequencies(frequencies);
}

const std::map<char, std::string>& HuffmanCodec::codes() const noexcept {
    return codes_;
}

std::string HuffmanCodec::encode(const std::string& text) const {
    std::string bits;
    for (char symbol : text) {
        const auto found = codes_.find(symbol);
        if (found == codes_.end()) {
            throw std::invalid_argument("Text contains a symbol absent from the Huffman tree.");
        }
        bits += found->second;
    }
    return bits;
}

std::string HuffmanCodec::decode(const std::string& bits) const {
    if (bits.empty()) {
        return {};
    }
    if (!root_) {
        throw std::invalid_argument("Cannot decode nonempty bits with an empty Huffman tree.");
    }
    if (root_->isLeaf) {
        for (char bit : bits) {
            if (bit != '0') {
                throw std::invalid_argument("Single-symbol Huffman codes contain only zero bits.");
            }
        }
        return std::string(bits.size(), root_->symbol);
    }
    std::string text;
    const Node* current = root_.get();
    for (char bit : bits) {
        if (bit != '0' && bit != '1') {
            throw std::invalid_argument("Huffman input must contain only zero and one bits.");
        }
        current = bit == '0' ? current->left.get() : current->right.get();
        if (current == nullptr) {
            throw std::invalid_argument("Bit sequence does not follow a Huffman code.");
        }
        if (current->isLeaf) {
            text.push_back(current->symbol);
            current = root_.get();
        }
    }
    if (current != root_.get()) {
        throw std::invalid_argument("Bit sequence ends in the middle of a Huffman code.");
    }
    return text;
}

} // namespace project31::algorithms
