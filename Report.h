#ifndef REPORT_H
#define REPORT_H

#include "DataTypes.h"

//Report ReportLong[0]|ReportLat[1]|ReporterLong[2]|ReporterLat[3]|ReporterSpeed[4]|ReportState[5]|ReportJudgements[6]
// e.g. 22.964000701904|40.605001149459|22.9607|40.6031|14.25|true|11122213221
// 1 = true judjement, 2 = false judgement, 3 = do not know
class Report
{
	double reportLat;
	double reportLon;
	bool reportState;
	int usersFalseJudgements;

	double reporterLat;
	double reporterLon;
	double reporterSpeed;
	double reporterDistance;
	
	double toRadians(const double& degrees);
	void calculateReporterDistance();
	void calculateFalseJudgements(std::string usersJudgements);

public:
	/*!
	 * Default Constructor.
	 */
	Report();
	
	/*!
	 * Constructor.
	 */
	Report(StringVector report);
	
	/*!
	 * Getter of the latitude of the report. 
	 * @return the latitude of the report.
	 */
	double getReportLat();

	/*!
	 * Getter of the longitude of the report. 
	 * @return the longitude of the report.
	 */
	double getReportLon();

	/*!
	 * Getter of the state (i.e. true or false) of the report. 
	 * @return the state of the report.
	 */
	bool getReportState();

	/*!
	 * Getter of the number of false judgements of the report from other users. 
	 * @return the number of false judgements of the report from other users.
	 */
	int getUsersFalseJudgements();

	/*!
	 * Getter of the latitude of the reporter. 
	 * @return the latitude of the reporter.
	 */
	double getReporterLat();

	/*!
	 * Getter of the longitude of the reporter. 
	 * @return the longitude of the reporter.
	 */
	double getReporterLon();

	/*!
	 * Getter of the speed of the reporter. 
	 * @return the speed of the reporter.
	 */
	double getReporterSpeed();
	
	/*!
	 * Getter of the distance of the reporter from the location of the report. 
	 * @return the distance of the reporter from the location of the report.
	 */
	double getReporterDistance();

};

#endif	//	REPORT_H