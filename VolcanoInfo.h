#pragma once

#include <vector>

/** The <b>CVolcanoInfo</b>-class is a class that stores known information
		about a set of volcanoes. This information can then later be used in the program
		for various purposes.*/

class CVolcanoInfo
{
public:
	/** Default constructor */
	CVolcanoInfo(void);

	/** Default destructor */
	~CVolcanoInfo(void);
	
	// -----------------------------------------------------------
	// ---------------------- PUBLIC DATA ------------------------
	// -----------------------------------------------------------

	/** The number of volcanoes that are configured */
	unsigned int		m_volcanoNum;

	/** This is the number of volcanoes that are automatically
		configured. Any additional volcanoes are configured
		by the user... */
	unsigned int		m_preConfiguredVolcanoNum;

	// -------------------------------------------------------------
	// --------------------- PUBLIC METHODS ------------------------
	// -------------------------------------------------------------

	/** Adds a new volcano to the list */
	void AddVolcano(const CString &name, const CString &number, const CString &country, double latitude, double longitude, double altitude, double hoursToGMT = 0.0, int observatory = 1);
	void UpdateVolcano(unsigned int index, const CString &name, const CString &number, const CString &country, double latitude, double longitude, double altitude, double hoursToGMT = 0.0, int observatory = 1);

	/** Retrieves the name of the volcano with the given index */
	void GetVolcanoName(unsigned int index, CString &name);
	const CString &GetVolcanoName(unsigned int index);

	/** Retrieves the code of the volcano with the given index */
	void GetVolcanoCode(unsigned int index, CString &code);
	const CString &GetVolcanoCode(unsigned int index);

	/** Retrieves the location of the volcano */
	void GetVolcanoLocation(unsigned int index, CString &location);
	const CString &GetVolcanoLocation(unsigned int index);

	/** Retrieves the simplified name of the volcano with the given index */
	void GetSimpleVolcanoName(unsigned int index, CString &name);
	const CString &GetSimpleVolcanoName(unsigned int index);

	/** Retrieves the volcano index from a given name (or code) */
	int GetVolcanoIndex(const CString &name);
	
	/** Retrieves the volcano position from the given index */
	double GetPeakLatitude(unsigned int index);
	double GetPeakLatitude(const CString &name){ return GetPeakLatitude(GetVolcanoIndex(name)); }
	double GetPeakLongitude(unsigned int index);
	double GetPeakLongitude(const CString &name){ return GetPeakLongitude(GetVolcanoIndex(name)); }
	double GetPeakAltitude(unsigned int index);
	double GetPeakAltitude(const CString &name){ return GetPeakAltitude(GetVolcanoIndex(name)); }
	
	/** Retrieves the time-zone this volcano is in */
	double GetHoursToGMT(unsigned int index);
	double GetHoursToGMT(const CString &name){ return GetHoursToGMT(GetVolcanoIndex(name)); }
	
	/** Retrieves the observatory that monitors this volcano */
	int GetObservatoryIndex(unsigned int index);
	int GetObservatoryIndex(const CString &name){ return GetObservatoryIndex(GetVolcanoIndex(name)); }

private:
	class CVolcano{
	public:
		CVolcano();
		CVolcano(const CString &name, const CString &number, const CString &country, double latitude, double longitude, double altitude, double hoursToGMT = 0.0, int observatory = 1);
		CVolcano(const CString &name, const CString &simpleName, const CString &number, const CString &country, double latitude, double longitude, double altitude, double hoursToGMT = 0.0, int observatory = 1);
		~CVolcano();

		/** The name of the volcano */
		CString	m_name;

		/** The simplified name of the volcano */
		CString	m_simpleName;

		/** The number of the volcano. This is from the 
			Smithsonian's inventory of the worlds volcanoes 
			http://www.volcano.si.edu */
		CString	m_number;
		
		/** The country where this volcano is located */
		CString m_country;

		/** The latitude of the peak(s) */
		double m_peakLatitude;

		/** The longitude of the peak(s) */
		double m_peakLongitude;

		/** The altitude of the peak(s) (masl) */
		double m_peakHeight;

		/** The number of hours to GMT, used to calculate the local-time from the GPS-time */
		double m_hoursToGMT;

		/** The observatory in charge of this volcano */
		int m_observatory;
	};

	// ----------------------------------------------------------------
	// ---------------------- PRIVATE DATA ----------------------------
	// ----------------------------------------------------------------

	/** The list of volcanoes that belongs to this CVolcanoInfo object */
	std::vector<CVolcano> m_volcanoes;

	// ----------------------------------------------------------------
	// --------------------- PRIVATE METHODS --------------------------
	// ----------------------------------------------------------------

	/** Fills in all the fields into the database */
	void InitializeDatabase();
	
	void InitializeDatabase_01();
	void InitializeDatabase_02();
	void InitializeDatabase_03();
	void InitializeDatabase_04();
	void InitializeDatabase_05();
	void InitializeDatabase_06();
	void InitializeDatabase_07();
	void InitializeDatabase_08();
	void InitializeDatabase_09();
	void InitializeDatabase_10();
	void InitializeDatabase_11();
	void InitializeDatabase_12();
	void InitializeDatabase_13();
	void InitializeDatabase_14();
	void InitializeDatabase_15();
	void InitializeDatabase_16();
	void InitializeDatabase_17();
	void InitializeDatabase_18();
	void InitializeDatabase_19();
	

};
