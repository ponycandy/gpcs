
#include "gpcsserver.h"
int readconfig()
{
    //read xml config here
         // Create an empty property tree object
    boost::property_tree::ptree pt;

    // Load the XML file into the property tree
    boost::property_tree::read_xml("coreconfig.xml", pt);

    // Access the value of 'a' and 'b' and assign them to variables
    //std::string a = pt.get<std::string>("parameters.a.value");
    //double b = pt.get<double>("parameters.b.value");
    return pt.get<int>("parameters.coreportnum.value");
    //读取核心端口，同时也是起始端口

}
int main()
{
    int portnum = readconfig();
	gpcs::gpcsserver coremodule(portnum);
	coremodule.setupserver();
	coremodule.run();

	return 0;
}