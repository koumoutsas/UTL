#ifndef Graph_KCore_H
#define Graph_KCore_H

#include "UndirectedGraph.h"

namespace Graph
{

template<typename T>
class KCore
{
public:
	KCore(const UndirectedGraph<T>& graph,const unsigned int k) noexcept:
		m_graph(graph),
		m_k(k)
	{
	}

	using NodeSet=typename UndirectedGraph<T>::NodeSet;

	NodeSet operator()() const noexcept
	{
		NodeSet good,bad;
		sieve(good,bad);
		while(not bad.empty())
		{
			using Node=typename UndirectedGraph<T>::Node;
			const typename NodeSet::iterator it=bad.begin();
			const Node next=*it;
			bad.erase(it);
			const typename UndirectedGraph<T>::AdjacencyList& l=m_graph.neighbors(next);
			for(const auto& n:l)
			{
				if(m_graph.degree(n)==m_k)
				{
					good.erase(n);
					bad.insert(n);
				}
			}
			m_graph.remove(next);
		}
		return good;
	}
private:
	mutable UndirectedGraph<T> m_graph;

	const unsigned int m_k;

	void sieve(NodeSet& good,NodeSet& bad) const noexcept
	{
		const NodeSet nodes=m_graph.nodes();
		for(const auto& n:nodes)
		{
			NodeSet& nodeSet=m_graph.degree(n)<m_k?bad:good;
			nodeSet.insert(n);
		}
	}
};

}

#endif // Graph_KCore_H
