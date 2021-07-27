// CurlTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define CURL_STATICLIB

#include <iostream>
#include <string>
#include <fstream>
#include "curl/curl.h"
#include "rapidjson/document.h"

using namespace rapidjson;

static size_t my_write(void* buffer, size_t size, size_t nmemb, void* param)
{
    std::string& text = *static_cast<std::string*>(param);
    size_t totalSize = size * nmemb;
    text.append(static_cast<char*>(buffer), totalSize);
    return totalSize;
}

std::string collectDataFromWeb() {
    std::string result;
    CURL* curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "https://coronavirus-19-api.herokuapp.com/countries");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_write);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);

        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        res = curl_easy_perform(curl);

        if (CURLE_OK != res)
        {
            std::cout << "CURL error: " << res << std::endl;
        }
    }

    curl_global_cleanup();
    return result;
}

void updateData(std::string data) {
    std::ofstream f;
    f.open("dataCovid.txt", std::ios::out);
    f << data;
    f.close();
}

bool searchDataByDate(std::string date, Document &doc) {
    std::ifstream fileIn;
    fileIn.open("Data\\" + date + ".txt");

    if (fileIn.fail()) return 0;

    std::string temp;

    getline(fileIn, temp);
    doc.Parse(temp.c_str());

    fileIn.close();
}

bool searchDataByCountryName(std::string name, std::string date, Value& data) {
    Document doc;
    if (searchDataByDate(date, doc)) {
        return 0;
    }
    for (int i = 0; i < doc.Size(); i++) {
        if (doc[i]["country"].GetString() == name) {
            data = doc[i].GetObjectW();
            return 1;
        }
    }
}

int main() {
    Document document;
    Value res;
    std::string temp = collectDataFromWeb();
    
    document.Parse(temp.c_str());

    for (int i = 0; i < document.Size(); i++) {
        if (strcmp(document[i]["country"].GetString(), "Croatia") == 0) {
            res = document[i].GetObjectW();
        }
    } 
    std::cout << res["country"].GetString() << " " << res["cases"].GetInt64();
}