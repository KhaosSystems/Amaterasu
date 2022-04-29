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

        SceneNode* GetParent() const { return m_Parent; }

		virtual bool IsOverlapping(ImVec2 point);

        virtual std::deque<SceneNode*>& GetChildren();

		virtual void Render();

	protected:
		SceneNode* m_Parent;
		ImVec2 m_Position;
		std::deque<SceneNode*> m_Children;
	};

    typedef class Node*(*myfunc)(class NodeGraph*);

	class NodeGraph : public SceneNode
	{
	public:
		NodeGraph();
		
		void Initialize();
		void Render() override;
		void Terminate();

        template<std::derived_from<Node> T>
        void RegisterNodeType();

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

        std::unordered_map<std::string, myfunc> m_NodeTypes;
	};

    class INodeParameter : public SceneNode
    {
    public:
        virtual void GetData(void* data) = 0;
        virtual void SetData(void* data) = 0;
        virtual std::type_index GetDataType() const = 0;
        virtual uint32_t GetDataSize() const = 0;
        virtual void Connect(INodeParameter* other) = 0;
        virtual void SetDisplayName(const std::string& newDisplayName) = 0;
        virtual const std::string& GetDisplayName() const = 0;
        virtual const std::vector<INodeParameter*>& GetConnections() const = 0;
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
            ImGuiIO& io = ImGui::GetIO();
            ImDrawList* drawList = ImGui::GetWindowDrawList();

            ImVec2 parameterPosition = GetWorldPosition();

            for (INodeParameter* connection : m_Connections)
            {
                ImVec2 start = parameterPosition;
                ImVec2 end = connection->GetWorldPosition();
                drawList->AddBezierCurve(start, ImVec2((end.x * 0.6) + (start.x * 0.4), start.y), ImVec2((end.x * 0.4) + (start.x * 0.6), end.y), end, IM_COL32(51, 51, 51, 255), 6);
                drawList->AddBezierCurve(start, ImVec2((end.x * 0.6) + (start.x * 0.4), start.y), ImVec2((end.x * 0.4) + (start.x * 0.6), end.y), end, IM_COL32(0, 194, 255, 255), 2);
            }

            drawList->AddCircleFilled(parameterPosition, 8, IM_COL32(51, 51, 51, 255));
            drawList->AddCircleFilled(parameterPosition, 6, IM_COL32(0, 194, 255, 255));

            drawList->AddText(io.FontDefault, 12.0f, parameterPosition + ImVec2(16.0f, -6.0f), IM_COL32(194, 194, 194, 194), GetDisplayName().c_str());
        }

		bool IsOverlapping(ImVec2 point) override
        {

            ImVec2 p1 = GetWorldPosition() + ImVec2(-8.0f, -8.0f);
            ImVec2 p2 = GetWorldPosition() + ImVec2(8.0f, 8.0f);
            // ImDrawList* drawList = ImGui::GetWindowDrawList();
            // drawList->AddRectFilled(p1, p2, IM_COL32(251, 51, 51, 255));
            return point.x >= p1.x && point.x <= p2.x && point.y >= p1.y && point.y <= p2.y;
        }
    
        // INodeParameter
        virtual void GetData(void* data) override { memcpy(data, &m_Data, sizeof(T)); }
        virtual void SetData(void* data) override { memcpy(&m_Data, data, sizeof(T)); }
        virtual std::type_index GetDataType() const override { return std::type_index(typeid(T)); }
        virtual uint32_t GetDataSize() const override { return sizeof(T); }
        virtual void Connect(INodeParameter* other) override { m_Connections.push_back(other); }
        virtual void  SetDisplayName(const std::string& newDisplayName) override { m_DisplayName = newDisplayName; }
        virtual const std::string& GetDisplayName() const override { return m_DisplayName; }
        virtual const std::vector<INodeParameter*>& GetConnections() const override { return m_Connections; };

    private:
        std::string m_DisplayName;
        std::vector<INodeParameter*> m_Connections;
        T m_Data;
    };

    class INode : public SceneNode
    {
    
    };

	class Node : public SceneNode 
	{
    public:
        template<std::derived_from<Node> T>
        static Node* Construct(NodeGraph* parent)
        {
            return new T(parent);
        }
	
    public:
		Node(NodeGraph* parent);

		virtual void Render() override;
		bool IsOverlapping(ImVec2 point) override;

        virtual void Execute() = 0;

        template<typename T>
        void RegisterInput(const std::string key)
        {
            assert(!m_Inputs.contains(key));

            NodeParameter<T>* input = new NodeParameter<T>(this);
            input->SetDisplayName(key);
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

        virtual void SetDisplayName(const std::string& newDisplayName) { m_DisplayName = newDisplayName; }
        virtual const std::string& GetDisplayName() const { return m_DisplayName; }
        
        virtual void SetTypeName(const std::string& newTypeName) { m_TypeName = newTypeName; }
        virtual const std::string& GetTypeName() const { return m_TypeName; }

    protected:
        std::string m_TypeName;
        std::string m_DisplayName;
        std::unordered_map<std::string, INodeParameter*> m_Inputs;
        std::unordered_map<std::string, INodeParameter*> m_Outputs;
	};

    template<std::derived_from<Node> T>
    void NodeGraph::RegisterNodeType()
    {
        T* instance = new T(this);
        m_NodeTypes[instance->GetTypeName()] = &Node::Construct<T>;
        delete instance;
    }

    // Nodes
    class KSAddFloatNode : public Node
    {
    public:
        KSAddFloatNode(NodeGraph* parent);
        virtual void Execute() override;
    };

    struct ExecuteInfo {};

    class KSExecuteNode : public Node
    {
    public:
        KSExecuteNode(NodeGraph* parent);
        virtual void Execute() override;
    };
}