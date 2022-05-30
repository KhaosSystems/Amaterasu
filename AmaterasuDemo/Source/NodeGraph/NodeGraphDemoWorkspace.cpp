#include "NodeGraphDemoWorkspace.h"

#include "NodeGraphDebugTool.h"
#include "NodeGraphViewportTool.h"

namespace AmaterasuDemo
{
    NodeGraphDemoWorkspace::NodeGraphDemoWorkspace()
        : Workspace<NodeGraphDemoWorkspace>("Node Graph Demo Workspace"), MyNodeGraph()
    {
        RegisterTool<NodeGraphDebugTool>();
        RegisterTool<NodeGraphViewportTool>();
 
        MyNodeGraph.Nodes.push_back(new AddNode());
        MyNodeGraph.Nodes.push_back(new AddNode());
    }
}