#pragma once

#include "Amaterasu/GUIFramework/Tool.h"

#include "NodeGraphDemoWorkspace.h"

#include <map>

namespace AmaterasuDemo
{
    struct ViewportParameterData
    {
        ImVec2 Position;
    };

    struct ViewportNodeData
    {
        ImVec2 Position;
    };

    class NodeGraphViewportTool : public Amaterasu::Tool<NodeGraphDemoWorkspace, NodeGraphViewportTool>
    {
    public:
        NodeGraphViewportTool();
 
        void Initialize(NodeGraphDemoWorkspace* workspace) override;
        void Render() override;

    private:
        NodeGraphDemoWorkspace* m_Workspace;
        std::map<NodeIdentifier, ViewportNodeData> m_ViewportNodeData;
        std::map<ParameterIdentifier, ViewportParameterData> m_ViewportParameterData;

        ImVec2 m_ViewportPosition;
        float m_ViewportZoom;
        INode* m_DragNode;
        IInputParameter* m_DragParameter;
        ImVec2 m_DragInitialPosition;
        ImVec2 m_DragInitialMousePosition;
    };
}