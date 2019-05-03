#include "stdafx.h"
#include "FileInfo.h"

CFileInfo::CFileInfo(void)
{
    this->m_fileName.Format("");
    this->m_fullFileName.Format("");
    this->m_fileSize = 0;
    this->m_isDirectory = false;
}

CFileInfo::~CFileInfo(void)
{
}
CFileInfo::CFileInfo(novac::CString fileName, long fileSize, bool isDirectory)
{
    m_fileName.Format(fileName);
    m_fullFileName.Format(fileName);
    m_fileSize = fileSize;
    m_isDirectory = isDirectory;
}