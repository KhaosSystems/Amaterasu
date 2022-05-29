#include "NodeGraphDebugTool.h"

#include "NodeGraphDemoWorkspace.h"

#include <iostream>

namespace AmaterasuDemo
{
    NodeGraphDebugTool::NodeGraphDebugTool()
        : Tool<NodeGraphDemoWorkspace, NodeGraphDebugTool>("Node Graph Viewport Tool")
    {
    }
}