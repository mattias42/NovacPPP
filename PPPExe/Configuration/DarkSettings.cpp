#include "DarkSettings.h"

using namespace Configuration;

CDarkSettings::CDarkSettings(){
	this->Clear();
}

CDarkSettings::~CDarkSettings(){
	this->Clear();
}

void CDarkSettings::Clear(){
	m_darkSpecOption		= MEASURE;

	m_darkCurrentOption = MEASURED;
	m_darkCurrentSpec.Format("");

	m_offsetOption		  = MEASURED;
	m_offsetSpec.Format("");
}

CDarkSettings& CDarkSettings::operator =(const CDarkSettings &dark2){
	m_darkSpecOption		= dark2.m_darkSpecOption;

	m_darkCurrentOption = dark2.m_darkCurrentOption;
	m_offsetOption			= dark2.m_offsetOption;

	m_darkCurrentSpec.Format(dark2.m_darkCurrentSpec);
	m_offsetSpec.Format(dark2.m_offsetSpec);

	return *this;
}