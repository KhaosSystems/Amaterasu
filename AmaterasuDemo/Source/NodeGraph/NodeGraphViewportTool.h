#pragma once

#include "GUIFramework/Tool.h"

#include "NodeGraphDemoWorkspace.h"

#include <map>

namespace AmaterasuDemo
{
    struct ViewportNodeData
    {
        ImVec2 Position;
    };

    class NodeGraphViewportTool : public Tool<NodeGraphDemoWorkspace, NodeGraphViewportTool>
    {
    public:
        NodeGraphViewportTool();
 
        void Initialize(NodeGraphDemoWorkspace* workspace) override;
        void Render() override;

    private:
        NodeGraphDemoWorkspace* m_Workspace;
        std::map<INode*, ViewportNodeData> m_ViewportNodeData;

        ImVec2 m_ViewportPosition;
        float m_ViewportZoom;
        INode* m_DragNode;
        IInputParameter* m_DragParameter;
        ImVec2 m_DragInitialPosition;
        ImVec2 m_DragInitialMousePosition;
    };
}