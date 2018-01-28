#include "stdafx.h"
#include "SetupFileReader.h"
#include <PPPLib/CStdioFile.h>
#include <cstring>

using namespace FileHandler;
using namespace novac;

CSetupFileReader::CSetupFileReader(void)
{
}

CSetupFileReader::~CSetupFileReader(void)
{
}

RETURN_CODE CSetupFileReader::ReadSetupFile(const novac::CString &filename, Configuration::CNovacPPPConfiguration &setup){
	novac::CStdioFile file;

	// 1. Open the file
	if(!file.Open(filename, novac::CStdioFile::ModeFlag::modeRead | novac::CStdioFile::ModeFlag::typeText)){
		ShowMessage(std::string("Failed to open setup file for reading: '") + filename.std_str() + std::string("'"));
		return FAIL;
	}
	this->m_File = &file;
	
	// Parse the file
	while(nullptr != (szToken = NextToken())) {
		
		// no use to parse empty lines, don't parse lines with less than 3 characters
		if(strlen(szToken) < 3) {
			continue;
		}
		
		//* Look for the xml tag 'instrument' and use Parse_Instrument and Parse_Location to read serial number and location to object 'setup' */
		if(novac::Equals(szToken, "instrument", 9)){
			this->Parse_Instrument(setup.m_instrument[setup.m_instrumentNum]);
			setup.m_instrumentNum++;
			continue;
		}
		
	
	}//end while
	file.Close();

	return SUCCESS;
}

// Parse for serial tag and store in the InstrumentConfiguration object
void CSetupFileReader::Parse_Instrument(Configuration::CInstrumentConfiguration &instr){
	// Parse the file
	while(nullptr != (szToken = NextToken())) {

		if(novac::Equals(szToken, "/instrument", 11)){
			return;
		}

		if(novac::Equals(szToken, "serial", 6)){
			Parse_StringItem("/serial", instr.m_serial);
			continue;
		}
		if(novac::Equals(szToken, "location", 8)){
			Parse_Location(instr.m_location);
		}
	}
}

// Parse for location area and store in the LocationConfiguration object
void CSetupFileReader::Parse_Location(Configuration::CLocationConfiguration &loc){
	Configuration::CInstrumentLocation location;

	// Parse the other 
	while(nullptr != (szToken = NextToken())) {

		if(novac::Equals(szToken, "/location", 8)){
			 // insert the information that we have into the array and return.
			loc.InsertLocation(location);
			return;
		}

		if(novac::Equals(szToken, "name", 4)){
			Parse_StringItem("/name", location.m_locationName);
			continue;
		}
		if(novac::Equals(szToken, "latitude", 8)){
			Parse_FloatItem("/latitude", location.m_latitude);
			continue;
		}
		if(Equals(szToken, "longitude", 9)){
			Parse_FloatItem("/longitude", location.m_longitude);
			continue;
		}
		if(Equals(szToken, "altitude", 8)){
			Parse_IntItem("/altitude", location.m_altitude);
			continue;
		}
		if(Equals(szToken, "compass", 7)){
			Parse_FloatItem("/compass", location.m_compass);
			continue;
		}
		if(Equals(szToken, "coneangle", 9)){
			Parse_FloatItem("/coneangle", location.m_coneangle);
			continue;
		}
		if(Equals(szToken, "tilt", 4)){
			Parse_FloatItem("/tilt", location.m_tilt);
			continue;
		}
		if(Equals(szToken, "type", 4)){
			Parse_IntItem("/type", (int &)location.m_instrumentType);
			continue;
		}
		if(Equals(szToken, "spec", 4)){
			Parse_SpectrometerModel("/spec", location.m_spectrometerModel);
			continue;
		}
		if(Equals(szToken, "volcano", 7)){
			Parse_StringItem("/volcano", location.m_volcano);
			continue;
		}
		if(Equals(szToken, "time_from", 9)){
			Parse_Date("/time_from", location.m_validFrom);
			continue;
		}
		if(Equals(szToken, "time_to", 7)){
			Parse_Date("/time_to", location.m_validTo);
			continue;
		}
	}
}

/** This takes care of writing the contents of a setup data-structure to file
	Only the part regarding the instrument's location will be written to the file */
RETURN_CODE CSetupFileReader::WriteSetupFile(const novac::CString &fileName, const Configuration::CNovacPPPConfiguration &setup){
	Configuration::CInstrumentLocation instrLocation;
	novac::CString spectrometerModel;

	// Open the file
	FILE *f = fopen(fileName, "w");
	if(f == nullptr)
		return FAIL;

	// Write the header
	fprintf(f, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
	fprintf(f, "<!-- This is the global configuration file for the evaluation of spectra in the NOVAC Post Processing Program -->\n");

	// the main item
	fprintf(f, "<NovacPPPConfiguration>\n");

	// loop through each instrument
	for(unsigned int k = 0; k < setup.m_instrumentNum; ++k){
		const Configuration::CInstrumentConfiguration &instr = setup.m_instrument[k];
		
		fprintf(f, "\t<instrument>\n");

		// The serial-number
		fprintf(f, "\t\t<serial>%s</serial>\n", (const char*)instr.m_serial);
		
		// loop through the locations
		for(unsigned int j = 0; j < instr.m_location.GetLocationNum(); ++j){
			instr.m_location.GetLocation(j, instrLocation);
			CSpectrometerModel::ToString(instrLocation.m_spectrometerModel, spectrometerModel);
		
			// The location
			fprintf(f, "\t\t<location>\n");

			fprintf(f, "\t\t\t<name>%s</name>\n", (const char*)instrLocation.m_locationName);
			fprintf(f, "\t\t\t<volcano>%s</volcano>\n", (const char*)instrLocation.m_volcano);
			fprintf(f, "\t\t\t<time_from>%04d.%02d.%02d</time_from>\n", instrLocation.m_validFrom.year, instrLocation.m_validFrom.month, instrLocation.m_validFrom.day);
			fprintf(f, "\t\t\t<time_to>%04d.%02d.%02d</time_to>\n",     instrLocation.m_validTo.year, instrLocation.m_validTo.month, instrLocation.m_validTo.day);
			fprintf(f, "\t\t\t<latitude>%.5lf</latitude>\n",            instrLocation.m_latitude);
			fprintf(f, "\t\t\t<longitude>%.5lf</longitude>\n",          instrLocation.m_longitude);
			fprintf(f, "\t\t\t<altitude>%d</altitude>\n",               instrLocation.m_altitude);
			fprintf(f, "\t\t\t<compass>%.1lf</compass>\n",              instrLocation.m_compass);
			fprintf(f, "\t\t\t<coneangle>%.0lf</coneangle>\n",          instrLocation.m_coneangle);
			fprintf(f, "\t\t<tilt>%.0lf</tilt>\n",                      instrLocation.m_tilt);
			fprintf(f, "\t\t<type>%d</type>\n",                         instrLocation.m_instrumentType);
			fprintf(f, "\t\t<spec>%s</spec>\n", (const char*)spectrometerModel);

			fprintf(f, "\t\t</location>\n");
		}
		
		fprintf(f, "\t</instrument>\n");
	}	

	fprintf(f, "</NovacPPPConfiguration>\n");

	// remember to close the file
	fclose(f);

	return SUCCESS;
}

void CSetupFileReader::Parse_SpectrometerModel(const novac::CString &label, SPECTROMETER_MODEL &model){
	char buffer[2048];

	while(nullptr != (szToken = NextToken())) {

		if(Equals(szToken, label)) {
			return;
		}

		sscanf(szToken, "%s", buffer);
		model = CSpectrometerModel::GetModel(novac::CString(buffer));
	}

	return;
}