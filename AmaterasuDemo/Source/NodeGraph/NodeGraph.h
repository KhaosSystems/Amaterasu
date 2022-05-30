#include <vector>
#include <assert.h>

namespace AmaterasuDemo
{
	class IOutputParameter {};
	class IInputParameter {};

	class INode
	{
	public:
		std::vector<IInputParameter*> GetInputParameters() const { return m_InputParameters; }
		std::vector<IOutputParameter*> GetOutputParameters() const { return m_OutputParameters; }

		void AppendInputParameter(IInputParameter* inputParameter) { m_InputParameters.push_back(inputParameter); }
		void AppendOutputParameter(IOutputParameter* outputParameter) { m_OutputParameters.push_back(outputParameter); }

	private:
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
		void Connect(InputParameter<T>& inputParameter) { inputParameter.Connect(this); }

	private:
		T m_Data;
	};

	template<typename T>
	class InputParameter : public IInputParameter
	{
	public:
		InputParameter(INode* node) : m_Data(nullptr) { node->AppendInputParameter(this); }

		inline const T& Get() { assert(m_Data != nullptr); return m_Data->Get(); }
		void Connect(OutputParameter<T>& outputParameter) { m_Data = &outputParameter.m_Data; }

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