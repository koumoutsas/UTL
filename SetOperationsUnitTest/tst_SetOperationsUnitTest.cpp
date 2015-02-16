#include <QtTest>
#include "DisjointSets.h"

using namespace SetOperations;

template<typename T>
using unordered_set=std::unordered_set<T>;

template<typename T>
using vector=std::vector<T>;

class TestElement
{
public:
	TestElement(const int x) noexcept:
		m_x(x)
	{
	}

	operator int() const noexcept
	{
		return m_x;
	}

	bool operator==(const TestElement& other) const noexcept
	{
		return m_x==other.m_x;
	}

	bool operator!=(const TestElement& other) const noexcept
	{
		return not (*this==other);
	}

	bool operator==(const int i) const noexcept
	{
		return m_x==i;
	}
private:
	int m_x;
};

namespace std
{

template<>
struct hash<TestElement>
{
	std::size_t operator()(const TestElement& e) const noexcept
	{
		return std::hash<int>()(e);
	}
};

}

bool operator==(const DisjointSets<TestElement>::ElementSet& l1,const std::unordered_set<int>& l2)
{
	DisjointSets<TestElement>::ElementSet l3;
	for(const auto& i:l2)
	{
		l3.insert(i);
	}
	return l1==l3;
}

bool operator==(const std::unordered_set<int>& l1,const DisjointSets<TestElement>::ElementSet& l2)
{
	return l2==l1;
}

class SetOperationsUnitTest:public QObject
{
	Q_OBJECT	
public:
	SetOperationsUnitTest();
private Q_SLOTS:
	void disjointSetsAdd();
	void disjointSetsFind();
	void disjointSetsJoin();
	void disjointSetsAndJoin();
	void disjointSetsSets();
	void disjointSetsSet();
	void disjointSets();
private:
	template<typename T,template<typename> class S>
	static bool disjoint(const std::vector<S<T>>& sets)
	{
		typename S<T>::size_type total=0;
		S<T> all;
		for(const auto& s:sets)
		{
			all.insert(s.begin(),s.end());
			total+=s.size();
			if(all.size()!=total)
			{
				return false;
			}
		}
		return true;
	}

	template<typename T,template<typename> class S,template<typename> class C>
	static C<T> flatten(const C<S<T>>& container)
	{
		C<T> result;
		for(const auto& next:container)
		{
			for(const auto& e:next)
			{
				result.insert(result.end(),e);
			}
		}
		return result;
	}

	using IntVector=vector<int>;
	using IntSet=unordered_set<int>;
	using SetVector=vector<IntSet>;

	const SetVector m_all={{2,5,10,56},{1,32,101},{0},{-10,3}};

	inline DisjointSets<TestElement> createComplex();
};

SetOperationsUnitTest::SetOperationsUnitTest()
{
}

std::ostream& operator<<(std::ostream& o,const TestElement& e) noexcept
{
	return o<<e;
}

void SetOperationsUnitTest::disjointSetsAdd()
{
	DisjointSets<int> sets;
	sets.add(0);
	try
	{
		sets.add(0);
		QVERIFY(false);
	}
	catch(const DisjointSets<int>::ElementExists& e)
	{
		QVERIFY(e.element()==0);
	}
	const int f=sets.find(0);
	QVERIFY(f==0);
}

void SetOperationsUnitTest::disjointSetsFind()
{
	DisjointSets<int> sets;
	sets.add(0);
	try
	{
		sets.find(1);
		QVERIFY(false);
	}
	catch(const DisjointSets<int>::NoSuchElement& e)
	{
		QVERIFY(e.element()==1);
	}
}

void SetOperationsUnitTest::disjointSetsJoin()
{
	DisjointSets<TestElement> sets;
	sets.add(0);
	try
	{
		sets.join(0,1);
		QVERIFY(false);
	}
	catch(const DisjointSets<TestElement>::NoSuchElement& e)
	{
		QVERIFY(e.element()==1);
	}
	try
	{
		sets.join(1,0);
		QVERIFY(false);
	}
	catch(const DisjointSets<TestElement>::NoSuchElement& e)
	{
		QVERIFY(e.element()==1);
	}
	sets.add(1);
	sets.add(2);
	sets.add(3);
	sets.add(4);
	sets.join(1,2);
	sets.join(2,3);
	sets.join(0,4);
	const TestElement& set0=sets.find(0);
	const TestElement& set1=sets.find(1);
	QVERIFY(sets.find(4)==set0);
	QVERIFY(sets.find(2)==set1);
	QVERIFY(sets.find(3)==set1);
}

void SetOperationsUnitTest::disjointSetsAndJoin()
{
	DisjointSets<TestElement> sets;
	sets.add(0);
	try
	{
		sets.add(1,2);
		QVERIFY(false);
	}
	catch(const DisjointSets<TestElement>::NoSuchElement& e)
	{
		QVERIFY(e.element()==2);
	}
	sets.add(1,0);
	QVERIFY(sets.find(0)==sets.find(1));
}

void SetOperationsUnitTest::disjointSetsSets()
{
	const DisjointSets<TestElement>::ElementSets lSets=createComplex().sets();
	QVERIFY(lSets.size()==m_all.size());
	QVERIFY(std::is_permutation(lSets.begin(),lSets.end(),m_all.begin()));
}

void SetOperationsUnitTest::disjointSetsSet()
{
	const DisjointSets<TestElement> sets=createComplex();
	for(const auto& s:m_all)
	{
		QVERIFY(sets.set(*s.begin())==s);
	}
}

DisjointSets<TestElement> SetOperationsUnitTest::createComplex()
{
	const IntVector elements=flatten<int,unordered_set,vector>(m_all);
	DisjointSets<TestElement> sets;
	for(const auto& e:elements)
	{
		sets.add(e);
	}
	for(const auto& next:m_all)
	{
		const TestElement& first=*next.begin();
		for(const auto& e:next)
		{
			sets.join(e,first);
		}
	}
	return sets;
}

void SetOperationsUnitTest::disjointSets()
{
	const DisjointSets<TestElement> sets=createComplex();
	unordered_set<TestElement> firstElements;
	for(const auto& next:m_all)
	{
		const TestElement& first=sets.find(*next.begin());
		for(const auto& e:next)
		{
			QVERIFY(sets.find(e)==first);
		}
		QVERIFY(firstElements.insert(first).second);
	}
}

QTEST_APPLESS_MAIN(SetOperationsUnitTest)

#include "tst_SetOperationsUnitTest.moc"
