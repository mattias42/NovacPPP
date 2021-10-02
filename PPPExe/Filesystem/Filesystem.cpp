#include "Filesystem.h"

#include <PPPLib/CFileUtils.h>
#include <Poco/DirectoryIterator.h>
#include <Poco/Exception.h>

using namespace novac;

void ShowMessage(const char message[]);

namespace Filesystem
{
    void SearchDirectoryForFiles(const novac::CString &path, bool includeSubdirectories, std::vector<std::string>& fileList, FileSearchCriterion* criteria)
    {
        try
        {
            Poco::DirectoryIterator dir{ path.std_str() };
            Poco::DirectoryIterator end;

            while (dir != end) {
                novac::CString fileName, fullFileName;
                fileName.Format("%s", dir.name().c_str());
                fullFileName.Format("%s/%s", (const char*)path, dir.name().c_str());
                const bool isDirectory = dir->isDirectory();

                ++dir; // go to next file in the directory

                if (novac::Equals(dir.name(), ".") || novac::Equals(dir.name(), "..")) {
                    continue;
                }

                // if this is a directory...
                if (isDirectory && includeSubdirectories)
                {
                    SearchDirectoryForFiles(fullFileName, includeSubdirectories, fileList, criteria);
                    continue;
                }

                // check that this file is in the time-interval that we should evaluate spectra.
                if (nullptr != criteria)
                {
                    if (criteria->endTime > criteria->startTime)
                    {
                        int channel;
                        CDateTime startTime;
                        MEASUREMENT_MODE mode;
                        novac::CString serial;
                        novac::CFileUtils::GetInfoFromFileName(fileName, startTime, serial, channel, mode);

                        if (startTime < criteria->startTime || criteria->endTime < startTime)
                        {
                            continue;
                        }
                    }

                    if (criteria->fileExtension.size() > 0)
                    {
                        if (!novac::Equals(fileName.Right(criteria->fileExtension.size()), criteria->fileExtension))
                        {
                            continue;
                        }
                        if (novac::Equals(criteria->fileExtension, ".pak"))
                        {
                            if (novac::CFileUtils::IsIncompleteFile(fileName))
                            {
                                continue;
                            }
                        }
                    }

                    // We've passed all the tests for the .pak-file.
                    // Append the found file to the list of files to split and evaluate...
                    std::string filenameStr = fullFileName.ToStdString();
                    fileList.push_back(filenameStr);
                }
            }
        }
        catch (Poco::PathNotFoundException& e)
        {
            ShowMessage(e.what());
        }
    }

}