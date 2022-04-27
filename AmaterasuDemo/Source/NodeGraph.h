#include <deque>
#include <vector>
#include <typeindex>
#include <unordered_map>

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

namespace AmaterasuDemo
{
	// Scene
	class SceneNode
	{
	public:
		SceneNode();
		SceneNode(SceneNode* parent);

		void SetRelativePosition(ImVec2 newPosition);
		void SetWorldPosition(ImVec2 newPosition);

		ImVec2 GetRelativePosition();
		ImVec2 GetWorldPosition();

		virtual bool IsOverlapping(ImVec2 point);

        virtual std::deque<SceneNode*>& GetChildren();

		virtual void Render();

	protected:
		SceneNode* m_Parent;
		ImVec2 m_Position;
		std::deque<SceneNode*> m_Children;
	};

	class NodeGraph : public SceneNode
	{
	public:
		NodeGraph();
		
		void Initialize();
		void Render() override;
		void Terminate();

	private:
		ImVec2 m_LastMousePosition;
		bool m_LastMouseDown;
		bool m_LastMouseClicked;
		bool m_LastMouseReleased;
		ImVec2 m_MousePosition;
		bool m_MouseDown;
		bool m_MouseClicked;
		bool m_MouseReleased;

        class INodeParameter* m_StartNodeParameter;
        ImVec2 m_DragItemOffset;
		SceneNode* m_DragItem;
	};

    class INodeParameter : public SceneNode
    {
    public:
        virtual void GetData(void* data) = 0;
        virtual void SetData(void* data) = 0;
        virtual std::type_index GetDataType() const = 0;
        virtual uint32_t GetDataSize() const = 0;
        virtual void Connect(INodeParameter* other) = 0;
    };

    template<typename T>
	class NodeParameter : public INodeParameter
	{
	public:
		NodeParameter(class Node* parent)
        {
            m_Parent = parent;
        }

		void Render() override
        {
            ImDrawList* drawList = ImGui::GetWindowDrawList();

            for (INodeParameter* connection : m_Connections)
            {
                ImVec2 start = GetWorldPosition();
                ImVec2 end = connection->GetWorldPosition();
                drawList->AddBezierCurve(start, ImVec2((end.x * 0.6) + (start.x * 0.4), start.y), ImVec2((end.x * 0.4) + (start.x * 0.6), end.y), end, IM_COL32(51, 51, 51, 255), 6);
                drawList->AddBezierCurve(start, ImVec2((end.x * 0.6) + (start.x * 0.4), start.y), ImVec2((end.x * 0.4) + (start.x * 0.6), end.y), end, IM_COL32(0, 194, 255, 255), 2);
            }

            ImVec2 position = GetWorldPosition();

            drawList->AddCircleFilled(position, 8, IM_COL32(51, 51, 51, 255));
            drawList->AddCircleFilled(position, 6, IM_COL32(0, 194, 255, 255));
        }

		bool IsOverlapping(ImVec2 point) override
        {
            ImDrawList* drawList = ImGui::GetWindowDrawList();

            ImVec2 p1 = GetWorldPosition() + ImVec2(-8.0f, -8.0f);
            ImVec2 p2 = GetWorldPosition() + ImVec2(8.0f, 8.0f);
            drawList->AddRectFilled(p1, p2, IM_COL32(251, 51, 51, 255));
            return point.x >= p1.x && point.x <= p2.x && point.y >= p1.y && point.y <= p2.y;
        }
    
        // INodeParameter
        virtual void GetData(void* data) override { memcpy(data, &m_Data, sizeof(T)); }
        virtual void SetData(void* data) override { memcpy(&m_Data, data, sizeof(T)); }
        virtual std::type_index GetDataType() const override { return std::type_index(typeid(T)); }
        virtual uint32_t GetDataSize() const override { return sizeof(T); }
        virtual void Connect(INodeParameter* other) override { m_Connections.push_back(other); }

    private:
        std::vector<INodeParameter*> m_Connections;
        T m_Data;
    };

    class INode : public SceneNode
    {
    };

	class Node : public SceneNode 
	{
	public:
		Node(NodeGraph* parent);

		void Render() override;
		bool IsOverlapping(ImVec2 point) override;

        virtual void Execute() = 0;

        template<typename T>
        void RegisterInput(const std::string key)
        {
            assert(!m_Inputs.contains(key));

            NodeParameter<T>* input = new NodeParameter<T>(this);
            input->SetRelativePosition(ImVec2(0.0f, 13.0f + 18.0f + (22.0f * m_Inputs.size())));
            m_Inputs[key] = input;
            m_Children.push_back(input);
        }

        template<typename T>
        void RegisterOutput(const std::string key)
        {
            assert(!m_Outputs.contains(key));

            NodeParameter<T>* output = new NodeParameter<T>(this);
            output->SetRelativePosition(ImVec2(250.0f, 13.0f + 18.0f + (22.0f * m_Outputs.size())));
            m_Outputs[key] = output;
            m_Children.push_back(output);
        }

        template<typename T>
        void SetOutput(const std::string key, T data)
        {
            assert(m_Outputs.contains(key));
            assert(m_Outputs[key]->GetDataType() == std::type_index(typeid(T)));

            m_Outputs[key]->SetData(&data);
        }

        template<typename T>
        T GetInput(const std::string key)
        {
            assert(m_Inputs.contains(key));
            assert(m_Inputs[key]->GetDataType() == std::type_index(typeid(T)));

            T data;
            m_Inputs[key]->GetData(&data);

            return data;
        }

        std::unordered_map<std::string, INodeParameter*>& GetInputsDictionary() { return m_Inputs; }
        std::unordered_map<std::string, INodeParameter*>& GetOutputsDictionary() { return m_Outputs; }

    protected:
        std::unordered_map<std::string, INodeParameter*> m_Inputs;
        std::unordered_map<std::string, INodeParameter*> m_Outputs;
	};

    // Nodes
    class KSAddFloatNode : public Node
    {
    public:
        KSAddFloatNode(NodeGraph* parent);

        virtual void Execute() override;
    };
}