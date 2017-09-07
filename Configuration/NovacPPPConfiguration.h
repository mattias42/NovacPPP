#pragma once

#include "InstrumentConfiguration.h"

	/**
		The class <b>CNovacPPPConfiguration</b> is the main configuration
			for the NovacPPP. This configuration stores an array of
			configured instruments (for each of these instruments there
			is a set of fit-windows and locations configured).
			
			This also defines the directories that we should use to store
			the output data and the temporary files.
	*/
namespace Configuration{
	class CNovacPPPConfiguration
	{
	public:
		CNovacPPPConfiguration(void);
		~CNovacPPPConfiguration(void);
		
		// ----------------------------------------------------------------------
		// ---------------------- PUBLIC DATA -----------------------------------
		// ----------------------------------------------------------------------

		/** The array of instruments that are/can be configured. */
		CInstrumentConfiguration m_instrument[256];

		/** Number of instruments that have been configured */
		unsigned int m_instrumentNum;
				
		// ----------------------------------------------------------------------
		// --------------------- PUBLIC METHODS ---------------------------------
		// ----------------------------------------------------------------------

		/** Retrieves the CInstrumentConfiguration that is connected with a given
			serial-number.
			@return a pointer to the found CInstrumentconfiguraion. If none is found
				then return value is NULL. */
		const CInstrumentConfiguration *GetInstrument(const CString &serial) const;

		/** Retrieves the CInstrumentLocation that is valid for the given instrument and
			for the given time 
			@return 0 if successful otherwise non-zero		
		*/
		int GetInstrumentLocation(const CString &serial, const CDateTime &dateAndTime, CInstrumentLocation &instrLocation) const;

		/** Retrieves the CFitWindow that is valid for the given instrument and
			for the given time
			if 'fitWindowName' is not NULL then only the fit-window with the specified
				name will be returned.
			if 'fitWindowName' is NULL then the first fit-window valid at the given time
				will be returned.
			@return 0 if successful otherwise non-zero		
		*/
		int GetFitWindow(const CString &serial, int channel, const CDateTime &dateAndTime, Evaluation::CFitWindow &window, const CString *fitWindowName = NULL) const;

		/** Retrieves the CDarkSettings that is valid for the given instrument and
			for the given time

			@return 0 if successful otherwise non-zero		
		*/
		int GetDarkCorrection(const CString &serial, const CDateTime &dateAndTime, CDarkSettings &settings) const;

	};
}