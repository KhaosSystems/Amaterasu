#pragma once

#include "Amaterasu/GUIFramework/Workspace.h"

#include "NodeGraph/NodeGraph.h"

namespace AmaterasuDemo
{
    class NodeGraphDemoWorkspace : public Amaterasu::Workspace<NodeGraphDemoWorkspace>
    {
    public:
        NodeGraphDemoWorkspace();

        virtual void Render() override;

    public:
        NodeGraph MyNodeGraph;
    };
}