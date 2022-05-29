#pragma once

#include "GUIFramework/Workspace.h"

#include "NodeGraph/NodeGraph.h"

namespace AmaterasuDemo
{
    class NodeGraphDemoWorkspace : public Workspace<NodeGraphDemoWorkspace>
    {
    public:
        NodeGraphDemoWorkspace();

    public:
        NodeGraph MyNodeGraph;
    };
}