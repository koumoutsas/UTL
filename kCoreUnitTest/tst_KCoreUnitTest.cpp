#include <QtTest>
#include "KCore.h"

using namespace Graph;

class KCoreUnitTest:public QObject
{
	Q_OBJECT
public:
	KCoreUnitTest();
private Q_SLOTS:
	void kCore();
	void kCoreBenchmark();
private:
	using UndirectedGraph=Graph::UndirectedGraph<unsigned int>;
	UndirectedGraph randomGraph(const unsigned int numberOfNodes,const double averageDegree);
};

KCoreUnitTest::KCoreUnitTest()
{
}

void KCoreUnitTest::kCore()
{
	UndirectedGraph graph;
	graph.insert(1,{2,3});
	graph.insert(4,{1,2,3});
	for(unsigned int i=0;i<3;++i)
	{
		const KCore<unsigned int>::NodeSet core=KCore<unsigned int>(graph,i)();
		QVERIFY(core==graph.nodes() and core.size()==4);
	}
	QVERIFY(KCore<unsigned int>(graph,3)().empty());
	graph.edge(2,3);
	for(unsigned int i=0;i<4;++i)
	{
		const KCore<unsigned int>::NodeSet core=KCore<unsigned int>(graph,i)();
		QVERIFY(core==graph.nodes() and core.size()==4);
	}
	graph.remove(2,3);
	graph.insert(5,{1,4});
	graph.insert(6,{2,4,5});
	QVERIFY(KCore<unsigned int>(graph,3)()==KCore<unsigned int>::NodeSet({1,2,4,5,6}));
}

KCoreUnitTest::UndirectedGraph KCoreUnitTest::randomGraph(const unsigned int numberOfNodes,const double averageDegree)
{
	const unsigned int pickProbability=RAND_MAX*averageDegree/(numberOfNodes-1);
	std::srand(time(nullptr));
	UndirectedGraph result;
	for(unsigned int i=0;i<numberOfNodes;++i)
	{
		UndirectedGraph::AdjacencyList l;
		for(unsigned int j=0;j<numberOfNodes;++j)
		{
			if(j!=i)
			{
				if(unsigned(std::rand())<=pickProbability)
				{
					l.insert(j);
				}
			}
		}
		result.insert(i,l);
	}
	return result;
}

void KCoreUnitTest::kCoreBenchmark()
{
	const unsigned int numberOfNodes=1000;
	const unsigned int averageDegree=100;
	const unsigned int k=15;
	const UndirectedGraph graph=randomGraph(numberOfNodes,averageDegree);
	QBENCHMARK
	{
		KCore<unsigned int>(graph,k);
	}
}

QTEST_APPLESS_MAIN(KCoreUnitTest)

#include "tst_KCoreUnitTest.moc"
