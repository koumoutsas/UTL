#ifndef Graph_IncreasingUndirectedGraph_H
#define Graph_IncreasingUndirectedGraph_H

#include "UndirectedGraph.h"
#include "DisjointSets.h"

namespace Graph
{

template<typename T>
class IncreasingUndirectedGraph:public UndirectedGraph<T>
{
public:
	using UndirectedGraph<T>::UndirectedGraph;

	using Node=typename UndirectedGraph<T>::Node;
	using AdjacencyList=typename UndirectedGraph<T>::AdjacencyList;

	void insert(const Node& node,const AdjacencyList& neighbors) override
	{
		UndirectedGraph<T>::insert(node,neighbors);
		try
		{
			m_components.add(node);
		}
		catch(typename SetOperations::DisjointSets<T>::ElementExists&)
		{
		}
		for(const auto& n:neighbors)
		{
			try
			{
				m_components.add(n);
			}
			catch(typename SetOperations::DisjointSets<T>::ElementExists&)
			{
			}
			m_components.join(node,n);
		}
	}

	void edge(const Node& n1,const Node& n2) override
	{
		UndirectedGraph<T>::edge(n1,n2);
		try
		{
			m_components.add(n1);
		}
		catch(typename SetOperations::DisjointSets<T>::ElementExists&)
		{
		}
		try
		{
			m_components.add(n2);
		}
		catch(typename SetOperations::DisjointSets<T>::ElementExists&)
		{
		}
		m_components.join(n1,n2);
	}

	using ConnectedComponentSet=typename UndirectedGraph<T>::ConnectedComponentSet;

	ConnectedComponentSet connectedComponents() const noexcept override
	{
		return m_components.sets();
	}

	bool sameComponent(const Node& n1,const Node& n2) const
	{
		try
		{
			return m_components.find(n1)==m_components.find(n2);
		}
		catch(typename SetOperations::DisjointSets<T>::NoSuchElement& e)
		{
			throw typename UndirectedGraph<T>::NoSuchNode(e.element());
		}
	}

	using ConnectedComponent=typename UndirectedGraph<T>::ConnectedComponent;

	const ConnectedComponent& component(const Node& n) const
	{
		return m_components.set(n);
	}
private:
	void remove(const Node&, const Node&) override
	{
		throw typename UndirectedGraph<T>::CorruptedGraph("Illegal remove edge operation");
	}

	void remove(const Node&) override
	{
		throw typename UndirectedGraph<T>::CorruptedGraph("Illegal remove node operation");
	}

	SetOperations::DisjointSets<T> m_components;
};

}

#endif // Graph_IncreasingUndirectedGraph_H
