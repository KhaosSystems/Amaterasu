#include <vector>
#include <assert.h>
#include <string>
#include <typeinfo>

namespace AmaterasuDemo
{
	template<typename T>
	class IncrementalIdentifier
	{
	public:
		IncrementalIdentifier() : m_Identifier(s_Counter) { s_Counter++; }
		IncrementalIdentifier(uint32_t identifier) : m_Identifier(identifier) {}

		bool operator==(const IncrementalIdentifier<T>& other) const { return this->m_Identifier == other.m_Identifier; }
		bool operator<(const IncrementalIdentifier<T>& other) const noexcept { return this->m_Identifier < other.m_Identifier; }

		std::string ToString() const { return std::to_string(m_Identifier); }

	public:
		const static IncrementalIdentifier<T> NullIdentifier;

	private:
		uint32_t m_Identifier;

		static uint32_t s_Counter;
	};
	
	template<typename T> const IncrementalIdentifier<T> IncrementalIdentifier<T>::NullIdentifier = IncrementalIdentifier<T>(0);
	template<typename T> uint32_t IncrementalIdentifier<T>::s_Counter = 1;

	typedef IncrementalIdentifier<class INode> NodeIdentifier;
	typedef IncrementalIdentifier<class IParameter> ParameterIdentifier;

	class IParameter
	{
	public:
		IParameter() : m_Identifier() {}

		const ParameterIdentifier GetIdentifier() { return m_Identifier; }

		virtual const std::type_info& GetType() const = 0;

	private:
		const ParameterIdentifier m_Identifier;
	};

	class IInputParameter : public IParameter
	{
	public:
		virtual void Connect(class IOutputParameter* outputParameter) = 0;
		virtual IOutputParameter* GetConnectedParameter() const = 0;
	};

	class IOutputParameter : public IParameter
	{
	public:
		virtual void Connect(IInputParameter* inputParameter) = 0;
	};

	class INode
	{
	public:
		INode() : m_Identifier() {}

		std::vector<IInputParameter*> GetInputParameters() const { return m_InputParameters; }
		std::vector<IOutputParameter*> GetOutputParameters() const { return m_OutputParameters; }

		const NodeIdentifier GetIdentifier() { return m_Identifier; }
		void AppendInputParameter(IInputParameter* inputParameter) { m_InputParameters.push_back(inputParameter); }
		void AppendOutputParameter(IOutputParameter* outputParameter) { m_OutputParameters.push_back(outputParameter); }

	private:
		const NodeIdentifier m_Identifier;
		std::vector<IInputParameter*> m_InputParameters;
		std::vector<IOutputParameter*> m_OutputParameters;
	};

	template<typename T> class InputParameter;

	template<typename T>
	class OutputParameter : public IOutputParameter
	{
		friend InputParameter<T>;

	public:
		OutputParameter(INode* node) : m_Data() { node->AppendOutputParameter(this); }

		inline void Set(T newData) { m_Data = newData; }
		inline const T& Get() { return m_Data; }

		virtual void Connect(IInputParameter* inputParameter) override { inputParameter->Connect(this); }
		virtual const std::type_info& GetType() const override { return typeid(T); }

	private:
		T m_Data;
	};

	template<typename T>
	class InputParameter : public IInputParameter
	{
	public:
		InputParameter(INode* node) : m_Data(nullptr) { node->AppendInputParameter(this); }

		inline const T& Get() { assert(m_Data != nullptr); return m_Data->Get(); }

		// TODO: This should properly be a reference, not a pointer.
		virtual void Connect(IOutputParameter* outputParameter) override
		{
			assert(outputParameter != nullptr);
			assert(GetType() == outputParameter->GetType());
			m_Data = static_cast<OutputParameter<T>*>(outputParameter);
		}
		virtual IOutputParameter* GetConnectedParameter() const override { return m_Data; }

		virtual const std::type_info& GetType() const override { return typeid(T); }

	private:
		OutputParameter<T>* m_Data;
	};

	class NodeGraph
	{
	public:
		std::vector<INode*> Nodes;
	};

	// Math nodes
	class FloatLiteral : public INode
	{
	public:
		FloatLiteral() : Data(this) {}

	public:
		OutputParameter<float> Data;
	};

	class AddNode : public INode
	{
	public:
		AddNode() : A(this), B(this), Result(this) {}
		void Execute() { Result.Set(A.Get() + B.Get()); }

	public:
		InputParameter<float> A;
		InputParameter<float> B;
		OutputParameter<float> Result;
	};
}