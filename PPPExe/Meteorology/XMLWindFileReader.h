#pragma once

#include "../Common/XMLFileReader.h"
#include "../Common/Common.h"
#include "WindDataBase.h"

#include <PPPLib/CString.h>

namespace FileHandler {
    /** The class <b>CXMLWindFileReader</b> is used to read in the
        wind - field files that are used in the NovacPPP.
        The files supplies a (potentially) complicated wind-field
        using a rather simple XML-style file format.
    */
    class CXMLWindFileReader : public CXMLFileReader
    {
    public:
        CXMLWindFileReader(void);
        ~CXMLWindFileReader(void);

        /** Reads in an wind-field file.
            In the format specified for the NovacPostProcessingProgram (NPPP)
            the file can be on the local computer or on an FTP-server
            @param filename - the full path to the wind field file
            @param dataBase - this will on successfull return be filled with the wind
                information found in the wind field files
            @return 0 on sucess */
        int ReadWindFile(const novac::CString &fileName, Meteorology::CWindDataBase &dataBase);

        /** Reads in all the wind-field files that are found in a given directory
            The directory can be on the local computer or on the FTP-server
            @param directory - the full path to the directory where the files are
            @param dataBase - this will on successfull return be filled with the wind
                information found in the wind field files
            @param dateFrom - if not null then only files which contain a wind field after (and including)
                the date 'dateFrom' will be read in.
            @param dateTo - if not null then only file which contain a wind field before (and including)
                the date 'dateTo' will be read in.
            @return 0 on success */
        int ReadWindDirectory(const novac::CString &directory, Meteorology::CWindDataBase &dataBase, const CDateTime *dateFrom = NULL, const CDateTime *dateTo = NULL);

        /** Writes an wind-field file in the NPPP-format
            @return 0 on success */
        int WriteWindFile(const novac::CString &fileName, const Meteorology::CWindDataBase &dataBase);

    private:


        /** Reads a 'windfield' section */
        int Parse_WindField(Meteorology::CWindDataBase &dataBase);

    };
}