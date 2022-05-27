#pragma once

#include "GUIFramework/Tool.h"

#include "NodeGraphDemoWorkspace.h"

namespace AmaterasuDemo
{
    class NodeGraphViewportTool : public Tool<NodeGraphDemoWorkspace, NodeGraphViewportTool>
    {
    public:
        NodeGraphViewportTool();
    };
}