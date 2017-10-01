#ifndef NOVAC_PPPLIB_CLIST_H
#define NOVAC_PPPLIB_CLIST_H

#include <list>
#include <cassert>

namespace novac
{
	template<class TYPE>
	struct POSITION
	{
	public:
		typename std::list<TYPE>::const_iterator m_position;

		POSITION()
			: m_emptyList(), m_data(m_emptyList)
		{
			m_position = m_data.cbegin();
		}
		
		POSITION(const std::list<TYPE>& data)
			: m_emptyList(), m_data(data)
		{
			m_position = m_data.cbegin();
		}

		POSITION(TYPE* )
			: m_emptyList(), m_data(m_emptyList)
		{
			m_position = m_data.cbegin();
		}

		bool HasNext() const
		{
			if (m_data.size() == 0)
			{
				return false;
			}
			else
			{
				return m_position != m_data.end(); 
			}
		}

		bool operator==(void* data)
		{
			return (nullptr == data) ? (!this->HasNext()) : false;
		}

		bool operator!=(void* data)
		{
			return (nullptr == data) ? (this->HasNext()) : true;
		}

	private:
		const std::list<TYPE> m_emptyList;
		const std::list<TYPE>& m_data;
	};

	template<class TYPE>
	struct REVERSE_POSITION
	{
	public:
		typename std::list<TYPE>::const_reverse_iterator  m_position;

		REVERSE_POSITION()
			: m_emptyList(), m_data(m_emptyList)
		{
			m_position = m_data.crbegin();
		}

		REVERSE_POSITION(const std::list<TYPE>& data)
			: m_emptyList(), m_data(data)
		{
			m_position = data.crbegin();
		}

		REVERSE_POSITION(void*)
			: m_emptyList(), m_data(m_emptyList)
		{
			m_position = m_data.crbegin();
		}

		bool HasPrevious() const
		{
			if (m_data.size() == 0)
			{
				return false;
			}
			else
			{
				return m_position != m_data.crend();
			}
		}

		bool operator==(void* data)
		{
			return (nullptr == data) ? (!this->HasPrevious()) : false;
		}

		bool operator!=(void* data)
		{
			return (nullptr == data) ? (this->HasPrevious()) : true;
		}

	private:
		const std::list<TYPE> m_emptyList;
		const std::list<TYPE>& m_data;
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
			POSITION<TYPE> p(m_data);
			return p;
		}

		POSITION<TYPE> GetHeadPosition() const
		{
			POSITION<TYPE> p(m_data);
			return p;
		}

		REVERSE_POSITION<TYPE> GetTailPosition()
		{
			REVERSE_POSITION<TYPE> p(m_data);
			return p;
		}

		ARG_TYPE GetAt(POSITION<TYPE>& p) const
		{
			return *(p.m_position);
		}

		ARG_TYPE GetAt(REVERSE_POSITION<TYPE>& p) const
		{
			return *(p.m_position);
		}

		ARG_TYPE GetNext(POSITION<TYPE>& p) const
		{
			ARG_TYPE data = *p.m_position;
			++(p.m_position);
			return data;
		}

		ARG_TYPE GetPrev(REVERSE_POSITION<TYPE>& p) const
		{
			ARG_TYPE data = *p.m_position;
			++(p.m_position);
			return data;
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
