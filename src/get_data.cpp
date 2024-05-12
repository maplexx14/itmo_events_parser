#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <curl/curl.h>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <fstream>
#include <iostream>


size_t WriteCallbackFunc(void* buffer, size_t size, size_t nmemb, std::string* data) {
	data->append(static_cast<char*>(buffer), size * nmemb);
	return size * nmemb;
}
int get_data_site() {
	CURL* curl;
	CURLcode res;
	std::string dataCall;
	const char* url = "https://itmo.events/";
	std::ofstream ofs("data.json", std::ios::trunc);
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallbackFunc);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &dataCall);

		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			std::cerr << "Error: " << curl_easy_strerror(res) << std::endl;
		}
		curl_easy_cleanup(curl);

		xmlDocPtr doc = htmlReadDoc((xmlChar*)dataCall.c_str(), NULL, NULL, HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
		if (doc == NULL) {
			std::cerr << "Failed to parse the HTML content." << std::endl;
		}
		else {
			xmlXPathContextPtr context = xmlXPathNewContext(doc);
			if (context == NULL) {
				std::cerr << "Failed to create XPath context." << std::endl;
			}
			else {
				xmlXPathObjectPtr event_dates = xmlXPathEvalExpression((xmlChar*)"//p[@class='event__date']", context);
				xmlXPathObjectPtr event_addresses = xmlXPathEvalExpression((xmlChar*)"//div[@class='event__address']", context);
				xmlXPathObjectPtr event_names = xmlXPathEvalExpression((xmlChar*)"//div[@class='event__name']", context);
			
				if (event_addresses && event_dates && event_names) {
					rapidjson::Document document;
					document.SetArray();
					std::cout << event_names->nodesetval->nodeNr << std::endl;
					for (int i = 0; i < event_names->nodesetval->nodeNr; ++i) {
						xmlNodePtr node_name = event_names->nodesetval->nodeTab[i];
						xmlChar* content_name = xmlNodeGetContent(node_name);
						std::cout << content_name << std::endl;
						xmlNodePtr node_date = event_dates->nodesetval->nodeTab[i];
						xmlChar* content_date = xmlNodeGetContent(node_date);
						std::cout << content_date << std::endl;

						xmlNodePtr node_address = event_addresses->nodesetval->nodeTab[i];
						xmlChar* content_address = xmlNodeGetContent(node_address);
						std::cout << content_address << std::endl << std::endl;
						std::cout << content_name << std::endl;
					

						// Create a new JSON document
						std::ifstream file("data.json");
						std::string jsonStr((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

						document.Parse(jsonStr.c_str());


						// Create a JSON object with some data
						rapidjson::Value object(rapidjson::kObjectType);
						object.AddMember("name", rapidjson::Value(std::string(reinterpret_cast<char*>(content_name)).c_str(), 
							document.GetAllocator()).Move(), document.GetAllocator());
						object.AddMember("date", rapidjson::Value(std::string(reinterpret_cast<char*>(content_date)).c_str(), 
							document.GetAllocator()).Move(), document.GetAllocator());
						object.AddMember("address", rapidjson::Value(std::string(reinterpret_cast<char*>(content_address)).c_str(),
							document.GetAllocator()).Move(), document.GetAllocator());

						// Add the object to the array
						document.PushBack(object, document.GetAllocator());


						// Convert the JSON document to a string
						rapidjson::StringBuffer buffer;
						rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
						document.Accept(writer);
						std::string newJsonStr = buffer.GetString();
						std::cout << newJsonStr << std::endl;
						std::ofstream ofs("data.json", std::ios::app);
						std::cout << i << " "<< event_names->nodesetval->nodeNr << std::endl;
						if (i ==6) {
						
							if (ofs.is_open()) {
								ofs << newJsonStr << std::endl;
								ofs.close();
								std::cout << "Data appended to data.json successfully." << std::endl;


							}
							else {
								std::cout << "Failed to open data.json for append." << std::endl;
							}

						}
						
						xmlFree(content_name);

						xmlFree(content_date);
						xmlFree(content_address);
					
						
					}


					xmlXPathFreeObject(event_addresses);
					xmlXPathFreeObject(event_dates);
					xmlXPathFreeObject(event_names);
				}

				xmlXPathFreeContext(context);
			}
			xmlFreeDoc(doc);
		}
		xmlCleanupParser();
	}
	curl_global_cleanup();

	return 0;
}
