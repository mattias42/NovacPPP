#include "FileInfo.h"

CFileInfo::CFileInfo(novac::CString fileName, long fileSize, bool isDirectory)
{
    m_fileName.Format(fileName);
    m_fullFileName.Format(fileName);
    m_fileSize = fileSize;
    m_isDirectory = isDirectory;
}