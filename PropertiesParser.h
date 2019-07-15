#if !defined(_PROPERTY_FILE_PARSER_H)
#define _PROPERTY_FILE_PARSER_H

#include <map>

class PropertiesParser{
protected:
	/*!A map containing property names as keys and property values as values.*/
	std::map<std::string, std::string> PropMap;
public:
	PropertiesParser(std::string propertiesFilename);
	void AddPropertiesFile(std::string propertiesFilename);
	bool PropertyExists(std::string propertyName);
	std::string GetPropertyAsString(std::string propertyName);
	int GetPropertyAsInt(std::string propertyName);
	bool GetPropertyAsBool(std::string propertyName);
	double GetPropertyAsFloat(std::string propertyName);
	double GetPropertyAsDouble(std::string propertyName);
	std::string GetPropertyAsStringOrDefaultTo(std::string propertyName, std::string defaultPropertyValue);
	int GetPropertyAsIntOrDefaultTo(std::string propertyName, int defaultPropertyValue);
	bool GetPropertyAsBoolOrDefaultTo(std::string propertyName, bool defaultPropertyValue);
	double GetPropertyAsFloatOrDefaultTo(std::string propertyName, double defaultPropertyValue);
	double GetPropertyAsDoubleOrDefaultTo(std::string propertyName, double defaultPropertyValue);
};

#endif