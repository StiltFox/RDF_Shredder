#include "RDF_Shredder.hpp"
#include <vector>
#include <thread>
#include <mutex>
#include <iostream>

using namespace std;
using namespace pugi;
using namespace StiltFox;
using namespace StiltFox::UniversalLibrary;
using namespace nlohmann;

mutex context_map_mutex;
mutex ld_data_mutex;

string shaveUrl(string url)
{
    return url.substr(url.find_last_of("#") + 1);
}

RDF_Shredder::RDF_Shredder(string ontologyPath) : ontologyFile(ontologyPath)
{}

void runQuery(string query, RDF_Map rdfMap, DatabaseConnection* connection, OntologyFile* ontologyFile, json* ld)
{
    vector<unordered_map<string,string>> results = connection->performQuery(query);
    json contextMap = ontologyFile->getContext(rdfMap.owl_class);

    
    for (int x=0; x<results.size(); x++)
    {
        lock_guard<mutex> guard(ld_data_mutex);
        (*ld)[results[x][rdfMap.owl_id]]["@context"] = contextMap;
        (*ld)[results[x][rdfMap.owl_id]]["@id"] = results[x][rdfMap.owl_id];
        (*ld)[results[x][rdfMap.owl_id]]["@type"] = contextMap["@vocab"].get<string>() + rdfMap.owl_class;

        for (auto& item : contextMap.items())
        {
            string propertyId = shaveUrl(item.value().get<string>());
            for (string columnName : rdfMap.property_map[propertyId])
            {
                if (results[x][columnName] != "" && !item.key().starts_with("@"))
                {
                    if (ontologyFile->isPropertyData(propertyId)) 
                    {
                        if (ontologyFile->getRange(propertyId) != "string")
                        {
                            try
                            {
                                (*ld)[results[x][rdfMap.owl_id]][propertyId] = stoi(results[x][columnName]);
                            }
                            catch(...)
                            {
                                // do nothing. Allow value to not be set.
                            }
                        }
                        else
                        {
                            (*ld)[results[x][rdfMap.owl_id]][propertyId] = results[x][columnName];
                        }
                    }
                    else
                    {
                        (*ld)[results[x][rdfMap.owl_id]][propertyId].push_back(results[x][columnName]);
                        string inverse = ontologyFile->getInverse(propertyId);
                        if (inverse != "")
                            (*ld)[results[x][columnName]][inverse].push_back((*ld)[results[x][rdfMap.owl_id]]["@id"]);
                    }
                }
            }
        }
    }
}

json RDF_Shredder::generateJsonLd(unordered_map<string, RDF_Map> queryMap, DatabaseConnection* connection)
{
    json json_ld;
    std::vector<thread> activeThreads;
    connection->connect();

    for (auto entry : queryMap)
    {
        activeThreads.push_back(thread(runQuery, entry.first, entry.second, connection, &ontologyFile, &json_ld));
    }

    for (int x=0; x<activeThreads.size(); x++) activeThreads[x].join();
    
    json output = json::array();
    for (auto& item : json_ld.items()) output.push_back(item.value());

    return output;
}

RDF_Map::RDF_Map(string clazz, string id, unordered_map<string,vector<string>> map)
{
    owl_class = clazz;
    owl_id = id;
    property_map = map;
}

void processProperty(xml_node property, bool isData, unordered_map<string,unordered_map<string,string>>* contextMap, unordered_map<string, OntologyProperty>* proprtyData)
{
    OntologyProperty values;
    values.name = shaveUrl(property.attribute("rdf:about").as_string());
    values.uri = property.attribute("rdf:about").as_string();
    values.range = shaveUrl(property.child("rdfs:range").attribute("rdf:resource").as_string());
    values.inverse = shaveUrl(property.child("owl:inverseOf").attribute("rdf:resource").as_string());
    values.isDataProperty = isData;

    lock_guard guard(context_map_mutex);
    if (values.inverse != "")
    {
        (*proprtyData)[values.inverse].inverse = values.name;
    }
    else
    {
        values.inverse = (*proprtyData)[values.name].inverse;
    }

    for (xml_node domain : property.children("rdfs:domain"))
    {
        string domainName = shaveUrl(domain.attribute("rdf:resource").as_string());
        (*contextMap)[domainName][values.name] = values.uri;
        (*proprtyData)[values.name] = values;
    }
}

OntologyFile::OntologyFile(std::string filePath)
{
    xml_document document;
    document.load_file(filePath.c_str());

    vocabulary = string(document.child("rdf:RDF").child("owl:Ontology").attribute("rdf:about").as_string()) + "#";
    
    for (xml_node property : document.child("rdf:RDF").children("owl:DatatypeProperty"))
        processProperty(property, true, &contextMap, &propertyData);
    for (xml_node property : document.child("rdf:RDF").children("owl:ObjectProperty"))
        processProperty(property, false, &contextMap, &propertyData);
}

json OntologyFile::getContext(string owlClass)
{
    json output;

    if (contextMap.contains(owlClass))
    {
        contextMap[owlClass]["@vocab"] = vocabulary;
        output = contextMap[owlClass];
    }

    return output;
}

string OntologyFile::getInverse(string objectProperty)
{
    return propertyData[objectProperty].inverse;
}

bool OntologyFile::isPropertyData(string objectProperty)
{
    return propertyData[objectProperty].isDataProperty;
}

string OntologyFile::getRange(string objectProperty)
{
    return propertyData[objectProperty].range;
}