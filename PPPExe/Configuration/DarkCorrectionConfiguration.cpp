#include "DarkCorrectionConfiguration.h"

using namespace Configuration;

CDarkCorrectionConfiguration::CDarkCorrectionConfiguration(void)
{
    Clear();
}

CDarkCorrectionConfiguration::~CDarkCorrectionConfiguration(void)
{
    m_darkSettings.RemoveAll();
    m_validFrom.RemoveAll();
    m_validTo.RemoveAll();
}

void CDarkCorrectionConfiguration::Clear() {
    m_darkSettings.RemoveAll();
    m_validFrom.RemoveAll();
    m_validTo.RemoveAll();

    m_darkSettings.SetAtGrow(0, new CDarkSettings());

    m_validFrom.SetAtGrow(0, new CDateTime(0, 0, 0, 0, 0, 0));
    m_validTo.SetAtGrow(0, new CDateTime(9999, 12, 31, 23, 59, 59));
}

/** Inserts a new set of settings for correcting dark spectra into the configuration for this spectrometer.
    @param dSettings - dark-current correction settings
    @param validFrom - the time from which this settings is valid, NULL if valid since the beginning of time
    @param validTo - the time to which this settings is valid, NULL if valid until the end of time */
void CDarkCorrectionConfiguration::InsertDarkCurrentCorrectionSettings(const CDarkSettings &dSettings, const CDateTime *validFrom, const CDateTime *validTo) {

    // make a copy of the settings
    CDarkSettings *newSettings = new CDarkSettings();
    *newSettings = dSettings;

    // Get the time-range for which this window is valid
    CDateTime *fromTime = new CDateTime(0000, 00, 00, 00, 00, 00);
    CDateTime *toTime = new CDateTime(9999, 12, 31, 23, 59, 59);
    if (validFrom != NULL) {
        *fromTime = *validFrom;
    }
    if (validTo != NULL) {
        *toTime = *validTo;
    }

    // insert the fit-window into the array
    int length = (int)m_darkSettings.GetCount();
    m_darkSettings.SetAtGrow(length, newSettings);
    m_validFrom.SetAtGrow(length, fromTime);
    m_validTo.SetAtGrow(length, toTime);


    return;
}

/** Retrieves how the dark-current should be corrected for this spectrometer at the
        given time.
    @param dSettings - will on successfull return be filled with the settings that are
        valid at the given time
    @param time - the time when we want to know the settings for how the
        dark current should be corrected
    @return 0 if sucessful, otherwise 1 */
int CDarkCorrectionConfiguration::GetDarkSettings(CDarkSettings &dSettings, const CDateTime &time) const {
    int length = (int)m_darkSettings.GetCount();

    for (int k = 0; k < length; ++k) {
        CDateTime *from = m_validFrom.GetAt(k);
        CDateTime *to = m_validTo.GetAt(k);
        if ((*from < time || *from == time) && time < *to) {
            dSettings = *m_darkSettings.GetAt(k);
            return 0;
        }
    }


    return 1;
}

/** Retrieves a dark-current settings from the configuration for this spectrometer.
    @param index - the index of the configuration to get. If this is < 0 or
        larger than the number of dark-current settings configured this function
        returns 1 and the dark-current setting is undefined
    @param dSettings - the dark-current settings to get
    @param validFrom - the time from which this fit-window is valid
    @param validTo - the time to which this fit-window is valid
    @return 0 if sucessful, otherwise 1 */
int CDarkCorrectionConfiguration::GetDarkSettings(int index, CDarkSettings &dSettings, CDateTime &validFrom, CDateTime &validTo) const {
    if (index < 0 || index >= m_darkSettings.GetSize())
        return 1;

    dSettings = *(m_darkSettings.GetAt(index));
    validFrom = *(m_validFrom.GetAt(index));
    validTo = *(m_validTo.GetAt(index));

    return 0;
}

/** Gets the number of fit-windows configured for this spectrometer */
unsigned long CDarkCorrectionConfiguration::GetSettingsNum() const {
    return m_darkSettings.GetSize();
}
