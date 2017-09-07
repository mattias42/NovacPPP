#include "StdAfx.h"
#include "CrossSectionData.h"
#include "../Common/Common.h"
#include "BasicMath.h"

using namespace Evaluation;

CCrossSectionData::CCrossSectionData(void)
{
	m_length = 0;
	m_crossSection.assign(2048, 0.0); // make an initial guess how long the cross sections are
	m_waveLength.assign(2048, 0.0); // make an initial guess how long the cross sections are
}

CCrossSectionData::~CCrossSectionData(void)
{
	m_waveLength.clear();
	m_crossSection.clear();
	m_length = 0;
}

/** Sets the reference information at the given pixel */
void CCrossSectionData::SetAt(int index, double wavel, double value){
	if(index < 0)
		return;
		
	this->m_waveLength.at(index) = wavel;
	this->m_crossSection.at(index) = value;
	this->m_length = max((unsigned long)index, m_length);
}

/** Sets the cross-section information to the values in the 
	supplied array */
void CCrossSectionData::Set(double *wavelength, double *crossSection, unsigned long pointNum){
	this->m_length = pointNum;
	m_waveLength.resize(pointNum);
	m_crossSection.resize(pointNum);
	
	for(unsigned int k = 0; k < pointNum; ++k){
		this->m_waveLength.at(k) = wavelength[k];
		this->m_crossSection.at(k) = crossSection[k];
	}
}

/** Sets the cross-section information to the values in the 
	supplied array */
void CCrossSectionData::Set(double *crossSection, unsigned long pointNum){
	this->m_length = pointNum;
	m_waveLength.resize(pointNum);
	m_crossSection.resize(pointNum);

	for(unsigned int k = 0; k < pointNum; ++k){
		double lambda = (double)k;
		this->m_waveLength.at(k) = lambda;

		this->m_crossSection.at(k) = crossSection[k];
	}
}

/** Sets the cross-section information to the values in the 
	supplied array */
void CCrossSectionData::Set(MathFit::CVector &crossSection, unsigned long pointNum){
	this->m_length = pointNum;
	m_waveLength.resize(pointNum);
	m_crossSection.resize(pointNum);

	for(unsigned int k = 0; k < pointNum; ++k){
		double value = crossSection.GetAt(k);

		double lambda = (double)k;
		this->m_waveLength.at(k) = lambda;

		this->m_crossSection.at(k) = value;
	}
}

/** Gets the cross section at the given pixel */
double CCrossSectionData::GetAt(unsigned int index) const{
	if(index >= m_length)
		return 0.0;
	else{
		return m_crossSection.at(index);
	}
}

unsigned long CCrossSectionData::GetSize() const{
	return this->m_length;
}

/** Gets the wavelength at the given pixel */
double CCrossSectionData::GetWavelengthAt(unsigned int index) const{
	if(index >= this->m_length)
		return 0.0;
	else{
		return m_waveLength.at(index);
	}
}

/** Reads the cross section from a file */
int CCrossSectionData::ReadCrossSectionFile(const CString &fileName){
	CFileException exceFile;
	CStdioFile fileRef;
	char szLine[4096];
	int valuesReadNum, nColumns;
	double fValue1[MAX_SPECTRUM_LENGTH];
	double fValue2[MAX_SPECTRUM_LENGTH];
	
	if(!fileRef.Open(fileName, CFile::modeRead | CFile::typeText, &exceFile))
	{
		CString str;
		str.Format("ERROR: Cannot open reference file: %s", fileName);
		ShowMessage(str);
		return 1; /* Failed to open the file */
	}

	valuesReadNum = 0;

	// read reference spectrum into the 'fValue's array
	while(fileRef.ReadString(szLine, 4095))
	{
		// this construction enables us to read files with both one or two columns
		nColumns = sscanf(szLine, "%lf\t%lf", &fValue1[valuesReadNum], &fValue2[valuesReadNum]);

		// check so that we actually could read the data
		if(nColumns < 1 || nColumns > 2)
			break;

		++valuesReadNum;

		// if we have read as much as we can, return
		if(valuesReadNum == MAX_SPECTRUM_LENGTH)
			break;
	}
	fileRef.Close();

	m_length = valuesReadNum;
	m_waveLength.resize(m_length);
	m_crossSection.resize(m_length);

	if(nColumns == 1){
		// If there's no wavelength column in the cross-section file
		for(int index = 0; index < valuesReadNum; ++index){
			double lambda = (double)index;
			
			m_waveLength.at(index)	 = lambda;
			m_crossSection.at(index) = fValue1[index];
		}
	}else{
		// If there's a wavelength column in the cross-section file
		for(int index = 0; index < valuesReadNum; ++index){
			m_waveLength.at(index)   = fValue1[index];
			m_crossSection.at(index) = fValue2[index];
		}
	}

	return 0;
}

/** Performs a high-pass filtering of this cross section file */
int CCrossSectionData::HighPassFilter(){
	CBasicMath mathObject;
	
	// copy the data
	double *data = new double[m_length];
	for(unsigned long k = 0; k < m_length; ++k){
		data[k] = this->GetAt(k);
	}

	mathObject.Mul(data, m_length, -2.5e15);
	mathObject.Delog(data, m_length);
	mathObject.HighPassBinomial(data, m_length, 500);
	mathObject.Log(data, m_length);
	mathObject.Div(data, m_length, 2.5e15);
	
	for(int k = 0; k < m_length; ++k){
		this->SetAt(k, k, data[k]);
	}
	
	delete data;

	return 0;
}

int CCrossSectionData::HighPassFilter_Ring(){
	CBasicMath mathObject;
	
	// copy the data
	double *data = new double[m_length];
	for(unsigned long k = 0; k < m_length; ++k){
		data[k] = this->GetAt(k);
	}

	mathObject.HighPassBinomial(data, m_length, 500);
	mathObject.Log(data, m_length);
	
	for(int k = 0; k < m_length; ++k){
		this->SetAt(k, k, data[k]);
	}
	
	delete data;

	return 0;
}


/** Multiplies this cross section with the given constant */
int CCrossSectionData::Multiply(double scalar){
	CBasicMath mathObject;
	
	// copy the data
	double *data = new double[m_length];
	for(unsigned long k = 0; k < m_length; ++k){
		data[k] = this->GetAt(k);
	}

	mathObject.Mul(data, m_length, scalar);
	
	for(int k = 0; k < m_length; ++k){
		this->SetAt(k, k, data[k]);
	}
	
	delete data;

	return 0;
}

int CCrossSectionData::Log(){
	CBasicMath mathObject;
	
	// copy the data
	double *data = new double[m_length];
	for(unsigned long k = 0; k < m_length; ++k){
		data[k] = this->GetAt(k);
	}

	mathObject.Log(data, m_length);
	
	for(int k = 0; k < m_length; ++k){
		this->SetAt(k, k, data[k]);
	}
	
	delete data;

	return 0;
}

/** Assignment operator*/
CCrossSectionData &CCrossSectionData::operator=(const CCrossSectionData &xs2){
	this->m_length = xs2.m_length;
	
	// make sure the arrays are the same size
	this->m_crossSection.resize(xs2.m_crossSection.size());
	this->m_waveLength.resize(xs2.m_waveLength.size());

	// copy the data of the arrays
	for(unsigned int i = 0; i < m_length; ++i){
		double lambda = xs2.GetWavelengthAt(i);
		double value = xs2.GetAt(i);

		this->m_crossSection.at(i) = value;
		this->m_waveLength.at(i) = lambda;
	}
	
	return *this;
}
