#ifndef StiltFox_RDF_Shredder
#define StiltFox_RDF_Shredder
#include <Stilt_Fox/UniversalLibrary/DatabaseConnection.hpp>
#include <nlohmann/json.hpp>
#include <pugixml.hpp>
#include <string>

namespace StiltFox
{
    struct OntologyProperty
    {
        std::string name;
        std::string uri;
        std::string range;
        std::string inverse;
        bool isDataProperty;
    };

    class OntologyFile
    {
        std::string vocabulary;
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> contextMap;
        std::unordered_map<std::string, OntologyProperty> propertyData;

        public:
        OntologyFile(std::string filePath);
        nlohmann::json getContext(std::string owlClass);
        std::string getInverse(std::string objectProperty);
        bool isPropertyData(std::string objectProperty);
    };

    struct RDF_Map
    {
        std::string owl_class;
        std::string owl_id;
        std::unordered_map<std::string,std::vector<std::string>> property_map;

        RDF_Map(std::string owl_class, std::string owl_id, std::unordered_map<std::string,std::vector<std::string>> property_map);
        RDF_Map(){};
    };

    class RDF_Shredder
    {
        OntologyFile ontologyFile;

        public:
        RDF_Shredder(std::string ontologyPath);
        nlohmann::json generateJsonLd(std::unordered_map<std::string, RDF_Map> queryMap, UniversalLibrary::DatabaseConnection* connection);
    };
}
#endif