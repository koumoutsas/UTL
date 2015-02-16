#ifndef SetOperations_DisjointSets_H
#define SetOperations_DisjointSets_H

#include <unordered_set>
#include <unordered_map>

namespace SetOperations
{

//! Union set implementation - This class maintains sets across union operations in almost constant time
//!(inverse α() amortized)
//! The class makes copies of all input elements
template<typename T>
class DisjointSets
{
public:
	DisjointSets() =default;
private:
	//! Base class for all exceptions thrown by DisjointSets
	class ElementException:public std::exception
	{
	public:
		ElementException(const T& x) noexcept:
			m_x(x)
		{
		}

		//! This method returns the element that caused the exception
		const T& element() const noexcept
		{
			return m_x;
		}
	private:
		const T m_x;
	};
public:
	//! Thrown by methods when one of the arguments used in a lookup doesn't exist
	class NoSuchElement:public ElementException
	{
	public:
		using ElementException::ElementException;
	};

	//! Thrown when trying to re-insert an element
	class ElementExists:public ElementException
	{
	public:
		using ElementException::ElementException;
	};

	//! Not thrown under normal operation,
	//! only during tests to signal that somewhere the implementation has left a dangling element
	class CorruptedParent:public ElementException
	{
		using ElementException::ElementException;
	};
private:
	//! Wrapper around the external element type. This class keep the value, the parent and the rank of an element
	class Element
	{
	public:
		Element(const T& x) noexcept:
			m_x(x),
			m_parent(x),
			m_rank(0)
		{
		}

		const T& parent() const noexcept
		{
			return m_parent;
		}

		//! ATENTION: Do not use this method directly. Use the parent(x,p) method of DisjointSets that maintains all
		//! structures correctly
		void parent(const T& p) const noexcept
		{
			m_parent=p;
		}

		unsigned int rank() const noexcept
		{
			return m_rank;
		}

		void increaseRank() const noexcept
		{
			++m_rank;
		}

		bool operator==(const Element& other) const noexcept
		{
			return m_x==other.m_x;
		}

		const T& element() const noexcept
		{
			return m_x;
		}
	private:
		const T m_x;

		mutable T m_parent;

		mutable unsigned int m_rank;
	};

	//! Struct so that Element can be used with hash sets
	class ElementHash
	{
	public:
		::std::size_t operator()(const Element& e) const
		{
			return ::std::hash<T>()(e.element());
		}
	};
public:
	/*!
	 * \brief add Add an element and keep it in its own set
	 * \param x
	 * \throw ElementExists if the element is already added
	 */
	void add(const T& x)
	{
		if(not m_elements.insert(x).second)
		{
			throw ElementExists(x);
		}
		m_sets[x].insert(x);
	}

	/*!
	 * \brief add Add an element and add it to an existing set
	 * \param x
	 * \param set
	 * \throw ElementExists if the element is already added
	 * \throw NoSuchElement If the set doesn't exist
	 */
	void add(const T& x, const T& set)
	{
		add(x);
		try
		{
			join(x,set);
		}
		catch(...)
		{
			m_sets.erase(x);
			m_elements.erase(x);
			throw;
		}
	}

	using ElementSet=std::unordered_set<T>;
private:
	using Container=std::unordered_set<Element,ElementHash>;
	using SetOfSets=std::unordered_map<T,ElementSet>;

	/*!
	 * \brief findHelper Helper method that does the lookup for an element
	 * \param x
	 * \return A reference to the element that is the representative of x's set
	 * \throw NoSuchElement If x is not in the set
	 */
	const Element& findHelper(const T& x) const
	{
		const typename Container::const_iterator it=m_elements.find(x);
		if(it==m_elements.end())
		{
			throw NoSuchElement(x);
		}
		const Element& original=*it;
		const Element* p=&original;
		while(not (p->parent()==p->element()))
		{
			const typename Container::const_iterator itP=m_elements.find(p->parent());
			if(itP==m_elements.end())
			{
				throw CorruptedParent(p->element());
			}
			p=&*itP;
		}
		const T& pElement=p->element();
		for(const Element* q=&original;not (q->parent()==q->element());)
		{
			const typename Container::const_iterator itQ=m_elements.find(q->parent());
			if(itQ==m_elements.end())
			{
				throw CorruptedParent(pElement);
			}
			parent(*q,pElement);
			q=&*itQ;
		}
		return *p;
	}
public:
	/*!
	 * \brief find Find the set an element belongs to
	 * \param x
	 * \return The representative element of the set
	 * \throw NoSuchElement If x is not in the set
	 */
	const T& find(const T& x) const
	{
		return findHelper(x).element();
	}

	/*!
	 * \brief join Join the sets two elements belong to. If they belong to the same set, no action is taken
	 * \param x
	 * \param y
	 * \throw NoSuchElement If x is not in the set
	 */
	void join(const T& x, const T& y)
	{
		const Element& xRoot=findHelper(x);
		const Element& yRoot=findHelper(y);
		if(xRoot==yRoot)
		{
			return;
		}
		const unsigned int xRank=xRoot.rank();
		const unsigned int yRank=yRoot.rank();
		if(xRank<yRank)
		{
			parent(xRoot,yRoot.element());
		}
		else if(yRank<xRank)
		{
			parent(yRoot,xRoot.element());
		}
		else
		{
			parent(yRoot,xRoot.element());
			xRoot.increaseRank();
		}
	}

	using ElementSets=std::vector<ElementSet>;

	//! Get all the sets. Note that this method copies all elements in the return value
	ElementSets sets() const noexcept
	{
		ElementSets result;
		result.reserve(m_sets.size());
		for(const auto& s:m_sets)
		{
			result.push_back(s.second);
		}
		return result;
	}

	//! Get the set of an element. This method returns a reference to the internal set structure,
	//! so modifying the sets afterwards can invalidate the returned value
	const ElementSet& set(const T& x) const
	{
		const typename SetOfSets::const_iterator it=m_sets.find(find(x));
		if(it==m_sets.end())
		{
			throw CorruptedParent(x);
		}
		return it->second;
	}
private:
	template<typename S>
	friend std::ostream& ::operator<<(std::ostream&,const SetOperations::DisjointSets<S>&);

	Container m_elements;
	mutable SetOfSets m_sets;

	//! Use this method to reparent an element safely - do not manipulate the element directly
	void parent(const Element& e,const T& newParent) const
	{
		const T& qElement=e.element();
		const T& qParent=e.parent();
		const typename SetOfSets::iterator qSetIt=m_sets.find(qParent);
		if(qSetIt==m_sets.end())
		{
			throw CorruptedParent(qParent);
		}
		ElementSet& qSet=qSetIt->second;
		const typename ElementSet::iterator qIt=qSet.find(qElement);
		if(qIt==qSet.end())
		{
			throw CorruptedParent(qElement);
		}
		qSet.erase(qIt);
		if(qSet.empty())
		{
			m_sets.erase(qSetIt);
		}
		e.parent(newParent);
		m_sets[newParent].insert(qElement);

	}
};

}

template<typename T>
std::ostream& operator<<(std::ostream& o,const SetOperations::DisjointSets<T>& s)
{
	for(const auto& n:s.m_elements)
	{
		o<<n.element()<<" -> "<<s.find(n.element())<<std::endl;
	}
	return o;
}

#endif // SetOperations_DisjointSets_H
