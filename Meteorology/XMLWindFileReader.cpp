#include "StdAfx.h"
#include "xmlwindfilereader.h"

// This is the settings for how to do the procesing
#include "../Configuration/UserConfiguration.h"

// we need to be able to download data from the FTP-server
#include "../Communication/FTPServerConnection.h"

extern Configuration::CUserConfiguration			g_userSettings;// <-- The settings of the user

using namespace FileHandler;
using namespace Meteorology;

CXMLWindFileReader::CXMLWindFileReader(void)
{
}

CXMLWindFileReader::~CXMLWindFileReader(void)
{
}

/** Reads in an wind-field file. 
	In the format specified for the NovacPostProcessingProgram (NPPP) 
	@return 0 on sucess */
int CXMLWindFileReader::ReadWindFile(const CString &fileName, Meteorology::CWindDataBase &dataBase){
	CFileException exceFile;
	CStdioFile file;
	int curWindow = 0;
	CString localFileName, userMessage;

	// 0. If the file is on the server, then download it first
	if(Equals(fileName.Left(6), "ftp://")){
		Communication::CFTPServerConnection *ftp = new Communication::CFTPServerConnection();
		
		CString tmpFileName;
		tmpFileName.Format(fileName);
		Common::GetFileName(tmpFileName); // this is the name of the file, without the path...
		localFileName.Format("%s\\%s", g_userSettings.m_tempDirectory, tmpFileName);
		
		// make sure that the tmp-directory exists
		if(CreateDirectoryStructure(g_userSettings.m_tempDirectory)){
			userMessage.Format("Could not create temp directory: %s", g_userSettings.m_tempDirectory);
			ShowMessage(userMessage);
			return 1;
		}

		if(ftp->DownloadFileFromFTP(fileName, localFileName, g_userSettings.m_FTPUsername, g_userSettings.m_FTPPassword)){
			ShowMessage("Failed to download wind file from FTP server");
			delete ftp;
			return 1;
		}
	}else{
		localFileName.Format("%s", fileName);
	}
	

	// 1. Open the file
	if(!file.Open(localFileName, CFile::modeRead | CFile::typeText, &exceFile)){
		return 1;
	}
	this->m_File		= &file;
	this->m_filename.Format(localFileName);

	// parse the file
	while(szToken = NextToken()){

		// no use to parse empty lines
		if(strlen(szToken) < 3)
			continue;

		// If this is a wind-field item then parse this. 
		if(Equals(szToken, "windfield", 9)){
			Parse_WindField(dataBase);
			continue;
		}

		// if this is the beginning of the wind-section,  extract the name of the 
		//	database
		if(Equals(szToken, "Wind", 4)){
			const char *volcanoName = GetAttributeValue("volcano");
			if(volcanoName != NULL){
				dataBase.m_dataBaseName.Format("%s", volcanoName);
			}
			continue;
		}
	}
	
	file.Close();
	
	return 0;
}

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
int CXMLWindFileReader::ReadWindDirectory(const CString &directory, Meteorology::CWindDataBase &dataBase, const CDateTime *dateFrom, const CDateTime *dateTo){
	CFileException exceFile;
	CStdioFile file;
	int curWindow = 0;
	CString localFileName, remoteFileName, userMessage, ftpDir;
	CList <CString, CString &> remoteFileList; // the list of wind field files
	CList <CString, CString &> localFileList; // the list of files on the local computer

	if(Equals(directory.Left(6), "ftp://")){
		// If the directory is on the server, then this is how to check the files
		Communication::CFTPServerConnection *ftp = new Communication::CFTPServerConnection();
		
		// Make sure that the directory does end with a trailing '/'
		ftpDir.Format(directory);
		if(!Equals(ftpDir.Right(1), "/"))
			ftpDir.AppendFormat("/");
		
		// Get the list of files on the server
		if(ftp->DownloadFileListFromFTP(ftpDir, remoteFileList, g_userSettings.m_FTPUsername, g_userSettings.m_FTPPassword)){
			ShowMessage("Failed to download list of wind files from FTP server");
			delete ftp;
			return 1;
		}

		// make sure that the tmp-directory exists
		if(CreateDirectoryStructure(g_userSettings.m_tempDirectory)){
			userMessage.Format("Could not create temp directory: %s", g_userSettings.m_tempDirectory);
			ShowMessage(userMessage);
			return 1;
		}

		// Download the files, one at a time
		POSITION p = remoteFileList.GetHeadPosition();
		while(p != NULL){
			CString &name = remoteFileList.GetNext(p);
			
			// only download .wxml files
			if(!Equals(name.Right(5), ".wxml"))
				continue;
				
			// if this file has a name that matches XXXX_YYYYMMDD.wxml then use this info to see 
			//	weather we actually should download this file
			int rpos = name.ReverseFind('_');
			if(rpos > 0 && ((name.GetLength() - rpos) == 14)){
				CString dateStr(name.Right(13).Left(8));
				CDateTime t;
				if(CDateTime::ParseDate(dateStr, t)){
					if(t < g_userSettings.m_fromDate || t > g_userSettings.m_toDate)
						continue;
				}
			}
			
			localFileName.Format("%s%s", g_userSettings.m_tempDirectory, name);

			if(IsExistingFile(localFileName)){
				userMessage.Format("File %s is already downloaded", localFileName);
				ShowMessage(userMessage);
			}else{
				remoteFileName.Format("%s%s", ftpDir, name);

				if(ftp->DownloadFileFromFTP(remoteFileName, localFileName, g_userSettings.m_FTPUsername, g_userSettings.m_FTPPassword)){
					ShowMessage("Failed to download wind file from FTP server");
					delete ftp;
					return 1;
				}
			}
			localFileList.AddTail(localFileName);
		}
	}else{
		// If the directory is on the local computer, then this is how to check the files
		HANDLE hFile;
		WIN32_FIND_DATA FindFileData;
		char fileToFind[MAX_PATH];

		sprintf(fileToFind, "%s\\*.wxml", directory);

		// Search for the file
		hFile = FindFirstFile(fileToFind, &FindFileData);

		if(hFile == INVALID_HANDLE_VALUE)
			return 1;
		do{
			// make sure that this is not a directory...
			if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)){
				localFileName.Format("%s\\%s", directory, FindFileData.cFileName);
				localFileList.AddTail(localFileName);	
			}
		}while(0 != FindNextFile(hFile, &FindFileData));
		FindClose(hFile);
	}

	// Now we got a list of files on the local computer. Read them in!
	POSITION p = localFileList.GetHeadPosition();
	int nFilesRead = 0;
	while(p != NULL){
		localFileName.Format("%s", localFileList.GetNext(p));
		
		// make sure that this file falls in the appropriate date-range
		if(dateFrom != NULL){
			
		}
		if(dateTo != NULL){
		
		}
		
		if(0 == ReadWindFile(localFileName, dataBase))
			++nFilesRead;
	}
	
	// Tell the user what we've done
	if(nFilesRead > 0){
		userMessage.Format("Successfully read in %d wind field files", nFilesRead);
		ShowMessage(userMessage);
		return 0;
	}else{
		ShowMessage("Failed to find any wind field files in the specified directory");
		return 1;
	}
}

/** Reads a 'windfield' section */
int CXMLWindFileReader::Parse_WindField(Meteorology::CWindDataBase &dataBase){
	CString sourceStr, userMessage;
	Meteorology::CWindField w;
	CDateTime validFrom, validTo;
	double latitude = 0.0;
	double longitude = 0.0;
	double altitude = 0.0;
	double windspeed = 0.0, windspeederror = 0.0;
	double winddirection = 0.0, winddirectionerror = 0.0;
	MET_SOURCE windSource;

	// parse the file
	while(szToken = NextToken()){
		// no use to parse empty lines
		if(strlen(szToken) < 2)
			continue;

		// ignore comments
		if(Equals(szToken, "!--", 3)){
			continue;
		}

		// end of fit-window section
		if(Equals(szToken, "/windfield")){
			return 0;
		}

		if(Equals(szToken, "source")){
			Parse_StringItem("/source", sourceStr);
			windSource = Meteorology::StringToMetSource(sourceStr);
			continue;
		}

		if(Equals(szToken, "altitude")){
			Parse_FloatItem("/altitude", altitude);
			continue;
		}
		
		if(Equals(szToken, "valid_from")){
			Parse_Date("/valid_from", validFrom);
			continue;
		}		

		if(Equals(szToken, "valid_to")){
			Parse_Date("/valid_to", validTo);
			continue;
		}
		
		if(Equals(szToken, "item", 4)){
			// find the wind-speed
			const char *str = GetAttributeValue("ws");
			if(str != NULL)
				windspeed = atof(str);
		
			// find the wind-speed-error
			str = GetAttributeValue("wse");
			if(str != NULL)
				windspeederror = atof(str);

			// find the wind-direction
			str = GetAttributeValue("wd");
			if(str != NULL)
				winddirection = atof(str);

			// find the wind-direction-error
			str = GetAttributeValue("wde");
			if(str != NULL)
				winddirectionerror = atof(str);

			// find the latitude
			str = GetAttributeValue("lat");
			if(str != NULL)
				latitude = atof(str);

			// find the longitude
			str = GetAttributeValue("lon");
			if(str != NULL)
				longitude = atof(str);
			else{
				str = GetAttributeValue("long");
				if(str != NULL)
					longitude = atof(str);
			}	
			
			// check that the latitude is within -90 to +90 degrees...
			latitude	= (latitude > 90.0) ? latitude - floor(latitude / 90.0) * 90.0 : latitude;

			// check that the longitude is within -180 to +180 degrees...
			longitude	= (longitude > 180.0) ? longitude - (1 + floor(longitude / 360.0)) * 360.0 : longitude;
			
			// check the reasonability of the values
			if(winddirection < -360.0 || winddirection > 360.0){
				userMessage.Format("Received wind-field file with invalid wind direction (%lf degrees) in file %s", winddirection, m_filename);
				ShowMessage(userMessage);
			}
			if(windspeed < 0.0 || windspeed > 50.0){
				userMessage.Format("Received wind-field file with invalid wind speed (%lf m/s) in file %s", windspeed, m_filename);
				ShowMessage(userMessage);
			}

			// we have now enough information to make a wind-field and insert it into the database
			w = CWindField(windspeed, windspeederror, windSource, winddirection, winddirectionerror, windSource, validFrom, validTo, latitude, longitude, altitude);
			
			dataBase.InsertWindField(w);
		}
	}

	return 1;
}

/** Writes an wind-field file in the NPPP-format 
	@return 0 on success */
int CXMLWindFileReader::WriteWindFile(const CString &fileName, const Meteorology::CWindDataBase &dataBase){
	return dataBase.WriteToFile(fileName);
}


