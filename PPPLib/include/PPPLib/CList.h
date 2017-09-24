#ifndef NOVAC_PPPLIB_CLIST_H
#define NOVAC_PPPLIB_CLIST_H

#include <list>

namespace novac
{
	template<class TYPE>
	struct POSITION
	{
	public:
		typename std::list<TYPE>::iterator* m_position;

		POSITION(typename std::list<TYPE>::iterator* pos, std::list<TYPE>* data)
			: m_position(pos), m_data(data)
		{
		}

		bool HasNext() const { return (*(*m_position)) < m_data->end(); }

	private:
		std::list<TYPE>* m_data;
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

		POSITION<TYPE> GetHeadPosition()
		{
			return POSITION<TYPE>{m_data.begin(), m_data};
		}

		POSITION<TYPE> GetTailPosition()
		{
			POSITION<TYPE> p(&(--m_data.end()), &m_data);
			return p;
		}

		ARG_TYPE GetAt(POSITION<TYPE> p) const
		{
			return *(*(p.m_position));
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

		void RemoveTail()
		{
			m_data.erase(--m_data.end());
		}

		void AddTail(TYPE item)
		{
			m_data.push_back(item);
		}

	private:
		std::list<TYPE> m_data;
	};
}

#endif // !NOVAC_PPPLIB_CLIST_H
