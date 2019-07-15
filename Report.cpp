#include "Report.h"

Report::Report(){
	reportLong = 0.0;
	reportLat = 0.0;
	reporterLong = 0.0;
	reporterLat = 0.0;
	reporterSpeed = 0.0;
	reportState = true;
	usersFalseJudgements = 0;
	reporterDistance = 0.0;	
}

Report::Report(StringVector report){
	reportLong = stod(report[0]);
	reportLat = stod(report[1]);
	reporterLong = stod(report[2]);
	reporterLat = stod(report[3]);
	reporterSpeed = stod(report[4]);
	reportState = report[5].compare("true")==0;
	CalculateFalseJudgements(report[6]);
	CalculateReporterDistance();
}

double Report::GetReportLong(){
	return reportLong;
}

double Report::GetReportLat(){
	return reportLat;
}

double Report::GetReporterLong(){
	return reporterLong;
}

double Report::GetReporterLat(){
	return reporterLat;
}

double Report::GetReporterSpeed(){
	return reporterSpeed;
}

double Report::GetReporterDistance(){
	return reporterDistance;
}

bool Report::GetReportState(){
	return reportState;
}

double Report::ToRadians(const double& degrees){
	const double LOCAL_PI = 3.1415926535897932385;
	double radians = degrees * LOCAL_PI / 180;
	return radians;
}

void Report::CalculateReporterDistance(){
	/***
	Haversine formula
	http://en.wikipedia.org/wiki/Haversine_formula
	Coordinates in signed decimal degrees (WGS84)
	***/
  double earthRadius = 3958.75;
  double dLat = ToRadians(reporterLat-reportLat);
  double dLng = ToRadians(reporterLong-reportLong);
  double a = sin(dLat/2) * sin(dLat/2) + 
             cos(ToRadians(reportLat)) * cos(ToRadians(reporterLat)) * 
             sin(dLng/2) * sin(dLng/2);
  double c = 2 * atan2(sqrt(a), sqrt(1-a));
  double dist = earthRadius * c;
  double meterConversion = 1609.00;
  reporterDistance = ((dist * meterConversion)/1000.0);//distance in km
}

void Report::CalculateFalseJudgements(std::string usersJudgements){
	usersFalseJudgements = 0;
	for(int i=0;i<usersJudgements.size();i++){
		if(usersJudgements[i]=='2'){
			usersFalseJudgements++;
		}
	}
}


int Report::GetUsersFalseJudgements(){
	return usersFalseJudgements;
}