#pragma once

#include "Amaterasu/GUIFramework/Tool.h"

#include "NodeGraphDemoWorkspace.h"

namespace AmaterasuDemo
{
    class NodeGraphDebugTool : public Amaterasu::Tool<NodeGraphDemoWorkspace, NodeGraphDebugTool>
    {
    public:
        NodeGraphDebugTool();

        void Initialize(NodeGraphDemoWorkspace* workspace) override;
        void Render() override;
    
    private:
        NodeGraphDemoWorkspace* m_Workspace;
    };
}