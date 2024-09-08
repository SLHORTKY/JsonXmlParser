#pragma once
#include <string>
#include "Tokenizer.hpp"
#include "Json.hpp"
#include "Xml.hpp"

class Parser
{
private:
    unsigned int current;
    std::vector<TokenJson> JsonTokens;
    std::vector<TokenXml> XmlTokens;

private:
    Json::Object *ParseJsonValue();
    Json::Object *ParseJsonObject();
    Json::Object *ParseJsonArray();

    Xml::Object *ParseXmlValue();
    Xml::Object *ParseXmlObject();
    Xml::Object *ParseXmlArray();

public:
    Parser()
    {
        std::vector<TokenJson> JsonTokens = std::vector<TokenJson>();
        std::vector<TokenXml> XmlTokens = std::vector<TokenXml>();
    }
    Json::Object *ParseJson(std::string &jsonString);
    std::string UnParseJson(Json::Object &object);

    Xml::Object *ParseXml(std::string &XmlString);
    std::string UnParseXml(Xml::Object &object);

    std::string JsonToXml(std::string& jsonString);
    std::string XmlToJson(std::string& XmlString);

private:
    inline TokenJson currentTokenJson() { return this->JsonTokens[current]; };
    inline TokenJson nextTokenJson() { return this->JsonTokens[++current]; };

    inline TokenXml currentTokenXml() { return this->XmlTokens[current]; };
    inline TokenXml nextTokenXml() { return  this->XmlTokens[++current]; };
};
