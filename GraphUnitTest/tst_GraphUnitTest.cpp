#include <QtTest>
#include <deque>
#include "DepthFirstVisitor.h"
#include "BreadthFirstVisitor.h"
#include "IncreasingUndirectedGraph.h"

class Node
{
public:
	Node(const int i) noexcept:
		m_i(i)
	{
	}

	int value() const noexcept
	{
		return m_i;
	}

	bool operator==(const Node& other) const noexcept
	{
		return m_i==other.m_i;
	}

	bool operator<(const Node& other) const noexcept
	{
		return m_i<other.m_i;
	}
private:

	friend std::ostream& operator<<(std::ostream&,const Node&);

	int m_i;
};

std::ostream& operator<<(std::ostream& o,const Node& n)
{
	return o<<n.m_i;
}

namespace std
{

template<>
struct hash<Node>
{
	size_t operator()(const Node& n) const noexcept
	{
		return hash<int>()(n.value());
	}
};

}

class GraphUnitTest:public QObject
{
	Q_OBJECT	
public:
	GraphUnitTest();
private:
	using UndirectedGraph=Graph::UndirectedGraph<Node>;
	using IncreasingUndirectedGraph=Graph::IncreasingUndirectedGraph<Node>;
	using DepthFirstVisitor=Graph::DepthFirstVisitor<Node,Graph::UndirectedGraph>;
	using BreadthFirstVisitor=Graph::BreadthFirstVisitor<Node,Graph::UndirectedGraph>;
private Q_SLOTS:
	void graphEmpty();
	void increasingGraphEmpty();
	void graphSize();
	void increasingGraphSize();
	void graphEquality();
	void increasingGraphEquality();
	void graphNodes();
	void increasingGraphNodes();
	void graphDegree();
	void increasingGraphDegree();
	void graphIsEdge();
	void increasingGraphIsEdge();
	void graphNeighbors();
	void increasingGraphNeighbors();
	void graphInsert();
	void increasingGraphInsert();
	void graphInduced();
	void increasingGraphInduced();
	void graphEdge();
	void increasingGraphEdge();
	void graphRemove();
	void graphRemoveEdge();
	void graphConnectedComponents();
	void increasingGraphConnectedComponents();
	void depthFirstVisitor();
	void breadthFirstVisitor();
private:
	template<class T>
	static T buildDepthFirstTree() noexcept;

	template<class T>
	static T buildBreadthFirstTree() noexcept;

	template<class T>
	static T buildDepthFirstSegmented() noexcept;

	template<class T>
	static T buildBreadthFirstSegmented() noexcept;

	template<class T>
	void graphEmpty();

	template<class T>
	void graphSize();

	template<class T>
	void graphEquality();

	template<class T>
	void graphNodes();

	template<class T>
	void graphDegree();

	template<class T>
	void graphIsEdge();

	template<class T>
	void graphNeighbors();

	template<class T>
	void graphInsert();

	template<class T>
	void graphInduced();

	template<class T>
	void graphEdge();
};

template<typename T>
bool equal(const T& set1,const T& set2)
{
	if(set1.size()!=set2.size())
	{
		return false;
	}
	std::unordered_set<const typename T::value_type*> matched;
	for(const auto& l1:set1)
	{
		bool found=false;
		for(const auto& l2:set2)
		{
			if(matched.find(&l2)==matched.end() and l1.size()==l2.size() and std::is_permutation(l1.begin(),l1.end(),l2.begin()))
			{
				matched.insert(&l2);
				found=true;
				break;
			}
		}
		if(not found)
		{
			return false;
		}
	}
	return true;
}

GraphUnitTest::GraphUnitTest()
{
}

template<class T>
void GraphUnitTest::graphEmpty()
{
	T graph;
	QVERIFY(graph.empty());
	graph.insert(0,T::s_emptyAdjacencyList);
	QVERIFY(not graph.empty());
	graph.insert(1,T::s_emptyAdjacencyList);
	QVERIFY(not graph.empty());
}

void GraphUnitTest::graphEmpty()
{
	graphEmpty<UndirectedGraph>();
}

void GraphUnitTest::increasingGraphEmpty()
{
	graphEmpty<IncreasingUndirectedGraph>();
}

template<class T>
void GraphUnitTest::graphSize()
{
	T graph;
	for(unsigned int i=0;i<5;++i)
	{
		QVERIFY(graph.size()==i);
		graph.insert(i,T::s_emptyAdjacencyList);
	}
}

void GraphUnitTest::graphSize()
{
	graphSize<UndirectedGraph>();
}

void GraphUnitTest::increasingGraphSize()
{
	graphSize<IncreasingUndirectedGraph>();
}

template<class T>
void GraphUnitTest::graphEquality()
{
	T graph1;
	graph1.insert(0,{1,3});
	T graph2;
	graph2.insert(1,{0});
	graph2.insert(3,{0});
	QVERIFY(graph1==graph2);
	T graph3(graph2);
	graph3.edge(3,1);
	QVERIFY(not(graph1==graph3));
	QVERIFY(not(graph2==graph3));
}

void GraphUnitTest::graphEquality()
{
	graphEquality<UndirectedGraph>();
}

void GraphUnitTest::increasingGraphEquality()
{
	graphEquality<IncreasingUndirectedGraph>();
}

template<class T>
void GraphUnitTest::graphNodes()
{
	T graph;
	QVERIFY(graph.nodes().empty());
	typename T::AdjacencyList l={1,5,6};
	const typename T::Node node(0);
	graph.insert(node,l);
	l.insert(node);
	QVERIFY(graph.nodes()==l);
}

void GraphUnitTest::graphNodes()
{
	graphNodes<UndirectedGraph>();
}

void GraphUnitTest::increasingGraphNodes()
{
	graphNodes<IncreasingUndirectedGraph>();
}

template<class T>
void GraphUnitTest::graphDegree()
{
	T graph;
	typename T::AdjacencyList l={1,5,6};
	const typename T::Node node(0);
	graph.insert(node,l);
	QVERIFY(graph.degree(node)==l.size());
	for(const auto& n:l)
	{
		QVERIFY(graph.degree(n)==1);
	}
	graph.edge(1,5);
	QVERIFY(graph.degree(1)==2);
	QVERIFY(graph.degree(5)==2);
	const typename T::Node noNode(2);
	try
	{
		graph.degree(noNode);
		QVERIFY(false);
	}
	catch(const typename T::NoSuchNode& e)
	{
		QVERIFY(e.node()==noNode);
	}
}

void GraphUnitTest::graphDegree()
{
	graphDegree<UndirectedGraph>();
}

void GraphUnitTest::increasingGraphDegree()
{
	graphDegree<IncreasingUndirectedGraph>();
}

template<class T>
void GraphUnitTest::graphIsEdge()
{
	T graph;
	typename T::AdjacencyList l={1,5,6};
	const typename T::Node node(0);
	graph.insert(node,l);
	const typename T::Node& first=*l.begin();
	for(const auto& n:l)
	{
		QVERIFY(graph.isEdge(node,n) and graph.isEdge(n,node));
		QVERIFY(not graph.isEdge(first,n) and not graph.isEdge(n,first));
	}
	const typename T::Node noNode(2);
	try
	{
		graph.isEdge(node,noNode);
		QVERIFY(false);
	}
	catch(const typename T::NoSuchNode& e)
	{
		QVERIFY(e.node()==noNode);
	}
	try
	{
		graph.isEdge(noNode,node);
		QVERIFY(false);
	}
	catch(const typename T::NoSuchNode& e)
	{
		QVERIFY(e.node()==noNode);
	}
}

void GraphUnitTest::graphIsEdge()
{
	graphIsEdge<UndirectedGraph>();
}

void GraphUnitTest::increasingGraphIsEdge()
{
	graphIsEdge<IncreasingUndirectedGraph>();
}

template<class T>
void GraphUnitTest::graphNeighbors()
{
	T graph;
	const typename T::AdjacencyList l={1,5,6};
	const typename T::AdjacencyList reverse={0};
	const typename T::Node node(*reverse.begin());
	graph.insert(node,l);
	const typename T::Node isolated(2);
	graph.insert(isolated,T::s_emptyAdjacencyList);
	QVERIFY(graph.neighbors(node)==l);
	QVERIFY(graph.neighbors(isolated).empty());
	for(const auto& n:l)
	{
		QVERIFY(graph.neighbors(n)==reverse);
	}
	const typename T::Node noNode(2);
	try
	{
		graph.neighbors(noNode);
	}
	catch(const typename T::NoSuchNode& e)
	{
		QVERIFY(e.node()==noNode);
	}
}

void GraphUnitTest::graphNeighbors()
{
	graphNeighbors<UndirectedGraph>();
}

void GraphUnitTest::increasingGraphNeighbors()
{
	graphNeighbors<IncreasingUndirectedGraph>();
}

template<class T>
void GraphUnitTest::graphInsert()
{
	T graph;
	const typename T::AdjacencyList l1={1,5,6};
	const typename T::Node node1(0);
	graph.insert(node1,l1);
	const typename T::AdjacencyList l2={3,5};
	const typename T::Node node2(1);
	graph.insert(node2,l2);
	graph.insert(node2,T::s_emptyAdjacencyList);
	typename T::NodeSet nodeSet(l1);
	nodeSet.insert(node1);
	nodeSet.insert(l2.begin(),l2.end());
	QVERIFY(graph.nodes()==nodeSet);
	for(const auto& n:l1)
	{
		QVERIFY(graph.isEdge(n,node1));
	}
	for(const auto& n:l2)
	{
		QVERIFY(graph.isEdge(n,node2));
	}
	T graph1;
	try
	{
		typename T::AdjacencyList l(l1);
		l.insert(node1);
		graph1.insert(node1,l);
		QVERIFY(false);
	}
	catch(const typename T::TrivialEdge& e)
	{
		QVERIFY(e.node()==node1);
		QVERIFY(graph1.nodes().empty());
	}
}

void GraphUnitTest::graphInsert()
{
	graphInsert<UndirectedGraph>();
}

void GraphUnitTest::increasingGraphInsert()
{
	graphInsert<IncreasingUndirectedGraph>();
}

template<class T>
void GraphUnitTest::graphInduced()
{
	T graph;
	QVERIFY(not graph.induced({0,1}).isEdge(0,1));
	const typename T::AdjacencyList l1={1,5,6};
	const typename T::Node node1(0);
	graph.insert(node1,l1);
	const typename T::AdjacencyList l2={3,5};
	const typename T::Node node2(1);
	graph.insert(node2,l2);
	const typename T::NodeSet inducedNodes({0,1,4,6});
	const UndirectedGraph induced=graph.induced(inducedNodes);
	QVERIFY(induced.nodes()==inducedNodes);
	for(const auto& n:inducedNodes)
	{
		const typename T::AdjacencyList& inducedNeighbors=induced.neighbors(n);
		try
		{
			const typename T::AdjacencyList& originalNeighbors=graph.neighbors(n);
			typename T::AdjacencyList intersection;
			for(const auto& m:originalNeighbors)
			{
				if(inducedNodes.find(m)!=inducedNodes.end())
				{
					intersection.insert(m);
				}
			}
			QVERIFY(intersection==inducedNeighbors);
		}
		catch(const typename T::NoSuchNode&)
		{
			QVERIFY(inducedNeighbors.empty());
		}
	}
}

void GraphUnitTest::graphInduced()
{
	graphInduced<UndirectedGraph>();
}

void GraphUnitTest::increasingGraphInduced()
{
	graphInduced<IncreasingUndirectedGraph>();
}

template<class T>
void GraphUnitTest::graphEdge()
{
	T graph;
	const typename T::Node node1(0);
	try
	{
		graph.edge(node1,node1);
		QVERIFY(false);
	}
	catch(const typename T::TrivialEdge& e)
	{
		QVERIFY(e.node()==node1);
	}
	graph.insert(node1,T::s_emptyAdjacencyList);
	const typename T::Node node2(1);
	try
	{
		graph.edge(node1,node2);
		QVERIFY(false);
	}
	catch(const typename T::NoSuchNode& e)
	{
		QVERIFY(e.node()==node2);
	}
	try
	{
		graph.edge(node2,node1);
		QVERIFY(false);
	}
	catch(const typename T::NoSuchNode& e)
	{
		QVERIFY(e.node()==node2);
	}
	graph.insert(node2,T::s_emptyAdjacencyList);
	graph.edge(node1,node2);
	try
	{
		graph.edge(node1,node2);
		QVERIFY(false);
	}
	catch(const typename T::EdgeExists& e)
	{
		QVERIFY(e.edge()==std::make_pair(node1,node2));
	}
	try
	{
		graph.edge(node2,node1);
		QVERIFY(false);
	}
	catch(const typename T::EdgeExists& e)
	{
		QVERIFY(e.edge()==std::make_pair(node2,node1));
	}
	const typename T::Node node3(2);
	graph.insert(node3,T::s_emptyAdjacencyList);
	graph.edge(node1,node3);
	QVERIFY(graph.degree(node1)==2 and graph.degree(node2)==1 and graph.degree(node3)==1);
}

void GraphUnitTest::graphEdge()
{
	graphEdge<UndirectedGraph>();
}

void GraphUnitTest::increasingGraphEdge()
{
	graphEdge<IncreasingUndirectedGraph>();
}

void GraphUnitTest::graphRemove()
{
	UndirectedGraph graph;
	const UndirectedGraph::Node node(0);
	try
	{
		graph.remove(node);
		QVERIFY(false);
	}
	catch(const UndirectedGraph::NoSuchNode& e)
	{
		QVERIFY(e.node()==node);
	}
	const UndirectedGraph::AdjacencyList l={1,5,6};
	graph.insert(node,l);
	const UndirectedGraph snapshot(graph);
	const UndirectedGraph::Node isolated(2);
	graph.insert(isolated,UndirectedGraph::s_emptyAdjacencyList);
	graph.remove(isolated);
	QVERIFY(graph==snapshot);
	graph.edge(5,6);
	graph.remove(1);
	QVERIFY(graph.degree(node)==2);
	graph.remove(node);
	UndirectedGraph::NodeSet nodes=graph.nodes();
	QVERIFY(nodes.size()==2);
	for(const auto& n:nodes)
	{
		QVERIFY(graph.degree(n)==1);
	}
}

void GraphUnitTest::graphRemoveEdge()
{
	UndirectedGraph graph;
	const UndirectedGraph::Node n0(0);
	const UndirectedGraph::Node n1(1);
	const UndirectedGraph::Node n2(2);
	const UndirectedGraph::Node n3(3);
	graph.insert(n0,{n1,n2});
	try
	{
		graph.remove(n0,n0);
		QVERIFY(false);
	}
	catch(const UndirectedGraph::TrivialEdge& e)
	{
		QVERIFY(e.node()==n0);
	}
	try
	{
		graph.remove(n3,n0);
		QVERIFY(false);
	}
	catch(const UndirectedGraph::NoSuchNode& e)
	{
		QVERIFY(e.node()==n3);
	}
	try
	{
		graph.remove(n0,n3);
		QVERIFY(false);
	}
	catch(const UndirectedGraph::NoSuchNode& e)
	{
		QVERIFY(e.node()==n3);
	}
	try
	{
		graph.remove(n1,n2);
		QVERIFY(false);
	}
	catch(const UndirectedGraph::NoSuchEdge& e)
	{
		QVERIFY(e.edge()==std::make_pair(n1,n2));
	}
	try
	{
		graph.remove(n2,n1);
		QVERIFY(false);
	}
	catch(const UndirectedGraph::NoSuchEdge& e)
	{
		QVERIFY(e.edge()==std::make_pair(n2,n1));
	}
	graph.remove(n0,n1);
	QVERIFY(graph.degree(n0)==1);
	QVERIFY(graph.degree(n1)==0);
	QVERIFY(graph.degree(n2)==1);
}

template<class T>
T GraphUnitTest::buildDepthFirstTree() noexcept
{
	T graph;
	graph.insert(1,{6,2});
	graph.insert(2,{4,5,3});
	return graph;
}


template<class T>
T GraphUnitTest::buildBreadthFirstTree() noexcept
{
	T graph;
	graph.insert(1,{2,3});
	graph.insert(3,{5,4,6});
	return graph;
}

template<class T>
T GraphUnitTest::buildDepthFirstSegmented() noexcept
{
	T graph;
	graph.insert(1,{4,5,2});
	graph.insert(4,{5,3});
	graph.insert(5,{3});
	graph.insert(3,{2,6});
	graph.insert(2,{6});
	graph.insert(7,{9,8});
	return graph;
}

template<class T>
T GraphUnitTest::buildBreadthFirstSegmented() noexcept
{
	T graph;
	graph.insert(1,{3,2,4});
	graph.insert(3,{2,5});
	graph.insert(2,{5});
	graph.insert(5,{4,6});
	graph.insert(4,{6});
	graph.insert(7,{8,9});
	return graph;
}

void GraphUnitTest::graphConnectedComponents()
{
	const UndirectedGraph graph=buildDepthFirstSegmented<UndirectedGraph>();
	const UndirectedGraph::ConnectedComponentSet components=graph.connectedComponents();
	const UndirectedGraph::ConnectedComponentSet correct={{1,2,3,4,5,6},{7,8,9}};
	QVERIFY(equal(components,correct));
}

void GraphUnitTest::depthFirstVisitor()
{
	UndirectedGraph graph=buildDepthFirstTree<UndirectedGraph>();
	DepthFirstVisitor treeVisitor(graph);
	std::deque<Node> dft;
	for(auto it=treeVisitor.next();it!=treeVisitor.end();it=treeVisitor.next())
	{
		dft.push_back(*it.first);
	}
	QVERIFY(dft.size()==graph.size());
	QVERIFY(std::is_sorted(dft.begin(),dft.end()));
	dft.clear();
	graph=buildDepthFirstSegmented<UndirectedGraph>();
	DepthFirstVisitor visitor(graph);
	for(auto it=visitor.next();it!=visitor.end();it=visitor.next())
	{
		const Node& n=*it.first;
		QVERIFY(not ((n==7 or n==1) xor it.second));
		dft.push_back(n);
	}
	QVERIFY(dft.size()==graph.size());
	QVERIFY(std::is_sorted(dft.begin(),dft.end()));
}

void GraphUnitTest::breadthFirstVisitor()
{
	UndirectedGraph graph=buildBreadthFirstTree<UndirectedGraph>();
	BreadthFirstVisitor treeVisitor(graph);
	std::deque<Node> bft;
	for(auto it=treeVisitor.next();it!=treeVisitor.end();it=treeVisitor.next())
	{
		bft.push_back(*it.first);
	}
	QVERIFY(bft.size()==graph.size());
	QVERIFY(std::is_sorted(bft.begin(),bft.end()));
	bft.clear();
	graph=buildBreadthFirstSegmented<UndirectedGraph>();
	BreadthFirstVisitor visitor(graph);
	for(BreadthFirstVisitor::Element it=visitor.next();it!=visitor.end();it=visitor.next())
	{
		const Node& n=*it.first;
		QVERIFY(not ((n==7 or n==1) xor it.second));
		bft.push_back(n);
	}
	QVERIFY(bft.size()==graph.size());
	QVERIFY(std::is_sorted(bft.begin(),bft.end()));
}

void GraphUnitTest::increasingGraphConnectedComponents()
{
	const IncreasingUndirectedGraph graph=buildDepthFirstSegmented<IncreasingUndirectedGraph>();
	const IncreasingUndirectedGraph::ConnectedComponentSet expected=graph.UndirectedGraph::connectedComponents();
	const IncreasingUndirectedGraph::ConnectedComponentSet actual=graph.connectedComponents();
	QVERIFY(equal(actual,expected));
	for(const auto& l:actual)
	{
		QVERIFY(not l.empty());
		const auto& first=*l.begin();
		const IncreasingUndirectedGraph::ConnectedComponent& component=graph.component(first);
		for(const auto& m:l)
		{
			QVERIFY(&graph.component(m)==&component);
			QVERIFY(graph.sameComponent(first,m));
		}
		for(const auto& g:actual)
		{
			QVERIFY(not g.empty());
			if(&g!=&l)
			{
				QVERIFY(not graph.sameComponent(first,*g.begin()));
			}
		}
	}

}

QTEST_APPLESS_MAIN(GraphUnitTest)

#include "tst_GraphUnitTest.moc"
