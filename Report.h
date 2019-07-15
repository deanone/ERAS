#if !defined(_REPORT_H)
#define _REPORT_H
#include "DataTypes.h"

//Report ReportLong[0]|ReportLat[1]|ReporterLong[2]|ReporterLat[3]|ReporterSpeed[4]|ReportState[5]|ReportJudgements[6]
// e.g. 22.964000701904|40.605001149459|22.9607|40.6031|14.25|true|11122213221
// 1 = true judjement, 2 = false judgement, 3 = do not know
class Report{
	double reportLong;
	double reportLat;
	double reporterLong;
	double reporterLat;
	double reporterSpeed;
	double reporterDistance;
	bool reportState;
	int usersFalseJudgements;
	void CalculateReporterDistance();
	double ToRadians(const double& degrees);
	void CalculateFalseJudgements(std::string usersJudgements);
public:
	Report();
	Report(StringVector report);
	double GetReportLong();
	double GetReportLat();
	double GetReporterLong();
	double GetReporterLat();
	double GetReporterSpeed();
	double GetReporterDistance();
	bool GetReportState();
	int GetUsersFalseJudgements();

};
#endif