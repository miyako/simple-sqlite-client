/* --------------------------------------------------------------------------------
 #
 #  4DPlugin-Simple-SQLite-Client.cpp
 #	source generated by 4D Plugin Wizard
 #	Project : Simple SQLite Client
 #	author : miyako
 #	2020/03/03
 #  
 # --------------------------------------------------------------------------------*/

#include "4DPlugin-Simple-SQLite-Client.h"

#include "sqlite3.h"

#include <mutex>

std::mutex mutexSqlite;

#pragma mark -

void PluginMain(PA_long32 selector, PA_PluginParameters params) {
    
	try
	{
        switch(selector)
        {
			// --- Simple SQLite Client
            
			case 1 :
				SQLite_EXECUTE(params);
				break;

        }

	}
	catch(...)
	{

	}
}

#pragma mark -

void SQLite_EXECUTE(PA_PluginParameters params) {

    PackagePtr pParams = (PackagePtr)params->fParameters;

    C_TEXT Param1;
    Param1.fromParamAtIndex(pParams, 1);
    
    C_TEXT Param2;
    Param2.fromParamAtIndex(pParams, 2);
    
    CUTF8String fileName;
    Param1.copyPath(&fileName);
    
    CUTF8String sql;
    Param2.copyUTF8String(&sql);
    
    PA_CollectionRef Param3_params = PA_GetCollectionParameter(params, 3);
    
    C_LONGINT Param4;
    Param4.fromParamAtIndex(pParams, 4);
    
    integer_format_t integer_format = (integer_format_t)Param4.getIntValue();
    
    PA_ObjectRef returnValue = PA_CreateObject();
    
    unsigned int paramCount = 0;
    
    if(Param3_params) {
        paramCount = PA_GetCollectionLength(Param3_params);
    }
    
    bool useParams = (paramCount > 0);
        
    std::lock_guard<std::mutex> lock(mutexSqlite);
    
    sqlite3 *pDb = NULL;
    
    int err = sqlite3_open((const char *)fileName.c_str(), &pDb);
    
    if(err == SQLITE_OK)
    {
        sqlite3_stmt *stmt = NULL;
        
        err = sqlite3_prepare_v2(pDb,
                                 (const char *)sql.c_str(),
                                 (int)sql.length(), &stmt, NULL);
        if(err == SQLITE_OK)
        {
            std::vector<CUTF8String> paramStringValues;
            
            if(useParams) {
               
                for(unsigned int i = 0; i < paramCount; ++i) {
                    
                    PA_Variable v = PA_GetCollectionElement(Param3_params, i);
                    
                    PA_VariableKind kind = PA_GetVariableKind(v);
                    
                    switch (kind) {
                        case eVK_Unistring:
                        {
                            PA_Unistring u = PA_GetStringVariable(v);
                            
                            C_TEXT t;
                            t.setUTF16String((const PA_Unichar *)u.fString, u.fLength);
                            CUTF8String u8;
                            t.copyUTF8String(&u8);
                            
                            sqlite3_bind_text(stmt, i + 1,
                                              (const char *)u8.c_str(), (int)u8.length(), NULL);
                            
                        }
                            break;
                        case eVK_Real:
                        {
                            double r = PA_GetRealVariable(v);
                            sqlite3_bind_double(stmt, i + 1, r);
                        }
                            break;
                        case eVK_Time:
                        {
                            PA_long32 t = PA_GetTimeVariable(v);
                            sqlite3_bind_int(stmt, i + 1, t);
                        }
                            break;
                        case eVK_Longint:
                        {
                            PA_long32 t = PA_GetLongintVariable(v);
                            sqlite3_bind_int(stmt, i + 1, t);
                        }
                            break;
                        case eVK_Null:
                        {
                            sqlite3_bind_null(stmt, i + 1);
                        }
                            break;
                        case eVK_Boolean:
                        {
                            char b = PA_GetBooleanVariable(v);
                            sqlite3_bind_int(stmt, i + 1, b);
                        }
                            break;
                        default:
                            break;
                    }
                }/* paramCount */
            }
            
            time_t startTime = time(0);
            
            PA_long32 r = 0;
            
            PA_CollectionRef resultRows = PA_CreateCollection();
            
            while(SQLITE_ROW == (err = sqlite3_step(stmt)))
            {
                time_t now = time(0);
                time_t elapsedTime = abs(startTime - now);
                
                if(elapsedTime > 0)
                {
                    startTime = now;
                    PA_YieldAbsolute();
                }
                
                PA_ObjectRef resultRow = PA_CreateObject();
                
                for(unsigned int i = 0; i < sqlite3_column_count(stmt); ++i) {
                    
                    int type = sqlite3_column_type(stmt, i);
                    const char * fieldName = sqlite3_column_name(stmt, i);
                    
                    switch (type) {
                                                        
                        case SQLITE_INTEGER:
                        {
                            int intValue = sqlite3_column_int(stmt, i);
                            
                            switch (integer_format) {
                                case integer_format_complex:
                                {
                                    sqlite3_int64 int64Value = sqlite3_column_int64(stmt, i);
                                    
                                    PA_ObjectRef resultCell = PA_CreateObject();
                                    ob_set_n(resultCell, L"intValue", intValue);
                                    
                                    std::vector<char> buf(20);
                                    memset((char *)&buf[0], 0, buf.size());
#if VERSIONMAC
                                    sprintf((char *)&buf[0], "%lld", int64Value);
#else
                                    sprintf_s((char *)&buf[0], 20, "%lld", int64Value);
#endif
                                    ob_set_s(resultCell, L"int64Value", &buf[0]);
                                    
                                    ob_set_o(resultRow, fieldName, resultCell);
                                }
                                    break;
                                    
                                default:
                                    ob_set_n(resultRow, fieldName, intValue);
                                    break;
                            }
                        }
                            break;
                        case SQLITE_FLOAT:
                        {
                            double fieldValue = sqlite3_column_double(stmt, i);
                            ob_set_n(resultRow, fieldName, fieldValue);
                        }
                            break;
                        case SQLITE_TEXT:
                        {
                            const char *fieldValue = (const char *)sqlite3_column_text(stmt, i);
                            ob_set_s(resultRow, fieldName, fieldValue);
                        }
                            break;
                        case SQLITE_BLOB:
                        {
                            const void *fieldValue = sqlite3_column_blob(stmt, i);
                            int fieldLength = sqlite3_column_bytes(stmt, i);
                            
                            C_BLOB data;
                            data.setBytes((const uint8_t *)fieldValue, fieldLength);
                            
                            C_TEXT t;
                            data.toB64Text(&t);
                            
                            CUTF16String u16;
                            t.copyUTF16String(&u16);
                            
                            PA_ObjectRef resultCell = PA_CreateObject();
                            ob_set_a(resultCell, L"data", &u16);
                            
                            ob_set_o(resultRow, fieldName, resultCell);
                            
                        }
                            break;
                        case SQLITE_NULL:
                        {
                            ob_set_0(resultRow, fieldName);
                        }
                            break;
                        default:
                            break;
                    }
                    
                }
                
                PA_Variable v = PA_CreateVariable(eVK_Object);
                PA_SetObjectVariable(&v, resultRow);
                PA_SetCollectionElement(resultRows, r, v);
                
                r++;
            }

            ob_set_c(returnValue, L"values", resultRows);
            
            sqlite3_finalize(stmt);
        }else{
            ob_set_n(returnValue, L"error", err);
        }
        sqlite3_close(pDb);
    }else{
        ob_set_n(returnValue, L"error", err);
    }
    
    PA_ReturnObject(params, returnValue);
}

