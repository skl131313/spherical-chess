
#include "packagebinarytree.hpp"

#include <algorithm>

namespace Impl
{


int PackageBinaryTree::Insert(int nodeIndex, Vec2i size, int id, bool canExpand)
{
    if(nodeIndex == None)
    {
        return None;
    }

    if(nodes[nodeIndex].IsLeaf())
    {
        // avoid making a copy of this node in upper scope which could live through
        // to the "else" branch that makes the recursive calls.
        const Node node = nodes[nodeIndex]; 

        if(node.id != None || size.x > node.rect.width || size.y > node.rect.height)
        {
            return None;
        }

        if(node.rect.width == size.x && node.rect.height == size.y)
        {
            nodes[nodeIndex].id = id;
            return nodeIndex;
        }

        if(!canExpand)
        {
            if(node.rect.x + size.x > minSize.x || node.rect.y + size.y > minSize.y)
            {
                return None;
            }
        }

        // split this subsection of package into two such that the larger axis stays whole.

        int leftNodeIndex  = None;
        int rightNodeIndex = None;

        if(node.rect.width - size.x > node.rect.height - size.y)
        {
            leftNodeIndex  = AddNode(Rect2i{ node.rect.x, node.rect.y + size.y, size.x, node.rect.height - size.y });
            rightNodeIndex = AddNode(Rect2i{ node.rect.x + size.x, node.rect.y, node.rect.width - size.x, node.rect.height });
        }
        else
        {
            leftNodeIndex  = AddNode(Rect2i{ node.rect.x, node.rect.y + size.y, node.rect.width, node.rect.height - size.y });
            rightNodeIndex = AddNode(Rect2i{ node.rect.x + size.x, node.rect.y, node.rect.width - size.x, size.y });
        }

        // note: vector could be reallocated above due to AddNode()
        //       as such we only take a reference to our node here now.

        Node& n = nodes[nodeIndex];

        n.rect.width  = size.x;
        n.rect.height = size.y;
        n.id = id;

        n.leftNodeIndex  = leftNodeIndex;
        n.rightNodeIndex = rightNodeIndex;

        minSize.x = std::max(minSize.x, n.rect.Right());
        minSize.y = std::max(minSize.y, n.rect.Bot());

        return nodeIndex;
    }
    else
    {
        int first  = nodes[nodeIndex].leftNodeIndex;
        int second = nodes[nodeIndex].rightNodeIndex;

        // try to insert into the direction that has the smaller
        // size first to try and keep the "package" square.

        if(minSize.x < minSize.y)
        {
            std::swap(first, second);
        }

        int index = Insert(first, size, id, canExpand);

        if(index != None)
        {
            return index;
        }

        return Insert(second, size, id, canExpand);
    }

}

int PackageBinaryTree::AddNode(Rect2i rect)
{
    if(rect.width > 0 && rect.height > 0)
    {
        nodes.emplace_back();
        nodes.back().rect = rect;

        return int(nodes.size()) - 1;
    }

    return None;
}


}
