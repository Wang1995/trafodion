// **********************************************************************
// @@@ START COPYRIGHT @@@
//
// (C) Copyright 2013-2014 Hewlett-Packard Development Company, L.P.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
// @@@ END COPYRIGHT @@@
// **********************************************************************

#include "SequenceFileReader.h"
#include "HdfsLogger.h"

// ===========================================================================
// ===== Class SequenceFileReader
// ===========================================================================

JavaMethodInit* SequenceFileReader::JavaMethods_ = NULL;
jclass SequenceFileReader::javaClass_ = 0;
bool SequenceFileReader::javaMethodsInitialized_ = false;
pthread_mutex_t SequenceFileReader::javaMethodsInitMutex_ = PTHREAD_MUTEX_INITIALIZER;

static const char* const sfrErrorEnumStr[] = 
{
  "No more data."
 ,"JNI NewStringUTF() in initSerDe()"
 ,"Java exception in initSerDe()"
 ,"JNI NewStringUTF() in open()"
 ,"Java exception in open()"
 ,"Java exception in getPos()"
 ,"Java exception in seeknSync()"
 ,"Java exception in isEOF()"
 ,"Java exception in fetchNextRow()"
 ,"Java exception in close()"
};

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
char* SequenceFileReader::getErrorText(SFR_RetCode errEnum)
{
  if (errEnum < (SFR_RetCode)JOI_LAST)
    return JavaObjectInterface::getErrorText((JOI_RetCode)errEnum);
  else    
    return (char*)sfrErrorEnumStr[errEnum-JOI_LAST];
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
SequenceFileReader::~SequenceFileReader()
{
  close();
}
 
//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
SFR_RetCode SequenceFileReader::init()
{
  static char className[]="org/trafodion/sql/HBaseAccess/SequenceFileReader";
  SFR_RetCode rc; 

  if (javaMethodsInitialized_)
    return (SFR_RetCode)JavaObjectInterface::init(className, javaClass_, JavaMethods_, (Int32)JM_LAST, javaMethodsInitialized_); 
  else
  {
    pthread_mutex_lock(&javaMethodsInitMutex_);
    if (javaMethodsInitialized_)
    {
      pthread_mutex_unlock(&javaMethodsInitMutex_);
      return (SFR_RetCode)JavaObjectInterface::init(className, javaClass_, JavaMethods_, (Int32)JM_LAST, javaMethodsInitialized_);
    }
    JavaMethods_ = new JavaMethodInit[JM_LAST];
    
    JavaMethods_[JM_CTOR      ].jm_name      = "<init>";
    JavaMethods_[JM_CTOR      ].jm_signature = "()V";
  //  JavaMethods_[JM_INITSERDE ].jm_name      = "initSerDe";
  //  JavaMethods_[JM_INITSERDE ].jm_signature = "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V";
    JavaMethods_[JM_GETERROR  ].jm_name      = "getLastError";
    JavaMethods_[JM_GETERROR  ].jm_signature = "()Ljava/lang/String;";
    JavaMethods_[JM_OPEN      ].jm_name      = "open";
    JavaMethods_[JM_OPEN      ].jm_signature = "(Ljava/lang/String;)Ljava/lang/String;";
    JavaMethods_[JM_GETPOS    ].jm_name      = "getPosition";
    JavaMethods_[JM_GETPOS    ].jm_signature = "()J";
    JavaMethods_[JM_SYNC      ].jm_name      = "seeknSync";
    JavaMethods_[JM_SYNC      ].jm_signature = "(J)Ljava/lang/String;";
    JavaMethods_[JM_ISEOF     ].jm_name      = "isEOF";
    JavaMethods_[JM_ISEOF     ].jm_signature = "()Z";
  //  JavaMethods_[JM_FETCHCOLS ].jm_name      = "fetchArrayOfColumns";
  //  JavaMethods_[JM_FETCHCOLS ].jm_signature = "()[Ljava/lang/String;";
    JavaMethods_[JM_FETCHROW  ].jm_name      = "fetchNextRow";
    JavaMethods_[JM_FETCHROW  ].jm_signature = "()Ljava/lang/String;";
    JavaMethods_[JM_FETCHROW2 ].jm_name      = "fetchNextRow";
    JavaMethods_[JM_FETCHROW2 ].jm_signature = "(J)Ljava/lang/String;";
    JavaMethods_[JM_FETCHBUFF1].jm_name      = "fetchArrayOfRows";
    JavaMethods_[JM_FETCHBUFF1].jm_signature = "(I)[Ljava/lang/String;";
    JavaMethods_[JM_FETCHBUFF2].jm_name      = "fetchArrayOfRows";
    JavaMethods_[JM_FETCHBUFF2].jm_signature = "(II)[Ljava/lang/String;";
    JavaMethods_[JM_CLOSE     ].jm_name      = "close";
    JavaMethods_[JM_CLOSE     ].jm_signature = "()Ljava/lang/String;";
   
    setHBaseCompatibilityMode(FALSE);
    rc = (SFR_RetCode)JavaObjectInterface::init(className, javaClass_, JavaMethods_, (Int32)JM_LAST, javaMethodsInitialized_);
    javaMethodsInitialized_ = TRUE;
    pthread_mutex_unlock(&javaMethodsInitMutex_);
  }
  return rc;
}
        
//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
//SFR_RetCode SequenceFileReader::initSerDe(int numColumns, 
//                                         const char* fieldDelim,
//                                         const char* columns, 
//                                         const char* colTypes, 
//                                         const char* nullFormat)
//{
//  char numColumns_s[10];
//  sprintf(numColumns_s, "%d", numColumns);
//  jstring js_numColumns = jenv_->NewStringUTF(numColumns_s);
//  if (js_numColumns == NULL)
//    return SFR_ERROR_INITSERDE_PARAMS;
//  
//  jstring js_delim = jenv_->NewStringUTF(fieldDelim);
//  if (js_delim == NULL) 
//    return SFR_ERROR_INITSERDE_PARAMS;
//
//  jstring js_columns = jenv_->NewStringUTF(columns);
//  if (js_columns == NULL) 
//    return SFR_ERROR_INITSERDE_PARAMS;
//
//  jstring js_colTypes = jenv_->NewStringUTF(colTypes);
//  if (js_colTypes == NULL) 
//    return SFR_ERROR_INITSERDE_PARAMS;
//
//  jstring js_nullFormat = jenv_->NewStringUTF(nullFormat);
//  if (js_nullFormat == NULL) 
//    return SFR_ERROR_INITSERDE_PARAMS;
//
//  jenv_->CallVoidMethod(javaObj_, JavaMethods_[JM_INITSERDE].methodID, js_numColumns, js_delim, js_columns, js_colTypes, js_nullFormat);
//
//  jenv_->DeleteLocalRef(js_numColumns);  
//  jenv_->DeleteLocalRef(js_delim);  
//  jenv_->DeleteLocalRef(js_columns);  
//  jenv_->DeleteLocalRef(js_colTypes);  
//  jenv_->DeleteLocalRef(js_nullFormat);  
//  
//  if (jenv_->ExceptionCheck()) 
//  {
//    //jenv_->ExceptionDescribe();
//    jenv_->ExceptionClear();
//    return SFR_ERROR_INITSERDE_EXCEPTION;
//  }
//
//  return SFR_OK;
//}
	
//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
SFR_RetCode SequenceFileReader::open(const char* path)
{
  HdfsLogger::log(CAT_SEQ_FILE_READER, LL_DEBUG, "SequenceFileReader::open(%s) called.", path);
  jstring js_path = jenv_->NewStringUTF(path);
  if (js_path == NULL) 
    return SFR_ERROR_OPEN_PARAM;

  // String open(java.lang.String);
  jstring jresult = (jstring)jenv_->CallObjectMethod(javaObj_, JavaMethods_[JM_OPEN].methodID, js_path);

  jenv_->DeleteLocalRef(js_path);  

  if (jresult != NULL)
  {
    logError(CAT_SEQ_FILE_READER, "SequenceFileReader::open()", jresult);
    return SFR_ERROR_OPEN_EXCEPTION;
  }
  
  return SFR_OK;
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
SFR_RetCode SequenceFileReader::getPosition(Int64& pos)
{
  HdfsLogger::log(CAT_SEQ_FILE_READER, LL_DEBUG, "SequenceFileReader::getPosition(%ld) called.", pos);

  // long getPosition();
  Int64 result = jenv_->CallLongMethod(javaObj_, JavaMethods_[JM_GETPOS].methodID);

  if (result == -1) 
  {
    logError(CAT_SEQ_FILE_READER, "SequenceFileReader::getPosition()", getLastError());
    return SFR_ERROR_GETPOS_EXCEPTION;
  }

  pos = result;
  return SFR_OK;
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
SFR_RetCode SequenceFileReader::seeknSync(Int64 pos)
{
  HdfsLogger::log(CAT_SEQ_FILE_READER, LL_DEBUG, "SequenceFileReader::seeknSync(%ld) called.", pos);

  // String seeknSync(long);
  jstring jresult = (jstring)jenv_->CallObjectMethod(javaObj_, JavaMethods_[JM_SYNC].methodID, pos);

  if (jresult != NULL)
  {
    logError(CAT_SEQ_FILE_READER, "SequenceFileReader::seeknSync()", jresult);
    return SFR_ERROR_SYNC_EXCEPTION;
  }
  
  return SFR_OK;
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
SFR_RetCode SequenceFileReader::isEOF(bool& isEOF)
{
  HdfsLogger::log(CAT_SEQ_FILE_READER, LL_DEBUG, "SequenceFileReader::isEOF() called.");

  // boolean isEOF();
  bool result = jenv_->CallBooleanMethod(javaObj_, JavaMethods_[JM_ISEOF].methodID);

  isEOF = result;
  return SFR_OK;
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
//char** SequenceFileReader::fetchArrayOfColumns()
//{
//  // java.lang.String[] fetchArrayOfColumns();
//  jobjectArray jresult = (jobjectArray)jenv_->CallObjectMethod(javaObj_, JavaMethods_[JM_FETCHCOLS].methodID);
//  if (jenv_->ExceptionCheck()) 
//  {
//    jenv_->ExceptionDescribe();
//    jenv_->ExceptionClear();
//    jenv_->DeleteLocalRef(jresult);
//    return NULL;
//  }
//
//  if (jresult == NULL)
//  {
//    return NULL;
//  }
//	
//  return SequenceFileReader::JStringArray2CharsArray(jresult);
//}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
SFR_RetCode SequenceFileReader::fetchNextRow(Int64 stopOffset, char* buffer)
{
  // java.lang.String fetchNextRow(long stopOffset);
  jstring jresult = (jstring)jenv_->CallObjectMethod(javaObj_, JavaMethods_[JM_FETCHROW2].methodID, stopOffset);
  if (jresult==NULL && getLastError()) 
  {
    logError(CAT_SEQ_FILE_READER, "SequenceFileReader::fetchNextRow()", getLastError());
    return SFR_ERROR_FETCHROW_EXCEPTION;
  }

  if (jresult == NULL)
  {
    return SFR_NOMORE;
  }
  
  const char* char_result = jenv_->GetStringUTFChars(jresult, 0);
  strcpy(buffer, char_result);
  jenv_->ReleaseStringUTFChars(jresult, char_result);
  jenv_->DeleteLocalRef(jresult);  
  return SFR_OK;
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
SFR_RetCode SequenceFileReader::close()
{
  HdfsLogger::log(CAT_SEQ_FILE_READER, LL_DEBUG, "SequenceFileReader::close() called.");
  if (javaObj_ == NULL)
  {
    // Maybe there was an initialization error.
    return SFR_OK;
  }
    
  // String close();
  jstring jresult = (jstring)jenv_->CallObjectMethod(javaObj_, JavaMethods_[JM_CLOSE].methodID);

  if (jresult!=NULL) 
  {
    logError(CAT_SEQ_FILE_READER, "SequenceFileReader::close()", jresult);
    return SFR_ERROR_CLOSE_EXCEPTION;
  }
  
  return SFR_OK;
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
jstring SequenceFileReader::getLastError()
{
  // String getLastError();
  jstring jresult = (jstring)jenv_->CallObjectMethod(javaObj_, JavaMethods_[JM_GETERROR].methodID);

  return jresult;
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
//char** SequenceFileReader::JStringArray2CharsArray(jobjectArray jarray)
//{
//  char **chars_array;
//  int len;
//  int i;
//  const char *ret_val;
//  jstring jst_ret;
//
//  len = jenv_->GetArrayLength(jarray);
//
//  chars_array = (char **)malloc(sizeof(char*) * (len + 1));
//  if (chars_array == NULL) 
//  {
//    //TRACE(stderr, "<%s:%d> malloc() failed\n", __FILE__, __LINE__);
//    return NULL;
//  }
//
//  for (i = 0; i < len; i++) 
//  {
//    ret_val = "";
//
//    jst_ret = (jstring)jenv_->GetObjectArrayElement(jarray, i);
//    if (jst_ret != NULL) 
//    {
//      ret_val = jenv_->GetStringUTFChars(jst_ret, 0);
//    }
//
//    //TRACE(stderr, "<%s:%d> %d) => %s\n", __FILE__, __LINE__, i, ret_val);
//
//    chars_array[i] = strdup(ret_val);
//    if (chars_array[i] == NULL) 
//    {
//      // TODO: Add error handling
//      return NULL;
//    }
//
//    if (jst_ret != NULL) 
//    {
//      jenv_->ReleaseStringUTFChars(jst_ret, ret_val);
//      jenv_->DeleteLocalRef(jst_ret);  
//    }
//  }
//  chars_array[i] = NULL;
//  jenv_->DeleteLocalRef(jarray);  
//
//  return chars_array;
//}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
SFR_RetCode SequenceFileReader::fetchRowsIntoBuffer(Int64   stopOffset, 
                                                    char*   buffer, 
                                                    Int64   buffSize, 
                                                    Int64&  bytesRead, 
                                                    char    rowDelimiter)
{
  HdfsLogger::log(CAT_SEQ_FILE_READER, LL_DEBUG, "SequenceFileReader::fetchRowsIntoBuffer(stopOffset: %ld, buffSize: %ld) called.", stopOffset, buffSize);
  Int32 maxRowLength = 0;
  char* pos = buffer;
  Int64 limit = buffSize;
  SFR_RetCode retCode;
  long rowsRead=0;
  bytesRead = 0;
  do
  {
    retCode = fetchNextRow(stopOffset, pos);
    if (retCode == SFR_OK)
    {
      rowsRead++;
      Int32 rowLength = strlen(pos);
      pos += rowLength;
      *pos = rowDelimiter;
      pos++;
      *pos = 0;
      
      bytesRead += rowLength+1;
      if (maxRowLength < rowLength)
        maxRowLength = rowLength;
      limit = buffSize - maxRowLength*2;
    }
  } while (retCode == SFR_OK && bytesRead < limit);
  
  HdfsLogger::log(CAT_SEQ_FILE_READER, LL_DEBUG, "  =>Returning %d, read %ld bytes in %d rows.", retCode, bytesRead, rowsRead);
  return retCode;
}

// ===========================================================================
// ===== Class SequenceFileWriter
// ===========================================================================

JavaMethodInit* SequenceFileWriter::JavaMethods_ = NULL;
jclass SequenceFileWriter::javaClass_ = 0;
bool SequenceFileWriter::javaMethodsInitialized_ = false;
pthread_mutex_t SequenceFileWriter::javaMethodsInitMutex_ = PTHREAD_MUTEX_INITIALIZER;

static const char* const sfwErrorEnumStr[] = 
{
  "JNI NewStringUTF() in open() for writing."
 ,"Java exception in open() for writing."
 ,"JNI NewStringUTF() in write()"
 ,"Java exception in write()"
 ,"Java exception in close() after writing."
 ,"JNI NewStringUTF() in hdfsCreate()."
 ,"Java exception in hdfsCreate()."
 ,"JNI NewStringUTF() in hdfsWrite()."
 ,"Java exception in hdfsWrite()."
 ,"Java exception in hdfsClose()."
 ,"JNI NewStringUTF() in hdfsMergeFiles()."
 ,"Java exception in hdfsMergeFiles()."
};

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
char* SequenceFileWriter::getErrorText(SFW_RetCode errEnum)
{
  if (errEnum < (SFW_RetCode)JOI_LAST)
    return JavaObjectInterface::getErrorText((JOI_RetCode)errEnum);
  else    
    return (char*)sfwErrorEnumStr[errEnum-SFW_FIRST];
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
SequenceFileWriter::~SequenceFileWriter()
{
  close();
}
 
//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
SFW_RetCode SequenceFileWriter::init()
{
  static char className[]="org/trafodion/sql/HBaseAccess/SequenceFileWriter";
  SFW_RetCode rc;
  
  if (javaMethodsInitialized_)
    return (SFW_RetCode)JavaObjectInterface::init(className, javaClass_, JavaMethods_, (Int32)JM_LAST, javaMethodsInitialized_);
  else
  {
    pthread_mutex_lock(&javaMethodsInitMutex_);
    if (javaMethodsInitialized_)
    {
      pthread_mutex_unlock(&javaMethodsInitMutex_);
      return (SFW_RetCode)JavaObjectInterface::init(className, javaClass_, JavaMethods_, (Int32)JM_LAST, javaMethodsInitialized_);
    }
    JavaMethods_ = new JavaMethodInit[JM_LAST];
    
    JavaMethods_[JM_CTOR      ].jm_name      = "<init>";
    JavaMethods_[JM_CTOR      ].jm_signature = "()V";
    JavaMethods_[JM_OPEN      ].jm_name      = "open";
    JavaMethods_[JM_OPEN      ].jm_signature = "(Ljava/lang/String;I)Ljava/lang/String;";
    JavaMethods_[JM_WRITE     ].jm_name      = "write";
    JavaMethods_[JM_WRITE     ].jm_signature = "(Ljava/lang/String;)Ljava/lang/String;";
    JavaMethods_[JM_CLOSE     ].jm_name      = "close";
    JavaMethods_[JM_CLOSE     ].jm_signature = "()Ljava/lang/String;";
   
    JavaMethods_[JM_HDFS_CREATE      ].jm_name      = "hdfsCreate";
    JavaMethods_[JM_HDFS_CREATE      ].jm_signature = "(Ljava/lang/String;)Z";
    JavaMethods_[JM_HDFS_WRITE      ].jm_name      = "hdfsWrite";
    JavaMethods_[JM_HDFS_WRITE      ].jm_signature = "(Ljava/lang/String;J)Z";
    JavaMethods_[JM_HDFS_CLOSE      ].jm_name      = "hdfsClose";
    JavaMethods_[JM_HDFS_CLOSE      ].jm_signature = "()Z";
    JavaMethods_[JM_HDFS_MERGE_FILES].jm_name      = "hdfsMergeFiles";
    JavaMethods_[JM_HDFS_MERGE_FILES].jm_signature = "(Ljava/lang/String;Ljava/lang/String;)Z";

    JavaMethods_[JM_HDFS_CLEAN_UNLOAD_PATH].jm_name      = "hdfsCleanUnloadPath";
    JavaMethods_[JM_HDFS_CLEAN_UNLOAD_PATH].jm_signature = "(Ljava/lang/String;ZLjava/lang/String;)Z";


    setHBaseCompatibilityMode(FALSE);
    rc = (SFW_RetCode)JavaObjectInterface::init(className, javaClass_, JavaMethods_, (Int32)JM_LAST, javaMethodsInitialized_);
    javaMethodsInitialized_ = TRUE;
    pthread_mutex_unlock(&javaMethodsInitMutex_);
  }
  return rc;
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
SFW_RetCode SequenceFileWriter::open(const char* path, SFW_CompType compression)
{
  HdfsLogger::log(CAT_SEQ_FILE_WRITER, LL_DEBUG, "SequenceFileWriter::open(%s) called.", path);
  jstring js_path = jenv_->NewStringUTF(path);
  if (js_path == NULL) 
    return SFW_ERROR_OPEN_PARAM;

  // String open(java.lang.String);
  jstring jresult = (jstring)jenv_->CallObjectMethod(javaObj_, JavaMethods_[JM_OPEN].methodID, js_path, compression);

  jenv_->DeleteLocalRef(js_path);  

  if (jresult != NULL)
  {
    logError(CAT_SEQ_FILE_WRITER, "SequenceFileWriter::open()", jresult);
    return SFW_ERROR_OPEN_EXCEPTION;
  }
  
  return SFW_OK;
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
SFW_RetCode SequenceFileWriter::write(const char* data)
{
  HdfsLogger::log(CAT_SEQ_FILE_WRITER, LL_DEBUG, "SequenceFileWriter::write(%s) called.", data);
  jstring js_data = jenv_->NewStringUTF(data);
  if (js_data == NULL) 
    return SFW_ERROR_WRITE_PARAM;

  // String write(java.lang.String);
  jstring jresult = (jstring)jenv_->CallObjectMethod(javaObj_, JavaMethods_[JM_WRITE].methodID, js_data);

  jenv_->DeleteLocalRef(js_data);  

  if (jresult != NULL)
  {
    logError(CAT_SEQ_FILE_WRITER, "SequenceFileWriter::write()", jresult);
    return SFW_ERROR_WRITE_EXCEPTION;
  }
  
  return SFW_OK;
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
SFW_RetCode SequenceFileWriter::writeBuffer(char* data, Int64 buffSize, const char* rowDelimiter)
{
  HdfsLogger::log(CAT_SEQ_FILE_WRITER, LL_DEBUG, "SequenceFileWriter::writeBuffer() called.");

  // Point to the first row.
  char* nextRow = data;
  char* buffEnd = data + buffSize;
  char* rowEnd = NULL;
  do
  {
    // Find the end of the row
    rowEnd = strchr(nextRow, *rowDelimiter);
    if (rowEnd && rowEnd < buffEnd)
    {
      // We need the row to become a null-terminated string.
      *rowEnd = '\0';
      SFW_RetCode result = write(nextRow);
      if (result != SFW_OK)
        return result;
      
      // Skip to the next row.
      nextRow = rowEnd+1;
    } 
  } while (rowEnd && nextRow < buffEnd);
  
  return SFW_OK;
}

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////
SFW_RetCode SequenceFileWriter::close()
{
  HdfsLogger::log(CAT_SEQ_FILE_WRITER, LL_DEBUG, "SequenceFileWriter::close() called.");
  if (javaObj_ == NULL)
  {
    // Maybe there was an initialization error.
    return SFW_OK;
  }
    
  // String close();
  jstring jresult = (jstring)jenv_->CallObjectMethod(javaObj_, JavaMethods_[JM_CLOSE].methodID);

  if (jresult != NULL)
  {
    logError(CAT_SEQ_FILE_WRITER, "SequenceFileWriter::close()", jresult);
    return SFW_ERROR_CLOSE_EXCEPTION;
  }
  
  
  return SFW_OK;
}


//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
SFW_RetCode SequenceFileWriter::hdfsCreate(const char* path)
{
  HdfsLogger::log(CAT_SEQ_FILE_WRITER, LL_DEBUG, "SequenceFileWriter::hdfsCreate(%s) called.", path);
  jstring js_path = jenv_->NewStringUTF(path);
  if (js_path == NULL)
    return SFW_ERROR_HDFS_CREATE_PARAM;


  jboolean jresult = jenv_->CallBooleanMethod(javaObj_, JavaMethods_[JM_HDFS_CREATE].methodID, js_path);

  jenv_->DeleteLocalRef(js_path);

  if (jenv_->ExceptionCheck())
  {
    getExceptionDetails();
    logError(CAT_HBASE, __FILE__, __LINE__);
    logError(CAT_HBASE, "SequenceFileWriter::hdfsCreate()", getLastError());
    return SFW_ERROR_HDFS_CREATE_EXCEPTION;
  }

  if (jresult == false)
  {
    logError(CAT_HBASE, "SequenceFileWriter::hdfsCreaten()", getLastError());
    return SFW_ERROR_HDFS_CREATE_EXCEPTION;
  }

  return SFW_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
SFW_RetCode SequenceFileWriter::hdfsWrite(const char* data, Int64 len)
{
  HdfsLogger::log(CAT_SEQ_FILE_WRITER, LL_DEBUG, "SequenceFileWriter::hdfsWrite called.", data);
  jstring js_data = jenv_->NewStringUTF(data);
  if (js_data == NULL)
    return SFW_ERROR_HDFS_WRITE_PARAM;

  jlong j_len = len;
  // String write(java.lang.String);
  jboolean jresult = jenv_->CallBooleanMethod(javaObj_, JavaMethods_[JM_HDFS_WRITE].methodID, js_data, j_len);

  jenv_->DeleteLocalRef(js_data);


  if (jenv_->ExceptionCheck())
  {
    getExceptionDetails();
    logError(CAT_HBASE, __FILE__, __LINE__);
    logError(CAT_HBASE, "SequenceFileWriter::hdfsWrite()", getLastError());
    return SFW_ERROR_HDFS_WRITE_EXCEPTION;
  }

  if (jresult == false)
  {
    logError(CAT_HBASE, "SequenceFileWriter::hdfsWrite()", getLastError());
    return SFW_ERROR_HDFS_WRITE_EXCEPTION;
  }

  return SFW_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
SFW_RetCode SequenceFileWriter::hdfsClose()
{
  HdfsLogger::log(CAT_SEQ_FILE_WRITER, LL_DEBUG, "SequenceFileWriter::close() called.");
  if (javaObj_ == NULL)
  {
    // Maybe there was an initialization error.
    return SFW_OK;
  }

  // String close();
  jboolean jresult = jenv_->CallBooleanMethod(javaObj_, JavaMethods_[JM_HDFS_CLOSE].methodID);

  if (jenv_->ExceptionCheck())
  {
    getExceptionDetails();
    logError(CAT_HBASE, __FILE__, __LINE__);
    logError(CAT_HBASE, "SequenceFileWriter::hdfsClose()", getLastError());
    return SFW_ERROR_HDFS_CLOSE_EXCEPTION;
  }

  if (jresult == false)
  {
    logError(CAT_HBASE, "SequenceFileWriter::hdfsClose()", getLastError());
    return SFW_ERROR_HDFS_CLOSE_EXCEPTION;
  }


  return SFW_OK;
}

SFW_RetCode SequenceFileWriter::hdfsCleanUnloadPath( const std::string& uldPath, NABoolean checkExistence,const std::string& mergePath )
{
  HdfsLogger::log(CAT_HBASE, LL_DEBUG, "SequenceFileWriter::hdfsCleanUnloadPath(...) called.",
                                                      uldPath.data());

  jstring js_UldPath = jenv_->NewStringUTF(uldPath.c_str());
   if (js_UldPath == NULL)
   {
     //GetCliGlobals()->setJniErrorStr(getErrorText(SFW_ERROR_HDFS_MERGE_FILES_PARAM));
     return SFW_ERROR_HDFS_MERGE_FILES_PARAM;
   }

   jstring js_MergePath = jenv_->NewStringUTF(mergePath.c_str());
    if (js_MergePath == NULL)
    {
      //GetCliGlobals()->setJniErrorStr(getErrorText(SFW_ERROR_HDFS_MERGE_FILES_PARAM));
      return SFW_ERROR_HDFS_MERGE_FILES_PARAM;
    }

  if (jenv_->ExceptionCheck())
  {
    getExceptionDetails();
    logError(CAT_HBASE, __FILE__, __LINE__);
    logError(CAT_HBASE, "SequenceFileWriter::hdfsCleanUnloadPath(..) => before calling Java.", getLastError());
    return SFW_ERROR_HDFS_MERGE_FILES_EXCEPTION;
  }

  jboolean j_checkExistence = checkExistence;

  jboolean jresult = jenv_->CallBooleanMethod(javaObj_, JavaMethods_[JM_HDFS_CLEAN_UNLOAD_PATH].methodID, js_UldPath, j_checkExistence, js_MergePath);

  jenv_->DeleteLocalRef(js_UldPath);

  if (jenv_->ExceptionCheck())
  {
    getExceptionDetails();
    logError(CAT_HBASE, __FILE__, __LINE__);
    logError(CAT_HBASE, "SequenceFileWriter::hdfsCleanUnloadPath()", getLastError());
    return SFW_ERROR_HDFS_MERGE_FILES_EXCEPTION;
  }

  if (jresult == false)
  {
    logError(CAT_HBASE, "SequenceFileWriter::hdfsCleanUnloadPath()", getLastError());
    return SFW_ERROR_HDFS_MERGE_FILES_EXCEPTION;
  }

  return SFW_OK;
}
SFW_RetCode SequenceFileWriter::hdfsMergeFiles( const std::string& srcPath,
                                                const std::string& dstPath)
{
  HdfsLogger::log(CAT_HBASE, LL_DEBUG, "SequenceFileWriter::hdfsMergeFiles(...) called.",
                  srcPath.data(), dstPath.data());

  jstring js_SrcPath = jenv_->NewStringUTF(srcPath.c_str());
   if (js_SrcPath == NULL)
   {
     //GetCliGlobals()->setJniErrorStr(getErrorText(SFW_ERROR_HDFS_MERGE_FILES_PARAM));
     return SFW_ERROR_HDFS_MERGE_FILES_PARAM;
   }
  jstring js_DstPath= jenv_->NewStringUTF(dstPath.c_str());
   if (js_DstPath == NULL)
   {
     //GetCliGlobals()->setJniErrorStr(getErrorText(SFW_ERROR_HDFS_MERGE_FILES_PARAM));
     return SFW_ERROR_HDFS_MERGE_FILES_PARAM;
   }

  if (jenv_->ExceptionCheck())
  {
    getExceptionDetails();
    logError(CAT_HBASE, __FILE__, __LINE__);
    logError(CAT_HBASE, "SequenceFileWriter::hdfsMergeFiles(..) => before calling Java.", getLastError());
    return SFW_ERROR_HDFS_MERGE_FILES_EXCEPTION;
  }


  jboolean jresult = jenv_->CallBooleanMethod(javaObj_, JavaMethods_[JM_HDFS_MERGE_FILES].methodID, js_SrcPath, js_DstPath);

  jenv_->DeleteLocalRef(js_SrcPath);
  jenv_->DeleteLocalRef(js_DstPath);

  if (jenv_->ExceptionCheck())
  {
    getExceptionDetails();
    logError(CAT_HBASE, __FILE__, __LINE__);
    logError(CAT_HBASE, "SequenceFileWriter::hdfsMergeFiles()", getLastError());
    return SFW_ERROR_HDFS_MERGE_FILES_EXCEPTION;
  }

  if (jresult == false)
  {
    logError(CAT_HBASE, "SequenceFileWriter::hdfsMergeFiles()", getLastError());
    return SFW_ERROR_HDFS_MERGE_FILES_EXCEPTION;
  }

  return SFW_OK;
}