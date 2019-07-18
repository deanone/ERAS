#include <sstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <dirent.h>
#include <unistd.h>
#include <math.h>

#include "ERAS.h"

bool compare(const double& i, const double& j)
{
  return i < j;
}

void ERAS::histogramCreator(const DoubleVector &input, const BoolVector& states, int typeOfBinsEstimation, int fixedNumOfBins, HISTOGRAM &histogram)
{
	DoubleVector bins;
	IntVector frequencies;
	IntVector frequenciesFalse;
	double minValue = *std::min_element(input.begin(), input.end());
	double maxValue = *std::max_element(input.begin(), input.end());
	
	int numOfBins = (typeOfBinsEstimation == 1) ? fixedNumOfBins : binsEstimationByRule(input, typeOfBinsEstimation);
	
	//minimum number of bins in the histogram is 2
	if (numOfBins == 1)
	{
		numOfBins++;
	}

	double binSize = ((maxValue - minValue) / (double) numOfBins); 
	double tempBin = minValue;
	
	bins.push_back(tempBin);
	frequencies.push_back(0);
	frequenciesFalse.push_back(0);

	for (size_t j = 1; j < numOfBins; ++j)
	{
		tempBin += binSize;
		bins.push_back(tempBin);
		frequencies.push_back(0);
		frequenciesFalse.push_back(0);
	}

	for (size_t k = 0; k < input.size(); ++k)
	{
		if (input[k]>=bins[0] && input[k]<=bins[1])
		{
			frequencies[0]++;
			if (!states[k])
			{
				frequenciesFalse[0]++;
			}
		}
		else if (input[k] > bins[numOfBins-1])
		{
			frequencies[numOfBins-1]++;
			if (!states[k])
			{
				frequenciesFalse[numOfBins-1]++;
			}
		}
		else
		{
			for (size_t l = 1; l < numOfBins - 1; ++l)
			{
				if (input[k] > bins[l] && input[k] <= bins[l+1])
				{
					frequencies[l]++;
					if (!states[k])
					{
						frequenciesFalse[l]++;
					}
					break;
				}
			}
		}
	}

	for (int i = 0; i < numOfBins; ++i)
	{
		histogram.push_back(std::make_pair(bins[i], std::make_pair(frequencies[i], frequenciesFalse[i])));
	}

	bins.clear();
	frequencies.clear();
	frequenciesFalse.clear();
}

std::string ERAS::getExecutablePath()
{
	char result[PATH_MAX];
	size_t found;
	size_t count = readlink("/proc/self/exe", result, PATH_MAX);
	found = std::string(result).find_last_of("/");
	return(std::string(result).substr(0,found) + "/");
}

std::string ERAS::getExecutablePathAndMatchItWithFilename(const std::string& fileName)
{
	std::string execPath = getExecutablePath();
	size_t found = execPath.find_last_of("/");
	execPath = execPath.substr(0, found);
	std::stringstream ss;
	ss << execPath << "/" << fileName;
	return ss.str();
}

double ERAS::bayesProbabilityCalculator(HISTOGRAM& histogram,const double& val)
{
	double BayesProb;
	double binSize = histogram[1].first - histogram[0].first;
	
	if((val<histogram[0].first) || (val>(histogram[histogram.size()-1].first + binSize)))	//	outside histogram
	{
		BayesProb = 0.4;
	}
	else //	inside histogram
	{
		double prob, probF;
		prob = probF = 0.0;
		int numOfVals = 0;

		for (size_t i = 0; i < histogram.size(); ++i)
		{
			numOfVals+=histogram[i].second.first;
		}

		if (val >= histogram[0].first && val <= histogram[1].first)
		{
			prob = ((double)histogram[0].second.first) / ((double)numOfVals);
			probF = ((double)histogram[0].second.second) / ((double)numOfVals);
		}
		else if (val > histogram[histogram.size() - 1].first && val <= (histogram[histogram.size()-1].first + binSize))
		{
			prob = ((double)histogram[histogram.size() - 1].second.first) / ((double)numOfVals);
			probF = ((double)histogram[histogram.size() - 1].second.second) / ((double)numOfVals);
		}
		else
		{
			for (size_t i = 1; i < histogram.size() - 1; ++i)
			{
				if( val > histogram[i].first && val <= histogram[i+1].first)
				{
					prob = ((double)histogram[i].second.first) / ((double)numOfVals);
					probF = ((double)histogram[i].second.second) / ((double)numOfVals);
					break;
				}
			}
		}

		if (probF == prob)	//	all false in specific bin
		{
			BayesProb = 0.9;
		}
		else if (probF==0.0)	//	all true in specific bin
		{
			BayesProb = 0.1;
		}
		else
		{
			BayesProb = probF / prob;
		}
	}
	return BayesProb;
}

double ERAS::scoreCalculator(const DoubleVector& bayesProbs)
{
	//	Product of bayes probs
	double probProduct = 1.0;

	//	Product of the supplementaries of bayes probs
	double _probProduct = 1.0;
	
	for (size_t i = 0; i < bayesProbs.size(); ++i)
	{
		probProduct *= bayesProbs[i];
		_probProduct *= (1-bayesProbs[i]);
	}
	
	double score = probProduct / (probProduct + _probProduct);
	return score;
}

double ERAS::spamProbabilityCalculator(Report *inReport, std::vector<Report*> pastReports, bool useJudgements, int typeOfBinsEstimation, int fixedNumOfBins)
{
	DoubleVector pastDistances;
	DoubleVector pastSpeeds;
	DoubleVector pastFalseJudgements;
	BoolVector pastStates;
	
	for(size_t i = 0; i < pastReports.size(); ++i)
	{
		pastDistances.push_back(pastReports[i]->getReporterDistance());
		pastSpeeds.push_back(pastReports[i]->getReporterSpeed());
		pastStates.push_back(pastReports[i]->getReportState());
		pastFalseJudgements.push_back((double)pastReports[i]->getUsersFalseJudgements());
	}

	DoubleVector bayesProbs;
	
	//	Distance Bayes prob
	HISTOGRAM pastDistancesHistogram;
	ERAS::histogramCreator(pastDistances, pastStates, typeOfBinsEstimation, fixedNumOfBins, pastDistancesHistogram);
	pastDistances.clear();
	double currentDistance = inReport->getReporterDistance();
	double currentDistanceBayesProb = ERAS::bayesProbabilityCalculator(pastDistancesHistogram, currentDistance);
	pastDistancesHistogram.clear();
	bayesProbs.push_back(currentDistanceBayesProb);
	
	//	Speed Bayes prob
	HISTOGRAM pastSpeedsHistogram;
	ERAS::histogramCreator(pastSpeeds, pastStates, typeOfBinsEstimation, fixedNumOfBins, pastSpeedsHistogram);
	pastSpeeds.clear();
	double currentSpeed = inReport->getReporterSpeed();
	double currentSpeedBayesProb = ERAS::bayesProbabilityCalculator(pastSpeedsHistogram, currentSpeed);
	pastSpeedsHistogram.clear();
	bayesProbs.push_back(currentSpeedBayesProb);
	
	//	False judgements Bayes prob
	if (useJudgements)
	{
		HISTOGRAM pastFalseJudgementsHistogram;
		ERAS::histogramCreator(pastFalseJudgements, pastStates, typeOfBinsEstimation, fixedNumOfBins, pastFalseJudgementsHistogram);
		pastFalseJudgements.clear();
		double currentFalseJudgements = (double)inReport->getUsersFalseJudgements();
		double currentFalseJudjementsBayesProb = ERAS::bayesProbabilityCalculator(pastFalseJudgementsHistogram, currentFalseJudgements);
		pastFalseJudgementsHistogram.clear();
		bayesProbs.push_back(currentFalseJudjementsBayesProb);
	}

	pastStates.clear();
	double score = ERAS::scoreCalculator(bayesProbs);
	return score;
}

int ERAS::binsEstimationByRule(const DoubleVector& input, int typeOfBinEstimation)
{
	int numOfBins;
	switch(typeOfBinEstimation)
	{
		case 2:
			numOfBins = binsEstimationRule_2(input.size());
			break;
		case 3:
			numOfBins = binsEstimationRule_3(input.size());
			break;
		case 4:
			numOfBins = binsEstimationRule_4(input.size());
			break;
		case 5:
			numOfBins = binsEstimationRule_5(input);
			break;
		case 6:
			numOfBins = binsEstimationRule_6(input);
			break;
	}
	return numOfBins;
}

int ERAS::binsEstimationRule_2(int n)
{
	double numOfBins_D = std::sqrt((double)n);
	int numOfBins = (int)ceil(numOfBins_D);
	return numOfBins;
}

int ERAS::binsEstimationRule_3(int n)
{
	double log2_n = log10((double)n) / log10(2.0);
	double numOfBins_D = log2_n + 1.0;
	int numOfBins = (int)ceil(numOfBins_D);
	return numOfBins;
}

int ERAS::binsEstimationRule_4(int n)
{
	double numOfBins_D = 2 * std::pow(n, 1 / 3.);
	int numOfBins = (int)ceil(numOfBins_D);
	return numOfBins;
}

int ERAS::binsEstimationRule_5(const DoubleVector& input)
{
	int n = input.size();
	double mean, stdDev;
	findMeanAndStdDevOfVector(input, mean, stdDev);
	double binSize = (3.5 * stdDev) / (double)(std::pow(n, 1 / 3.));
	double minElement = *std::min_element(input.begin(), input.end());
	double maxElement = *std::max_element(input.begin(), input.end());
	double numOfBins_D = (maxElement - minElement) / binSize;
	int numOfBins = (int)ceil(numOfBins_D);
	return numOfBins;
}

int ERAS::binsEstimationRule_6(DoubleVector input)
{
	int n = input.size();
	double iqr = IQR(input);
	double binSize = (2 * iqr) / (double)(std::pow(n, 1 / 3.));
	double minElement = *std::min_element(input.begin(), input.end());
	double maxElement = *std::max_element(input.begin(), input.end());
	double numOfBins_D = (maxElement - minElement) / binSize;
	int numOfBins = (int)ceil(numOfBins_D);
	return numOfBins;
}

void ERAS::findMeanAndStdDevOfVector(const DoubleVector& v, double& mean, double& stdDev)
{
	mean = stdDev = 0.0;
	double sum = std::accumulate(v.begin(), v.end(), 0.0);
	mean = sum / (double) v.size();
	double sq_sum = std::inner_product(v.begin(), v.end(), v.begin(), 0.0);
	stdDev = std::sqrt(std::abs(sq_sum / (double)v.size() - mean * mean));
}

double ERAS::IQR(DoubleVector v)
{
	std::stable_sort(v.begin(), v.end(), compare);
	double Q1 = v[v.size() * 1 / 4];
	double Q3 = v[v.size() * 3 / 4];
	return (Q3 - Q1);
}