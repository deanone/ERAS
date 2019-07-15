#include <Windows.h>
#include <sstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include "ERAS.h"

bool compare(const double& i,const double& j){
  return i<j;
}
void ERAS::HistogramCreator(const DoubleVector& input,const BoolVector& states,int typeOfBinsEstimation,int fixedNumOfBins, HISTOGRAM& histogram){
	DoubleVector bins;
	IntVector frequencies;
	IntVector frequenciesFalse;
	double minValue = *std::min_element(input.begin(),input.end());
	double maxValue = *std::max_element(input.begin(),input.end());
	int numOfBins = (typeOfBinsEstimation == 1)?fixedNumOfBins:BinsEstimationByRule(input,typeOfBinsEstimation);
	if(numOfBins==1){numOfBins++;}//minimum number of bins in the histogram is 2
	double binSize = ((maxValue-minValue)/(double)numOfBins); 
	double tempBin = minValue;
	bins.push_back(tempBin);
	frequencies.push_back(0);
	frequenciesFalse.push_back(0);
	for(unsigned int j=1;j<numOfBins;j++){
		tempBin += binSize;
		bins.push_back(tempBin);
		frequencies.push_back(0);
		frequenciesFalse.push_back(0);
	}
	for(unsigned int k=0;k<input.size();k++){
		if(input[k]>=bins[0] && input[k]<=bins[1]){
			frequencies[0]++;
			if(!states[k]){frequenciesFalse[0]++;}
		}else if(input[k]>bins[numOfBins-1]){
			frequencies[numOfBins-1]++;
			if(!states[k]){frequenciesFalse[numOfBins-1]++;}
		}else{
			for(unsigned int l=1;l<numOfBins-1;l++){
				if(input[k]>bins[l] && input[k]<=bins[l+1]){
					frequencies[l]++;
					if(!states[k]){frequenciesFalse[l]++;}
					break;
				}
			}
		}
	}

	for(int i=0;i<numOfBins;i++){
		histogram.push_back(std::make_pair(bins[i],std::make_pair(frequencies[i],frequenciesFalse[i])));
	}
	bins.clear();
	frequencies.clear();
	frequenciesFalse.clear();
}

std::string ERAS::GetExecutablePath() {
	std::vector<char> executablePath(MAX_PATH);
	DWORD result = ::GetModuleFileNameA(
		NULL, &executablePath[0], static_cast<DWORD>(executablePath.size())
		);
	while(result == executablePath.size()){
		executablePath.resize(executablePath.size() * 2);
		result = ::GetModuleFileNameA(
			NULL, &executablePath[0], static_cast<DWORD>(executablePath.size())
			);
	}
	if(result == 0){
		throw std::runtime_error("GetModuleFileName() failed");
	}
	return std::string(executablePath.begin(), executablePath.begin() + result);
}
std::string ERAS::GetExecutablePathAndMatchItWithFilename(std::string ffname){
	std::string execPath = GetExecutablePath();
	size_t found=execPath.find_last_of("\\");
	execPath = execPath.substr(0,found);
	std::stringstream ss;
	ss << execPath << "\\" << ffname;
	return ss.str();
}

double ERAS::BayesProbabilityCalculator(HISTOGRAM& histogram,const double& val){
	double BayesProb;
	double binSize = histogram[1].first - histogram[0].first;
	if((val<histogram[0].first) || (val>(histogram[histogram.size()-1].first + binSize))){//outside histogram
		BayesProb = 0.4;
	}else{//inside histogram
		double prob,probF;
		prob = probF= 0.0;
		int numOfVals=0;
		for(int i=0;i<histogram.size();i++){
			numOfVals+=histogram[i].second.first;
		}
		if(val>=histogram[0].first && val<=histogram[1].first){
			prob = ((double)histogram[0].second.first)/((double)numOfVals);
			probF = ((double)histogram[0].second.second)/((double)numOfVals);
		}else if(val>histogram[histogram.size()-1].first && val<=(histogram[histogram.size()-1].first + binSize)){
			prob = ((double)histogram[histogram.size()-1].second.first)/((double)numOfVals);
			probF = ((double)histogram[histogram.size()-1].second.second)/((double)numOfVals);
		}else{
			for(unsigned int i=1;i<histogram.size()-1;i++){
				if(val>histogram[i].first && val<=histogram[i+1].first){
					prob = ((double)histogram[i].second.first)/((double)numOfVals);
					probF = ((double)histogram[i].second.second)/((double)numOfVals);
					break;
				}
			}
		}
		if(probF==prob){//all false in specific bin
			BayesProb = 0.9;
		}else if(probF==0.0){//all true in specific bin
			BayesProb = 0.1;
		}else{
			BayesProb = probF/prob;
		}
	}
	return BayesProb;
}

double ERAS::ScoreCalculator(const DoubleVector& bayesProbs){
	double probProduct = 1.0;//product of bayes probs
	double _probProduct = 1.0;//product of the supplementaries of bayes probs
	for(int i=0;i<bayesProbs.size();i++){
		probProduct *= bayesProbs[i];
		_probProduct *= (1-bayesProbs[i]);
	}
	double score = probProduct/(probProduct + _probProduct);
	return score;
}

double ERAS::SpamProbabilityCalculator(Report *inReport,std::vector<Report*> pastReports,bool UseJudgements,int typeOfBinsEstimation,int fixedNumOfBins){
	DoubleVector pastDistances;
	DoubleVector pastSpeeds;
	DoubleVector pastFalseJudgements;
	BoolVector pastStates;
	for(int i=0;i<pastReports.size();i++){
		pastDistances.push_back(pastReports[i]->GetReporterDistance());
		pastSpeeds.push_back(pastReports[i]->GetReporterSpeed());
		pastStates.push_back(pastReports[i]->GetReportState());
		pastFalseJudgements.push_back((double)pastReports[i]->GetUsersFalseJudgements());
	}

	DoubleVector BayesProbs;
	//distance Bayes prob
	HISTOGRAM pastDistancesHistogram;
	ERAS::HistogramCreator(pastDistances,pastStates,typeOfBinsEstimation,fixedNumOfBins,pastDistancesHistogram);
	pastDistances.clear();
	double currentDistance = inReport->GetReporterDistance();
	double currentDistanceBayesProb = ERAS::BayesProbabilityCalculator(pastDistancesHistogram,currentDistance);
	pastDistancesHistogram.clear();
	BayesProbs.push_back(currentDistanceBayesProb);
	//speed Bayes prob
	HISTOGRAM pastSpeedsHistogram;
	ERAS::HistogramCreator(pastSpeeds,pastStates,typeOfBinsEstimation,fixedNumOfBins,pastSpeedsHistogram);
	pastSpeeds.clear();
	double currentSpeed = inReport->GetReporterSpeed();
	double currentSpeedBayesProb = ERAS::BayesProbabilityCalculator(pastSpeedsHistogram,currentSpeed);
	pastSpeedsHistogram.clear();
	BayesProbs.push_back(currentSpeedBayesProb);
	//false judgements Bayes prob
	if(UseJudgements){
		HISTOGRAM pastFalseJudgementsHistogram;
		ERAS::HistogramCreator(pastFalseJudgements,pastStates,typeOfBinsEstimation,fixedNumOfBins,pastFalseJudgementsHistogram);
		pastFalseJudgements.clear();
		double currentFalseJudgements = (double)inReport->GetUsersFalseJudgements();
		double currentFalseJudjementsBayesProb = ERAS::BayesProbabilityCalculator(pastFalseJudgementsHistogram,currentFalseJudgements);
		pastFalseJudgementsHistogram.clear();
		BayesProbs.push_back(currentFalseJudjementsBayesProb);
	}
	pastStates.clear();
	double score = ERAS::ScoreCalculator(BayesProbs);
	return score;
}

int ERAS::BinsEstimationByRule(const DoubleVector& input,int typeOfBinEstimation){
	int numOfBins;
	switch(typeOfBinEstimation){
		case 2://Square-root choice: k= ceil(n^1/2)
			numOfBins = BinsEstimationRule_2(input.size());
			break;
		case 3://Sturges' formula k = ceil(log2n + 1)
			numOfBins = BinsEstimationRule_3(input.size());
			break;
		case 4://Rice Rule k = ceil(2*n^1/3)
			numOfBins = BinsEstimationRule_4(input.size());
			break;
		case 5://Scott's normal reference rule h = 3.5*StdDev/n^1/3
			numOfBins = BinsEstimationRule_5(input);
			break;
		case 6://Freedman–Diaconis' choice h = 2*IQR/n^1/3
			numOfBins = BinsEstimationRule_6(input);
			break;
	}
	return numOfBins;
}
int ERAS::BinsEstimationRule_2(int n){//Square-root choice: k= ceil(n^1/2)
	double numOfBins_D = std::sqrt((double)n);
	int numOfBins = (int)ceil(numOfBins_D);
	return numOfBins;
}
int ERAS::BinsEstimationRule_3(int n){//Sturges' formula k = ceil(log2n + 1)
	double log2_n = log10((double)n)/log10(2.0);
	double numOfBins_D = log2_n+1.0;
	int numOfBins = (int)ceil(numOfBins_D);
	return numOfBins;
}
int ERAS::BinsEstimationRule_4(int n){//Rice Rule k = ceil(2*n^1/3)
	double numOfBins_D = 2*std::pow(n,1/3.);
	int numOfBins = (int)ceil(numOfBins_D);
	return numOfBins;
}
int ERAS::BinsEstimationRule_5(const DoubleVector& input){//Scott's normal reference rule h = 3.5*StdDev/n^1/3
	int n = input.size();
	double Mean, StdDev;
	FindMeanAndStdDevOfVector(input,Mean,StdDev);
	double binSize = (3.5*StdDev)/(double)(std::pow(n,1/3.));
	double minElement = *std::min_element(input.begin(),input.end());
	double maxElement = *std::max_element(input.begin(),input.end());
	double numOfBins_D = (maxElement-minElement)/binSize;
	int numOfBins = (int)ceil(numOfBins_D);
	return numOfBins;
}
int ERAS::BinsEstimationRule_6(DoubleVector input){//Freedman–Diaconis' choice h = 2*IQR/n^1/3
	int n = input.size();
	double iqr = IQR(input);
	double binSize = (2*iqr)/(double)(std::pow(n,1/3.));
	double minElement = *std::min_element(input.begin(),input.end());
	double maxElement = *std::max_element(input.begin(),input.end());
	double numOfBins_D = (maxElement-minElement)/binSize;
	int numOfBins = (int)ceil(numOfBins_D);
	return numOfBins;
}

void ERAS::FindMeanAndStdDevOfVector(const DoubleVector& v, double& Mean, double& StdDev){
	Mean = StdDev = 0.0;
	double sum = std::accumulate(v.begin(), v.end(), 0.0);
	Mean = sum / (double)v.size();
	double sq_sum = std::inner_product(v.begin(), v.end(), v.begin(), 0.0);
	StdDev = std::sqrt(std::abs(sq_sum / (double)v.size() - Mean * Mean));
}
double ERAS::IQR(DoubleVector v){//Interquartile range
	std::stable_sort(v.begin(),v.end(),compare);
	double Q1 = v[v.size()*1/4];
	double Q3 = v[v.size()*3/4];
	return (Q3-Q1);
}