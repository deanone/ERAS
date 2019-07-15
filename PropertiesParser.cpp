#include <fstream>
#include <sstream>
#include "PropertiesParser.h"


PropertiesParser::PropertiesParser(std::string propertiesFilename){
	AddPropertiesFile(propertiesFilename);
}

void PropertiesParser::AddPropertiesFile(std::string propertiesFilename){
	std::ifstream properties_file;
	properties_file.open(propertiesFilename.c_str(), std::ios::in);
	std::string line;
	if (properties_file.good()){
		while (properties_file.eof() == false){
			line.clear();
			getline(properties_file, line);
			if (line.length() > 0 && line[0] != '#'){
				std::string propertyName;
				std::string propertyValue;
				std::stringstream ss(line);
				char c;//c used to eat the '=' character
				ss >> propertyName >> c >> propertyValue;
				PropMap[propertyName] = propertyValue;
			}
		}
	}
	properties_file.close();
}

bool PropertiesParser::PropertyExists(std::string propertyName){
	return (!(PropMap.find(propertyName) == PropMap.end()));
}

std::string PropertiesParser::GetPropertyAsString(std::string propertyName){
	return PropMap[propertyName];
}

int PropertiesParser::GetPropertyAsInt(std::string propertyName){
	int propertyValue;
	std::stringstream strStream(PropMap[propertyName]);
	strStream >> propertyValue;
	return propertyValue;
}

bool PropertiesParser::GetPropertyAsBool(std::string propertyName){
	bool propertyValue;
	propertyValue = (PropMap[propertyName] == "true");
	return propertyValue;
}

double PropertiesParser::GetPropertyAsFloat(std::string propertyName){
	double propertyValue;
	std::stringstream strStream(PropMap[propertyName]);
	strStream >> propertyValue;
	return propertyValue;
}

double PropertiesParser::GetPropertyAsDouble(std::string propertyName){
	double propertyValue;
	std::stringstream strStream(PropMap[propertyName]);
	strStream >> propertyValue;
	return propertyValue;
}

std::string PropertiesParser::GetPropertyAsStringOrDefaultTo(std::string propertyName, std::string defaultPropertyValue){
	if (PropertyExists(propertyName))
		return GetPropertyAsString(propertyName);
	else
		return defaultPropertyValue;
}

int PropertiesParser::GetPropertyAsIntOrDefaultTo(std::string propertyName, int defaultPropertyValue){
	if (PropertyExists(propertyName))
		return GetPropertyAsInt(propertyName);
	else
		return defaultPropertyValue;
}

bool PropertiesParser::GetPropertyAsBoolOrDefaultTo(std::string propertyName, bool defaultPropertyValue){
	if (PropertyExists(propertyName))
		return GetPropertyAsBool(propertyName);
	else
		return defaultPropertyValue;
}

double PropertiesParser::GetPropertyAsFloatOrDefaultTo(std::string propertyName, double defaultPropertyValue){
	if (PropertyExists(propertyName))
		return GetPropertyAsFloat(propertyName);
	else
		return defaultPropertyValue;
}

double PropertiesParser::GetPropertyAsDoubleOrDefaultTo(std::string propertyName, double defaultPropertyValue){
	if (PropertyExists(propertyName))
		return GetPropertyAsDouble(propertyName);
	else
		return defaultPropertyValue;
}