#pragma once

#include "GUIFramework/Tool.h"

#include "NodeGraphDemoWorkspace.h"

namespace AmaterasuDemo
{
    class NodeGraphDebugTool : public Tool<NodeGraphDemoWorkspace, NodeGraphDebugTool>
    {
    public:
        NodeGraphDebugTool();

        void Initialize(NodeGraphDemoWorkspace* workspace) override;
        void Render() override;
    
    private:
        NodeGraphDemoWorkspace* m_Workspace;
    };
}