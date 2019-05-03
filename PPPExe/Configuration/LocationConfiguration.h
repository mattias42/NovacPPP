#pragma once

#include "InstrumentLocation.h"
#include <PPPLib/CString.h>

namespace Configuration {
    class CLocationConfiguration
    {
    public:
        CLocationConfiguration(void);
        ~CLocationConfiguration(void);

        /** The serial-number of the spectrometer for which this setup is valid */
        novac::CString m_serial;

        /** Clears all configurations for this location */
        void Clear();

        /** Inserts a new location.
                @param loc - the location to insert
        */
        void InsertLocation(const CInstrumentLocation &loc);

        /** Sets the properties of the location number 'index'
                @param index - the index of the location to set. If this is < 0 this function returns 1 and nothing is changed
                @param location - the location to set
                @return 0 if successful, otherwise 1 */
        int SetLocation(int index, const CInstrumentLocation &loc);

        /** Retrieves a location for this specrometer.
                @param index - the index of the location to get. If this is < 0 this function returns 1 and nothing is changed
                @param loc - the location to get
                @return 0 if successful, otherwise 1 */
        int GetLocation(int index, CInstrumentLocation &loc) const;

        /** Gets the number of locations configured for this spectrometer */
        unsigned long GetLocationNum() const;

        /** This goes through the settings for the locations to test that the settings
            make sense.
            @return 0 if all is ok
            @return 1 if no locations is defined for this spectrometer
            @return 2 if at least one locations has an in-valid time range (e.g. start>stop)
            @return 3 if there are two locations with overlapping definition time */
        int CheckSettings() const;
    private:

        /** Max number of locations for one instrument */
        static const int MAX_N_LOCATIONS = 32;

        /** The number of locations that are defined for this instrument */
        int m_locationNum;

        /** Array holding the locations that are configured for this
            instrument. Each of these also contains the time-frame
            they are valid for */
        CInstrumentLocation m_location[MAX_N_LOCATIONS];
    };
}