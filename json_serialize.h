#ifndef CJSONOBJECTBASE_H
#define CJSONOBJECTBASE_H

#include <string>  
#include <vector>  
#include <list>
#include <stdint.h>
#include <json/json.h>
#include <stdio.h>   //ansen for debug
#include <memory>

using std::string;  
using std::vector;  
using std::list;

#pragma warning(disable:4267)
 
#define DoArrayDeSerialize_Wrapper(listType, type)\
	DoArrayDeSerialize<listType<type>, type>
 
#define Type_Wrapper(listType, type)\
	(listType<type>*)
 
#define DoArraySerialize_Wrapper(listType, type)\
	DoArraySerialize(Type_Wrapper(listType, type)addr)
 
#define DoObjArrayDeSerialize_Wrapper(listType, type)\
	DoObjArrayDeSerialize<listType<type> >
 
#define DoObjPtrArrayDeSerialize_Wrapper(listType, type)\
	DoObjPtrArrayDeSerialize<listType<type> >
 
typedef void* (*GenerateJsonObjForDeSerializeFromOutSide)(const string& propertyName);

//namespace test{
 
class CJsonObjectBase  
{  
protected:  
	enum CEnumJsonTypeMap  
	{
		asBool = 1,  
		asInt,  
		asUInt,  
		asString,  
		asInt64,  
		asUInt64,  
		asDouble,  
		asJsonObj,
		asJsonObjPtr,
		asSpecialArray,
		asVectorArray,
		asListArray,
	};  
public:  
	CJsonObjectBase(void){}  
public:  
	virtual ~CJsonObjectBase(void){		
	}  

	string Serialize()  
	{  
		Json::Value new_item = DoSerialize();         
		Json::FastWriter writer;    
		std::string out2 = writer.write(new_item);   
		return out2;  
	}  
	
	bool DeSerialize(const char* str)  
	{  
		Json::Reader reader;    
		Json::Value root;  
		if (reader.parse(str, root))  
		{    
			return DoDeSerialize(root);  
		}  
		return false;  
	}  
 
	Json::Value DoSerialize()  
	{  
		Json::Value new_item;  
		int nSize = m_vectorName.size();  
		for (int i=0; i < nSize; ++i )  
		{  
			void* pAddr = m_vectorPropertyAddr[i];  
			switch(m_vectorType[i])  
			{  
			case asVectorArray:
			case asListArray:
				new_item[m_vectorName[i]] = DoArraySerialize(pAddr, m_vectorType[i], m_vectorListParamType[i]);
				break;
			case asSpecialArray:  
				new_item[m_vectorName[i]] = DoSpecialArraySerialize(m_vectorName[i]); 
				break;  
			case asJsonObj:  
                                // A ptr to point to the object, so it can Polymorphisms.
				new_item[m_vectorName[i]] = ((CJsonObjectBase*)pAddr)->DoSerialize();  
				break;  
			case asJsonObjPtr:  
                                // here since it's a shared_ptr, so just rerefrence the obj, it can support the Polymorphisms.
				new_item[m_vectorName[i]] = (*(std::shared_ptr<CJsonObjectBase>*)pAddr)->DoSerialize();  
				break;  
			case asBool:  
				new_item[m_vectorName[i]] = Serialize<bool>(pAddr); 
			case asInt:  
				new_item[m_vectorName[i]] = Serialize<int32_t>(pAddr); 
				break;  
			case asUInt:  
				new_item[m_vectorName[i]] = Serialize<uint32_t>(pAddr);  
				break;  
			case asInt64:  
				new_item[m_vectorName[i]] = Json::Int64(Serialize<int64_t>(pAddr));  
				break;  
			case asUInt64:  
				new_item[m_vectorName[i]] = Json::UInt64(Serialize<uint64_t>(pAddr));  
				break;  
			case asDouble:  
				new_item[m_vectorName[i]] = Serialize<double>(pAddr);  
				break;  
			case asString:  
				new_item[m_vectorName[i]] = Serialize<string>(pAddr);  
			default:  
				//暂时只支持这几种类型，需要的可以自行添加
				break;  
			}         
		}  
		return new_item;  
	} 
protected:
	bool DoDeSerialize(Json::Value& root)  
	{  
		int nSize = m_vectorName.size();  
		for (int i=0; i < nSize; ++i )  
		{  
			void* pAddr = m_vectorPropertyAddr[i];  
 
			switch(m_vectorType[i])  
			{  
			case asListArray:
			case asVectorArray:
				{
					if (root.isNull() || root[m_vectorName[i]].isNull())
					{
						break;
					}
					DoArrayDeSerialize(m_vectorName[i], pAddr, root[m_vectorName[i]], m_vectorType[i], m_vectorListParamType[i]);
				}
				break;
			case asJsonObj:  
				{  
					if (!root[ m_vectorName[i] ].isNull())   
						((CJsonObjectBase*)pAddr)->DoDeSerialize(root[m_vectorName[i]]);  
				}                 
				break;  
			case asJsonObjPtr:  
				{  
					if (!root[ m_vectorName[i] ].isNull())   
						(*(std::shared_ptr<CJsonObjectBase>*)pAddr)->DoDeSerialize(root[m_vectorName[i]]);  
				}                 
				break;  
			case asBool:  
				(*(bool*)pAddr) = root.get(m_vectorName[i], 0).asBool();  
				break;  
			case asInt:  
				(*(int32_t*)pAddr) = root.get(m_vectorName[i], 0).asInt();  
				break;  
			case asUInt:  
				(*(uint32_t*)pAddr) = root.get(m_vectorName[i], 0).asUInt();  
				break;  
			case asInt64:  
				(*(int64_t*)pAddr) = root.get(m_vectorName[i], 0).asInt64();  
				break;  
			case asUInt64:  
				(*(uint64_t*)pAddr) = root.get(m_vectorName[i], 0).asUInt64();  
				break;  
			case asDouble:  
				(*(double*)pAddr) = root.get(m_vectorName[i], 0).asDouble();  
				break;  
			case asString:  
				(*(string*)pAddr) = root.get(m_vectorName[i], "").asString();  
			default:  
				//暂时只支持这几种类型，需要的可以自行添加   
				break;  
			}             
		}  
		return true;  
	}  
	 
	void SetProperty(string name, CEnumJsonTypeMap type, void* addr,  CEnumJsonTypeMap listParamType = asInt)  
	{  
		m_vectorName.push_back(name);  
		m_vectorPropertyAddr.push_back(addr);  
		m_vectorType.push_back(type);  
		m_vectorListParamType.push_back(listParamType);
	}  
	virtual void SetPropertys() = 0;  
	//有特殊对象需要序列化时，请重载此函数 
	virtual Json::Value DoSpecialArraySerialize(const string& propertyName){ return "";}
 
	//在反序列化时，如果对象中嵌套了列表，并且列表中容纳的内容是其他特殊对象时，需要重载此函数，new出真正的对象
	virtual CJsonObjectBase* GenerateJsonObjForDeSerialize(const string& propertyName){ return NULL;}
 
	bool DoArrayDeSerialize(const string& propertyName, void* addr, Json::Value& root, CEnumJsonTypeMap listType, CEnumJsonTypeMap paramType)
	{
		if (listType == asVectorArray)
		{
			switch(paramType)
			{ 
			case asJsonObj:  
				{  
					return DoObjArrayDeSerialize_Wrapper(vector, CJsonObjectBase*)(propertyName, addr, root);
				}                 
				break;  
			case asJsonObjPtr:  
				{  
					return DoObjPtrArrayDeSerialize_Wrapper(vector, std::shared_ptr<CJsonObjectBase>)(propertyName, addr, root);
				}                 
				break;  
			case asBool:  
				//无 
				break; 
			case asInt:  
				return DoArrayDeSerialize_Wrapper(vector, int32_t)(addr, root); 
			case asUInt:  
				return DoArrayDeSerialize_Wrapper(vector, uint32_t)(addr, root);   
			case asInt64:  
				return DoArrayDeSerialize_Wrapper(vector, int64_t)(addr, root);     
			case asUInt64:  
				return DoArrayDeSerialize_Wrapper(vector, uint64_t)(addr, root);     
			case asDouble:  
				return DoArrayDeSerialize_Wrapper(vector, double)(addr, root);     
			case asString:  
				return DoArrayDeSerialize_Wrapper(vector, string)(addr, root);       
			default:
				break;  
			}
		}
		else if(listType == asListArray)
		{
			switch(paramType)
			{ 
			case asJsonObj:  
				{
					return DoObjArrayDeSerialize_Wrapper(list, CJsonObjectBase*)(propertyName, addr, root);
				}                 
				break;  
			case asJsonObjPtr:  
				{
					return DoObjPtrArrayDeSerialize_Wrapper(list, std::shared_ptr<CJsonObjectBase>)(propertyName, addr, root);
				}                 
				break;  
			case asBool:  
				return DoArrayDeSerialize_Wrapper(list, bool)(addr, root);
			case asInt:  
				return DoArrayDeSerialize_Wrapper(list, int32_t)(addr, root); 
 			case asUInt:  
				return DoArrayDeSerialize_Wrapper(list, uint32_t)(addr, root);   
 			case asInt64:  
 				return DoArrayDeSerialize_Wrapper(list, int64_t)(addr, root);     
			case asUInt64:  
 				return DoArrayDeSerialize_Wrapper(list, uint64_t)(addr, root);     
			case asDouble:  
 				return DoArrayDeSerialize_Wrapper(list, double)(addr, root);     
			case asString:  
				return DoArrayDeSerialize_Wrapper(list, string)(addr, root);     
			default:
				break;  
			}
		}
		
		return true;
	}
 
	//特殊对象列表的反序列化
	template<typename T1>
	bool DoObjArrayDeSerialize(const string& propertyName, void* addr, Json::Value& node)
	{
		if (!node.isArray())
		{
			return false;
		}
		T1* pList = (T1*)addr;
		int size = node.size();
		for(int i = 0; i < size; ++i)
		{
			CJsonObjectBase* pNode = GenerateJsonObjForDeSerialize(propertyName);
			pNode->DoDeSerialize(node[i]);
			pList->push_back(pNode);
		}
		return true;
	}

	//特殊对象列表的反序列化
	template<typename T1>
	bool DoObjPtrArrayDeSerialize(const string& propertyName, void* addr, Json::Value& node)
	{
		if (!node.isArray())
		{
			return false;
		}
		T1* pList = (T1*)addr;
		int size = node.size();
		for(int i = 0; i < size; ++i)
		{
			CJsonObjectBase* pNode = GenerateJsonObjForDeSerialize(propertyName);
		        std::shared_ptr<CJsonObjectBase> node_ptr(pNode);
                 	node_ptr->DoDeSerialize(node[i]);
			pList->push_back(node_ptr);
		}
		return true;
	}

public:
	//常见类型列表的反序列化 
	template <typename T1, typename T2>
	static bool DoArrayDeSerialize(void* addr, Json::Value& node)
	{
		if (!node.isArray())
		{
			return false;
		}
		T1* pList = (T1*)addr;
		int size = node.size();
		for(int i = 0; i < size; ++i)
			pList->push_back( DeSerialize<T2>(node[i]) );
		return true;
	}
 
	//特殊对象列表的反序列化
	template<typename T1>
	static bool DoObjArrayDeSerialize(const string& propertyName, void* addr, Json::Value& node, GenerateJsonObjForDeSerializeFromOutSide funGenerate)
	{
		if (!node.isArray())
		{
			return false;
		}
		T1* pList = (T1*)addr;
		int size = node.size();
		for(int i = 0; i < size; ++i)
		{
			CJsonObjectBase* pNode = (CJsonObjectBase*)funGenerate(propertyName);
			pNode->DoDeSerialize(node[i]);
			pList->push_back(pNode);
		}
		return true;
	}
protected:

	Json::Value DoArraySerialize(void* addr, CEnumJsonTypeMap listType, CEnumJsonTypeMap paramType);
public:
	template <typename T1>
	static Json::Value DoArraySerialize(T1* pList);

	static string JsonValueToString(Json::Value& tvalue)
	{
		Json::FastWriter writer;    
		return writer.write(tvalue);
	}
private:
	template <typename T>
	static T Serialize(void* addr)
	{
		return (*(T*)addr);
	}
 
	template <typename T>
	static T DeSerialize(Json::Value& root)
	{
		return GetData<T>(root);
	}
 
	template <typename T>
	static T GetData(Json::Value& root)
	{
                printf(" this is deducation by compiler\n");
		T temp;
		return temp;
	}

private:  
	vector<string> m_vectorName;  
	vector<void*>  m_vectorPropertyAddr;  
	vector<CEnumJsonTypeMap>     m_vectorType;
	vector<CEnumJsonTypeMap>     m_vectorListParamType;

};   //end of class defineation.

        template <>
	Json::Value CJsonObjectBase::DoArraySerialize(vector<std::shared_ptr<CJsonObjectBase>> * pList)
	{
		Json::Value arrayValue;
		for (vector<std::shared_ptr<CJsonObjectBase>>::iterator it = pList->begin(); it != pList->end(); ++ it)
		{
			arrayValue.append((*it)->DoSerialize());			
		}
		return arrayValue;
	}
 
	template <>
	Json::Value CJsonObjectBase::DoArraySerialize(list<std::shared_ptr<CJsonObjectBase>> * pList)
	{
		Json::Value arrayValue;
		for (list<std::shared_ptr<CJsonObjectBase>>::iterator it = pList->begin(); it != pList->end(); ++ it)
		{
			arrayValue.append((*it)->DoSerialize());			
		}
		return arrayValue;
	}

        template<typename T1>
	Json::Value CJsonObjectBase::DoArraySerialize(T1* pList)
	{
		Json::Value arrayValue;
		for (typename T1::iterator it = pList->begin(); it != pList->end(); ++ it)
		{
			arrayValue.append(*it);			
		}
		return arrayValue;
	}	





        template <>
	Json::Value CJsonObjectBase::DoArraySerialize(vector<CJsonObjectBase*>* pList)
	{
		Json::Value arrayValue;
		for (vector<CJsonObjectBase*>::iterator it = pList->begin(); it != pList->end(); ++ it)
		{
			arrayValue.append((*it)->DoSerialize());			
		}
		return arrayValue;
	}
 
	template <>
	Json::Value CJsonObjectBase::DoArraySerialize(list<CJsonObjectBase*>* pList)
	{
		Json::Value arrayValue;
		for (list<CJsonObjectBase*>::iterator it = pList->begin(); it != pList->end(); ++ it)
		{
			arrayValue.append((*it)->DoSerialize());			
		}
		return arrayValue;
	}

	Json::Value CJsonObjectBase::DoArraySerialize(void* addr, CEnumJsonTypeMap listType, CEnumJsonTypeMap paramType)
	{
		if (listType == asVectorArray)
		{
			switch(paramType)
			{ 
			case asBool:  
				return "";
			case asJsonObj:
				return DoArraySerialize_Wrapper(vector, CJsonObjectBase*);
		        case asJsonObjPtr:
				return DoArraySerialize_Wrapper(vector, std::shared_ptr<CJsonObjectBase>);
			case asInt:  
				return DoArraySerialize_Wrapper(vector, int32_t);
			case asUInt:  
				return DoArraySerialize_Wrapper(vector, uint32_t); 
			case asInt64:  
				//return DoArraySerialize_Wrapper(vector, int64_t);   
				return DoArraySerialize_Wrapper(vector, Json::Int64);   
			case asUInt64:  
				//return DoArraySerialize_Wrapper(vector, uint64_t);  
				return DoArraySerialize_Wrapper(vector, Json::UInt64);  
			case asDouble:  
				return DoArraySerialize_Wrapper(vector, double);  
			case asString:  
				return DoArraySerialize_Wrapper(vector, string);
			}
		}
		else 
		{
			switch(paramType)
			{ 
			case asBool:  
				return DoArraySerialize_Wrapper(list, bool);
			case asJsonObj:
				return DoArraySerialize_Wrapper(list, CJsonObjectBase*);
			case asJsonObjPtr:
				return DoArraySerialize_Wrapper(list, std::shared_ptr<CJsonObjectBase>);
			case asInt:  
				return DoArraySerialize_Wrapper(list, int32_t);
			case asUInt:  
				return DoArraySerialize_Wrapper(list, uint32_t); 
			case asInt64:  
				//return DoArraySerialize_Wrapper(list, int64_t);   
				return DoArraySerialize_Wrapper(vector, Json::Int64);   
			case asUInt64:  
				//return DoArraySerialize_Wrapper(list, uint64_t);  
				return DoArraySerialize_Wrapper(vector, Json::UInt64);  
			case asDouble:  
				//return DoArraySerialize_Wrapper(list, uint64_t);  
				return DoArraySerialize_Wrapper(vector, double);  
			case asString:  
				return DoArraySerialize_Wrapper(list, string);
			}
		}
		return "";
	}

 
	template <>
	bool CJsonObjectBase::GetData(Json::Value& root)
	{
                printf(" in specialization for bool\n");
		return root.asBool();
	}
 
	template <>
	int32_t CJsonObjectBase::GetData(Json::Value& root)
	{
                printf(" in specialization for int: %d\n", root.asInt());
		return root.asInt();
	}
 
	template <>
	uint32_t CJsonObjectBase::GetData(Json::Value& root)
	{
                printf(" in specialization for uint\n");
		return root.asUInt();
	}
	template <>
	int64_t CJsonObjectBase::GetData(Json::Value& root)
	{
                printf(" in specialization for int64\n");
		return root.asInt64();
	}
	template <>
	uint64_t CJsonObjectBase::GetData(Json::Value& root)
	{
                printf(" in specialization for uint64\n");
		return root.asUInt64();
	}
        template <>
	double CJsonObjectBase::GetData(Json::Value& root)
	{
                printf(" in specialization for uint64\n");
		return root.asDouble();
	}
	template <>
	string CJsonObjectBase::GetData(Json::Value& root)
	{
                printf(" in specialization for string: %s\n", root.asString().c_str());
		return root.asString();
	}

//}; //endof namespace.

#endif

