#include "NodeGraphDemoWorkspace.h"

#include "NodeGraphViewportTool.h"

namespace AmaterasuDemo
{
    NodeGraphDemoWorkspace::NodeGraphDemoWorkspace()
        : Workspace("NodeGraphDemoWorkspace", "Node Graph Demo Workspace")
    {
        m_Tools.push_back(&m_NodeGraphViewportTool);
    }
}