#include "LocationConfiguration.h"

using namespace Configuration;

CLocationConfiguration::CLocationConfiguration(void)
{
	Clear();
}

CLocationConfiguration::~CLocationConfiguration(void)
{
	Clear();
}

/** Clears all configurations for this location */
void CLocationConfiguration::Clear(){
	m_locationNum = 0;
	for(int k = 0 ; k < MAX_N_LOCATIONS; ++k){
		this->m_location[k].Clear();
	}
}

/** Inserts a new location.
		@param loc - the location to insert 
*/
void CLocationConfiguration::InsertLocation(const CInstrumentLocation &loc){
	if(m_locationNum == MAX_N_LOCATIONS)
		return;

	// insert the location into the array
	m_location[m_locationNum] = loc;

	// increase the counter for the number of locationss
	++m_locationNum;

	return;	
}

/** Sets the properties of the location number 'm_locationNum'
		@param index - the index of the location to set. If this is < 0 this function returns 1 and nothing is changed
		@param location - the location to set
		@return 0 if successful, otherwise 1 */
int CLocationConfiguration::SetLocation(int index, const CInstrumentLocation &loc){
	if(index < 0)
		return 1;

	// replace the old location with the new information
	this->m_location[index] = loc;

	return 0;
}

/** Retrieves a location for this specrometer.
		@param index - the index of the location to get. If this is < 0 this function returns 1 and nothing is changed
		@param loc - the location to get
		@return 0 if successful, otherwise 1 */
int CLocationConfiguration::GetLocation(int index, CInstrumentLocation &loc) const{
	if(index < 0 || index >= m_locationNum)
		return 1;

	// copy the data to the requested parameter
	loc = m_location[index];

	return 0;
}

/** Gets the number of locations configured for this spectrometer */
unsigned long CLocationConfiguration::GetLocationNum() const{
	return m_locationNum;
}

int CLocationConfiguration::CheckSettings() const{

	// make sure that at least one fit-window is defined
	if(m_locationNum == 0)
		return 1;

	// Check the time ranges
	for(int k = 0; k < m_locationNum; ++k){
		// check that the time range is valid
		if(m_location[k].m_validFrom >= m_location[k].m_validTo){
			return 2;
		}
		// check if this time range overlaps some other 
		for(int j = k+1; j < m_locationNum; ++j){
			if((m_location[k].m_validFrom < m_location[j].m_validFrom) && (m_location[k].m_validTo > m_location[j].m_validFrom)){
				return 3;
			}else if((m_location[j].m_validFrom < m_location[k].m_validFrom) && (m_location[j].m_validTo > m_location[k].m_validFrom)){
				return 3;
			}
		}
	}

	return 0; // all is ok.
}