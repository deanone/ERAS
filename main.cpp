#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include "ERAS.h"
#include "Report.h"
#include "PropertiesParser.h"

int main()
{
	std::ifstream incomingReportStream(ERAS::getExecutablePathAndMatchItWithFilename("incomingReport\\inF.txt").c_str());
	std::string dataline;
	int ReportID;
	StringVector incomingReport;
	while (getline(incomingReportStream,dataline))
	{
		std::istringstream ss(dataline);
		std::string item;
		std::vector<std::string> items;
		while (getline(ss, item, ' '))
		{
			items.push_back(item);
		}
		ReportID = atoi(items[0].c_str());
		for (size_t i = 1; i < items.size(); ++i)
		{
			incomingReport.push_back(items[i]);
		}
		items.clear();
	}
	incomingReportStream.close();
	Report *inReport = new Report(incomingReport);

	std::ifstream pastReportsStream(ERAS::getExecutablePathAndMatchItWithFilename("PastReportsWithNonZeroSpeeds\\5T0F.txt").c_str());
	std::vector<Report*> pastReports;
	while (getline(pastReportsStream,dataline))
	{
		std::istringstream ss(dataline);
		std::string item;
		std::vector<std::string> items;
		while (getline(ss, item, ' '))
		{
			items.push_back(item);
		}
		ReportID = atoi(items[0].c_str());
		StringVector pastReport;
		for (size_t i = 1; i < items.size(); ++i)
		{
			pastReport.push_back(items[i]);
		}
		items.clear();
		Report *pReport = new Report(pastReport);
		pastReports.push_back(pReport);
	}
	pastReportsStream.close();

	PropertiesParser *p = new PropertiesParser(ERAS::getExecutablePathAndMatchItWithFilename("ERAS.properties"));
	double distanceLimit = p->getPropertyAsDouble("distanceLimit"); //e.g. 1.0 km
	double speedLimit = p->getPropertyAsDouble("speedLimit"); //e.g. 40.0 km/h
	bool useJudgements = p->getPropertyAsBool("UseJudgements");
	int typeOfBinEstimation = p->getPropertyAsInt("typeOfBinEstimation");
	int fixedNumOfBins = p->getPropertyAsInt("fixedNumOfBins");
	int lamda = p->getPropertyAsInt("lamda");
	delete p;
	double spamProbability;

	if (pastReports.size() <= 4)
	{
		spamProbability = (inReport->getReporterDistance() <= distanceLimit && inReport->getReporterSpeed() <= speedLimit) ? 0.0 : 1.0;
	}
	else
	{
		spamProbability = ERAS::spamProbabilityCalculator(inReport, pastReports, useJudgements, typeOfBinEstimation, fixedNumOfBins);
	}
	for (size_t i = 0; i < pastReports.size(); ++i)
	{
		delete pastReports[i];
	}
	pastReports.clear();
	delete inReport;
	double threshold =(double)((double)lamda / (double)(1 + lamda));
	
	if (spamProbability>threshold)
	{
		std::cout << "FALSE\n";
	}
	else
	{
		std::cout << "TRUE\n";
	}

	return 0;
}