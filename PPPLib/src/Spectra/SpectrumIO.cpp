#include <PPPLib/Spectra/SpectrumIO.h>
#include <PPPLib/Spectra/Spectrum.h>

#include <algorithm>
#include <cstring>

#undef min
#undef max

using namespace SpectrumIO;

CSpectrumIO::CSpectrumIO(void)
{
	this->m_lastError = ERROR_NO_ERROR;
}

CSpectrumIO::~CSpectrumIO(void)
{
}

int CSpectrumIO::CountSpectra(const novac::CString &fileName) {
	unsigned long specNum = 0;
	int headerSize;

	FILE *f = fopen(fileName, "rb");

	if (f == NULL) {
		printf("Could not open spectrum file: %s", (const char*)fileName);
		m_lastError = ERROR_COULD_NOT_OPEN_FILE;
		return(1);
	}

	while (1)
	{
		int ret = ReadNextSpectrumHeader(f, headerSize);
		if (ret == 1)
			break;
		if (ret == 2)
			continue;

		char textBuffer[4];

		// Seek our way into the next spectrum...
		if (0 != fseek(f, std::min(MKZY.size, (unsigned short)(4 * MAX_SPECTRUM_LENGTH)), SEEK_CUR))
			break;

		// Make sure we're at the right place, if not rewind again and search for the next
		// occurence of the "MKZY" string, which signals the start of a 'new' spectrum.
		fread(textBuffer, 1, 4, f);
		if (NULL == strstr(textBuffer, "MKZY")) {
			// rewind
			if (0 != fseek(f, -std::min(MKZY.size, (unsigned short)(4 * MAX_SPECTRUM_LENGTH)), SEEK_CUR))
				break;
		}
		else {
			if (0 != fseek(f, -4, SEEK_CUR))
				break;
		}

		++specNum;
		continue;
	}

	// signals that there's no error in the file.
	m_lastError = ERROR_SPECTRUM_NOT_FOUND;

	fclose(f);

	return specNum;
}

int CSpectrumIO::ScanSpectrumFile(const novac::CString &fileName, const novac::CString *specNamesToLookFor, int numSpecNames, int *indices) {
	novac::CString errorMessage; // a string used for error messages
	novac::CString specName;
	unsigned long specNum = 0;
	int headerSize, nameIndex;

	FILE *f = fopen(fileName, "rb");

	if (f == NULL) {
		printf("Could not open spectrum file: %s", (const char*)fileName);
		m_lastError = ERROR_COULD_NOT_OPEN_FILE;
		return(1);
	}

	while (1)
	{
		int ret = ReadNextSpectrumHeader(f, headerSize);
		if (ret == 1)
			break;
		if (ret == 2)
			continue;

		/** Look in the buffer */
		// 1. Clean the spectrum name from special characters...
		novac::CleanString(MKZY.name, specName);
		specName.Trim(" \t");
		size_t size1 = strlen(specName);
		for (nameIndex = 0; nameIndex < numSpecNames; ++nameIndex) {
			// first of all, the strings must have equal size to be equal...
			size_t size2 = strlen(specNamesToLookFor[nameIndex]);
			if (size1 != size2)
				continue;

			if (Equals(specNamesToLookFor[nameIndex], specName)) {
				indices[nameIndex] = specNum;
				continue;
			}
		}

		char textBuffer[4];

		// Seek our way into the next spectrum...
		if (0 != fseek(f, std::min(MKZY.size, (unsigned short)(4 * MAX_SPECTRUM_LENGTH)), SEEK_CUR))
			break;

		// Make sure we're at the right place, if not rewind again and search for the next
		// occurence of the "MKZY" string, which signals the start of a 'new' spectrum.
		fread(textBuffer, 1, 4, f);
		if (NULL == strstr(textBuffer, "MKZY")) {
			// rewind
			if (0 != fseek(f, -std::min(MKZY.size, (unsigned short)(4 * MAX_SPECTRUM_LENGTH)), SEEK_CUR))
				break;
		}
		else {
			if (0 != fseek(f, -4, SEEK_CUR))
				break;
		}

		++specNum;
		continue;
	}

	// signals that there's no error in the file.
	this->m_lastError = ERROR_SPECTRUM_NOT_FOUND;

	fclose(f);

	return specNum;
}

RETURN_CODE CSpectrumIO::ReadSpectrum(const novac::CString &fileName, const int spectrumNumber, CSpectrum &spec, char *headerBuffer /* = NULL*/, int headerBufferSize /* = 0*/, int *headerSize /* = NULL*/) {
	MKPack mkPack;

	long i, j;
	long outlen;
	unsigned long chk;
	unsigned short checksum;
	int hdrSize;

	unsigned short *p = NULL;

	i = 0;
	FILE *f = fopen(fileName, "rb");

	if (f == NULL) {
		printf("Could not open spectrum file: %s", (const char*)fileName);
		m_lastError = ERROR_COULD_NOT_OPEN_FILE;
		return FAIL;
	}

	while (1)
	{
		int ret;
		if (headerBuffer != NULL)
			ret = ReadNextSpectrumHeader(f, *headerSize, &spec, headerBuffer, headerBufferSize);
		else
			ret = ReadNextSpectrumHeader(f, hdrSize, &spec);
		if (ret == 1)
			break;
		if (ret == 2)
			continue;

		if (i != spectrumNumber) {
			char textBuffer[4];

			// Seek our way into the next spectrum...
			if (0 != fseek(f, std::min(MKZY.size, (unsigned short)(4 * MAX_SPECTRUM_LENGTH)), SEEK_CUR))
				break;

			// Make sure we're at the right place, if not rewind again and search for the next
			// occurence of the "MKZY" string, which signals the start of a 'new' spectrum.
			if (fread(textBuffer, 1, 4, f) < 4)
				break;
			if (NULL == strstr(textBuffer, "MKZY")) {
				// rewind
				if (0 != fseek(f, -std::min(MKZY.size, (unsigned short)(4 * MAX_SPECTRUM_LENGTH)), SEEK_CUR))
					break;
			}
			else {
				if (0 != fseek(f, -4, SEEK_CUR))
					break;
			}

			++i;
			continue;
		}
		else
		{
			// read the spectrum from the file

			if (MKZY.size > sizeof(buffer)) {
				// compressed data is too long. We cannot read the full spectrum.
				m_lastError = ERROR_SPECTRUM_TOO_LARGE;
				fclose(f);
				return FAIL;
			}

			if (fread(buffer, 1, MKZY.size, f) < MKZY.size) //read compressed info
			{
				printf("Error EOF! in %s", (const char*)fileName);
				fclose(f);
				m_lastError = ERROR_EOF;
				return FAIL;
			}

			if (MKZY.pixels > sizeof(outbuf) * sizeof(long)) {
				// The spectrum is longer than what the buffer can handle. Trying to
				// uncompress the whole spectrum will result in a buffer overflow.
				// this spectrum cannot be read - return.
				m_lastError = ERROR_SPECTRUM_TOO_LARGE;
				fclose(f);
				return FAIL;
			}

			outlen = mkPack.UnPack(buffer, MKZY.pixels, outbuf); //uncompress info(compressed buffer,num of sampling points, uncompressedinfo)

			// validate that the decompression was ok - Added 2006.02.13 by MJ
			if (outlen < 0) {
				m_lastError = ERROR_DECOMPRESS;
				fclose(f);
				return FAIL;
			}

			// validate that the spectrum is not too large - Added 2006.02.13 by MJ
			if (outlen > MAX_SPECTRUM_LENGTH) {
				m_lastError = ERROR_SPECTRUM_TOO_LARGE;
				fclose(f);
				return FAIL;
			}

			// calculate the checksum
			chk = 0;
			for (j = 0; j < outlen && j < MAX_SPECTRUM_LENGTH; j++)
			{
				chk += outbuf[j];
				spec.m_data[j] = outbuf[j];
			}
			p = (unsigned short *)&chk;
			checksum = p[0] + p[1];
			if (checksum != MKZY.checksum) {
				printf("Checksum mismatch %04x!=x%04x\n", checksum, MKZY.checksum);

				m_lastError = ERROR_CHECKSUM_MISMATCH;
				fclose(f);
				return FAIL;
			}

			// Get the maximum intensity
			if (MKZY.pixels > 0) {
				spec.m_info.m_peakIntensity = (float)spec.MaxValue();
				spec.m_info.m_offset = (float)spec.GetOffset();
			}

			fclose(f);

			return SUCCESS;
		}
	}
	multisize = i;
	fclose(f);

	this->m_lastError = ERROR_SPECTRUM_NOT_FOUND;
	return FAIL; // spectrum not found
}

/** Rewinds the gien file to the beginning and forwards the current position
		in the file to the beginning of spectrum number 'spectrumNumber' (zero-based index).
		Return SUCCESS if all is ok, return FAIL if the file is corrupt in some
			way or the spectrum number 'spectrumNumber' does not exist in this file. */
RETURN_CODE CSpectrumIO::FindSpectrumNumber(FILE *f, int spectrumNumber) {
	novac::CString errorMessage; // a string used for error messages
	long curSpecNum = 0;

	if (f == NULL)
		return FAIL;

	// first rewind the file
	rewind(f);

	while (curSpecNum <= spectrumNumber)
	{
		int c;

		// find the next 'MKZY' - string in the spectrum file
		while ((c = getc(f)) != (int)'M') {
			if (c == EOF)
				return FAIL;
		}

		if (getc(f) != (int)'K')
			continue;

		if (getc(f) != (int)'Z')
			continue;

		if (getc(f) != (int)'Y')
			continue;

		// we've found a 'new' 'MKZY'-string, call this a spectrum and increase
		//	the spectrum counter...
		++curSpecNum;
	}

	// we've found the spectrum we're looking for, now rewind past the 'MKZY'-string
	if (0 != fseek(f, -4, SEEK_CUR))
		return FAIL;

	// signals that there's no error in the file.
	m_lastError = ERROR_NO_ERROR;

	return SUCCESS;
}

RETURN_CODE CSpectrumIO::ReadNextSpectrum(FILE *f, CSpectrum &spec) {
	int tmp;
	return ReadNextSpectrum(f, spec, tmp);
}

/** Reads the next spectrum in the provided spectrum file.
		The spectrum file (which must be in the .pak format) must be opened for reading
		in binary mode
	@param f - The opened spectrum file.
	@param spec - Will on successful return contain the desired spectrum.
	@return SUCCESS if all is ok. */
RETURN_CODE CSpectrumIO::ReadNextSpectrum(FILE *f, CSpectrum &spec, int &headerSize, char *headerBuffer, int headerBufferSize) {
	long outlen;
	long j;
	unsigned long chk;
	unsigned short checksum;
	MKPack mkPack;

	unsigned short *p = NULL;

	int ret = ReadNextSpectrumHeader(f, headerSize, &spec, headerBuffer, headerBufferSize);
	if (ret != 0)
		return FAIL;

	// read the spectrum from the file
	if (MKZY.size > sizeof(buffer)) {
		// compressed data is too long. We cannot read the full spectrum.
		this->m_lastError = ERROR_SPECTRUM_TOO_LARGE;
		return FAIL;
	}

	if (fread(buffer, 1, MKZY.size, f) < MKZY.size) //read compressed info
	{
		printf("Error EOF! in pak-file");

		m_lastError = ERROR_EOF;
		return FAIL;
	}

	if (MKZY.pixels > sizeof(outbuf) * sizeof(long)) {
		// The spectrum is longer than what the buffer can handle. Trying to
		// uncompress the whole spectrum will result in a buffer overflow.
		// this spectrum cannot be read - return.
		m_lastError = ERROR_SPECTRUM_TOO_LARGE;
		return FAIL;
	}

	// We've managed to read the spectrum header, write that information
	//	to the supplied spectrum data-structure
	spec.m_info.m_device.Format("%s", MKZY.instrumentname);
	spec.m_info.m_device.Trim(" ");  // remove spaces in the beginning or the end
	spec.m_info.m_name.Format("%s", MKZY.name);

	// Decompress the spectrum itself
	outlen = mkPack.UnPack(buffer, MKZY.pixels, outbuf); //uncompress info(compressed buffer,num of sampling points, uncompressedinfo)

	// validate that the decompression was ok - Added 2006.02.13 by MJ
	if (outlen < 0) {
		this->m_lastError = ERROR_DECOMPRESS;
		return FAIL;
	}
	// validate that the spectrum is not too large - Added 2006.02.13 by MJ
	if (outlen > MAX_SPECTRUM_LENGTH) {
		this->m_lastError = ERROR_SPECTRUM_TOO_LARGE;
		return FAIL;
	}

	// calculate the checksum
	chk = 0;
	for (j = 0; j < outlen && j < MAX_SPECTRUM_LENGTH; j++)
	{
		chk += outbuf[j];
	}
	p = (unsigned short *)&chk;
	checksum = p[0] + p[1];
	if (checksum != MKZY.checksum) {
		printf("Checksum mismatch %04x!=x%04x\n", checksum, MKZY.checksum);

		this->m_lastError = ERROR_CHECKSUM_MISMATCH;
		return FAIL;
	}

	// copy the spectrum
	for (j = 0; j < outlen && j < MAX_SPECTRUM_LENGTH; j++)
		spec.m_data[j] = outbuf[j];


	// Get the maximum intensity
	if (MKZY.pixels > 0) {
		spec.m_info.m_peakIntensity = (float)spec.MaxValue();
		spec.m_info.m_offset = (float)spec.GetOffset();
	}

	return SUCCESS;
}

void CSpectrumIO::ParseTime(const unsigned long t, novac::CDateTime &time) const {
	time.hour = (unsigned char)(t / 1000000);
	time.minute = (unsigned char)((t - time.hour * 1000000) / 10000);
	time.second = (unsigned char)((t - time.hour * 1000000 - time.minute * 10000) / 100);
	time.msec = 10 * ((unsigned short)(t % 100));
}

void CSpectrumIO::WriteTime(unsigned long &t, const novac::CDateTime &time) const {
	t = time.hour * 1000000 + time.minute * 10000 + time.second * 100 + time.msec / 10;
}
unsigned long CSpectrumIO::WriteTime(const novac::CDateTime &time) const {
	return time.hour * 1000000 + time.minute * 10000 + time.second * 100 + time.msec / 10;
}

void CSpectrumIO::ParseDate(const unsigned long d, novac::CDateTime &day) const {
	day.day = (unsigned char)(d / 10000);                  // the day
	day.month = (unsigned char)((d - day.day * 10000) / 100);  // the month
	day.year = (unsigned short)(d % 100);                  // the year

	if (day.year < 100)
		day.year += 2000; // assume the 21:st century (should be ok for another 95 years)
}

// Write the date in Manne's format: ddmmyy 
void CSpectrumIO::WriteDate(unsigned long &d, const novac::CDateTime &day) const {
	if (day.year < 100)
		d = day.day * 10000 + day.month * 100 + day.year;
	else
		d = day.day * 10000 + day.month * 100 + day.year - (day.year / 100) * 100;
}
unsigned long CSpectrumIO::WriteDate(const novac::CDateTime &day) const {
	if (day.year < 100)
		return day.day * 10000 + day.month * 100 + day.year;
	else
		return day.day * 10000 + day.month * 100 + day.year - (day.year / 100) * 100;
}

int CSpectrumIO::AddSpectrumToFile(const novac::CString &fileName, const CSpectrum &spectrum, const char *headerBuffer, int headerSize) {

	long last, tmp;
	int i;
	unsigned short outsiz;
	unsigned long checksum;
	unsigned short *p;
	MKPack mkPack;

	// Test the input-data
	if (spectrum.m_length <= 0)
		return 1;

	// ---- start by converting the spectrum into 'long'
	long *spec = new long[spectrum.m_length];
	for (i = 0; i < spectrum.m_length; ++i)
		spec[i] = (long)spectrum.m_data[i];

	// ---- create the proper header information ---- 

	// calculate checksum
	checksum = 0;
	for (i = 0; i < spectrum.m_length; ++i)
		checksum += spec[i];
	p = (unsigned short *)&checksum;
	MKZY.checksum = p[0] + p[1];

	// the spectrum should be stored as just the difference
	//	between each two pixels (delta compression)
	//	except for the first pixel (of course)
	last = spec[0];
	for (i = 1; i < spectrum.m_length; i++)
	{
		tmp = spec[i];
		spec[i] = tmp - last;
		last = tmp;
	}

	// Compress the spectrum..
	unsigned short *sbuf = new unsigned short[16384];
	memset(sbuf, 0, 16384);
	outsiz = mkPack.mk_compress(spec, (unsigned char *)sbuf, (unsigned short)spectrum.m_length);
	const CSpectrumInfo &info = spectrum.m_info;

	MKZY.ident[0] = 'M';
	MKZY.ident[1] = 'K';
	MKZY.ident[2] = 'Z';
	MKZY.ident[3] = 'Y';

	MKZY.altitude = (short)spectrum.Altitude();
	MKZY.channel = info.m_channel;
	MKZY.compassdir = (short)(info.m_compass * 10.0f);

	MKZY.ADC[0] = (unsigned short)(info.m_batteryVoltage * 100.0f);

	MKZY.coneangle = (char)info.m_coneAngle;
	MKZY.date = WriteDate(info.m_startTime);
	MKZY.exptime = (short)info.m_exposureTime;
	MKZY.flag = info.m_flag;
	MKZY.hdrsize = sizeof(struct MKZYhdr);
	MKZY.hdrversion = hdr_version;
	sprintf(MKZY.instrumentname, "%.16s", (const char*)spectrum.m_info.m_device);
	MKZY.lat = spectrum.Latitude();
	MKZY.lon = spectrum.Longitude();
	MKZY.measurecnt = (char)info.m_scanSpecNum;
	MKZY.measureidx = (char)info.m_scanIndex;
	sprintf(MKZY.name, "%.12s", (const char*)spectrum.m_info.m_name);
	MKZY.pixels = (unsigned short)spectrum.m_length;
	MKZY.size = outsiz;
	MKZY.startc = info.m_startChannel;
	MKZY.scans = (unsigned short)info.m_numSpec;
	MKZY.starttime = WriteTime(info.m_startTime);
	MKZY.stoptime = WriteTime(info.m_stopTime);
	MKZY.temperature = info.m_temperature;
	MKZY.tiltX = (short)info.m_roll;		// <-- The leaning in the direction perpendicular to the scanner
	MKZY.tiltY = (short)info.m_pitch;		// <-- The leaning in the direction of the scanner
	MKZY.viewangle = (unsigned short)info.m_scanAngle;
	MKZY.viewangle2 = (unsigned short)info.m_scanAngle2;

	FILE *f = fopen(fileName, "r+b");
	if (f == NULL) // this will happen if the file does not exist...
		f = fopen(fileName, "w+b");
	if (f == NULL) {
		delete sbuf;
		delete spec;
		return 1;
	}

	if (0 == fseek(f, 0, SEEK_END)) {
		// Write the header
		if (headerBuffer != NULL && headerSize != 0) {
			fwrite(headerBuffer, headerSize, 1, f);
		}
		else {
			fwrite(&MKZY, sizeof(struct MKZYhdr), 1, f);
		}

		// Write the spectrum data
		fwrite(sbuf, outsiz, 1, f);
	}
	fclose(f);

	delete sbuf;
	delete spec;

	return 0;
}

/** Reads a spectrum header from the supplied file. The result
		will be saved to the member-variable 'MKZY'. */
int CSpectrumIO::ReadNextSpectrumHeader(FILE *f, int &headerSize, CSpectrum *spec, char *headerBuffer, int headerBufferSize) {
	unsigned long local_headersize;
	long sizdiff;

	memset(&MKZY, 0, sizeof(MKZY));  // clear header information
	MKZY.measureidx = -1;   // this is for compatibility reasons, if the file does not contain spectrum number, we'll know about it

	if (fread(&MKZY, 1, 8, f) < 8) {        // reads MKZY.ident and MKZY.hdrsize
		return 1;  // could not read header info, break /* TODO - this is not a good way to quit */
	}

	// check that we are actually at the beginning of the header - Added 2006.02.13 by MJ
	if (strncmp(MKZY.ident, "MKZY", 4 * sizeof(char))) {
		return 2;
	}

	local_headersize = MKZY.hdrsize;
	headerSize = MKZY.hdrsize;

	if (sizeof(MKZY) < local_headersize)
		local_headersize = sizeof(MKZY);

	/** If the file contains a smaller header than the program version can read (MKZY.hdrsize < sizeof(MKZY))
				only read the actual header in the file.
			If the file contains a bigger header than the program version can read (sizeof(MKZY) < MKZY.hdrsize)
				only read what we can understand. */

	if (fread((char *)&MKZY + 8, 1, local_headersize - 8, f) < local_headersize - 8) // read the rest of the header
		return 1;

	// If the user wants the header in binary format, copy it
	if (headerBuffer != NULL && headerBufferSize > (int)local_headersize) {
		memset(headerBuffer, 0, headerBufferSize);
		memcpy(headerBuffer, &MKZY, local_headersize);
	}


	// Calculate how much of the information in the file that we could not read because the program version is too old.
	sizdiff = MKZY.hdrsize - sizeof(MKZY);
	if (sizdiff > 0)
	{
		// If the user want the whole header, read it. Otherwise jump formwards
		if (headerBuffer != NULL && headerBufferSize > MKZY.hdrsize) {
			if (fread(headerBuffer + local_headersize, 1, sizdiff, f) < (unsigned long)sizdiff)
				m_lastError = ERROR_SPECTRUM_NOT_FOUND;
			return FAIL;
		}
		else {
			fseek(f, sizdiff, SEEK_CUR);       // NOTE -- BUG CORRECTED 2006.02.14 BY MJ - was "fseek(f,sizdiff-8,SEEK_CUR);"
		}
	}

	if (spec != NULL) {
		// clear the spectrum
		memset(spec->m_data, 0, MAX_SPECTRUM_LENGTH * sizeof(SpecData));

		CSpectrumInfo *info = &spec->m_info;
		// save the spectrum information in the CSpectrum data structure
		spec->m_length = std::max(std::min(MKZY.pixels, (unsigned short)(MAX_SPECTRUM_LENGTH)), (unsigned short)(0));
		info->m_startChannel = MKZY.startc;
		info->m_numSpec = MKZY.scans;
		info->m_exposureTime = (MKZY.exptime > 0) ? MKZY.exptime : -MKZY.exptime;
		info->m_gps.m_longitude = MKZY.lon;
		info->m_gps.m_latitude = MKZY.lat;
		info->m_gps.m_altitude = MKZY.altitude;
		info->m_channel = MKZY.channel;
		CSpectrum::GetInterlaceSteps(info->m_channel, info->m_interlaceStep);
		info->m_scanAngle = MKZY.viewangle;
		if (info->m_scanAngle > 180.0)
			info->m_scanAngle -= 360.0; // map 270 -> -90
		info->m_scanAngle2 = (float)MKZY.viewangle2;
		info->m_coneAngle = MKZY.coneangle;
		info->m_compass = (float)MKZY.compassdir / 10.0f;
		if (info->m_compass > 360.0 || info->m_compass < 0) {
			printf("Spectrum has compass angle outside of the expected [0, 360] degree range. ");
		}
		info->m_batteryVoltage = (float)MKZY.ADC[0] / 100.0f;
		info->m_temperature = MKZY.temperature;

		info->m_scanIndex = MKZY.measureidx;
		info->m_scanSpecNum = MKZY.measurecnt;
		info->m_flag = MKZY.flag;

		ParseTime(MKZY.starttime, info->m_startTime);
		ParseTime(MKZY.stoptime, info->m_stopTime);
		ParseDate(MKZY.date, info->m_startTime);

		info->m_device.Format("%s", MKZY.instrumentname);
		info->m_device.Trim(" "); // remove spaces in the beginning or the end
		info->m_name.Format("%s", MKZY.name);
	}

	return 0;
}
