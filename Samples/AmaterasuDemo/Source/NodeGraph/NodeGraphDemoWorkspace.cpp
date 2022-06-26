#include "NodeGraphDemoWorkspace.h"

#include "NodeGraphDebugTool.h"
#include "NodeGraphViewportTool.h"

namespace AmaterasuDemo
{
    NodeGraphDemoWorkspace::NodeGraphDemoWorkspace()
        : Workspace<NodeGraphDemoWorkspace>("Node Graph Demo Workspace"), MyNodeGraph()
    {
        m_ActionSearchTool.AppendAction(Amaterasu::Action("Save", []() {}));
        m_ActionSearchTool.AppendAction(Amaterasu::Action("Save As...", []() {}));

        RegisterTool<NodeGraphDebugTool>();
        RegisterTool<NodeGraphViewportTool>();
 
        MyNodeGraph.Nodes.push_back(new AddNode());
        MyNodeGraph.Nodes.push_back(new AddNode());
    }

    void NodeGraphDemoWorkspace::Render()
    {
        Workspace::Render();
    }

}