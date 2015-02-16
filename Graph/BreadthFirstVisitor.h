#ifndef Graph_BreadthFirstVisitor_H
#define Graph_BreadthFirstVisitor_H

#include "GraphTraversalVisitor.h"

namespace Graph
{

//! Template class for a depth-first visitor.
template<typename T,template<typename> class S>
class BreadthFirstVisitor : public GraphTravesalVisitor<T,S>
{
public:
	using GraphTravesalVisitor<T,S>::GraphTravesalVisitor;
private:
	void pushNextNeighbors(const typename GraphTravesalVisitor<T,S>::Node& node) noexcept override
	{
		const auto nextNeighbors=GraphTravesalVisitor<T,S>::m_graph.neighbors(node);
		for(const auto& n:nextNeighbors)
		{
			const auto it=GraphTravesalVisitor<T,S>::m_remaining.find(n);
			if(it!=GraphTravesalVisitor<T,S>::m_remaining.end())
			{
				GraphTravesalVisitor<T,S>::m_nodes.push_back(n);
				GraphTravesalVisitor<T,S>::m_remaining.erase(it);
			}
		}
	}
};

}

#endif // Graph_BreadthFirstVisitor_H
