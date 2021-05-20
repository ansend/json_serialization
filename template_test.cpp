#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <json/json.h>
#include <string>
#include <json_serialize.h>

using namespace std;

template <typename T>
static T GetData(Json::Value& root)
{
        printf(" this is deducation by compiler\n");
	T temp;
	return temp;
}

template <>
bool GetData(Json::Value& root)
{
        printf(" in specialization for bool\n");
	return root.asBool();
}

template <>
int32_t GetData(Json::Value& root)
{
        printf(" in specialization for int\n");
	return root.asInt();
}

template <>
uint32_t GetData(Json::Value& root)
{
        printf(" in specialization for uint\n");
	return root.asUInt();
}

template <>
int64_t GetData(Json::Value& root)
{
        printf(" in specialization for int64\n");
	return root.asInt64();
}
template <>
uint64_t GetData(Json::Value& root)
{
        printf(" in specialization for uint64\n");
	return root.asUInt64();
}

/*
template <>
string GetData(Json::Value& root)
{
        printf(" in specialization for string\n");
	return root.asString();
}
*/


int main (int argc, char ** argv)
{
    
    Json::Value val_i(1);
    Json::Value val_s("hello world");
    uint32_t ret = GetData<uint32_t>(val_i);
    //uint32_t ret = GetData(val_i);  //argument deducation failed.
    
    printf("ret value is %d\n", ret);
    
    std::string tmp = GetData<string>(val_s);
    printf(" tmp string %s\n", tmp.c_str());
}
