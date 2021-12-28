#pragma once

#include <PPPLib/MFC/CString.h>

#define MAX_OBSERVATORIES 35

/** The class <b>CObservatoryInfo</b> keeps track of the volcano observatories
    that we have defined. */
class CObservatoryInfo
{
public:
    CObservatoryInfo(void);
    ~CObservatoryInfo(void);

    /** The number of observatories */
    unsigned int m_observatoryNum;

    /** The name of the observatories */
    novac::CString m_name[MAX_OBSERVATORIES];

};
