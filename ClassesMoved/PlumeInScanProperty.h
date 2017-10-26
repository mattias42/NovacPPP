#pragma once

/** The class <b>CPlumeInScanProperty</b> is used to describe
	how a plume is seen by a scan. This incorporates properties
	such as the angle at which the centre of the plume is seen
	or an estimate of how large portion of the plume is seen.
*/
class CPlumeInScanProperty
{
public:
	CPlumeInScanProperty(void);
	~CPlumeInScanProperty(void);
	
	/** clears the properties of this data storage */
	void Clear();
	
	/** Assignment operator */
	CPlumeInScanProperty &operator=(const CPlumeInScanProperty &p2);
	
	/** The centre position of the plume, 
		one parameter for each motor */
	double m_plumeCentre[2];
	
	/** The estimated error in the estimation of the 
		plume centre position. */
	double m_plumeCentreError[2];
	
	/** The offset of the scan, describes the column
		of gas in the sky-spectrum */
	double m_offset;
	
	/** The completeness of the scan. This is a value
		ranging from 0.5 to 1.0 trying to estimate
		how large portion of the plume is visible
		in the scan. */
	double m_completeness;
	
	/** The edges of the plume */
	double m_plumeEdge_low;
	double m_plumeEdge_high;
};
