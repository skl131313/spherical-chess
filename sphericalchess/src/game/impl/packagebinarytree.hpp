
#pragma once


#include "../../core.hpp"

#include <vector>


namespace Impl
{

class PackageBinaryTree
{
public:

    static constexpr int None = -1;

    struct Node
    {
        Rect2i rect;
        int id = None;

        int leftNodeIndex  = None;
        int rightNodeIndex = None;

        bool IsLeaf() const
        {
            return leftNodeIndex == rightNodeIndex;
        }
    };


    explicit PackageBinaryTree(Vec2i maxSize)
    {
        Node node;
        node.rect = Rect2i{ 0, 0, maxSize.x, maxSize.y };
        nodes.push_back(node);
    }


    int Insert(Vec2i size, int id)
    {
        if(size.x * size.y == 0)
        {
            return None;
        }

        int index = Insert(0, size, id, false);

        if(index != None)
        {
            return index;
        }

        return Insert(0, size, id, true);
    }

    Vec2i GetMinSize() const
    {
        return minSize;
    }

    const std::vector<Node>& GetNodes() const
    {
        return nodes;
    }

private:


    std::vector<Node> nodes;
    Vec2i             minSize = { 0, 0 };

    int Insert(int nodeIndex, Vec2i size, int id, bool canExpand);
    int AddNode(Rect2i rect);

};

}