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

int main() {
    Document d;
    d.Parse(collectDataFromWeb().c_str());
    for (int i = 0; i < d.Size(); i++) {
        if (strcmp(d[i]["country"].GetString(), "USA") == 0) {
            std::cout << d[i]["country"].GetString() << std::endl;
            std::cout << "Cases: " << d[i]["cases"].GetInt64() << " - " << "Today cases: " << d[i]["todayCases"].GetInt64() << std::endl;
        }
    }
}