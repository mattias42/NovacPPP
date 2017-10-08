#pragma once

#include "../Common/Common.h"

namespace Configuration{
	class CDarkSettings{
		public:
			CDarkSettings();
			~CDarkSettings();

		/** Resets all values to default */
		void Clear();

		/** The options for the how to get the dark.
			Can be: 0 - use measured (DEFAULT)
					1 - model of no measured is available
					2 - always model
					3 - the dark-spectrum is given by the user, do not model */
		DARK_SPEC_OPTION m_darkSpecOption;

		/** The offset-spectrum, only useful if 'm_darkSpecOption' is not 0.
				When this should be used is determined by 'm_offsetOption'.
				If 'm_darkSpecOption' is '3' then this is the dark-spectrum to use */
		novac::CString m_offsetSpec;

		/** The option for how to use the offset-spectrum.
			Can be:	0 - always use measured
					1 - use the user supplied */
		DARK_MODEL_OPTION m_offsetOption;

		/** The dark-current spectrum, only useful if 'm_darkSpecOption' is not 0.
				When this should be used is determined by 'm_darkCurrentOption'. */
		novac::CString m_darkCurrentSpec;

		/** The option for how to use the dark-current spectrum.
				Can be:	0 - always use measured
						1 - use the user supplied */
		DARK_MODEL_OPTION m_darkCurrentOption;

		/** Assignment operator */
		CDarkSettings& operator=(const CDarkSettings &dark2);
	};
}