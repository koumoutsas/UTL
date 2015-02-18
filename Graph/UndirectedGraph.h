#ifndef Graph_UndirectedGraph_H
#define Graph_UndirectedGraph_H

#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <functional>
#include <set>
#include "BreadthFirstVisitor.h"

namespace Graph
{

//! Undirected graph data template. All sets in the API are unordered
template<typename T>
class UndirectedGraph
{
public:
	using Node=T;

	using EdgeWeight=unsigned int;
private:	
	//! The default edge weight. It is used by the interface that inserts edges without weight
	static const EdgeWeight s_defaultEdgeWeight=1;

	//! Holds a node and weight pair, representing an entry in the adjacency list
	struct Neighbor
	{
		Node node;

		EdgeWeight weight;

		operator const Node&() const noexcept
		{
			return node;
		}

		bool operator==(const Neighbor& other) const noexcept
		{
			return node==other.node;
		}

		bool operator<(const Neighbor& other) const noexcept
		{
			return node<other.node;
		}

		friend std::ostream& operator<<(std::ostream& o,const typename Graph::UndirectedGraph<T>::Neighbor& n) noexcept
		{
			return o<<'('<<n.node<<" w = "<<n.weight<<')';
		}
	};

	//! Hashing struct for Neighbor. It is not declared as a specialization of std::hash because it is syntatically impossible
	//! to do in C++ with templates
	struct NeighborHash
	{
		std::size_t operator()(const Neighbor& n) const noexcept
		{
			return std::hash<Node>()(n.node);
		}
	};
public:
	using NodeSet=std::unordered_set<Node>;

	//! The adjacency list struct, with some convenience methods for initializing it from a node set, with default weights,
	//! getting the node set without the weights, and finding a node without weight. Pay attention to the fact that there is no
	//! find() or erase() with weight information. The insert interface from the base class is exposed, along with an insert(Node)
	//! that allows unweighted edges to be inserted
	class AdjacencyList:public std::unordered_set<Neighbor,NeighborHash>
	{
	public:
		using std::unordered_set<Neighbor,NeighborHash>::unordered_set;

		AdjacencyList()=default;

		AdjacencyList(const std::initializer_list<Node>& il)
		{
			for(auto it=il.begin();it!=il.end();++it)
			{
				insert(*it);
			}
		}

		operator NodeSet() const noexcept
		{
			NodeSet ret;
			for(const auto& n:*this)
			{
				ret.insert(n);
			}
			return ret;
		}

		using iterator=typename std::unordered_set<Neighbor,NeighborHash>::iterator;

		iterator find(const Node& n) noexcept
		{
			return std::unordered_set<Neighbor,NeighborHash>::find({n,0});
		}

		using const_iterator=typename std::unordered_set<Neighbor,NeighborHash>::const_iterator;

		const_iterator find(const Node& n) const noexcept
		{
			return std::unordered_set<Neighbor,NeighborHash>::find({n,0});
		}

		using size_type=typename std::unordered_set<Neighbor,NeighborHash>::size_type;

		size_type erase(const Node& n) noexcept
		{
			return std::unordered_set<Neighbor,NeighborHash>::erase({n,0});
		}

		iterator erase(iterator it) noexcept
		{
			return std::unordered_set<Neighbor,NeighborHash>::erase(it);
		}

		std::pair<iterator,bool> insert(const Node& n) noexcept
		{
			return std::unordered_set<Neighbor,NeighborHash>::insert({n,s_defaultEdgeWeight});
		}

		using std::unordered_set<Neighbor,NeighborHash>::insert;

		friend std::ostream& operator<<(std::ostream& o,const typename Graph::UndirectedGraph<T>::AdjacencyList& nodes) noexcept
		{
			return prettyPrintNodeList(o,nodes);
		}
	};

	//! Alias for size_t in most systems
	using NodeDegree=typename AdjacencyList::size_type;
private:
	//! The internal data structure for the graph is a hashmap
	using Container=std::unordered_map<Node,AdjacencyList>;
public:
	//! Alias for size_t in most systems
	using GraphSize=typename Container::size_type;
private:
	//! Base class for all exceptions involving nodes.
	//! You can get the node that caused the exception with the node() method
	class NodeException:public std::logic_error
	{
	public:
		NodeException(const Node& node) noexcept:
			std::logic_error([&node]()
			{
				std::stringstream ss;
				ss<<node;
				return ss.str();
			}()),
			m_node(node)
		{
		}

		const Node& node() const noexcept
		{
			return m_node;
		}
	private:
		const Node m_node;
	};

	//! Base class for all exceptions involving edges (except TrivialEdge, which is treated as a node exception).
	//! You can get the edge that caused the exception by calling edge(), which returns a pair of nodes
	class EdgeException:public std::logic_error
	{
	public:
		EdgeException(const Node& n1,const Node& n2) noexcept:
			std::logic_error([&n1,&n2]()
			{
				std::stringstream ss;
				ss<<'('<<n1<<", "<<n2<<')';
				return ss.str();
			}()),
			m_edge(std::make_pair(n1,n2))
			{
			}

		using Edge=std::pair<Node,Node>;

		const Edge& edge() const noexcept
		{
			return m_edge;
		}
	private:
		const Edge m_edge;
	};
public:
	//! This exception is thrown by methods that take a node as argument when the node is not part of the graph
	class NoSuchNode:public NodeException
	{
	public:
		using NodeException::NodeException;
	};

	//! This exception shouldn't ever be thrown. If it's thrown the graph has been corrupted internally.
	//! Code using UndirectedGraph shouldn't catch this exception. It's only useful for testing
	class CorruptedGraph:public std::logic_error
	{
	public:
		using std::logic_error::logic_error;
	};

	//! This exception is throw when a method tries to add an edge that already exists
	class EdgeExists:public EdgeException
	{
	public:
		using EdgeException::EdgeException;
	};

	//! This exception is thrown when a method tries to access an edge that doesn't exist
	class NoSuchEdge:public EdgeException
	{
	public:
		using EdgeException::EdgeException;
	};

	//! Thrown when a loop edge is attempted. Such edges are forbidden.
	//! It's a NodeExcption, because a single node is involved
	class TrivialEdge:public NodeException
	{
	public:
		using NodeException::NodeException;
	};

	//! Thrown when an edge with 0 weight is added
	class ZeroWeightEdge:public EdgeException
	{
	public:
		using EdgeException::EdgeException;
	};

	/*!
	 * \brief size Get the number of nodes in the graph
	 */
	GraphSize size() const noexcept
	{
		return m_graph.size();
	}

	/*!
	 * \brief empty True iff there are no nodes
	 */
	bool empty() const noexcept
	{
		return m_graph.empty();
	}

	/*!
	 * \brief operator == Equality operator between two graphs. This is not an isomorphism check, the node labels need to be identical
	 */
	bool operator==(const UndirectedGraph& other) const noexcept
	{
		return m_graph==other.m_graph;
	}

	/*!
	 * \brief nodes Get a set with all the nodes in the graph
	 */
	NodeSet nodes() const noexcept
	{
		NodeSet result;
		for(const auto& n:m_graph)
		{
			result.insert(n.first);
		}
		return result;
	}

	/*!
	 * \brief degree Get the degree of a node
	 * \throw NoSuchNode If the node doesn't belong to the graph
	 */
	NodeDegree degree(const Node& node) const
	{
		try
		{
			return m_graph.at(node).size();
		}
		catch(std::out_of_range&)
		{
			throw NoSuchNode(node);
		}
	}

	/*!
	 * \brief isEdge Test whether an edge between two nodes exists
	 * \throw NoSuchNode If one of the nodes doesn't belong to the graph
	 */
	bool isEdge(const Node& n1, const Node& n2) const
	{
		const std::function<bool(const Node,const Node)> lambda=[this](const Node& n,const Node& m)
		{
			const AdjacencyList& l=find(n)->second;
			return l.find(m)!=l.end();
		};
		const bool b1=lambda(n1,n2);
		const bool b2=lambda(n2,n1);
		if(b1==b2)
		{
			return b1;
		}
		throw CorruptedGraph("The adjacency lists are not symmetric");
	}

	/*!
	 * \brief edgeWeight Get the weight of an edge
	 * \throw NoSuchEdge If the edge doesn't exist
	 */
	EdgeWeight edgeWeight(const Node& n1,const Node& n2) const
	{
		if(not isEdge(n1,n2))
		{
			throw NoSuchEdge(n1,n2);
		}
		return find(n1)->second.find(n2)->weight;
	}

	/*!
	 * \brief neighbors Get the neighbors of a node
	 * \throw NoSuchNode If the node doesn't belong to the graph
	 */
	const AdjacencyList& neighbors(const Node& n) const
	{
		return find(n)->second;
	}

	//! Convenience method for inserting a node without neighbors
	virtual void insert(const Node& node)
	{
		insert(node,AdjacencyList());
	}

	//! Insert a node with a set of neighbors.
	//! If the node exists, the new neighbors are merged with the existing ones.
	//! If a neighbor is not  a node in the graph, it is added
	/*!
	 * \throw TrivialEdge If node is in neighbors
	 */
	virtual void insert(const Node& node,const AdjacencyList& neighbors)
	{
		if(neighbors.find(node)!=neighbors.end())
		{
			throw TrivialEdge(node);
		}
		m_graph[node].insert(neighbors.begin(),neighbors.end());
		for(const auto& n:neighbors)
		{
			m_graph[n].insert({node,n.weight});
		}
	}

	/*!
	 * \brief remove Remove a node and its edges from a graph
	 * \throw NoSuchNode If the node doesn't belong to the graph
	 */
	virtual void remove(const Node& node)
	{
		const typename Container::iterator it=find(node);
		const AdjacencyList& l=it->second;
		const std::function<bool(const Node)> undo=[this,&l,&node](const Node& n)
		{
			bool result=true;
			for(const auto& m:l)
			{
				if(&m.node==&n)
				{
					return result;
				}
				try
				{
					if(not m_graph.at(m).insert({node,m.weight}).second)
					{
						result=false;
					}
				}
				catch(std::out_of_range&)
				{
					result=false;
				}
			}
			return result;
		};
		for(const auto& n:l)
		{
			try
			{
				if(m_graph.at(n).erase(node)!=1)
				{
					const bool undoSuccess=undo(n);
					std::stringstream ss;
					ss<<"Adjacency list of removed node "<<node<<" contains node "<<n<<" whose adjacency list doesn't contain "<<node;
					if(not undoSuccess)
					{
						ss<<undoFailedString();
					}
					throw CorruptedGraph(ss.str());
				}
			}
			catch(std::out_of_range&)
			{
				const bool undoSuccess=undo(n);
				std::stringstream ss;
				ss<<"The adjacency list of removed node "<<node<<" contains non-existent node "<<n;
				if(not undoSuccess)
				{
					ss<<undoFailedString();
				}
				throw CorruptedGraph(ss.str());
			}
		}
		m_graph.erase(it);
	}

	/*!
	 * \brief edge Add a new edge between two existing nodes with the default weight
	 * \throw TrivialEdge If n1==n2
	 * \throw EdgeExists If there is already an edge between n1 and n2
	 */
	virtual void edge(const Node& n1,const Node& n2)
	{
		return edge(n1,n2,s_defaultEdgeWeight);
	}

	/*!
	 * \brief edge Add a new edge between two existing nodes
	 * \throw TrivialEdge If n1==n2
	 * \throw ZeroWeightEdge If th weight is 0
	 * \throw EdgeExists If there is already an edge between n1 and n2
	 */
	virtual void edge(const Node& n1,const Node& n2,const EdgeWeight weight)
	{
		if(n1==n2)
		{
			throw TrivialEdge(n1);
		}
		if(not weight)
		{
			throw ZeroWeightEdge(n1,n2);
		}
		AdjacencyList& l1=find(n1)->second;
		AdjacencyList& l2=find(n2)->second;
		const bool n1HasN2=l1.find(n2)!=l1.end();
		const bool n2HasN1=l2.find(n1)!=l2.end();
		if(n1HasN2 and n2HasN1)
		{
			throw EdgeExists(n1,n2);
		}
		else if(not n1HasN2 and not n2HasN1)
		{
			l1.insert({n2, weight});
			l2.insert({n1, weight});
		}
		else
		{
			if(n2HasN1)
			{
				throwCorruptedGraph(n1,n2);
			}
			else
			{
				throwCorruptedGraph(n2,n1);
			}
		}
	}

	/*!
	 * \brief setWeight Set the weight of an existing edge
	 * \throw TrivialEdge If n1==n2
	 * \throw ZeroWeightEdge If th weight is 0
	 * \throw NoSuchEdge If there is already no such edge
	 */
	void setWeight(const Node& n1,const Node& n2,const EdgeWeight weight)
	{
		if(n1==n2)
		{
			throw TrivialEdge(n1);
		}
		if(not weight)
		{
			throw ZeroWeightEdge(n1,n2);
		}
		AdjacencyList& l1=find(n1)->second;
		AdjacencyList& l2=find(n2)->second;
		const auto it1=l1.find(n2);
		const auto it2=l2.find(n1);
		const bool n1HasN2=it1!=l1.end();
		const bool n2HasN1=it2!=l2.end();
		if(n1HasN2 and n2HasN1)
		{
			Neighbor neighbor1=*it1;
			Neighbor neighbor2=*it2;
			neighbor1.weight=neighbor2.weight=weight;
			l1.erase(it1);
			l2.erase(it2);
			l1.insert(neighbor1);
			l2.insert(neighbor2);
		}
		else if(not n1HasN2 and not n2HasN1)
		{
			throw NoSuchEdge(n1,n2);
		}
		else
		{
			if(n2HasN1)
			{
				throwCorruptedGraph(n1,n2);
			}
			else
			{
				throwCorruptedGraph(n2,n1);
			}
		}

	}

	/*!
	 * \brief remove Remove an existing edge
	 * \throw TrivialEdge If n1==n2
	 * \throw NoSuchEdge If there is no edge between n1 and n2
	 */
	virtual void remove(const Node& n1,const Node& n2)
	{
		if(n1==n2)
		{
			throw TrivialEdge(n1);
		}
		AdjacencyList& l1=find(n1)->second;
		AdjacencyList& l2=find(n2)->second;
		const typename AdjacencyList::iterator it1=l1.find(n2);
		const typename AdjacencyList::iterator it2=l2.find(n1);
		const bool n1HasN2=it1!=l1.end();
		const bool n2HasN1=it2!=l2.end();
		if(n1HasN2 and n2HasN1)
		{
			l1.erase(it1);
			l2.erase(it2);
		}
		else if(not n1HasN2 and not n2HasN1)
		{
			throw NoSuchEdge(n1,n2);
		}
		else
		{
			if(n2HasN1)
			{
				throwCorruptedGraph(n1,n2);
			}
			else
			{
				throwCorruptedGraph(n2,n1);
			}
		}
	}

	//! brief induced Get the induced subgraph defined by a node set.
	//! No exception is thrown if the node set is not a strict subset of the graph nodes
	UndirectedGraph induced(const NodeSet& nodeSet) const noexcept
	{
		UndirectedGraph result;
		for(const auto& n:nodeSet)
		{
			AdjacencyList l;
			try
			{
				const AdjacencyList& l1=find(n)->second;
				for(const auto& n:l1)
				{
					if(nodeSet.find(n)!=nodeSet.end())
					{
						l.insert(n);
					}
				}
			}
			catch(const NoSuchNode&)
			{
			}
			result.insert(n,l);
		}
		return result;
	}

	using ConnectedComponent=NodeSet;
	using ConnectedComponentSet=std::vector<ConnectedComponent>;

	virtual ConnectedComponentSet connectedComponents() const noexcept
	{
		ConnectedComponentSet result;
		BreadthFirstVisitor<Node,UndirectedGraph> visitor(*this);
		for(auto it=visitor.next();it!=visitor.end();it=visitor.next())
		{
			if(it.second)
			{
				result.resize(result.size()+1);
			}
			(*result.rbegin()).insert(*it.first);
		}
		return result;
	}
private:
	//! A hashmap from nodes to their neighbors. Edges are stored at both endpoints to make search operations faster
	Container m_graph;

	//! A string attached to CorruptedGraph exceptions when the operations that fails cannot be rolled back
	static const std::string s_undoFailedString;

	//! Find a node or throw a NoSuchNode exception when it cannot be found
	typename Container::iterator find(const Node& n)
	{
		const typename Container::iterator it=m_graph.find(n);
		if(it==m_graph.end())
		{
			throw NoSuchNode(n);
		}
		return it;
	}

	//! Const version of find
	typename Container::const_iterator find(const Node& n) const
	{
		const typename Container::const_iterator it=m_graph.find(n);
		if(it==m_graph.end())
		{
			throw NoSuchNode(n);
		}
		return it;
	}

	//! Intersection of unordered sets. STL doesn't implement this operation with its set_intersection
	static AdjacencyList intersection(const AdjacencyList& l1,const AdjacencyList& l2) noexcept
	{
		AdjacencyList result;
		if(l2.size()<l1.size())
		{
			result=intersection(l2,l1);
		}
		else
		{
			for(const auto& n:l1)
			{
				if(l2.find(n)!=l2.end())
				{
					result.insert(n);
				}
			}
		}
		return result;
	}

	//! Convenience method for throwing a Corrupted graph exception with the proper messages
	static void throwCorruptedGraph(const Node& n1,const Node& n2)
	{
		std::stringstream ss;
		ss<<"Node "<<n1<<" doesn't have node "<<n2<<" in its adjacency list, but "<<n2<<" does";
		throw CorruptedGraph(ss.str());
	}

	static const std::string& undoFailedString() noexcept
	{
		static const std::string s(". Undoing the operation wasn't entrely successful");
		return s;
	}
};

template <typename T>
std::ostream& prettyPrintNodeList(std::ostream& o,const T& nodes) noexcept
{
	using OrderedNodeSet=std::set<typename T::value_type>;
	const OrderedNodeSet ordered(nodes.begin(),nodes.end());
	o<<"{ ";
	for(typename OrderedNodeSet::const_iterator lit=ordered.begin();lit!=ordered.end();)
	{
		o<<*lit++;
		if(lit!=ordered.end())
		{
			o<<", ";
		}
	}
	if(not ordered.empty())
	{
		o<<' ';
	}
	return o<<'}';
}

}

template<typename T>
std::ostream& operator<<(std::ostream& o,const typename Graph::UndirectedGraph<T>::NodeSet& nodes)
{
	return Graph::prettyPrintNodeList(o, nodes);
}

template<typename T>
std::ostream& operator<<(std::ostream& o,const Graph::UndirectedGraph<T>& g) noexcept
{
	const typename Graph::UndirectedGraph<T>::NodeSet& nodeSet=g.nodes();
	using Node=typename Graph::UndirectedGraph<T>::Node;
	using OrderedNodeSet=std::set<Node>;
	const OrderedNodeSet orderedNodeSet(nodeSet.begin(),nodeSet.end());
	for(typename OrderedNodeSet::const_iterator it=orderedNodeSet.begin();it!=orderedNodeSet.end();)
	{
		const Node& n=*it++;
		o<<n<<" -> "<<g.neighbors(n);
		if(it!=orderedNodeSet.end())
		{
			o<<std::endl;
		}
	}
	return o;
}

#endif // Graph_UndirectedGraph_H
