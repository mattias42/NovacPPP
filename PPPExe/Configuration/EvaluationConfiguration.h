#pragma once

#include <SpectralEvaluation/DateTime.h>
#include <SpectralEvaluation/Evaluation/FitWindow.h>
#include <PPPLib/CArray.h>
#include <PPPLib/CString.h>

namespace Configuration {
    class CEvaluationConfiguration
    {
    public:
        CEvaluationConfiguration(void);
        ~CEvaluationConfiguration(void);

        /** The serial-number of the spectrometer for which this setup is valid */
        novac::CString m_serial;

        /** Clears all configurations for this spectrometer */
        void Clear();

        /** Inserts a new fit-window into the configuration for this spectrometer.
            @param window - the fit-window to insert
            @param validFrom - the time from which this fit-window is valid, NULL if valid since the beginning of time
            @param validTo - the time to which this fit-window is valid, NULL if valid until the end of time */
        void InsertFitWindow(const Evaluation::CFitWindow &window, const CDateTime *validFrom, const CDateTime *validTo);

        /** Sets the properties of the fit-window number 'index'
            @param index - the index of the configuration to set. If this is < 0
                this function returns 1 and nothing is changed
            @param window - the fit-window to set
            @param validFrom - the time from which this fit-window is valid, NULL if valid since the beginning of time
            @param validTo - the time to which this fit-window is valid, NULL if valid until the end of time
            @return 0 if sucessful, otherwise 1 */
        int SetFitWindow(int index, const Evaluation::CFitWindow &window, CDateTime *validFrom, CDateTime *validTo);

        /** Retrieves a fit-window from the configuration for this spectrometer.
            @param index - the index of the configuration to get. If this is < 0 or
                larger than the number of fit-windows configured this function
                returns 1 and the window is undefined
            @param window - the fit-window to get
            @param validFrom - the time from which this fit-window is valid
            @param validTo - the time to which this fit-window is valid
            @return 0 if sucessful, otherwise 1 */
        int GetFitWindow(int index, Evaluation::CFitWindow &window, CDateTime &validFrom, CDateTime &validTo) const;

        /** Gets the number of fit-windows configured for this spectrometer */
        unsigned long GetFitWindowNum() const;

        /** This goes through the settings for the fit-windows to test that the settings
            make sense.
            @return 0 if all is ok
            @return 1 if no fit-window is defined for this spectrometer
            @return 2 if at least one fit-window has an in-valid time range (e.g. start>stop)
            @return 3 if there are two fit-windows with overlapping definition time */
        int CheckSettings() const;

    private:
        /** The array of fit-windows for this spectrometer */
        novac::CArray <Evaluation::CFitWindow *, Evaluation::CFitWindow *> m_fitWindows;
        novac::CArray <CDateTime *, CDateTime *> m_validFrom;
        novac::CArray <CDateTime *, CDateTime *> m_validTo;
    };
}