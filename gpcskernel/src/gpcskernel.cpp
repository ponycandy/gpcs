
#include "gpcsserver.h"
int main()
{
    int portnum;
    char* configFile = std::getenv("GPCS_CONFIG_XML_PATH");
    if (configFile) {
        std::cout << "Config file path: " << configFile << std::endl;
        // Now you can use configFile to access the config.xml file
    }
    else {
        std::cerr << "Environment variable CONFIG_XML_PATH is not set" << std::endl;
    }

    boost::property_tree::ptree pt;
    try {
        boost::property_tree::read_xml(configFile, pt);
    }
    catch (boost::property_tree::xml_parser_error& e) {
        std::cerr << "Failed to load config.xml: " << e.what() << std::endl;
        return 0;
    }
    try {
        portnum = pt.get<int>("parameters.coreportnum.value");
    }
    catch (boost::property_tree::ptree_bad_path& e) {
        std::cerr << "Invalid XML format: " << e.what() << std::endl;
        return 0;
    }
	gpcs::gpcsserver coremodule(portnum);
	coremodule.setupserver();
	coremodule.run();

	return 0;
}