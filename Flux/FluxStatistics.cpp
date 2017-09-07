#include "StdAfx.h"
#include "FluxStatistics.h"

using namespace Flux;

CFluxStatistics::CMeasurementDay::CMeasurementDay(){

}
CFluxStatistics::CMeasurementDay::~CMeasurementDay(){

}
CFluxStatistics::CMeasurementDay &CFluxStatistics::CMeasurementDay::operator =(const CFluxStatistics::CMeasurementDay &m){
	this->day = m.day;
	
	POSITION p = m.fluxList.GetHeadPosition();
	while(p != NULL){
		fluxList.AddTail(CFluxResult(m.fluxList.GetNext(p)));
	}
	
	return *this;
}

void CFluxStatistics::CMeasurementDay::GetHeaderLine(CString &str, CList <CString, CString &> &instruments){
	// the statistics of the fluxes
	str.Format("Date\tAverageFlux(kg/s)\tMedianFlux(kg/s)\tStdFlux(kg/s)\tnMeasurements");
	
	// the statistics of the instruments
	POSITION p = instruments.GetHeadPosition();
	while(p != NULL){
		str.AppendFormat("\t#MeasFrom_%s", instruments.GetNext(p));
	}
	str.AppendFormat("\n");
	
	return;
}

void CFluxStatistics::CMeasurementDay::GetStatistics(CString &str, CList <CString, CString &> &instruments){
	double average, median, std;
	long nMeasurements	= (long)this->fluxList.GetCount();
	long nInstruments	= (long)instruments.GetCount();
	double *data		= new double[nMeasurements];
	double *sortedData	= new double[nMeasurements];
	int nMeasurementsFromThisInstrument = 0;
	int k = 0;
	POSITION p, p2;
	
	// Write the day
	str.Format("%04d.%02d.%02d\t", day.year, day.month, day.day);
	
	// copy the data into the array
	p = fluxList.GetHeadPosition();
	while(p != NULL){
		data[k++] = fluxList.GetNext(p).m_flux;
	}
	
	// get the statistical data
	average = Average(data, nMeasurements);
	std		= Std(data, nMeasurements);
	
	// sort the data to get the median
	FindNLowest(data, nMeasurements, sortedData, nMeasurements);
	if(nMeasurements % 2 == 0)
		median = Average(sortedData + nMeasurements / 2-1, 2);
	else
		median = sortedData[nMeasurements / 2 + 1];
	
	// write what we now know to the string
	str.AppendFormat("%.2lf\t%.2lf\t%.2lf\t%d", average, median, std, nMeasurements);
	
	// go through the data and see how many points we have from each instrument
	p2 = instruments.GetHeadPosition();
	while(p2 != NULL){
		CString &serial = instruments.GetNext(p2);
		nMeasurementsFromThisInstrument = 0;
		POSITION p = fluxList.GetHeadPosition();
		while(p != NULL){
			CFluxResult &flux = fluxList.GetNext(p);
			if(Equals(flux.m_instrument, serial)){
				++nMeasurementsFromThisInstrument;
			}
		}
		
		str.AppendFormat("\t%d", nMeasurementsFromThisInstrument);
	}
	str.AppendFormat("\n");
	
	
	delete [] data;
	delete [] sortedData;
	return;
}


CFluxStatistics::CFluxStatistics(void)
{
	this->Clear();
}

CFluxStatistics::~CFluxStatistics(void)
{
	this->Clear();
}


/** Clears all information here */
void CFluxStatistics::Clear(){
	m_measurements.RemoveAll();
}

/** Attaches the supplied list of flux results to the current set
	of measured data. */
void CFluxStatistics::AttachFluxList(const CList <CFluxResult, CFluxResult &> &calculatedFluxes){
	POSITION p = calculatedFluxes.GetHeadPosition();
	while(p != NULL){
		AttachFlux(calculatedFluxes.GetNext(p));
	}
}

/** Attaches the given flux result to the current set of 
	measured data */
void CFluxStatistics::AttachFlux(const CFluxResult &result){
	CFluxResult r = result; // make a local copy of the result
	CMeasurementDay measday;
	CDateTime resultDay = CDateTime(result.m_startTime.year, result.m_startTime.month, result.m_startTime.day, 0, 0, 0);
	POSITION p;

	// find out if we know about this instrument
	bool foundInstrument = false;
	p = m_instruments.GetHeadPosition();
	while(p != NULL){
		if(Equals(m_instruments.GetNext(p), result.m_instrument)){
			foundInstrument = true;
			break;
		}
	}
	if(!foundInstrument){
		m_instruments.AddHead(CString(result.m_instrument));
	}

	// Loop through the list of measurementdays to find the 
	//	right place for the result and insert it.
	p = m_measurements.GetHeadPosition();
	while(p != NULL){
		CMeasurementDay &d = m_measurements.GetAt(p);
		
		if(d.day == resultDay){
			// insert the result on this day.
			d.fluxList.AddTail(r);
			return;
		}else if(resultDay < d.day){
			// insert the result at the position before this day
			measday.day = resultDay;
			measday.fluxList.AddTail(r);
			m_measurements.InsertBefore(p, measday);
			return;
		}
		
		// move on to the next measurement day
		m_measurements.GetNext(p);
	}
	
	// we've passed the whole list without finding anything that's larger than this day
	//	insert the result as a new measurement day in the end of the list
	measday.day = resultDay;
	measday.fluxList.AddTail(r);
	m_measurements.AddTail(measday);
	
	return;
}

/** Calculates statistics on the statistics we have here and writes 
	the results to file. */		
void CFluxStatistics::WriteFluxStat(const CString &fileName){
	CString str;
	FILE *f = NULL;

	// try to open the file
	if(IsExistingFile(fileName)){
		f = fopen(fileName, "a");
		if(f == NULL)
			return;
	}else{
		f = fopen(fileName, "w");
		if(f == NULL)
			return;

		// write the header line
		CMeasurementDay::GetHeaderLine(str, this->m_instruments);
		fprintf(f, str);
	}

	// For each day in the list of measurement days, calculate the average flux
	//	write the number of measurements made ...
	POSITION p = m_measurements.GetHeadPosition();
	while(p != NULL){
		CMeasurementDay &measDay = m_measurements.GetNext(p);
		
		measDay.GetStatistics(str, this->m_instruments);
		
		fprintf(f, str);			
	}


	// remember to close the file
	fclose(f);
}

