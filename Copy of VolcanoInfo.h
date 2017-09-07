#pragma once

/** The <b>CVolcanoInfo</b>-class is a class that stores known information
		about a set of volcanoes. This information can then later be used in the program
		for various purposes.*/

#define MAX_VOLCANOES 65

class CVolcanoInfo
{
public:
	CVolcanoInfo(void);
	~CVolcanoInfo(void);
	
	// ----------------------------------------------------------------------
	// ---------------------- PUBLIC DATA -----------------------------------
	// ----------------------------------------------------------------------

	/** The number of volcanoes that are configured */
	unsigned int		m_volcanoNum;

	/** The name of the volcano(es) */
	CString				m_name[MAX_VOLCANOES];

	/** The simplified name of the volcano(es) */
	CString				m_simpleName[MAX_VOLCANOES];

	/** The number of the volcano(es). This is from the 
		Smithsonian's inventory of the worlds volcanoes 
		http://www.volcano.si.edu */
	CString				m_number[MAX_VOLCANOES];
	
	CString				m_country[MAX_VOLCANOES];

	/** The latitude of the peak(s) */
	double				m_peakLatitude[MAX_VOLCANOES];

	/** The longitude of the peak(s) */
	double				m_peakLongitude[MAX_VOLCANOES];

	/** The altitude of the peak(s) (masl) */
	double				m_peakHeight[MAX_VOLCANOES];

	/** The number of hours to GMT, used to calculate the local-time from the GPS-time */
	double				m_hoursToGMT[MAX_VOLCANOES];

	/** The observatory in charge of this volcano */
	int					m_observatory[MAX_VOLCANOES];

	/** The number of volcanoes that are configured by the program	
			(if m_preConfiguredVolcanoNum > m_volcanoNum then the user has added a volcano) */
	unsigned int	m_preConfiguredVolcanoNum;

	// ----------------------------------------------------------------------
	// --------------------- PUBLIC METHODS ---------------------------------
	// ----------------------------------------------------------------------

	/** Retrieves the name of the volcano with the given index */
	static void GetVolcanoName(unsigned int index, CString &name);

	/** Retrieves the code of the volcano with the given index */
	static void GetVolcanoCode(unsigned int index, CString &code);

	/** Retrieves the simplified name of the volcano with the given index */
	static void GetSimpleVolcanoName(unsigned int index, CString &name);

	/** Retrieves the volcano index from a given name */
	static int GetVolcanoIndex(const CString &name);

private:
	// ----------------------------------------------------------------------
	// ---------------------- PRIVATE DATA ----------------------------------
	// ----------------------------------------------------------------------

	// ----------------------------------------------------------------------
	// --------------------- PRIVATE METHODS --------------------------------
	// ----------------------------------------------------------------------

};
