#ifndef ERAS_H
#define ERAS_H

#include <string>
#include "DataTypes.h"
#include "Report.h"

// model based on http://digital.cs.usu.edu/~erbacher/publications/Bayes-Vikas2.pdf
namespace ERAS
{
	std::string getExecutablePath(); 
	std::string getExecutablePathAndMatchItWithFilename(const std::string& fileName);
	
	void findMeanAndStdDevOfVector(const DoubleVector& v, double& mean, double& stdDev);

	//	Interquartile range
	double IQR(DoubleVector v);
	
	void histogramCreator(const DoubleVector &input, const BoolVector& states, int typeOfBinsEstimation, int fixedNumOfBins, HISTOGRAM &histogram);
	
	// Bin estimation rules
	int binsEstimationByRule(const DoubleVector &input, int typeOfBinEstimation);
	
	//	Square-root choice
	int binsEstimationRule_2(int n);
	
	//	Sturges' formula
	int binsEstimationRule_3(int n);
	
	//	Rice Rule
	int binsEstimationRule_4(int n);
	
	//	Scott's normal reference rule
	int binsEstimationRule_5(const DoubleVector &input);
	
	//	Freedman–Diaconis' choice
	int binsEstimationRule_6(DoubleVector input);
	
	double bayesProbabilityCalculator(HISTOGRAM &histogram, const double &val);
	double scoreCalculator(const DoubleVector &bayesProbs);
	double spamProbabilityCalculator(Report *inReport, std::vector<Report*> pastReports, bool useJudgements, int typeOfBinsEstimation, int fixedNumOfBins);
}

#endif	//	ERAS_H