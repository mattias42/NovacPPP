#ifndef NOVAC_PPPLIB_CLIST_H
#define NOVAC_PPPLIB_CLIST_H

#include <list>

namespace novac
{
	template<class TYPE>
	struct POSITION
	{
		typename std::list<TYPE>::iterator m_position;
	};

	template<class TYPE, class ARG_TYPE = const TYPE&>
	class CList
	{
	public:

		// ---------------------- Construction -----------------------
		CList()
		{
		}

		~CList()
		{
		}

		int GetSize() const
		{
			return (int)m_data.size();
		}

		int GetCount() const
		{
			return (int)m_data.size();
		}

		POSITION<TYPE> GetHeadPosition() const
		{
			return POSITION<TYPE>(m_data.begin());
		}

		ARG_TYPE GetNext(POSITION<TYPE> p) const
		{
			return p.m_position++;
		}

		// ---------------------- Operations -----------------------

		void RemoveAll()
		{
			m_data.clear();
		}

		void AddTail(TYPE item)
		{
			m_data.push_back();
		}

	private:
		std::list<TYPE> m_data;
	};
}

#endif // !NOVAC_PPPLIB_CLIST_H
