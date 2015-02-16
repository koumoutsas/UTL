#ifndef Graph_GraphTravesalVisitor_H
#define Graph_GraphTravesalVisitor_H

#include <list>

namespace Graph
{

//! Template abstract base class for graph traversal visitors.
//!The class is intended to operate on UndirectedGraph objects, but any graph-like structure works
template<typename T,template<typename> class S>
class GraphTravesalVisitor
{
public:
	GraphTravesalVisitor(const S<T>& graph) noexcept:
		m_graph(graph),
		m_remaining(graph.nodes()),
		m_next(m_nodes.end())
	{
	}
protected:
	using Node=typename S<T>::Node;

	using NodeList=typename std::list<Node>;
public:
	//! An element is a pair of an iterator to a node container
	//! and a bool that flags whether the traversal bottomed out
	//! and we picked up a node that we haven't encounter before
	using Element=std::pair<typename NodeList::const_iterator,bool>;

	//! Get the end of the container. The second part of the pair is false to flag that it's not a new component
	Element end() const noexcept
	{
		return std::make_pair(m_nodes.end(),false);
	}

	//! Get the next node in the traversal and a flag of whether this is a new component
	typename GraphTravesalVisitor<T,S>::Element next() noexcept
	{
		bool breakPoint=false;
		if(GraphTravesalVisitor<T,S>::m_next==GraphTravesalVisitor<T,S>::m_nodes.end())
		{
			if(GraphTravesalVisitor<T,S>::m_remaining.empty())
			{
				return GraphTravesalVisitor<T,S>::end();
			}
			else
			{
				breakPoint=true;
				const auto& nextNode=*GraphTravesalVisitor<T,S>::m_remaining.begin();
				GraphTravesalVisitor<T,S>::m_next=GraphTravesalVisitor<T,S>::m_nodes.insert(GraphTravesalVisitor<T,S>::m_nodes.end(),nextNode);
				pushNextNeighbors(nextNode);
				GraphTravesalVisitor<T,S>::m_remaining.erase(nextNode);
			}
		}
		else
		{
			pushNextNeighbors(*GraphTravesalVisitor<T,S>::m_next);
		}
		return std::make_pair(GraphTravesalVisitor<T,S>::m_next++,breakPoint);
	}
protected:
	//! The graph
	const S<T>& m_graph;

	//! The container of nodes that have been traversed so far
	NodeList m_nodes;

	using NodeSet=typename S<T>::NodeSet;

	//! The nodes that haven't been traversed so fast
	NodeSet m_remaining;
private:
	//! Utility method for pushing all neighbors of the next node in the traversal in the list of next nodes
	virtual void pushNextNeighbors(const typename GraphTravesalVisitor<T,S>::Node& node) noexcept =0;

	//! The next node that will be returned in the traversal
	typename NodeList::iterator m_next;
};

}

#endif // Graph_GraphTravesalVisitor_H
