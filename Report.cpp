#include "Report.h"

#include <math.h>

Report::Report()
{
	reportLat = 0.0;
	reportLon = 0.0;
	reportState = true;
	usersFalseJudgements = 0;

	reporterLat = 0.0;
	reporterLon = 0.0;
	reporterSpeed = 0.0;	
	reporterDistance = 0.0;	
}

Report::Report(StringVector report)
{
	reportLon = stod(report[0]);
	reportLat = stod(report[1]);
	reporterLon = stod(report[2]);
	reporterLat = stod(report[3]);
	reporterSpeed = stod(report[4]);
	reportState = report[5].compare("true")==0;
	calculateFalseJudgements(report[6]);
	calculateReporterDistance();
}

double Report::getReportLat()
{
	return reportLat;
}

double Report::getReportLon()
{
	return reportLon;
}

bool Report::getReportState()
{
	return reportState;
}

int Report::getUsersFalseJudgements()
{
	return usersFalseJudgements;
}

double Report::getReporterLat()
{
	return reporterLat;
}

double Report::getReporterLon()
{
	return reporterLon;
}

double Report::getReporterSpeed()
{
	return reporterSpeed;
}

double Report::getReporterDistance()
{
	return reporterDistance;
}

double Report::toRadians(const double& degrees)
{
	const double LOCAL_PI = 3.1415926535897932385;
	double radians = degrees * LOCAL_PI / 180;
	return radians;
}

void Report::calculateReporterDistance()
{
/***
Haversine formula
http://en.wikipedia.org/wiki/Haversine_formula
Coordinates in signed decimal degrees (WGS84)
***/
  double earthRadius = 3958.75;
  double dLat = toRadians(reporterLat - reportLat);
  double dLng = toRadians(reporterLon - reportLon);
  double a = sin(dLat / 2) * sin(dLat / 2) + 
             cos(toRadians(reportLat)) * cos(toRadians(reporterLat)) * 
             sin(dLng / 2) * sin(dLng / 2);
  double c = 2 * atan2(sqrt(a), sqrt(1 - a));
  double dist = earthRadius * c;
  double meterConversion = 1609.00;
  reporterDistance = ((dist * meterConversion) / 1000.0);	//	distance in km
}

void Report::calculateFalseJudgements(std::string usersJudgements)
{
	usersFalseJudgements = 0;
	for (size_t i = 0; i < usersJudgements.size(); ++i)
	{
		if (usersJudgements[i] == '2')
		{
			usersFalseJudgements++;
		}
	}
}