#include <deque>
#include <vector>
#include <typeindex>
#include <unordered_map>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include <rapidxml.hpp>
namespace rapidxml {
    namespace internal {
        // https://stackoverflow.com/questions/14113923/rapidxml-print-header-has-undefined-methods
        template <class OutIt, class Ch> inline OutIt print_children(OutIt out, const xml_node<Ch>* node, int flags, int indent);
        template <class OutIt, class Ch> inline OutIt print_attributes(OutIt out, const xml_node<Ch>* node, int flags);
        template <class OutIt, class Ch> inline OutIt print_data_node(OutIt out, const xml_node<Ch>* node, int flags, int indent);
        template <class OutIt, class Ch> inline OutIt print_cdata_node(OutIt out, const xml_node<Ch>* node, int flags, int indent);
        template <class OutIt, class Ch> inline OutIt print_element_node(OutIt out, const xml_node<Ch>* node, int flags, int indent);
        template <class OutIt, class Ch> inline OutIt print_declaration_node(OutIt out, const xml_node<Ch>* node, int flags, int indent);
        template <class OutIt, class Ch> inline OutIt print_comment_node(OutIt out, const xml_node<Ch>* node, int flags, int indent);
        template <class OutIt, class Ch> inline OutIt print_doctype_node(OutIt out, const xml_node<Ch>* node, int flags, int indent);
        template <class OutIt, class Ch> inline OutIt print_pi_node(OutIt out, const xml_node<Ch>* node, int flags, int indent);
    }
}
#include <rapidxml_print.hpp>

namespace AmaterasuDemo
{
	// Scene
	class SceneNode
	{
	public:
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

        std::string GenerateUnqiueIdentifier()
        {
            uint64_t milliseconds_since_epoch = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);

            m_IncrementalUniqueIdentifer++;
            return (std::ostringstream{} << std::setw(32) << std::setfill('0') << m_IncrementalUniqueIdentifer + milliseconds_since_epoch).str();
        }

        void Serialize();
        void Deserialize();

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

        uint64_t m_IncrementalUniqueIdentifer;

        std::unordered_map<std::string, myfunc> m_NodeTypes;
	};

    struct ExecuteInfo {};

    enum class ENodeParameterDirection
    {
        Output, 
        Input
    };

   class INodeParameter : public SceneNode
    {
    public:
        INodeParameter(Node* parent)
            : SceneNode((SceneNode*)parent)
        {
        }

        virtual void GetData(void* data) = 0;
        virtual void SetData(void* data) = 0;
        virtual std::type_index GetDataType() const = 0;
        virtual uint32_t GetDataSize() const = 0;
        virtual void Connect(INodeParameter* other) = 0;
        virtual void Connected(INodeParameter* other) = 0;
        virtual void SetDisplayName(const std::string& newDisplayName) = 0;
        virtual const std::string& GetDisplayName() const = 0;
        virtual const std::vector<INodeParameter*>& GetConnections() const = 0;
        virtual const std::string& GetUniqueIdentifier() const = 0;
        virtual void SetUniqueIdentifier(std::string newUnqiueIdentifier) = 0;
        virtual ImU32 GetDataTypeColor() const = 0;
        virtual void SetDirection(ENodeParameterDirection direction) = 0;
        virtual ENodeParameterDirection GetDirection() const = 0;

        virtual void Serialize(rapidxml::xml_document<>& xmlDocument, rapidxml::xml_node<>* xmlParentNode) = 0;
        virtual void Deserialize(rapidxml::xml_node<>* xmlNode) = 0;
        virtual void DeserializeConnections(rapidxml::xml_node<>* xmlNode) = 0;
    };

    template<typename T>
	class NodeParameter : public INodeParameter
	{
	public:
		NodeParameter(Node* parent)
            : INodeParameter(parent)
        {
            m_Parent = parent;

            std::cout << "New NodeParameter was just created" << std::endl;

            NodeGraph* graph = dynamic_cast<NodeGraph*>(m_Parent->GetParent());
            assert(graph != nullptr);
            m_UnqiueIdentifier = graph->GenerateUnqiueIdentifier();
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
                drawList->AddBezierCurve(start, ImVec2((end.x * 0.6) + (start.x * 0.4), start.y), ImVec2((end.x * 0.4) + (start.x * 0.6), end.y), end, GetDataTypeColor(), 2);
            }

            drawList->AddCircleFilled(parameterPosition, 8, IM_COL32(51, 51, 51, 255));
            drawList->AddCircleFilled(parameterPosition, 6, GetDataTypeColor());

            drawList->AddText(io.FontDefault, 12.0f, parameterPosition + ImVec2(16.0f, -6.0f), IM_COL32(194, 194, 194, 194), GetDisplayName().c_str());
            drawList->AddText(io.FontDefault, 12.0f, parameterPosition + ImVec2(16.0f, -16.0f), IM_COL32(194, 194, 194, 194), m_UnqiueIdentifier.c_str());
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
        virtual void Connect(INodeParameter* other) override
        {
            std::cout << "Connect: " << this->GetUniqueIdentifier() << ", " << other->GetUniqueIdentifier() << "\n";

            if (other == this)
            {
                std::cout << "Failed to connect node parameters, a parameter can't be connected with it self.\n";
                return;
            }

            if (other->GetDataType() != this->GetDataType())
            {
                std::cout << "Failed to connect node parameters, parameters need to be of same type.\n";
                return;
            }

            if (other->GetDirection() == this->GetDirection())
            {
                std::cout << "Failed to connect node parameters, parameters can't have the same direction.\n";
                return;
            }

            if (std::find(m_Connections.begin(), m_Connections.end(), other) != m_Connections.end())
            {
                std::cout << "Failed to connect node parameters, a connection already exists.\n";
                return;
            }

            m_Connections.push_back(other);
            other->Connected(this);
        }
        virtual void Connected(INodeParameter* other) override
        {
            std::cout << "Connected: " << this->GetUniqueIdentifier() << ", " << other->GetUniqueIdentifier() << "\n";

            if (other == this)
            {
                std::cout << "Failed to connect node parameters, a parameter can't be connected with it self.\n";
                return;
            }

            if (other->GetDataType() != this->GetDataType())
            {
                std::cout << "Failed to connect node parameters, parameters need to be of same type.\n";
                return;
            }

            if (other->GetDirection() == this->GetDirection())
            {
                std::cout << "Failed to connect node parameters, parameters can't have the same direction.\n";
                return;
            }

            if (std::find(m_Connections.begin(), m_Connections.end(), other) != m_Connections.end())
            {
                std::cout << "Failed to connect node parameters, a connection already exists.\n";
                return;
            }

            m_Connections.push_back(other);
        }
        virtual void  SetDisplayName(const std::string& newDisplayName) override { m_DisplayName = newDisplayName; }
        virtual const std::string& GetDisplayName() const override { return m_DisplayName; }
        virtual const std::vector<INodeParameter*>& GetConnections() const override { return m_Connections; };
        virtual const std::string& GetUniqueIdentifier() const override { return m_UnqiueIdentifier; }
        virtual void SetUniqueIdentifier(std::string newUnqiueIdentifier) { m_UnqiueIdentifier = newUnqiueIdentifier; }
        virtual ImU32 GetDataTypeColor() const override { return IM_COL32(125, 125, 125, 255); }
        virtual void SetDirection(ENodeParameterDirection direction) override { m_Direction = direction; }
        virtual ENodeParameterDirection GetDirection() const override { return m_Direction; }

        void Serialize(rapidxml::xml_document<>& xmlDocument, rapidxml::xml_node<>* xmlParentNode) override
        {
            rapidxml::xml_node<>* xmlParameterNode = xmlDocument.allocate_node(rapidxml::node_type::node_element, "Parameter");
            xmlParameterNode->append_node(xmlDocument.allocate_node(rapidxml::node_type::node_element, "ParameterType", "Output"));
            xmlParameterNode->append_node(xmlDocument.allocate_node(rapidxml::node_type::node_element, "DisplayName", GetDisplayName().c_str()));
            xmlParameterNode->append_node(xmlDocument.allocate_node(rapidxml::node_type::node_element, "TypeName", GetDataType().name()));
            xmlParameterNode->append_node(xmlDocument.allocate_node(rapidxml::node_type::node_element, "UniqueIdentifier", GetUniqueIdentifier().c_str()));

            rapidxml::xml_node<>* xmlConnectionsNode = xmlDocument.allocate_node(rapidxml::node_type::node_element, "Connections");
            for (INodeParameter* connection : GetConnections())
            {
                rapidxml::xml_node<>* xmlConnectionNode = xmlDocument.allocate_node(rapidxml::node_type::node_element, "Connection");
                xmlConnectionNode->append_node(xmlDocument.allocate_node(rapidxml::node_type::node_element, "UniqueIdentifier", connection->GetUniqueIdentifier().c_str()));
                xmlConnectionsNode->append_node(xmlConnectionNode);
            }
            xmlParameterNode->append_node(xmlConnectionsNode);

            xmlParentNode->append_node(xmlParameterNode);
        }

        void Deserialize(rapidxml::xml_node<>* xmlNode) override
        {
            std::cout << "I was:      " << m_UnqiueIdentifier << ",\n Now i am: " << xmlNode->first_node("UniqueIdentifier")->value() << std::endl;
            m_UnqiueIdentifier = xmlNode->first_node("UniqueIdentifier")->value();
        }

        void DeserializeConnections(rapidxml::xml_node<>* xmlNode) override
        {
            auto GetParameterWithUniqueIdentifier = [&](std::string uniqueIdentifier)
            {
                std::cout << "GetParameterWithUniqueIdentifier, " << uniqueIdentifier << std::endl;

                NodeGraph* nodeGraph = dynamic_cast<NodeGraph*>(GetParent()->GetParent());
                assert(nodeGraph);

                for (SceneNode* nodeGraphChild : nodeGraph->GetChildren())
                {
                    Node* node = dynamic_cast<Node*>(nodeGraphChild);
                    if (!node) continue;
                    for (const auto& [key, nodeChild] : node->GetInputsDictionary())
                    {
                        INodeParameter* parameter = dynamic_cast<INodeParameter*>(nodeChild);
                        if (parameter)
                        {
                            std::cout << "Parameter: " << parameter->GetUniqueIdentifier() << std::endl;
                            if (parameter->GetUniqueIdentifier() == uniqueIdentifier)
                            {
                                return parameter;
                            }
                        }
                        else
                        {
                            std::cout << "Ehhhh - Something wired happened!" << std::endl;
                        }
                    }

                    for (const auto& [key, nodeChild] : node->GetOutputsDictionary())
                    {
                        INodeParameter* parameter = dynamic_cast<INodeParameter*>(nodeChild);
                        if (parameter)
                        {
                            std::cout << "Parameter: " << parameter->GetUniqueIdentifier() << std::endl;
                            if (parameter->GetUniqueIdentifier() == uniqueIdentifier)
                            {
                                return parameter;
                            }
                        }
                        else
                        {
                            std::cout << "Ehhhh - Something wired happened!" << std::endl;
                        }
                    }
                }

                return (INodeParameter*)nullptr;
            };

            rapidxml::xml_node<>* xmlConnectionsNode = xmlNode->first_node("Connections");
            for (rapidxml::xml_node<>* xmlConnectionNode = xmlConnectionsNode->first_node("Connection"); xmlConnectionNode; xmlConnectionNode = xmlConnectionNode->next_sibling())
            {
                std::string targetConnectionUniqueIdentifier = xmlConnectionNode->first_node("UniqueIdentifier")->value();
                INodeParameter* target = GetParameterWithUniqueIdentifier(targetConnectionUniqueIdentifier);
                if (target)
                {
                    std::cout << "this is good?" << std::endl;
                    Connect(target);
                }
            }
        }

    private:
        std::string m_DisplayName;
        std::string m_UnqiueIdentifier;
        std::vector<INodeParameter*> m_Connections;
        T m_Data;
        ENodeParameterDirection m_Direction;
    };

    template<> inline ImU32 NodeParameter<ExecuteInfo>::GetDataTypeColor() const { return IM_COL32(255, 255, 255, 255); }
    template<> inline ImU32 NodeParameter<float>::GetDataTypeColor() const { return IM_COL32(0, 194, 255, 255); }
    template<> inline ImU32 NodeParameter<std::string>::GetDataTypeColor() const { return IM_COL32(154, 0, 255, 255); }

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
	
        std::string GetUniqueIdentifier() const { return m_UniqueIdentifier; }

        void Serialize(rapidxml::xml_document<>& xmlDocument, rapidxml::xml_node<>* xmlParentNode);
        void Deserialize(rapidxml::xml_node<>* xmlNode);
        void DeserializeParameters(rapidxml::xml_node<>* xmlNode);

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
            input->SetDirection(ENodeParameterDirection::Input);
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
            output->SetDirection(ENodeParameterDirection::Output);
            output->SetDisplayName(key);
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
        std::string m_UniqueIdentifier;
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

    class KSExecuteNode : public Node
    {
    public:
        KSExecuteNode(NodeGraph* parent);
        virtual void Execute() override;
    };

}