#ifndef NOVACPPP_FILESYSTEM_FILESYSTEM_H
#define NOVACPPP_FILESYSTEM_FILESYSTEM_H

#include <string>
#include <vector>
#include <PPPLib/CString.h>
#include <SpectralEvaluation/DateTime.h>

namespace Filesystem
{
    struct PakFileSearchCriterion
    {
        CDateTime startTime;
        CDateTime endTime;
    };

    /** Scans through the given directory in search for .pak-files
        @param path - the directory (on the local computer) where to search for files.
        @param includeSubdirectories If set to true then sub-directories of the provided path will also be searched.
        @param fileList Will be appended with the path's and file-names of the found .pak-files.
        @param criteria If not null, then this is used to filter the list of files. */
    void CheckForSpectraInDir(const novac::CString& path, bool includeSubdirectories, std::vector<std::string>& fileList, PakFileSearchCriterion* criteria = nullptr);

}

#endif // !NOVACPPP_FILESYSTEM_FILESYSTEM_H
