#include <string>
#include "DataTypes.h"
#include "Report.h"

//model based on http://digital.cs.usu.edu/~erbacher/publications/Bayes-Vikas2.pdf
namespace ERAS{
	std::string GetExecutablePath(); 
	std::string GetExecutablePathAndMatchItWithFilename(std::string ffname);
	void FindMeanAndStdDevOfVector(const DoubleVector& v, double& Mean, double& StdDev);
	double IQR(DoubleVector v);//Interquartile range
	void HistogramCreator(const DoubleVector& input,const BoolVector& states,int typeOfBinsEstimation,int fixedNumOfBins, HISTOGRAM& histogram);
	int BinsEstimationByRule(const DoubleVector& input,int typeOfBinEstimation);
	int BinsEstimationRule_2(int n);//Square-root choice
	int BinsEstimationRule_3(int n);//Sturges' formula
	int BinsEstimationRule_4(int n);//Rice Rule
	int BinsEstimationRule_5(const DoubleVector& input);//Scott's normal reference rule
	int BinsEstimationRule_6(DoubleVector input);//Freedman–Diaconis' choice
	double BayesProbabilityCalculator(HISTOGRAM& histogram,const double& val);
	double ScoreCalculator(const DoubleVector& bayesProbs);
	double SpamProbabilityCalculator(Report *inReport,std::vector<Report*> pastReports,bool UseJudgements,int typeOfBinsEstimation,int fixedNumOfBins);
}