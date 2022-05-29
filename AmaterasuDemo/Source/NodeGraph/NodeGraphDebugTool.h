#pragma once

#include "GUIFramework/Tool.h"

#include "NodeGraphDemoWorkspace.h"

namespace AmaterasuDemo
{
    class NodeGraphDebugTool : public Tool<NodeGraphDemoWorkspace, NodeGraphDebugTool>
    {
    public:
        NodeGraphDebugTool();
    };
}