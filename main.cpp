#include "json_serialize.h"
#include <stdio.h>
#include <stdint.h>
#include <iomanip>
#include <iostream>
using namespace std;


enum color_set 
   {   red,
    yellow,
      blue,
     white,
     black
   };
	


struct V2xLaneAttrStruct:public CJsonObjectBase
{
  
   bool           opt;
   int            share;      // 共享信息
   uint8_t        type;       // 车道类型
   int            attrs;      // 车道属性
};


struct CTestStruct : public CJsonObjectBase
{
CTestStruct()
	{
		SetPropertys();
	}
        color_set co;
        double Point;
	uint64_t MsgID;
	string MsgTitle;
	string MsgContent;
protected:
	//子类需要实现此函数，并且将相应的映射关系进行设置 
	virtual void SetPropertys()
	{
		SetProperty("Col", asUInt, &co);
		SetProperty("Point", asDouble, &Point);
		SetProperty("MsgID", asUInt64, &MsgID);
		SetProperty("MsgTitle", asString, &MsgTitle);
		SetProperty("MsgContent", asString, &MsgContent);
	}
public:
        void print_obj()
        {
           ::std::cout << "Col:" << co << endl;
           ::std::cout << "Point:" << setiosflags(ios::fixed) << Point << endl;
           ::std::cout << "MsgID:" << MsgID << endl;
           ::std::cout << "MsgTitle:" << MsgTitle << endl;
           ::std::cout << "MsgContent:" << MsgContent << endl;

        }
};

struct CSubTestStruct : public CJsonObjectBase  
{  
	CSubTestStruct()  
	{  
		SubMsgID = 0;  
		SetPropertys();  
	}  
	
	uint64_t SubMsgID;  
	string SubMsgTitle;  
protected:  
	//子类需要实现此函数，并且将相应的映射关系进行设置   
	virtual void SetPropertys()  
	{  
		SetProperty("SubMsgID", asUInt64, &SubMsgID);  
		SetProperty("SubMsgTitle", asString, &SubMsgTitle);  
	} 

public:
       void print_obj()
       {
           ::std::cout << "SubMsgID:" << SubMsgID << endl;
           ::std::cout << "SubMsgTitle:" << SubMsgTitle << endl;

       } 
};  


struct CBaseTestStruct : public CJsonObjectBase  
{  
	CBaseTestStruct()  
	{  
		SetPropertys();  
	}  
	~CBaseTestStruct()
	{
		for (vector<CSubTestStruct*>::iterator it = testListSpecial.begin(); it != testListSpecial.end(); ++ it)
		{
			delete (*it);
		}
	}
	uint64_t MsgID;  
	string MsgTitle;  
	string MsgContent;  
	CSubTestStruct subObj;  
	vector<int> intList;
	list<string> testList;
	vector<CSubTestStruct*> testListSpecial;
	vector<shared_ptr<CSubTestStruct>> testListSpecialPtr;
protected:  
	CJsonObjectBase* GenerateJsonObjForDeSerialize(const string& propertyName)
	{
		if("testListSpecial" == propertyName || "testListSpecialPtr" == propertyName)
		{
			return new CSubTestStruct();
		}
		return NULL;
	}
	//子类需要实现此函数，并且将相应的映射关系进行设置   
	virtual void SetPropertys()  
	{
		SetProperty("MsgID", asUInt64, &MsgID);  
		SetProperty("MsgTitle", asString, &MsgTitle);  
		SetProperty("MsgContent", asString, &MsgContent);  
		SetProperty("subObj", asJsonObj, &subObj);
		SetProperty("intList", asVectorArray, &intList);
	        SetProperty("testList", asListArray, &testList, asString);
	        SetProperty("testListSpecial", asVectorArray, &testListSpecial, asJsonObj);		
	        SetProperty("testListSpecialPtr", asVectorArray, &testListSpecialPtr, asJsonObjPtr);		
	} 	

public: 

       void print_obj()
       {
           ::std::cout << "MsgID:"          << MsgID       << endl;
           ::std::cout << "MsgTitle:"       << MsgTitle    << endl;
           ::std::cout << "MsgContent:"     << MsgContent  << endl;
           ::std::cout << "SubMsgContent:"  << "below" << endl;
           subObj.print_obj();
           for(vector<int>::iterator it = intList.begin(); it != intList.end(); it++)
           {
              ::std::cout << "intList:"  << *it << endl;
           }
         
           for(std::list<string>::iterator it = testList.begin(); it != testList.end(); it++)
           {
              ::std::cout << "testList:"  << *it << endl;
           }

           for(vector<CSubTestStruct*>::iterator it = testListSpecial.begin(); it != testListSpecial.end(); it++)
           {
              ::std::cout << "testListSpecial:"  <<  endl;
              (*it)->print_obj();
           }
 
           for(vector<shared_ptr<CSubTestStruct>>::iterator it = testListSpecialPtr.begin(); it != testListSpecialPtr.end(); it++)
           {
              ::std::cout << "testListSpecialPtr:"  <<  endl;
              (*it)->print_obj();
           }
 
       }

};  


void test_list()
{

   std::list<string> testList;
   testList.push_back("33333333");
   testList.push_back("55555555");
   Json::Value tvalue = CJsonObjectBase::DoArraySerialize<list<string>>(&testList);
   std::string out = CJsonObjectBase::JsonValueToString(tvalue);
   cout << out << endl;
}


void test_single_struct()
{
   
   CTestStruct * obj = new CTestStruct();
   obj->co     = red;
   obj->Point  = 104.0852525;
   obj->MsgID = 100;
   obj->MsgTitle   = "single struct test";
   obj->MsgContent  = "single msg content";
   std::string strTest = obj->Serialize();
   printf("point data is %.12f\n",obj->Point);
   cout << "single_struct:" <<  strTest  << endl;

   obj->print_obj();

   // DeSerialize
   CTestStruct empty;
   empty.DeSerialize(strTest.c_str());
   printf("DeSerialize Result\n");
   empty.print_obj();

   std::shared_ptr<CTestStruct>  obj_ptr(new CTestStruct());
   obj_ptr->co    = red;
   obj_ptr->Point  = 104.0852525;
   obj_ptr->MsgID = 100;
   obj_ptr->MsgTitle   = "single struct test";
   obj_ptr->MsgContent  = "single msg content";
   std::string strTestp = obj_ptr->Serialize();
   cout << "single_struct:" <<  strTestp  << endl;

   // DeSerialize
   CTestStruct emptyp;
   emptyp.DeSerialize(strTestp.c_str());
   emptyp.print_obj();

   
}

void test_common_struct()
{

   Json::Value tvalue;
   std::string out;
   vector<CSubTestStruct*> testListSpecial;
   CSubTestStruct * obj = new CSubTestStruct();
   obj->SubMsgID = 2;
   obj->SubMsgTitle = "333";
   testListSpecial.push_back(obj);
   obj = new CSubTestStruct();
   obj->SubMsgID = 3;
   obj->SubMsgTitle = "4444";
   testListSpecial.push_back(obj);	
   tvalue = CJsonObjectBase::DoArraySerialize<vector<CJsonObjectBase*>>((vector<CJsonObjectBase*>*)&testListSpecial);
   out = CJsonObjectBase::JsonValueToString(tvalue);
   cout << out << endl;

}

void test_common_struct_ptr()
{

   Json::Value tvalue;
   std::string out;
   vector<std::shared_ptr<CSubTestStruct>> testListSpecial;
   std::shared_ptr<CSubTestStruct>  obj( new CSubTestStruct());
   obj->SubMsgID = 2;
   obj->SubMsgTitle = "333";
   testListSpecial.push_back(obj);
   std::shared_ptr<CSubTestStruct> obj2( new CSubTestStruct());
   obj2->SubMsgID = 3;
   obj2->SubMsgTitle = "4444";
   testListSpecial.push_back(obj2);	
   tvalue = CJsonObjectBase::DoArraySerialize<vector<std::shared_ptr<CJsonObjectBase>>>((vector<std::shared_ptr<CJsonObjectBase>>*)&testListSpecial);
   out = CJsonObjectBase::JsonValueToString(tvalue);
   cout << out << endl;

}





void test_obj_list_struct()
{
   CBaseTestStruct stru;
   stru.MsgID = 11223344;
   stru.MsgTitle = "黑黑";
   stru.MsgContent = "哈哈";
   stru.testList.push_back("aaaa");
   stru.testList.push_back("bbbb");
   stru.intList.push_back(3333);
   CSubTestStruct* obj = new CSubTestStruct();
   obj->SubMsgID = 2;
   obj->SubMsgTitle = "333";
   stru.testListSpecial.push_back(obj);
   obj = new CSubTestStruct();
   obj->SubMsgID = 3;
   obj->SubMsgTitle = "4444";
   stru.testListSpecial.push_back(obj);
  
   shared_ptr<CSubTestStruct> obj_ptr = std::make_shared<CSubTestStruct>();
   obj_ptr->SubMsgID = 4;
   obj_ptr->SubMsgTitle = "5555";
   stru.testListSpecialPtr.push_back(obj_ptr);
 
   stru.print_obj();
   std::string strTest = stru.Serialize();
   std::cout << strTest << endl;


   CBaseTestStruct toDeSelz;
   toDeSelz.DeSerialize(strTest.c_str());
   toDeSelz.print_obj();
}


int main(int argc, char ** argv)
{
    test_list();
    test_single_struct();
    test_common_struct();
    test_common_struct_ptr();


    test_obj_list_struct();
    printf("hello\n");

}
