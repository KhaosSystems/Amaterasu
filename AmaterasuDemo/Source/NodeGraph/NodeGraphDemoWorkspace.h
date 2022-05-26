#pragma once

#include "GUIFramework/Workspace.h"

#include "NodeGraphViewportTool.h"

namespace AmaterasuDemo
{
    class NodeGraphDemoWorkspace : public Workspace
    {
    public:
        NodeGraphDemoWorkspace();

    private:
        NodeGraphViewportTool m_NodeGraphViewportTool;
    };
}