#include <cstddef>
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <vector>

struct Change {
    void Apply() {}
    void Undo() {}
};

struct Node;

using NodePtr = std::shared_ptr<Node>;

struct Node {
    std::stack<Change> changes;
    std::vector<NodePtr> children;
};

class ChangeTreeBuilder {
public:
    NodePtr BuildTree(size_t depth, size_t beam_size) {
        if (depth == 0 || beam_size == 0) {
            return nullptr;
        }

        NodePtr root(std::make_shared<Node>());
        std::stack<std::pair<NodePtr, size_t>> traversal_stack;
        traversal_stack.push({root, 0});

        while (!traversal_stack.empty()) {
            auto& [current_node, next_child_id] = traversal_stack.top();
            if (next_child_id == 0) {
                std::cout << "Applying changes\n";
            }

            size_t current_depth = traversal_stack.size();

            if (current_depth >= depth || next_child_id >= beam_size) {
                std::cout << "Undoing changes\n";
                traversal_stack.pop();
                continue;
            }

            NodePtr child = std::make_shared<Node>();
            current_node->children.push_back(child);
            ++next_child_id;

            // Переходим к ребенку
            traversal_stack.push({child, 0});
        }

        return root;
    }
};

void PrintTree(NodePtr node, int depth = 0) {
    if (!node) {
        return;
    }

    std::string indent(depth * 2, ' ');
    std::cout << indent << "Node " << node
              << ", children: " << node->children.size() << ")\n";

    for (auto child : node->children) {
        PrintTree(child, depth + 1);
    }
}

int main() {
    std::cout << "=== Tree Change Management Demo ===\n" << std::endl;
    ChangeTreeBuilder builder;
    auto tree = builder.BuildTree(5, 4);
    PrintTree(tree);
    return 0;
}