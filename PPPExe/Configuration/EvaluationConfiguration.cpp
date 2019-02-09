#include "EvaluationConfiguration.h"

using namespace Configuration;

CEvaluationConfiguration::CEvaluationConfiguration(void)
{
}

CEvaluationConfiguration::~CEvaluationConfiguration(void)
{
	Clear();
}

void CEvaluationConfiguration::Clear(){
	this->m_fitWindows.RemoveAll();
	this->m_serial.Format("");
	this->m_validFrom.RemoveAll();
}

/** Inserts a new fit-window into the configuration for this spectrometer.
	@param window - the fit-window to insert 
	@param validFrom - the time from which this fit-window is valid, NULL if valid since the beginning of time
	@param validTo - the time to which this fit-window is valid, NULL if valid until the end of time */
void CEvaluationConfiguration::InsertFitWindow(const Evaluation::CFitWindow &window, const novac::CDateTime *validFrom, const novac::CDateTime *validTo){
	// make a copy of the fit - window
	Evaluation::CFitWindow *newWindow = new Evaluation::CFitWindow();
	*newWindow = window;
	
	// Get the time-range for which this window is valid
	novac::CDateTime *fromTime = new novac::CDateTime(0000, 00, 00, 00, 00, 00);
	novac::CDateTime *toTime = new novac::CDateTime(9999, 12, 31, 23, 59, 59);
	if(validFrom != NULL){
		*fromTime = *validFrom;
	}
	if(validTo != NULL){
		*toTime = *validTo;
	}
	
	// insert the fit-window into the array
	int length = m_fitWindows.GetCount();
	m_fitWindows.SetAtGrow(length, newWindow);
	m_validFrom.SetAtGrow(length, fromTime);
	m_validTo.SetAtGrow(length, toTime);
	
	return;
}

/** Sets the properties of the fit-window number 'index'
	@param index - the index of the configuration to set. If this is < 0 or
		larger than the number of fit-windows configured this function
		returns 1 and nothing is changed
	@param window - the fit-window to set 
	@param validFrom - the time from which this fit-window is valid, NULL if valid since the beginning of time
	@param validTo - the time to which this fit-window is valid, NULL if valid until the end of time 
	@return 0 if sucessful, otherwise 1 */
int CEvaluationConfiguration::SetFitWindow(int index, const Evaluation::CFitWindow &window, novac::CDateTime *validFrom, novac::CDateTime *validTo){
	if(index < 0)
		return 1;

	// make a copy of the fit - window
	Evaluation::CFitWindow *newWindow = new Evaluation::CFitWindow();
	*newWindow = window;
	
	// Get the time-range for which this window is valid
	novac::CDateTime *fromTime = new novac::CDateTime(0000, 00, 00, 00, 00, 00);
	novac::CDateTime *toTime = new novac::CDateTime(9999, 12, 31, 23, 59, 59);
	if(validFrom != NULL){
		*fromTime = *validFrom;
	}
	if(validTo != NULL){
		*toTime = *validTo;
	}
	
	// If there's an item already at this location then delete it
	int length = m_fitWindows.GetCount();
	if(index < length){
		Evaluation::CFitWindow *window = m_fitWindows.GetAt(index);
		novac::CDateTime *from = m_validFrom.GetAt(index);
		novac::CDateTime *to   = m_validTo.GetAt(index);
		
		delete window;
		delete from;
		delete to;
	}
	
	// insert the fit-window into the array
	m_fitWindows.SetAtGrow(index, newWindow);
	m_validFrom.SetAtGrow(index, fromTime);
	m_validTo.SetAtGrow(index, toTime);
	
	return 0;
}

/** Retrieves a fit-window from the configuration for this spectrometer.
	@param index - the index of the configuration to get. If this is < 0 or
		larger than the number of fit-windows configured this function
		returns 1 and the window is undefined
	@param window - the fit-window to insert 
	@param validFrom - the time from which this fit-window is valid, NULL if valid since the beginning of time
	@param validTo - the time to which this fit-window is valid, NULL if valid until the end of time 
	@return 0 if sucessful, otherwise 1 */
int CEvaluationConfiguration::GetFitWindow(int index, Evaluation::CFitWindow &window, novac::CDateTime &validFrom, novac::CDateTime &validTo) const{
	if(index < 0 || index >= m_fitWindows.GetSize())
		return 1;

	window    = *(m_fitWindows.GetAt(index));
	validFrom = *(m_validFrom.GetAt(index));
	validTo   = *(m_validTo.GetAt(index));

	return 0;
}

/** Gets the number of fit-windows configured for this spectrometer */
unsigned long CEvaluationConfiguration::GetFitWindowNum() const{
	return m_fitWindows.GetSize();
}


int CEvaluationConfiguration::CheckSettings() const{

	// make sure that at least one fit-window is defined
	int nWindows = m_fitWindows.GetSize();
	if(nWindows == 0)
		return 1;

	// Check the time ranges
	for(int k = 0; k < nWindows; ++k){
		// check that the time range is valid
		if(*m_validFrom[k] >=* m_validTo[k]){
			return 2;
		}
		// check if this time range overlaps some other 
		for(int j = k+1; j < nWindows; ++j){
			if(m_fitWindows[j]->channel != m_fitWindows[k]->channel){
				continue; // no use to compare master and slave...
			}else if(m_fitWindows[j]->name.compare(m_fitWindows[k]->name) != 0){
				continue; // if the windows have different names, then don't compare...
			}else{
				if((*m_validFrom[k] < *m_validFrom[j]) && (*m_validTo[k] > *m_validFrom[j])){
					return 3;
				}else if((*m_validFrom[j] < *m_validFrom[k]) && (*m_validTo[j] > *m_validFrom[k])){
					return 3;
				}
			}
		}
	}

	return 0; // all is ok.
}