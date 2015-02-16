#ifndef Graph_DepthFirstVisitor_H
#define Graph_DepthFirstVisitor_H

#include "GraphTraversalVisitor.h"

namespace Graph
{

//! Template class for a depth-first visitor.
template<typename T,template<typename> class S>
class DepthFirstVisitor : public GraphTravesalVisitor<T,S>
{
public:
	using GraphTravesalVisitor<T,S>::GraphTravesalVisitor;
private:
	typename GraphTravesalVisitor<T,S>::NodeList m_stack;

	void pushNextNeighbors(const typename GraphTravesalVisitor<T,S>::Node& node) noexcept override
	{
		const auto nextNeighbors=GraphTravesalVisitor<T,S>::m_graph.neighbors(node);
		for(const auto& n:nextNeighbors)
		{
				m_stack.push_front(n);
		}
		for(bool done=false;not done and not m_stack.empty();)
		{
			const auto& n = m_stack.front();
			const auto it=GraphTravesalVisitor<T,S>::m_remaining.find(n);
			if(it!=GraphTravesalVisitor<T,S>::m_remaining.end())
			{
				GraphTravesalVisitor<T,S>::m_remaining.erase(it);
				GraphTravesalVisitor<T,S>::m_nodes.push_back(n);
				done=true;
			}
			m_stack.pop_front();
		}
	}
};

}

#endif // Graph_DepthFirstVisitor_H
