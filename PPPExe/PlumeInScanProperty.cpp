#include "StdAfx.h"
#include "plumeinscanproperty.h"
#include "Common/Common.h"

CPlumeInScanProperty::CPlumeInScanProperty(void)
{
	this->Clear();
}

CPlumeInScanProperty::~CPlumeInScanProperty(void)
{
}

/** clears the properties of this data storage */
void CPlumeInScanProperty::Clear(){
	m_plumeCentre[0]		= NOT_A_NUMBER;
	m_plumeCentre[1]		= NOT_A_NUMBER;
	m_plumeCentreError[0]	= NOT_A_NUMBER;
	m_plumeCentreError[1]	= NOT_A_NUMBER;
	m_offset				= 0.0;
	m_completeness			= 0.0;
	m_plumeEdge_low			= NOT_A_NUMBER;
	m_plumeEdge_high		= NOT_A_NUMBER;
}

/** Assignment operator */
CPlumeInScanProperty &CPlumeInScanProperty::operator=(const CPlumeInScanProperty &p2){
	m_plumeCentre[0]		= p2.m_plumeCentre[0];
	m_plumeCentre[1]		= p2.m_plumeCentre[1];
	m_plumeCentreError[0]	= p2.m_plumeCentreError[0];
	m_plumeCentreError[1]	= p2.m_plumeCentreError[1];
	m_offset				= p2.m_offset;
	m_completeness			= p2.m_completeness;
	m_plumeEdge_low			= p2.m_plumeEdge_low;
	m_plumeEdge_high		= p2.m_plumeEdge_high;

	return *this;
}
