#pragma once

#include "GUIFramework/Workspace.h"

#include "NodeGraph/NodeGraph.h"

namespace AmaterasuDemo
{
    class NodeGraphDemoWorkspace : public Workspace<NodeGraphDemoWorkspace>
    {
    public:
        NodeGraphDemoWorkspace();

        virtual void Render() override;

    public:
        NodeGraph MyNodeGraph;
    };
}