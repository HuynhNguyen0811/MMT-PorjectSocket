// CurlTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define CURL_STATICLIB

#include <iostream>
#include <string>
#include <fstream>
#include "curl/curl.h"
#include "rapidjson/document.h"

using namespace rapidjson;
using namespace std;

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

bool checkLogin(string user, string pass) {
    ifstream fileIn;
    fileIn.open("login.txt");

    if (fileIn.fail()) return 0;

    string checkUser, checkPass;
    while (fileIn) {
        getline(fileIn, checkUser, ',');
        getline(fileIn, checkPass, '\n');
        if (checkUser == user && checkPass == pass) {
            fileIn.close();
            return 1;
        }
    }

    fileIn.close();
    return 0;
}

bool checkRegis(string user) {
    ifstream fileIn;
    fileIn.open("login.txt");

    if (fileIn.fail()) return 1;

    string checkUser, checkPass;
    while (fileIn) {
        getline(fileIn, checkUser, ',');
        getline(fileIn, checkPass, '\n');
        if (checkUser == user) {
            fileIn.close();
            return 0;
        }
    }

    fileIn.close();
    return 1;
}

void regis(string user, string pass) {
    ofstream fileOut;
    fileOut.open("login.txt", ios_base::app);

    fileOut << user << "," << pass << endl;

    fileOut.close();
}

void charToString(string& destination, const char* source, int n) {
    for (int i = 0; i < n; i++) {
        destination += source[i];
    }
}

int main() {
    //Document document;
    //Value res;
    //std::string temp = collectDataFromWeb();
    //
    //document.Parse(temp.c_str());

    //for (int i = 0; i < document.Size(); i++) {
    //    if (strcmp(document[i]["country"].GetString(), "Croatia") == 0) {
    //        res = document[i].GetObjectW();
    //    }
    //    //std::cout << document[i]["cases"].GetInt64() << std::endl;
    //} 
    //std::cout << res["country"].GetString() << " " << res["cases"].GetInt64();

    string checkUser, checkPass;
    /*while (true) {
        cin >> checkUser;
        cin >> checkPass;
        cout << "Login " << checkLogin(checkUser, checkPass);
        cout << "Regis" << checkRegis(checkUser);
        regis(checkUser, checkPass);
    }*/
    charToString(checkUser, "hahahihi", 9);
    cout << checkUser;
}