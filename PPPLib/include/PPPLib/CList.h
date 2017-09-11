#ifndef NOVAC_PPPLIB_CLIST_H
#define NOVAC_PPPLIB_CLIST_H

#include <list>

namespace novac
{
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

    private:
        std::list<TYPE> m_data;
    };
}

#endif // !NOVAC_PPPLIB_CLIST_H
