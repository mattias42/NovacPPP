#include <PPPLib/File/Filesystem.h>
#include <PPPLib/MFC/CFileUtils.h>
#include <Poco/DirectoryIterator.h>
#include <Poco/Exception.h>
#include <sstream>

void ShowMessage(const char message[]);

namespace Filesystem
{

    novac::CString AppendPathSeparator(novac::CString path)
    {
        if (path.Right(1) == "/" || path.Right(1) == "\\")
        {
            return path;
        }

        char separator[8];
        sprintf_s(separator, "%c", Poco::Path::separator());

        return path.Append(separator);
    }

    void SearchDirectoryForFiles(novac::CString path, bool includeSubdirectories, std::vector<std::string>& fileList, ILogger& logger, const FileSearchCriterion* const criteria)
    {
        path = AppendPathSeparator(path);

        try
        {
            Poco::DirectoryIterator dir{ path.std_str() };
            Poco::DirectoryIterator end;

            while (dir != end)
            {
                novac::CString fileName, fullFileName;
                fileName.Format("%s", dir.name().c_str());
                fullFileName.Format("%s%s", (const char*)path, dir.name().c_str());
                const bool isDirectory = dir->isDirectory();

                ++dir; // go to next file in the directory

                if (novac::Equals(dir.name(), ".") || novac::Equals(dir.name(), ".."))
                {
                    continue;
                }

                // if this is a directory...
                if (isDirectory && includeSubdirectories)
                {
                    SearchDirectoryForFiles(fullFileName, includeSubdirectories, fileList, logger, criteria);
                    continue;
                }

                // check that this file is in the time-interval that we should evaluate spectra.
                if (nullptr != criteria)
                {
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
                                std::stringstream msg;
                                msg << "Ignoring incomplete pak file: '" << fileName << "'";
                                logger.Information(msg.str());

                                continue;
                            }
                        }
                    }

                    if (criteria->endTime > criteria->startTime)
                    {
                        int channel;
                        novac::CDateTime startTime;
                        MEASUREMENT_MODE mode;
                        novac::CString serial;
                        const bool fileInfoCouldBeParsed = novac::CFileUtils::GetInfoFromFileName(fileName, startTime, serial, channel, mode);

                        if (!fileInfoCouldBeParsed)
                        {
                            std::stringstream msg;
                            msg << "Ignoring file: '" << fileName << "' since the start time and serial could not be determined from the file name";
                            logger.Information(msg.str());

                            continue;
                        }

                        if (startTime < criteria->startTime || criteria->endTime < startTime)
                        {
                            std::stringstream msg;
                            msg << "Ignoring file: '" << fileName << "' since the start time (" << startTime << ") falls outside of the specified range [" << criteria->startTime << " to " << criteria->endTime << "]";
                            logger.Debug(msg.str());

                            continue;
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

    bool IsExistingFile(const novac::CString& fileName)
    {
        try
        {
            Poco::File file(fileName.c_str());
            return file.exists();
        }
        catch (const std::exception& e)
        {
            novac::CString message;
            message.Format("Exception happened when searching for file: '%s', message: '%s'", fileName.c_str(), e.what());
            ShowMessage(message);
            return false;
        }
    }

    int CreateDirectoryStructure(const novac::CString& path)
    {
        try
        {
            Poco::File directory(path.c_str());
            directory.createDirectories();

            if (directory.exists()) {
                return 0;
            }
            else {
                return 1; // error
            }
        }
        catch (std::exception& e)
        {
            novac::CString message = "Failed to create directory: ";
            message.AppendFormat("%s", e.what());
            ShowMessage(message);
            return 1;
        }
    }


}