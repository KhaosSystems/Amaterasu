#pragma once

#include "GUIFramework/Tool.h"

#include "NodeGraphDemoWorkspace.h"

namespace AmaterasuDemo
{
    class NodeGraphViewportTool : public Tool<NodeGraphDemoWorkspace, NodeGraphViewportTool>
    {
    public:
        NodeGraphViewportTool();
 
        void Initialize(NodeGraphDemoWorkspace* workspace) override;
        void Render() override;

    private:
        NodeGraphDemoWorkspace* m_Workspace;
        ImVec2 m_ViewportPosition;
        float m_ViewportZoom;
    };
}