/*
 * Copyright (C) 2008 Search Solution Corporation. All rights reserved by Search Solution.
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

/*
 * log_applier_plugin.c - Applylogdb Plug-in Interface
 */

#ident "$Id$"

#include "config.h"
#include <dlfcn.h>
#include <jni.h>

#include "db.h"
#include "environment_variable.h"
#include "system_parameter.h"
#include "release_string.h"
#include "memory_alloc.h"
#include "storage_common.h"
#include "error_manager.h"
#include "object_template.h"
#include "class_object.h"
#include "log_applier_plugin_config.h"
#include "log_applier_plugin.h"

#if defined(sparc)
#define JVM_LIB_PATH "jre/lib/sparc/client"
#elif defined(HPUX) && defined(IA64)
#define JVM_LIB_PATH "jre/lib/IA64N/hotspot"
#elif defined(HPUX) && !defined(IA64)
#define JVM_LIB_PATH "jre/lib/PA_RISC2.0/hotspot"
#elif defined(AIX)
#if __WORDSIZE == 32
#define JVM_LIB_PATH "jre/bin/classic"
#elif __WORDSIZE == 64
#define JVM_LIB_PATH "jre/lib/ppc64/classic"
#endif
#elif defined(__i386) || defined(__x86_64)
#if __WORDSIZE == 32
#define JVM_LIB_PATH "jre/lib/i386/client"
#else
#define JVM_LIB_PATH "jre/lib/amd64/server"
#endif
#else /* ETC */
#define JVM_LIB_PATH ""
#endif /* ETC */

#if defined(AIX)
#define JVM_LIB_FILE "libjvm.so"
#elif defined(HPUX) && !defined(IA64)
#define JVM_LIB_FILE "libjvm.sl"
#else /* not AIX , not ( HPUX && (not IA64)) */
#define JVM_LIB_FILE "libjvm.so"
#endif /* not AIX , not ( HPUX && (not IA64)) */

#define MAX_OPTIONS	100
#define BUF_SIZE        2048
typedef jint (*CREATE_VM_FUNC) (JavaVM **, void **, void *);

#ifdef __cplusplus
#define JVM_FindClass(ENV, NAME)	\
	(ENV)->FindClass(NAME)
#define JVM_GetMethodID(ENV, CLAZZ, NAME, SIG)	\
	(ENV)->GetMethodID(CLAZZ, NAME, SIG)
#define JVM_GetStaticMethodID(ENV, CLAZZ, NAME, SIG)	\
	(ENV)->GetStaticMethodID(CLAZZ, NAME, SIG)
#define JVM_NewObject(ENV, CLAZZ, METHODID, ...)	\
	(ENV)->NewObject(CLAZZ, METHODID, __VA_ARGS__)
#define JVM_NewObjectNoArg(ENV, CLAZZ, METHODID)	\
	(ENV)->NewObject(CLAZZ, METHODID)
#define JVM_CallIntMethod(ENV, JOB, METHODID, ...)	\
	(ENV)->CallIntMethod(JOB, METHODID, __VA_ARGS__)
#define JVM_CallIntMethodNoArg(ENV, JOB, METHODID)	\
	(ENV)->CallIntMethod(JOB, METHODID)
#define JVM_CallObjectMethod(ENV, JOB, METHODID, ...)	\
	(ENV)->CallObjectMethod(JOB, METHODID, __VA_ARGS__)
#define JVM_CallVoidMethodNoArg(ENV, JOB, METHODID)	\
	(ENV)->CallVoidMethod(JOB, METHODID)
#define JVM_CallStaticIntMethodNoArg(ENV, JOB, METHODID)	\
	(ENV)->CallStaticIntMethod(JOB, METHODID)
#define JVM_CallStaticLongMethodNoArg(ENV, JOB, METHODID)	\
	(ENV)->CallStaticLongMethod(JOB, METHODID)
#define JVM_NewStringUTF(ENV, BYTES)	\
	(ENV)->NewStringUTF(BYTES)
#define JVM_NewGlobalRef(ENV, OBJ)	\
	(ENV)->NewGlobalRef(OBJ)
#define JVM_DeleteGlobalRef(ENV, OBJ)	\
	(ENV)->DeleteGlobalRef(OBJ)
#define JVM_DeleteLocalRef(ENV, OBJ)	\
	(ENV)->DeleteLocalRef(OBJ)
#define JVM_DestroyJavaVM(JVM)	\
	(JVM)->DestroyJavaVM()
#else
#define JVM_FindClass(ENV, NAME)	\
	(*ENV)->FindClass(ENV, NAME)
#define JVM_GetMethodID(ENV, CLAZZ, NAME, SIG)	\
	(*ENV)->GetMethodID(ENV, CLAZZ, NAME, SIG)
#define JVM_GetStaticMethodID(ENV, CLAZZ, NAME, SIG)	\
	(*ENV)->GetStaticMethodID(ENV, CLAZZ, NAME, SIG)
#define JVM_NewObject(ENV, CLAZZ, METHODID, ...)	\
	(*ENV)->NewObject(ENV, CLAZZ, METHODID, __VA_ARGS__)
#define JVM_NewObjectNoArg(ENV, CLAZZ, METHODID)	\
	(*ENV)->NewObject(ENV, CLAZZ, METHODID)
#define JVM_CallIntMethod(ENV, JOB, METHODID, ...)	\
	(*ENV)->CallIntMethod(ENV, JOB, METHODID, __VA_ARGS__)
#define JVM_CallIntMethodNoArg(ENV, JOB, METHODID)	\
	(*ENV)->CallIntMethod(ENV, JOB, METHODID)
#define JVM_CallObjectMethod(ENV, JOB, METHODID, ...)	\
	(*ENV)->CallObjectMethod(ENV, JOB, METHODID, __VA_ARGS__)
#define JVM_CallVoidMethodNoArg(ENV, JOB, METHODID)	\
	(*ENV)->CallVoidMethod(ENV, JOB, METHODID)
#define JVM_CallStaticIntMethodNoArg(ENV, JOB, METHODID)	\
	(*ENV)->CallStaticIntMethod(ENV, JOB, METHODID)
#define JVM_CallStaticLongMethodNoArg(ENV, JOB, METHODID)	\
	(*ENV)->CallStaticLongMethod(ENV, JOB, METHODID)
#define JVM_NewStringUTF(ENV, BYTES)	\
	(*ENV)->NewStringUTF(ENV, BYTES)
#define JVM_NewGlobalRef(ENV, OBJ)	\
	(*ENV)->NewGlobalRef(ENV, OBJ)
#define JVM_DeleteGlobalRef(ENV, OBJ)	\
	(*ENV)->DeleteGlobalRef(ENV, OBJ)
#define JVM_DeleteLocalRef(ENV, OBJ)	\
	(*ENV)->DeleteLocalRef(ENV, OBJ)
#define JVM_DestroyJavaVM(JVM)	\
	(*JVM)->DestroyJavaVM(JVM)
#endif

/* JVM Global References */
static JavaVM *jvm = NULL;
static JNIEnv *env_p = NULL;

static jclass server_global_cls = NULL;
static jobject server_global_obj = NULL;
static jmethodID server_init_mid = NULL;
static jmethodID server_put_mid = NULL;
static jmethodID server_pageid_mid = NULL;
static jmethodID server_offset_mid = NULL;
static jmethodID server_gc_mid = NULL;
static jmethodID server_finish_mid = NULL;

static jclass hashmap_global_cls = NULL;
static jmethodID hashmap_init_mid = NULL;
static jmethodID hashmap_put_mid = NULL;

/* JVM Logs */
static jclass insert_global_cls = NULL;
static jmethodID insert_init_mid = NULL;
static jclass update_global_cls = NULL;
static jmethodID update_init_mid = NULL;
static jclass delete_global_cls = NULL;
static jmethodID delete_init_mid = NULL;
static jclass ddl_global_cls = NULL;
static jmethodID ddl_init_mid = NULL;

/* JVM Values */
static jclass null_global_cls = NULL;
static jmethodID null_init_mid = NULL;
static jclass short_global_cls = NULL;
static jmethodID short_init_mid = NULL;
static jclass int_global_cls = NULL;
static jmethodID int_init_mid = NULL;
static jclass long_global_cls = NULL;
static jmethodID long_init_mid = NULL;
static jclass float_global_cls = NULL;
static jmethodID float_init_mid = NULL;
static jclass double_global_cls = NULL;
static jmethodID double_init_mid = NULL;
static jmethodID double_init_type_mid = NULL;
static jclass string_global_cls = NULL;
static jmethodID string_init_mid = NULL;
static jclass time_global_cls = NULL;
static jmethodID time_init_mid = NULL;
static jclass timestamp_global_cls = NULL;
static jmethodID timestamp_init_mid = NULL;
static jclass datetime_global_cls = NULL;
static jmethodID datetime_init_mid = NULL;
static jclass date_global_cls = NULL;
static jmethodID date_init_mid = NULL;
static jclass numeric_global_cls = NULL;
static jmethodID numeric_init_mid = NULL;

static jobject la_make_column_jobject (DB_OTMPL *);
static jobject la_make_pk_jobject (SM_CLASS *, DB_VALUE *);
static jobject la_make_old_column_jobject (DB_OBJECT *);
static jobject la_db_value_to_jobject (DB_VALUE *);
static int set_jvm_options (JavaVMOption **, char *, char *);
static int set_global_references (void);
static void *get_create_java_vm_function_ptr (void);

/*
 * la_plugin_start
 *   return: Error Code
 *
 * Note:
 */

int
la_plugin_start (LA_PLUGIN_INFO * plagin_info, char *local_db_name)
{
  int i, cnt = 0;
  jlong pageid = plagin_info->callback_lsa.pageid;
  jint offset = plagin_info->callback_lsa.offset;
  jint res;
  jint result;
  jmethodID mid;
  jclass server_local_cls = NULL;
  jobject server_local_obj = NULL;
  jstring jstr_class_names = NULL;
  jstring jstr_path = NULL;
  jstring jstr_version = NULL;
  jstring jstr_envroot = NULL;
  jstring jstr_db_name = NULL;
  jstring jstr_user = NULL;
  JavaVMInitArgs vm_arguments;
  JavaVMOption *options;
  char classpath[PATH_MAX + 32];
  char *loc_p, *locale;
  const char *envroot;
  char plugin_file_path[PATH_MAX];
  char str[LINE_MAX];
  char *option_string;
  CREATE_VM_FUNC create_vm_func = NULL;

  if (prm_get_bool_value (PRM_ID_HA_APPLY_PLUGIN) == false)
    {
      return NO_ERROR;
    }

  if (jvm != NULL)
    {
      return NO_ERROR;
    }

  envroot = envvar_root ();
  if (envroot == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_CANNOT_START_JVM,
	      1, "envvar_root");
      return er_errid ();
    }

  snprintf (classpath, sizeof (classpath) - 1,
	    "-Djava.class.path=%s:%s/*",
	    envvar_javadir_file (plugin_file_path, PATH_MAX, "plugin.jar"),
	    plagin_info->class_path);

  option_string =
    (char *) prm_get_string_value (PRM_ID_HA_APPLY_PLUGIN_JVM_OPTIONS);
  strncpy (str, option_string, LINE_MAX - 1);
  cnt = set_jvm_options (&options, classpath, str);

  vm_arguments.version = JNI_VERSION_1_6;
  vm_arguments.options = options;
  vm_arguments.nOptions = cnt;
  vm_arguments.ignoreUnrecognized = JNI_TRUE;

  locale = NULL;
  loc_p = setlocale (LC_TIME, NULL);
  if (loc_p != NULL)
    {
      locale = strdup (loc_p);
    }

  create_vm_func = (CREATE_VM_FUNC) get_create_java_vm_function_ptr ();
  if (create_vm_func)
    {
      res = (*create_vm_func) (&jvm, (void **) &env_p, &vm_arguments);
    }
  else
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_JVM_LIB_NOT_FOUND,
	      1, dlerror ());
      if (locale != NULL)
	{
	  free (locale);
	}
      return er_errid ();
    }

  setlocale (LC_TIME, locale);
  if (locale != NULL)
    {
      free (locale);
    }

  if (res < 0)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_CANNOT_START_JVM,
	      1, "JNI_CreateJavaVM");
      jvm = NULL;
      return er_errid ();
    }

  server_local_cls = JVM_FindClass (env_p, "com/cubrid/plugin/PluginServer");
  if (server_local_cls == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_CANNOT_FIND_CLASS,
	      1, "FindClass: " "com/cubrid/plugin/PluginServer");
      goto error;
    }

  server_global_cls = (jclass) JVM_NewGlobalRef (env_p, server_local_cls);
  if (server_global_cls == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
	      ER_PLUGIN_CREATE_REFERENCE_FAIL, 1,
	      "NewGlobalRef: " "com/cubrid/plugin/PluginServer Class");
      goto error;
    }
  JVM_DeleteLocalRef (env_p, server_local_cls);

  mid = JVM_GetMethodID (env_p, server_global_cls, "<init>",
			 "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;JI)V");
  if (mid == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_GET_METHOD_FAIL,
	      1, "GetMethodID: " "<init>");
      goto error;
    }

  jstr_class_names = JVM_NewStringUTF (env_p, plagin_info->java_class);
  if (jstr_class_names == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_CREATE_OBJECT_FAIL,
	      1, "NewStringUTF");
      goto error;
    }

  jstr_envroot = JVM_NewStringUTF (env_p, envvar_root ());
  if (jstr_envroot == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_CREATE_OBJECT_FAIL,
	      1, "NewStringUTF");
      goto error;
    }

  jstr_path = JVM_NewStringUTF (env_p, plagin_info->class_path);
  if (jstr_path == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_CREATE_OBJECT_FAIL,
	      1, "NewStringUTF");
      goto error;
    }

  jstr_version = JVM_NewStringUTF (env_p, rel_build_number ());
  if (jstr_version == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_CREATE_OBJECT_FAIL,
	      1, "NewStringUTF");
      goto error;
    }

  jstr_db_name = JVM_NewStringUTF (env_p, local_db_name);
  if (jstr_db_name == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_CREATE_OBJECT_FAIL,
	      1, "NewStringUTF");
      goto error;
    }

  jstr_user = JVM_NewStringUTF (env_p, plagin_info->user_string);
  if (jstr_user == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_CREATE_OBJECT_FAIL,
	      1, "NewStringUTF");
      goto error;
    }

  server_local_obj =
    JVM_NewObject (env_p, server_global_cls, mid, jstr_class_names,
		   jstr_envroot, jstr_path, jstr_version, jstr_db_name,
		   jstr_user, pageid, offset);
  if (server_local_obj == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_CREATE_OBJECT_FAIL,
	      1, "NewObject: " "com/cubrid/plugin/PluginServer");
      goto error;
    }

  server_global_obj = JVM_NewGlobalRef (env_p, server_local_obj);
  if (server_global_obj == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
	      ER_PLUGIN_CREATE_REFERENCE_FAIL, 1,
	      "NewGlobalRef: " "com/cubrid/plugin/PluginServer Object");
      goto error;
    }
  JVM_DeleteLocalRef (env_p, server_local_obj);

  server_init_mid = JVM_GetMethodID (env_p, server_global_cls, "init", "()I");
  if (server_init_mid == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_GET_METHOD_FAIL,
	      1, "GetMethodID: " "init");
      goto error;
    }

  server_put_mid =
    JVM_GetMethodID (env_p, server_global_cls, "call",
		     "(Lcom/cubrid/plugin/log/Log;)I");
  if (server_put_mid == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_GET_METHOD_FAIL,
	      1, "GetMethodID: " "put or call");
      goto error;
    }

  server_pageid_mid =
    JVM_GetStaticMethodID (env_p, server_global_cls, "getCallbackPageId",
			   "()J");
  if (server_pageid_mid == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_GET_METHOD_FAIL,
	      1, "GetMethodID: " "getCallbackPageId");
      goto error;
    }

  server_offset_mid =
    JVM_GetStaticMethodID (env_p, server_global_cls, "getCallbackOffset",
			   "()I");
  if (server_offset_mid == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_GET_METHOD_FAIL,
	      1, "GetMethodID: " "getCallbackOffset");
      goto error;
    }

  server_gc_mid =
    JVM_GetStaticMethodID (env_p, server_global_cls, "getGCCont", "()I");
  if (server_gc_mid == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_GET_METHOD_FAIL,
	      1, "GetMethodID: " "getGCCont");
      goto error;
    }

  server_finish_mid =
    JVM_GetMethodID (env_p, server_global_cls, "finish", "()V");
  if (server_finish_mid == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_GET_METHOD_FAIL,
	      1, "GetMethodID: " "finish");
      goto error;
    }

  result = JVM_CallIntMethodNoArg (env_p, server_global_obj, server_init_mid);
  if ((int) result == ER_PLUGIN_JVM_FATAL_EXCEPTION)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_JVM_FATAL_EXCEPTION,
	      1, "plugin init");
      goto error;
    }

  if (set_global_references () != 0)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
	      ER_PLUGIN_CREATE_REFERENCE_FAIL, 1, "Set global references");
      goto error;
    }

  JVM_DeleteLocalRef (env_p, jstr_class_names);
  JVM_DeleteLocalRef (env_p, jstr_path);
  JVM_DeleteLocalRef (env_p, jstr_version);
  JVM_DeleteLocalRef (env_p, jstr_envroot);
  JVM_DeleteLocalRef (env_p, jstr_user);

  return 0;

error:
  if (server_local_cls)
    {
      JVM_DeleteLocalRef (env_p, server_local_cls);
    }
  if (server_local_obj)
    {
      JVM_DeleteLocalRef (env_p, server_local_obj);
    }
  if (jstr_class_names)
    {
      JVM_DeleteLocalRef (env_p, jstr_class_names);
    }
  if (jstr_path)
    {
      JVM_DeleteLocalRef (env_p, jstr_path);
    }
  if (jstr_version)
    {
      JVM_DeleteLocalRef (env_p, jstr_version);
    }
  if (jstr_envroot)
    {
      JVM_DeleteLocalRef (env_p, jstr_envroot);
    }
  if (jstr_user)
    {
      JVM_DeleteLocalRef (env_p, jstr_user);
    }

  la_plugin_stop ();

  return er_errid ();
}

/*
 * la_plugin_stop
 *   return: 0
 *
 * Note:
 */

int
la_plugin_stop (void)
{
  if (server_global_obj != NULL && server_finish_mid != NULL)
    {
      JVM_CallVoidMethodNoArg (env_p, server_global_obj, server_finish_mid);
    }

  if (server_global_cls != NULL)
    {
      JVM_DeleteGlobalRef (env_p, server_global_cls);
    }

  if (server_global_obj != NULL)
    {
      JVM_DeleteGlobalRef (env_p, server_global_obj);
    }

  if (hashmap_global_cls != NULL)
    {
      JVM_DeleteGlobalRef (env_p, hashmap_global_cls);
    }

  if (insert_global_cls != NULL)
    {
      JVM_DeleteGlobalRef (env_p, insert_global_cls);
    }

  if (update_global_cls != NULL)
    {
      JVM_DeleteGlobalRef (env_p, update_global_cls);
    }

  if (delete_global_cls != NULL)
    {
      JVM_DeleteGlobalRef (env_p, delete_global_cls);
    }

  if (ddl_global_cls != NULL)
    {
      JVM_DeleteGlobalRef (env_p, ddl_global_cls);
    }

  if (null_global_cls != NULL)
    {
      JVM_DeleteGlobalRef (env_p, null_global_cls);
    }

  if (short_global_cls != NULL)
    {
      JVM_DeleteGlobalRef (env_p, short_global_cls);
    }

  if (int_global_cls != NULL)
    {
      JVM_DeleteGlobalRef (env_p, int_global_cls);
    }

  if (long_global_cls != NULL)
    {
      JVM_DeleteGlobalRef (env_p, long_global_cls);
    }

  if (float_global_cls != NULL)
    {
      JVM_DeleteGlobalRef (env_p, float_global_cls);
    }

  if (double_global_cls != NULL)
    {
      JVM_DeleteGlobalRef (env_p, double_global_cls);
    }

  if (string_global_cls != NULL)
    {
      JVM_DeleteGlobalRef (env_p, string_global_cls);
    }

  if (time_global_cls != NULL)
    {
      JVM_DeleteGlobalRef (env_p, time_global_cls);
    }

  if (timestamp_global_cls != NULL)
    {
      JVM_DeleteGlobalRef (env_p, timestamp_global_cls);
    }

  if (datetime_global_cls != NULL)
    {
      JVM_DeleteGlobalRef (env_p, datetime_global_cls);
    }

  if (date_global_cls != NULL)
    {
      JVM_DeleteGlobalRef (env_p, date_global_cls);
    }

  if (numeric_global_cls != NULL)
    {
      JVM_DeleteGlobalRef (env_p, numeric_global_cls);
    }

  if (jvm != NULL)
    {
      JVM_DestroyJavaVM (jvm);
      jvm = NULL;
    }

  return NO_ERROR;
}

/*
 * la_plugin_insert
 *   return: Error Code
 *
 * Note:
 */

int
la_plugin_insert (DB_OTMPL * inst_tp, LOG_LSA * lsa)
{
  jobject msg_obj = NULL;
  jobject values = NULL;
  jstring table_name = NULL;
  jint result;
  jlong pageid = lsa->pageid;
  jint offset = lsa->offset;

  if (jvm == NULL)
    {
      return NO_ERROR;
    }

  assert (server_global_obj != NULL);
  assert (server_put_mid != NULL);
  assert (insert_global_cls != NULL);
  assert (insert_init_mid != NULL);

  table_name = JVM_NewStringUTF (env_p, inst_tp->class_->header.name);
  if (table_name == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_CREATE_OBJECT_FAIL,
	      1, "NewStringUTF");
      goto error;
    }

  values = la_make_column_jobject (inst_tp);
  if (values == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_CREATE_OBJECT_FAIL,
	      1, "Make insert values Jobject");
      goto error;
    }

  msg_obj =
    JVM_NewObject (env_p, insert_global_cls, insert_init_mid, table_name,
		   values, pageid, offset);
  if (msg_obj == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_CREATE_OBJECT_FAIL,
	      1, "NewObject");
      goto error;
    }

  result =
    JVM_CallIntMethod (env_p, server_global_obj, server_put_mid, msg_obj);
  if ((int) result == ER_PLUGIN_JVM_FATAL_EXCEPTION)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_JVM_FATAL_EXCEPTION,
	      1, "Insert");
      goto error;
    }

  JVM_DeleteLocalRef (env_p, msg_obj);
  JVM_DeleteLocalRef (env_p, values);
  JVM_DeleteLocalRef (env_p, table_name);

  return (int) result;

error:
  if (msg_obj)
    {
      JVM_DeleteLocalRef (env_p, msg_obj);
    }
  if (values)
    {
      JVM_DeleteLocalRef (env_p, values);
    }
  if (table_name)
    {
      JVM_DeleteLocalRef (env_p, table_name);
    }

  return er_errid ();
}

/*
 * la_plugin_update
 *   return: Error Code
 *
 * Note:
 */

int
la_plugin_update (DB_OTMPL * old_inst_tp, DB_OTMPL * inst_tp,
		  DB_VALUE * key, LOG_LSA * lsa)
{
  jobject msg_obj = NULL;
  jobject old_values = NULL;
  jobject values = NULL;
  jobject pk = NULL;
  jstring table_name = NULL;
  jint result;
  jlong pageid = lsa->pageid;
  jint offset = lsa->offset;

  if (jvm == NULL)
    {
      return NO_ERROR;
    }

  assert (server_global_obj != NULL);
  assert (server_put_mid != NULL);
  assert (update_global_cls != NULL);
  assert (update_init_mid != NULL);

  table_name = JVM_NewStringUTF (env_p, inst_tp->class_->header.name);
  if (table_name == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_CREATE_OBJECT_FAIL,
	      1, "NewStringUTF");
      goto error;
    }

  old_values = la_make_column_jobject (old_inst_tp);
  if (old_values == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_CREATE_OBJECT_FAIL,
	      1, "Make update old values Jobject");
      goto error;
    }

  values = la_make_column_jobject (inst_tp);
  if (values == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_CREATE_OBJECT_FAIL,
	      1, "Make update new values Jobject");
      goto error;
    }

  pk = la_make_pk_jobject (inst_tp->class_, key);
  if (pk == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_CREATE_OBJECT_FAIL,
	      1, "Make update pk Jobject");
      goto error;
    }

  msg_obj =
    JVM_NewObject (env_p, update_global_cls, update_init_mid, table_name,
		   old_values, values, pk, pageid, offset);
  if (msg_obj == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_CREATE_OBJECT_FAIL,
	      1, "NewObject");
      goto error;
    }

  result =
    JVM_CallIntMethod (env_p, server_global_obj, server_put_mid, msg_obj);

  if ((int) result == ER_PLUGIN_JVM_FATAL_EXCEPTION)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_JVM_FATAL_EXCEPTION,
	      1, "Update");
      goto error;
    }

  JVM_DeleteLocalRef (env_p, msg_obj);
  JVM_DeleteLocalRef (env_p, old_values);
  JVM_DeleteLocalRef (env_p, values);
  JVM_DeleteLocalRef (env_p, pk);
  JVM_DeleteLocalRef (env_p, table_name);

  return (int) result;

error:
  if (msg_obj)
    {
      JVM_DeleteLocalRef (env_p, msg_obj);
    }
  if (old_values)
    {
      JVM_DeleteLocalRef (env_p, old_values);
    }
  if (values)
    {
      JVM_DeleteLocalRef (env_p, values);
    }
  if (pk)
    {
      JVM_DeleteLocalRef (env_p, pk);
    }
  if (table_name)
    {
      JVM_DeleteLocalRef (env_p, table_name);
    }

  return er_errid ();
}


/*
 * la_plugin_update_with_apply_log
 *   return: Error Code
 *
 * Note:
 */

int
la_plugin_update_with_apply_log (DB_OBJECT * old_obj, DB_OTMPL * inst_tp,
				 DB_VALUE * key, LOG_LSA * lsa)
{
  jobject msg_obj = NULL;
  jobject old_values = NULL;
  jobject values = NULL;
  jobject pk = NULL;
  jstring table_name = NULL;
  jint result;
  jlong pageid = lsa->pageid;
  jint offset = lsa->offset;

  if (jvm == NULL)
    {
      return NO_ERROR;
    }

  assert (server_global_obj != NULL);
  assert (server_put_mid != NULL);
  assert (update_global_cls != NULL);
  assert (update_init_mid != NULL);

  table_name = JVM_NewStringUTF (env_p, inst_tp->class_->header.name);
  if (table_name == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_CREATE_OBJECT_FAIL,
	      1, "NewStringUTF");
      goto error;
    }

  old_values = la_make_old_column_jobject (old_obj);
  if (old_values == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_CREATE_OBJECT_FAIL,
	      1, "Make update old values Jobject");
      goto error;
    }

  values = la_make_column_jobject (inst_tp);
  if (values == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_CREATE_OBJECT_FAIL,
	      1, "Make update new values Jobject");
      goto error;
    }

  pk = la_make_pk_jobject (inst_tp->class_, key);
  if (pk == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_CREATE_OBJECT_FAIL,
	      1, "Make update pk Jobject");
      goto error;
    }

  msg_obj =
    JVM_NewObject (env_p, update_global_cls, update_init_mid, table_name,
		   old_values, values, pk, pageid, offset);
  if (msg_obj == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_CREATE_OBJECT_FAIL,
	      1, "NewObject");
      goto error;
    }

  result =
    JVM_CallIntMethod (env_p, server_global_obj, server_put_mid, msg_obj);

  if ((int) result == ER_PLUGIN_JVM_FATAL_EXCEPTION)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_JVM_FATAL_EXCEPTION,
	      1, "Update");
      goto error;
    }

  JVM_DeleteLocalRef (env_p, msg_obj);
  JVM_DeleteLocalRef (env_p, old_values);
  JVM_DeleteLocalRef (env_p, values);
  JVM_DeleteLocalRef (env_p, pk);
  JVM_DeleteLocalRef (env_p, table_name);

  return (int) result;

error:
  if (msg_obj)
    {
      JVM_DeleteLocalRef (env_p, msg_obj);
    }
  if (old_values)
    {
      JVM_DeleteLocalRef (env_p, old_values);
    }
  if (values)
    {
      JVM_DeleteLocalRef (env_p, values);
    }
  if (pk)
    {
      JVM_DeleteLocalRef (env_p, pk);
    }
  if (table_name)
    {
      JVM_DeleteLocalRef (env_p, table_name);
    }

  return er_errid ();
}

/*
 * la_plugin_delete
 *   return: Error Code
 *
 * Note:
 */

int
la_plugin_delete (char *cls_name, MOBJ mclass, DB_VALUE * key, LOG_LSA * lsa)
{
  jobject msg_obj = NULL;
  jobject pk = NULL;
  jstring table_name = NULL;
  jint result;
  jlong pageid = lsa->pageid;
  jint offset = lsa->offset;

  if (jvm == NULL)
    {
      return NO_ERROR;
    }

  assert (server_global_obj != NULL);
  assert (server_put_mid != NULL);
  assert (delete_global_cls != NULL);
  assert (delete_init_mid != NULL);

  table_name = JVM_NewStringUTF (env_p, cls_name);
  if (table_name == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_CREATE_OBJECT_FAIL,
	      1, "NewStringUTF");
      goto error;
    }

  pk = la_make_pk_jobject ((SM_CLASS *) mclass, key);
  if (pk == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_CREATE_OBJECT_FAIL,
	      1, "Make delete pk Jobject");
      goto error;
    }

  msg_obj =
    JVM_NewObject (env_p, delete_global_cls, delete_init_mid, table_name, pk,
		   pageid, offset);
  if (msg_obj == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_CREATE_OBJECT_FAIL,
	      1, "NewObject");
      goto error;
    }

  result =
    JVM_CallIntMethod (env_p, server_global_obj, server_put_mid, msg_obj);

  if ((int) result == ER_PLUGIN_JVM_FATAL_EXCEPTION)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_JVM_FATAL_EXCEPTION,
	      1, "Delete");
      goto error;
    }

  JVM_DeleteLocalRef (env_p, msg_obj);
  JVM_DeleteLocalRef (env_p, pk);
  JVM_DeleteLocalRef (env_p, table_name);

  return (int) result;

error:
  if (msg_obj)
    {
      JVM_DeleteLocalRef (env_p, msg_obj);
    }
  if (pk)
    {
      JVM_DeleteLocalRef (env_p, pk);
    }
  if (table_name)
    {
      JVM_DeleteLocalRef (env_p, table_name);
    }

  return er_errid ();
}

/*
 * la_plugin_ddl
 *   return: Error Code
 *
 * Note:
 */

int
la_plugin_ddl (char *cls_name, char *ddl)
{
  jobject msg_obj = NULL;
  jstring ddl_str = NULL;
  jstring table_name = NULL;
  jint result;

  if (jvm == NULL)
    {
      return NO_ERROR;
    }

  assert (server_global_obj != NULL);
  assert (server_put_mid != NULL);
  assert (ddl_global_cls != NULL);
  assert (ddl_init_mid != NULL);

  table_name = JVM_NewStringUTF (env_p, cls_name);
  if (table_name == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_CREATE_OBJECT_FAIL,
	      1, "NewStringUTF");
      goto error;
    }

  ddl_str = JVM_NewStringUTF (env_p, ddl);
  if (ddl_str == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_CREATE_OBJECT_FAIL,
	      1, "NewStringUTF");
      goto error;
    }

  msg_obj =
    JVM_NewObject (env_p, ddl_global_cls, ddl_init_mid, table_name, ddl_str);
  if (msg_obj == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_CREATE_OBJECT_FAIL,
	      1, "NewObject");
      goto error;
    }

  result =
    JVM_CallIntMethod (env_p, server_global_obj, server_put_mid, msg_obj);

  if ((int) result == ER_PLUGIN_JVM_FATAL_EXCEPTION)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_JVM_FATAL_EXCEPTION,
	      1, "Schema");
      goto error;
    }

  JVM_DeleteLocalRef (env_p, msg_obj);
  JVM_DeleteLocalRef (env_p, ddl_str);
  JVM_DeleteLocalRef (env_p, table_name);

  return (int) result;

error:
  if (msg_obj)
    {
      JVM_DeleteLocalRef (env_p, msg_obj);
    }
  if (ddl_str)
    {
      JVM_DeleteLocalRef (env_p, ddl_str);
    }
  if (table_name)
    {
      JVM_DeleteLocalRef (env_p, table_name);
    }

  return er_errid ();
}

/*
 * la_make_column_jobject
 *   return: return java vm object pointer
 *
 * Note:
 */

static jobject
la_make_column_jobject (DB_OTMPL * inst_tp)
{
  int i;
  jobject map_obj = NULL;

  OBJ_TEMPASSIGN **assignments;

  assert (hashmap_global_cls != NULL);
  assert (hashmap_init_mid != NULL);
  assert (hashmap_put_mid != NULL);

  map_obj = JVM_NewObjectNoArg (env_p, hashmap_global_cls, hashmap_init_mid);
  if (map_obj == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_CREATE_OBJECT_FAIL,
	      1, "NewObject");
      goto error;
    }

  if (inst_tp == NULL)
    {
      return map_obj;
    }

  assignments = inst_tp->assignments;

  for (i = 0; i < inst_tp->nassigns; i++)
    {
      jstring att_name;
      jobject att_value;

      if (assignments[i] == NULL)
	{
	  continue;
	}

      att_name = JVM_NewStringUTF (env_p, assignments[i]->att->header.name);
      if (att_name == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CREATE_OBJECT_FAIL, 1, "NewStringUTF");
	  goto error;
	}

      att_value = la_db_value_to_jobject (assignments[i]->variable);
      if (att_value == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CREATE_OBJECT_FAIL, 1, "Make DB_VALUE Jobject");
	  JVM_DeleteLocalRef (env_p, att_name);
	  goto error;
	}

      JVM_CallObjectMethod (env_p, map_obj, hashmap_put_mid, att_name,
			    att_value);

      JVM_DeleteLocalRef (env_p, att_name);
      JVM_DeleteLocalRef (env_p, att_value);
    }

  return map_obj;

error:
  if (map_obj)
    {
      JVM_DeleteLocalRef (env_p, map_obj);
    }
  return NULL;
}

/*
 * la_make_pk_jobject
 *   return: return java vm object pointer
 *
 * Note:
 */

static jobject
la_make_pk_jobject (SM_CLASS * sm_class, DB_VALUE * key)
{
  int i;
  jobject map_obj = NULL;

  DB_MIDXKEY *midxkey = NULL;
  SM_ATTRIBUTE *pk_att;
  SM_ATTRIBUTE **attributes;
  SM_CLASS_CONSTRAINT *pk_cons;

  assert (hashmap_global_cls != NULL);
  assert (hashmap_init_mid != NULL);
  assert (hashmap_put_mid != NULL);

  pk_cons = classobj_find_class_primary_key (sm_class);
  if (pk_cons == NULL || pk_cons->attributes == NULL
      || pk_cons->attributes[0] == NULL)
    {
      return NULL;
    }
  if (DB_VALUE_TYPE (key) == DB_TYPE_MIDXKEY)
    {
      midxkey = db_get_midxkey (key);
      attributes = pk_cons->attributes;
    }

  map_obj = JVM_NewObjectNoArg (env_p, hashmap_global_cls, hashmap_init_mid);
  if (map_obj == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_CREATE_OBJECT_FAIL,
	      1, "NewObject");
      goto error;
    }

  if (DB_VALUE_TYPE (key) == DB_TYPE_MIDXKEY)
    {
      int prev_i_index = 0;
      char *prev_i_ptr = NULL;
      DB_VALUE value;
      jstring att_name;
      jobject att_value;

      for (i = 0; i < midxkey->ncolumns && attributes[i] != NULL; i++)
	{
	  pk_att = attributes[i];
	  pr_midxkey_get_element_nocopy (midxkey, i, &value, &prev_i_index,
					 &prev_i_ptr);

	  att_name = JVM_NewStringUTF (env_p, pk_att->header.name);
	  if (att_name == NULL)
	    {
	      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		      ER_PLUGIN_CREATE_OBJECT_FAIL, 1, "NewStringUTF");
	      goto error;
	    }

	  att_value = la_db_value_to_jobject (&value);
	  if (att_value == NULL)
	    {
	      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		      ER_PLUGIN_CREATE_OBJECT_FAIL, 1,
		      "Make DB_VALUE Jobject");
	      JVM_DeleteLocalRef (env_p, att_name);
	      goto error;
	    }

	  JVM_CallObjectMethod (env_p, map_obj, hashmap_put_mid, att_name,
				att_value);

	  JVM_DeleteLocalRef (env_p, att_name);
	  JVM_DeleteLocalRef (env_p, att_value);
	}
    }
  else
    {
      jstring att_name;
      jobject att_value;

      pk_att = pk_cons->attributes[0];

      att_name = JVM_NewStringUTF (env_p, pk_att->header.name);
      if (att_name == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CREATE_OBJECT_FAIL, 1, "NewStringUTF");
	  goto error;
	}

      att_value = la_db_value_to_jobject (key);
      if (att_value == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CREATE_OBJECT_FAIL, 1, "Make DB_VALUE Jobject");
	  JVM_DeleteLocalRef (env_p, att_name);
	  goto error;
	}

      JVM_CallObjectMethod (env_p, map_obj, hashmap_put_mid, att_name,
			    att_value);

      JVM_DeleteLocalRef (env_p, att_name);
      JVM_DeleteLocalRef (env_p, att_value);
    }

  return map_obj;

error:
  if (map_obj)
    {
      JVM_DeleteLocalRef (env_p, map_obj);
    }

  return NULL;
}

/*
 * la_make_old_column_jobject
 *   return: return java object
 *
 * Note:
 */

static jobject
la_make_old_column_jobject (DB_OBJECT * old)
{
  int i;
  DB_ATTRIBUTE *attribute;
  jobject map_obj = NULL;

  assert (hashmap_global_cls != NULL);
  assert (hashmap_init_mid != NULL);
  assert (hashmap_put_mid != NULL);

  map_obj = JVM_NewObjectNoArg (env_p, hashmap_global_cls, hashmap_init_mid);
  if (map_obj == NULL)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_CREATE_OBJECT_FAIL,
	      1, "NewObject");
      goto error;
    }

  attribute = db_get_attributes (old);
  while (attribute)
    {
      DB_VALUE value;
      jstring att_name;
      jobject att_value;

      att_name = JVM_NewStringUTF (env_p, db_attribute_name (attribute));
      if (att_name == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CREATE_OBJECT_FAIL, 1, "NewStringUTF");
	  goto error;
	}

      obj_get (old, db_attribute_name (attribute), &value);

      att_value = la_db_value_to_jobject (&value);
      if (att_value == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CREATE_OBJECT_FAIL, 1, "Make DB_VALUE Jobject");
	  JVM_DeleteLocalRef (env_p, att_name);
	  goto error;
	}

      JVM_CallObjectMethod (env_p, map_obj, hashmap_put_mid, att_name,
			    att_value);

      attribute = db_attribute_next (attribute);

      JVM_DeleteLocalRef (env_p, att_name);
      JVM_DeleteLocalRef (env_p, att_value);
    }

  return map_obj;

error:
  if (map_obj)
    {
      JVM_DeleteLocalRef (env_p, map_obj);
    }

  return NULL;
}

/*
 * db_value_to_jobject
 *   return: return java object
 *
 * Note:
 */

static jobject
la_db_value_to_jobject (DB_VALUE * value)
{
  jobject val_obj = NULL;

  if (DB_IS_NULL (value))
    {
      jint type = 0;
      assert (null_global_cls != NULL);
      assert (null_init_mid != NULL);

      val_obj = JVM_NewObject (env_p, null_global_cls, null_init_mid, type);
      if (val_obj == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CREATE_OBJECT_FAIL, 1, "NewObject");
	  goto error;
	}

      return val_obj;
    }

  switch (DB_VALUE_TYPE (value))
    {
    case DB_TYPE_INTEGER:
      {
	jint i = DB_GET_INT (value);

	assert (int_global_cls != NULL);
	assert (int_init_mid != NULL);

	val_obj = JVM_NewObject (env_p, int_global_cls, int_init_mid, i);
	if (val_obj == NULL)
	  {
	    er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		    ER_PLUGIN_CREATE_OBJECT_FAIL, 1, "NewObject");
	    goto error;
	  }
      }
      break;

    case DB_TYPE_SMALLINT:
      {
	jshort s = DB_GET_SMALLINT (value);

	assert (short_global_cls != NULL);
	assert (short_init_mid != NULL);

	val_obj = JVM_NewObject (env_p, short_global_cls, short_init_mid, s);
	if (val_obj == NULL)
	  {
	    er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		    ER_PLUGIN_CREATE_OBJECT_FAIL, 1, "NewObject");
	    goto error;
	  }
      }
      break;

    case DB_TYPE_BIGINT:
      {
	jlong bigint = DB_GET_BIGINT (value);

	assert (long_global_cls != NULL);
	assert (long_init_mid != NULL);

	val_obj =
	  JVM_NewObject (env_p, long_global_cls, long_init_mid, bigint);
	if (val_obj == NULL)
	  {
	    er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		    ER_PLUGIN_CREATE_OBJECT_FAIL, 1, "NewObject");
	    goto error;
	  }
      }
      break;

    case DB_TYPE_FLOAT:
      {
	jfloat f = DB_GET_FLOAT (value);

	assert (float_global_cls != NULL);
	assert (float_init_mid != NULL);

	val_obj = JVM_NewObject (env_p, float_global_cls, float_init_mid, f);
	if (val_obj == NULL)
	  {
	    er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		    ER_PLUGIN_CREATE_OBJECT_FAIL, 1, "NewObject");
	    goto error;
	  }
      }
      break;

    case DB_TYPE_DOUBLE:
      {
	jdouble d = DB_GET_DOUBLE (value);

	assert (double_global_cls != NULL);
	assert (double_init_mid != NULL);

	val_obj =
	  JVM_NewObject (env_p, double_global_cls, double_init_mid, d);
	if (val_obj == NULL)
	  {
	    er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		    ER_PLUGIN_CREATE_OBJECT_FAIL, 1, "NewObject");
	    goto error;
	  }
      }
      break;

    case DB_TYPE_MONETARY:
      {
	jint type;

	DB_MONETARY *money = DB_GET_MONETARY (value);
	jdouble d = money->amount;
	type = DB_VALUE_TYPE (value);

	assert (double_global_cls != NULL);
	assert (double_init_type_mid != NULL);

	val_obj =
	  JVM_NewObject (env_p, double_global_cls, double_init_type_mid, d,
			 type);
	if (val_obj == NULL)
	  {
	    er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		    ER_PLUGIN_CREATE_OBJECT_FAIL, 1, "NewObject");
	    goto error;
	  }
      }
      break;

    case DB_TYPE_VARCHAR:
    case DB_TYPE_CHAR:
    case DB_TYPE_VARNCHAR:
    case DB_TYPE_NCHAR:
      {
	jint type;
	const char *p = DB_GET_STRING (value);
	int size = DB_GET_STRING_SIZE (value);
	char *str_buf = NULL;
	jstring str = NULL;

	if (p == NULL)
	  {
	    goto error;
	  }

	if (size < 0)
	  {
	    goto error;
	  }

	assert (string_global_cls != NULL);
	assert (string_init_mid != NULL);

	str_buf = malloc ((size + 1) * sizeof (char));
	if (str_buf == NULL)
	  {
	    goto error;
	  }
	memcpy (str_buf, p, size);
	str_buf[size] = '\0';
	type = DB_VALUE_TYPE (value);

	str = JVM_NewStringUTF (env_p, str_buf);
	if (str == NULL)
	  {
	    free (str_buf);

	    er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		    ER_PLUGIN_CREATE_OBJECT_FAIL, 1, "NewStringUTF");
	    goto error;
	  }

	val_obj =
	  JVM_NewObject (env_p, string_global_cls, string_init_mid, str,
			 type);
	if (val_obj == NULL)
	  {
	    free (str_buf);
	    JVM_DeleteLocalRef (env_p, str);

	    er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		    ER_PLUGIN_CREATE_OBJECT_FAIL, 1, "NewObject");
	    goto error;
	  }

	free (str_buf);
	JVM_DeleteLocalRef (env_p, str);
      }
      break;

    case DB_TYPE_TIME:
      {
	int hour, minute, second;
	DB_TIME *time = DB_GET_TIME (value);

	db_time_decode (time, &hour, &minute, &second);

	assert (time_global_cls != NULL);
	assert (time_init_mid != NULL);

	val_obj =
	  JVM_NewObject (env_p, time_global_cls, time_init_mid, (jint) hour,
			 (jint) minute, (jint) second);
	if (val_obj == NULL)
	  {
	    er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		    ER_PLUGIN_CREATE_OBJECT_FAIL, 1, "NewObject");
	    goto error;
	  }
      }
      break;

    case DB_TYPE_TIMESTAMP:
      {
	int hour, minute, second;
	int year, month, day;
	DB_DATE date;
	DB_TIME timeval;
	DB_TIMESTAMP *ts = DB_GET_TIMESTAMP (value);

	db_timestamp_decode (ts, &date, &timeval);
	db_date_decode (&date, &month, &day, &year);
	db_time_decode (&timeval, &hour, &minute, &second);

	assert (timestamp_global_cls != NULL);
	assert (timestamp_init_mid != NULL);

	val_obj =
	  JVM_NewObject (env_p, timestamp_global_cls, timestamp_init_mid,
			 (jint) year, (jint) month, (jint) day, (jint) hour,
			 (jint) minute, (jint) second);
	if (val_obj == NULL)
	  {
	    er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		    ER_PLUGIN_CREATE_OBJECT_FAIL, 1, "NewObject");
	    goto error;
	  }
      }
      break;

    case DB_TYPE_DATETIME:
      {
	int hour, minute, second, millisecond;
	int year, month, day;
	DB_DATETIME *dt = DB_GET_DATETIME (value);

	db_datetime_decode (dt, &month, &day, &year, &hour, &minute, &second,
			    &millisecond);

	assert (datetime_global_cls != NULL);
	assert (datetime_init_mid != NULL);

	val_obj =
	  JVM_NewObject (env_p, datetime_global_cls, datetime_init_mid,
			 (jint) year, (jint) month, (jint) day, (jint) hour,
			 (jint) minute, (jint) second, (jint) millisecond);
	if (val_obj == NULL)
	  {
	    er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		    ER_PLUGIN_CREATE_OBJECT_FAIL, 1, "NewObject");
	    goto error;
	  }
      }
      break;

    case DB_TYPE_DATE:
      {
	int year, month, day;
	DB_DATE *date = DB_GET_DATE (value);

	db_date_decode (date, &month, &day, &year);

	assert (date_global_cls != NULL);
	assert (date_init_mid != NULL);

	val_obj =
	  JVM_NewObject (env_p, date_global_cls, date_init_mid, (jint) year,
			 (jint) month, (jint) day);
	if (val_obj == NULL)
	  {
	    er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		    ER_PLUGIN_CREATE_OBJECT_FAIL, 1, "NewObject");
	    goto error;
	  }
      }
      break;

    case DB_TYPE_NUMERIC:
      {
        char tmp_buf[NUMERIC_MAX_STRING_SIZE]; 
	const char *s = (const char *) numeric_db_value_print (value, tmp_buf);
	jstring str = NULL;

	if (s == NULL)
	  {
	    goto error;
	  }

	assert (numeric_global_cls != NULL);
	assert (numeric_init_mid != NULL);

	str = JVM_NewStringUTF (env_p, s);
	if (str == NULL)
	  {
	    er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		    ER_PLUGIN_CREATE_OBJECT_FAIL, 1, "NewStringUTF");
	    goto error;
	  }

	val_obj =
	  JVM_NewObject (env_p, numeric_global_cls, numeric_init_mid, str);
	if (val_obj == NULL)
	  {
	    JVM_DeleteLocalRef (env_p, str);

	    er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		    ER_PLUGIN_CREATE_OBJECT_FAIL, 1, "NewObject");
	    goto error;
	  }

	JVM_DeleteLocalRef (env_p, str);
      }
      break;

    case DB_TYPE_SET:
    case DB_TYPE_MULTISET:
    case DB_TYPE_SEQUENCE:
      {
	//TODO
      }
      break;

    case DB_TYPE_BIT:
    case DB_TYPE_VARBIT:
      {
	//TODO
      }
      break;

    case DB_TYPE_BLOB:
    case DB_TYPE_CLOB:
      {
	//TODO
      }
      break;

    case DB_TYPE_OBJECT:
      {
	//TODO
      }
      break;

    case DB_TYPE_ELO:
    case DB_TYPE_VARIABLE:
    case DB_TYPE_SUB:
    case DB_TYPE_POINTER:
    case DB_TYPE_ERROR:
    case DB_TYPE_VOBJ:
    case DB_TYPE_FIRST:
    case DB_TYPE_DB_VALUE:
    case DB_TYPE_RESULTSET:
    case DB_TYPE_TABLE:
    case DB_TYPE_MIDXKEY:
    case DB_TYPE_OID:
    case DB_TYPE_ENUMERATION:
      break;

    default:
      break;
    }

  if (val_obj == NULL)
    {
      jint type;
      assert (null_global_cls != NULL);
      assert (null_init_mid != NULL);

      type = DB_VALUE_TYPE (value);

      val_obj = JVM_NewObject (env_p, null_global_cls, null_init_mid, type);
      if (val_obj == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CREATE_OBJECT_FAIL, 1, "NewObject");
	  goto error;
	}
    }

  return val_obj;

error:
  if (val_obj)
    {
      JVM_DeleteLocalRef (env_p, val_obj);
    }

  return NULL;
}

/*
 * set_global_references_of_java_values
 *   return: return error
 *
 * Note:
 */

static int
set_global_references (void)
{
  jclass hashmap_local_cls = NULL;

  jclass insert_local_cls = NULL;
  jclass update_local_cls = NULL;
  jclass delete_local_cls = NULL;
  jclass ddl_local_cls = NULL;

  jclass null_local_cls = NULL;
  jclass short_local_cls = NULL;
  jclass int_local_cls = NULL;
  jclass long_local_cls = NULL;
  jclass float_local_cls = NULL;
  jclass double_local_cls = NULL;
  jclass string_local_cls = NULL;
  jclass time_local_cls = NULL;
  jclass timestamp_local_cls = NULL;
  jclass datetime_local_cls = NULL;
  jclass date_local_cls = NULL;
  jclass numeric_local_cls = NULL;

  if (hashmap_global_cls == NULL)
    {
      hashmap_local_cls = JVM_FindClass (env_p, "java/util/HashMap");
      if (hashmap_local_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CANNOT_FIND_CLASS, 1,
		  "FindClass: " "java/util/HashMap");
	  goto error;
	}

      hashmap_global_cls =
	(jclass) JVM_NewGlobalRef (env_p, hashmap_local_cls);
      if (hashmap_global_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CREATE_REFERENCE_FAIL, 1,
		  "NewGlobalRef: " "java/util/HashMap Class");
	  goto error;
	}
      JVM_DeleteLocalRef (env_p, hashmap_local_cls);

      hashmap_init_mid =
	JVM_GetMethodID (env_p, hashmap_global_cls, "<init>", "()V");
      if (hashmap_init_mid == NULL)
	{
	  JVM_DeleteGlobalRef (env_p, hashmap_global_cls);

	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_GET_METHOD_FAIL,
		  1, "GetMethodID: " "<init>");
	  goto error;
	}

      hashmap_put_mid =
	JVM_GetMethodID (env_p, hashmap_global_cls, "put",
			 "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
      if (hashmap_put_mid == NULL)
	{
	  JVM_DeleteGlobalRef (env_p, hashmap_global_cls);
	  hashmap_init_mid = NULL;

	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_GET_METHOD_FAIL,
		  1, "GetMethodID: " "put");
	  goto error;
	}
    }

  if (insert_global_cls == NULL)
    {
      insert_local_cls =
	JVM_FindClass (env_p, "com/cubrid/plugin/log/InsertLog");
      if (insert_local_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CANNOT_FIND_CLASS, 1,
		  "FindClass: " "com/cubrid/plugin/log/InsertLog");
	  goto error;
	}

      insert_global_cls = (jclass) JVM_NewGlobalRef (env_p, insert_local_cls);
      if (insert_global_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CREATE_REFERENCE_FAIL, 1,
		  "NewGlobalRef: " "com/cubrid/plugin/log/InsertLog Class");
	  goto error;
	}
      JVM_DeleteLocalRef (env_p, insert_local_cls);

      insert_init_mid =
	JVM_GetMethodID (env_p, insert_global_cls, "<init>",
			 "(Ljava/lang/String;Ljava/util/HashMap;JI)V");
      if (insert_init_mid == NULL)
	{
	  JVM_DeleteGlobalRef (env_p, insert_global_cls);

	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_GET_METHOD_FAIL,
		  1, "GetMethodID: " "<init>");
	  goto error;
	}
    }

  if (update_global_cls == NULL)
    {
      update_local_cls =
	JVM_FindClass (env_p, "com/cubrid/plugin/log/UpdateLog");
      if (update_local_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CANNOT_FIND_CLASS, 1,
		  "FindClass: " "com/cubrid/plugin/log/UpdateLog");
	  goto error;
	}

      update_global_cls = (jclass) JVM_NewGlobalRef (env_p, update_local_cls);
      if (update_global_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CREATE_REFERENCE_FAIL, 1,
		  "NewGlobalRef: " "com/cubrid/plugin/log/UpdateLog Class");
	  goto error;
	}
      JVM_DeleteLocalRef (env_p, update_local_cls);

      update_init_mid =
	JVM_GetMethodID (env_p, update_global_cls, "<init>",
			 "(Ljava/lang/String;Ljava/util/HashMap;Ljava/util/HashMap;Ljava/util/HashMap;JI)V");
      if (update_init_mid == NULL)
	{
	  JVM_DeleteGlobalRef (env_p, update_global_cls);

	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_GET_METHOD_FAIL,
		  1, "GetMethodID: " "<init>");
	  goto error;
	}
    }

  if (delete_global_cls == NULL)
    {
      delete_local_cls =
	JVM_FindClass (env_p, "com/cubrid/plugin/log/DeleteLog");
      if (delete_local_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CANNOT_FIND_CLASS, 1,
		  "FindClass: " "com/cubrid/plugin/log/DeleteLog");
	  goto error;
	}

      delete_global_cls = (jclass) JVM_NewGlobalRef (env_p, delete_local_cls);
      if (delete_global_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CREATE_REFERENCE_FAIL, 1,
		  "NewGlobalRef: " "com/cubrid/plugin/log/DeleteLog Class");
	  goto error;
	}
      JVM_DeleteLocalRef (env_p, delete_local_cls);

      delete_init_mid =
	JVM_GetMethodID (env_p, delete_global_cls, "<init>",
			 "(Ljava/lang/String;Ljava/util/HashMap;JI)V");
      if (delete_init_mid == NULL)
	{
	  JVM_DeleteGlobalRef (env_p, delete_global_cls);

	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_GET_METHOD_FAIL,
		  1, "GetMethodID: " "<init>");
	  goto error;
	}
    }

  if (ddl_global_cls == NULL)
    {
      ddl_local_cls = JVM_FindClass (env_p, "com/cubrid/plugin/log/DdlLog");
      if (ddl_local_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CANNOT_FIND_CLASS, 1,
		  "FindClass: " "com/cubrid/plugin/log/DdlLog");
	  goto error;
	}

      ddl_global_cls = (jclass) JVM_NewGlobalRef (env_p, ddl_local_cls);
      if (ddl_global_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CREATE_REFERENCE_FAIL, 1,
		  "NewGlobalRef: " "com/cubrid/plugin/log/DdlLog Class");
	  goto error;
	}
      JVM_DeleteLocalRef (env_p, ddl_local_cls);

      ddl_init_mid =
	JVM_GetMethodID (env_p, ddl_global_cls, "<init>",
			 "(Ljava/lang/String;Ljava/lang/String;)V");
      if (ddl_init_mid == NULL)
	{
	  JVM_DeleteGlobalRef (env_p, ddl_global_cls);

	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_GET_METHOD_FAIL,
		  1, "GetMethodID: " "<init>");
	  goto error;
	}
    }

  if (null_global_cls == NULL)
    {
      null_local_cls =
	JVM_FindClass (env_p, "com/cubrid/plugin/dbvalue/NullValue");
      if (null_local_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CANNOT_FIND_CLASS, 1,
		  "FindClass: " "com/cubrid/plugin/dbvalue/NullValue");
	  goto error;
	}

      null_global_cls = (jclass) JVM_NewGlobalRef (env_p, null_local_cls);
      if (null_global_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CREATE_REFERENCE_FAIL, 1,
		  "NewGlobalRef: "
		  "com/cubrid/plugin/dbvalue/NullValue Class");
	  goto error;
	}
      JVM_DeleteLocalRef (env_p, null_local_cls);

      null_init_mid =
	JVM_GetMethodID (env_p, null_global_cls, "<init>", "(I)V");
      if (null_init_mid == NULL)
	{
	  JVM_DeleteGlobalRef (env_p, null_global_cls);

	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_GET_METHOD_FAIL,
		  1, "GetMethodID: " "<init>");
	  goto error;
	}
    }

  if (short_global_cls == NULL)
    {
      short_local_cls =
	JVM_FindClass (env_p, "com/cubrid/plugin/dbvalue/ShortValue");
      if (short_local_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CANNOT_FIND_CLASS, 1,
		  "FindClass: " "com/cubrid/plugin/dbvalue/ShortValue");
	  goto error;
	}

      short_global_cls = (jclass) JVM_NewGlobalRef (env_p, short_local_cls);
      if (short_global_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CREATE_REFERENCE_FAIL, 1,
		  "NewGlobalRef: "
		  "com/cubrid/plugin/dbvalue/ShortValue Class");
	  goto error;
	}
      JVM_DeleteLocalRef (env_p, short_local_cls);

      short_init_mid =
	JVM_GetMethodID (env_p, short_global_cls, "<init>", "(S)V");
      if (short_init_mid == NULL)
	{
	  JVM_DeleteGlobalRef (env_p, short_global_cls);

	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_GET_METHOD_FAIL,
		  1, "GetMethodID: " "<init>");
	  goto error;
	}
    }

  if (int_global_cls == NULL)
    {
      int_local_cls =
	JVM_FindClass (env_p, "com/cubrid/plugin/dbvalue/IntValue");
      if (int_local_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CANNOT_FIND_CLASS, 1,
		  "FindClass: " "com/cubrid/plugin/dbvalue/IntValue");
	  goto error;
	}

      int_global_cls = (jclass) JVM_NewGlobalRef (env_p, int_local_cls);
      if (int_global_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CREATE_REFERENCE_FAIL, 1,
		  "NewGlobalRef: "
		  "com/cubrid/plugin/dbvalue/IntValue Class");
	  goto error;
	}
      JVM_DeleteLocalRef (env_p, int_local_cls);

      int_init_mid =
	JVM_GetMethodID (env_p, int_global_cls, "<init>", "(I)V");
      if (int_init_mid == NULL)
	{
	  JVM_DeleteGlobalRef (env_p, int_global_cls);

	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_GET_METHOD_FAIL,
		  1, "GetMethodID: " "<init>");
	  goto error;
	}
    }

  if (long_global_cls == NULL)
    {
      long_local_cls =
	JVM_FindClass (env_p, "com/cubrid/plugin/dbvalue/LongValue");
      if (long_local_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CANNOT_FIND_CLASS, 1,
		  "FindClass: " "com/cubrid/plugin/dbvalue/LongValue");
	  goto error;
	}

      long_global_cls = (jclass) JVM_NewGlobalRef (env_p, long_local_cls);
      if (long_global_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CREATE_REFERENCE_FAIL, 1,
		  "NewGlobalRef: "
		  "com/cubrid/plugin/dbvalue/LongValue Class");
	  goto error;
	}
      JVM_DeleteLocalRef (env_p, long_local_cls);

      long_init_mid =
	JVM_GetMethodID (env_p, long_global_cls, "<init>", "(J)V");
      if (long_init_mid == NULL)
	{
	  JVM_DeleteGlobalRef (env_p, long_global_cls);

	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_GET_METHOD_FAIL,
		  1, "GetMethodID: " "<init>");
	  goto error;
	}
    }

  if (float_global_cls == NULL)
    {
      float_local_cls =
	JVM_FindClass (env_p, "com/cubrid/plugin/dbvalue/FloatValue");
      if (float_local_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CANNOT_FIND_CLASS, 1,
		  "FindClass: " "com/cubrid/plugin/dbvalue/FloatValue");
	  goto error;
	}

      float_global_cls = (jclass) JVM_NewGlobalRef (env_p, float_local_cls);
      if (float_global_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CREATE_REFERENCE_FAIL, 1,
		  "NewGlobalRef: "
		  "com/cubrid/plugin/dbvalue/FloatValue Class");
	  goto error;
	}
      JVM_DeleteLocalRef (env_p, float_local_cls);

      float_init_mid =
	JVM_GetMethodID (env_p, float_global_cls, "<init>", "(F)V");
      if (float_init_mid == NULL)
	{
	  JVM_DeleteGlobalRef (env_p, float_global_cls);

	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_GET_METHOD_FAIL,
		  1, "GetMethodID: " "<init>");
	  goto error;
	}
    }

  if (double_global_cls == NULL)
    {
      double_local_cls =
	JVM_FindClass (env_p, "com/cubrid/plugin/dbvalue/DoubleValue");
      if (double_local_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CANNOT_FIND_CLASS, 1,
		  "FindClass: " "com/cubrid/plugin/dbvalue/DoubleValue");
	  goto error;
	}

      double_global_cls = (jclass) JVM_NewGlobalRef (env_p, double_local_cls);
      if (double_global_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CREATE_REFERENCE_FAIL, 1,
		  "NewGlobalRef: "
		  "com/cubrid/plugin/dbvalue/DoubleValue Class");
	  goto error;
	}
      JVM_DeleteLocalRef (env_p, double_local_cls);

      double_init_mid =
	JVM_GetMethodID (env_p, double_global_cls, "<init>", "(D)V");
      if (double_init_mid == NULL)
	{
	  JVM_DeleteGlobalRef (env_p, double_global_cls);

	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_GET_METHOD_FAIL,
		  1, "GetMethodID: " "<init>");
	  goto error;
	}

      double_init_type_mid =
	JVM_GetMethodID (env_p, double_global_cls, "<init>", "(DI)V");
      if (double_init_type_mid == NULL)
	{
	  JVM_DeleteGlobalRef (env_p, double_global_cls);
	  double_init_mid = NULL;

	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_GET_METHOD_FAIL,
		  1, "GetMethodID: " "<init>");
	  goto error;
	}
    }

  if (string_global_cls == NULL)
    {
      string_local_cls =
	JVM_FindClass (env_p, "com/cubrid/plugin/dbvalue/StringValue");
      if (string_local_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CANNOT_FIND_CLASS, 1,
		  "FindClass: " "com/cubrid/plugin/dbvalue/StringValue");
	  goto error;
	}

      string_global_cls = (jclass) JVM_NewGlobalRef (env_p, string_local_cls);
      if (string_global_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CREATE_REFERENCE_FAIL, 1,
		  "NewGlobalRef: "
		  "com/cubrid/plugin/dbvalue/StringValue Class");
	  goto error;
	}
      JVM_DeleteLocalRef (env_p, string_local_cls);

      string_init_mid =
	JVM_GetMethodID (env_p, string_global_cls, "<init>",
			 "(Ljava/lang/String;I)V");
      if (string_init_mid == NULL)
	{
	  JVM_DeleteGlobalRef (env_p, string_global_cls);

	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_GET_METHOD_FAIL,
		  1, "GetMethodID: " "<init>");
	  goto error;
	}
    }

  if (time_global_cls == NULL)
    {
      time_local_cls =
	JVM_FindClass (env_p, "com/cubrid/plugin/dbvalue/TimeValue");
      if (time_local_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CANNOT_FIND_CLASS, 1,
		  "FindClass: " "com/cubrid/plugin/dbvalue/TimeValue");
	  goto error;
	}

      time_global_cls = (jclass) JVM_NewGlobalRef (env_p, time_local_cls);
      if (time_global_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CREATE_REFERENCE_FAIL, 1,
		  "NewGlobalRef: "
		  "com/cubrid/plugin/dbvalue/TimeValue Class");
	  goto error;
	}
      JVM_DeleteLocalRef (env_p, time_local_cls);

      time_init_mid =
	JVM_GetMethodID (env_p, time_global_cls, "<init>", "(III)V");
      if (time_init_mid == NULL)
	{
	  JVM_DeleteGlobalRef (env_p, time_global_cls);

	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_GET_METHOD_FAIL,
		  1, "GetMethodID: " "<init>");
	  goto error;
	}
    }

  if (timestamp_global_cls == NULL)
    {
      timestamp_local_cls =
	JVM_FindClass (env_p, "com/cubrid/plugin/dbvalue/TimestampValue");
      if (timestamp_local_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CANNOT_FIND_CLASS, 1,
		  "FindClass: " "com/cubrid/plugin/dbvalue/TimestampValue");
	  goto error;
	}

      timestamp_global_cls =
	(jclass) JVM_NewGlobalRef (env_p, timestamp_local_cls);
      if (timestamp_global_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CREATE_REFERENCE_FAIL, 1,
		  "NewGlobalRef: "
		  "com/cubrid/plugin/dbvalue/TimestampValue Class");
	  goto error;
	}
      JVM_DeleteLocalRef (env_p, timestamp_local_cls);

      timestamp_init_mid =
	JVM_GetMethodID (env_p, timestamp_global_cls, "<init>", "(IIIIII)V");
      if (timestamp_init_mid == NULL)
	{
	  JVM_DeleteGlobalRef (env_p, timestamp_global_cls);

	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_GET_METHOD_FAIL,
		  1, "GetMethodID: " "<init>");
	  goto error;
	}
    }

  if (datetime_global_cls == NULL)
    {
      datetime_local_cls =
	JVM_FindClass (env_p, "com/cubrid/plugin/dbvalue/DatetimeValue");
      if (datetime_local_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CANNOT_FIND_CLASS, 1,
		  "FindClass: " "om/cubrid/plugin/dbvalue/DatetimeValue");
	  goto error;
	}

      datetime_global_cls =
	(jclass) JVM_NewGlobalRef (env_p, datetime_local_cls);
      if (datetime_global_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CREATE_REFERENCE_FAIL, 1,
		  "NewGlobalRef: "
		  "om/cubrid/plugin/dbvalue/DatetimeValue Class");
	  goto error;
	}
      JVM_DeleteLocalRef (env_p, datetime_local_cls);

      datetime_init_mid =
	JVM_GetMethodID (env_p, datetime_global_cls, "<init>", "(IIIIIII)V");
      if (datetime_init_mid == NULL)
	{
	  JVM_DeleteGlobalRef (env_p, datetime_global_cls);

	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_GET_METHOD_FAIL,
		  1, "GetMethodID: " "<init>");
	  goto error;
	}
    }

  if (date_global_cls == NULL)
    {
      date_local_cls =
	JVM_FindClass (env_p, "com/cubrid/plugin/dbvalue/DateValue");
      if (date_local_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CANNOT_FIND_CLASS, 1,
		  "FindClass: " "com/cubrid/plugin/dbvalue/DateValue");
	  goto error;
	}

      date_global_cls = (jclass) JVM_NewGlobalRef (env_p, date_local_cls);
      if (date_global_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CREATE_REFERENCE_FAIL, 1,
		  "NewGlobalRef: "
		  "com/cubrid/plugin/dbvalue/DateValue Class");
	  goto error;
	}
      JVM_DeleteLocalRef (env_p, date_local_cls);

      date_init_mid =
	JVM_GetMethodID (env_p, date_global_cls, "<init>", "(III)V");
      if (date_init_mid == NULL)
	{
	  JVM_DeleteGlobalRef (env_p, date_global_cls);

	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_GET_METHOD_FAIL,
		  1, "GetMethodID: " "<init>");
	  goto error;
	}
    }

  if (numeric_global_cls == NULL)
    {
      numeric_local_cls =
	JVM_FindClass (env_p, "com/cubrid/plugin/dbvalue/NumericValue");
      if (numeric_local_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CANNOT_FIND_CLASS, 1,
		  "FindClass: " "com/cubrid/plugin/dbvalue/NumericValue");
	  goto error;
	}

      numeric_global_cls =
	(jclass) JVM_NewGlobalRef (env_p, numeric_local_cls);
      if (numeric_global_cls == NULL)
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_CREATE_REFERENCE_FAIL, 1,
		  "NewGlobalRef: "
		  "com/cubrid/plugin/dbvalue/NumericValue Class");
	  goto error;
	}
      JVM_DeleteLocalRef (env_p, numeric_local_cls);

      numeric_init_mid =
	JVM_GetMethodID (env_p, numeric_global_cls, "<init>",
			 "(Ljava/lang/String;)V");
      if (numeric_init_mid == NULL)
	{
	  JVM_DeleteGlobalRef (env_p, numeric_global_cls);

	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_GET_METHOD_FAIL,
		  1, "GetMethodID: " "<init>");
	  goto error;
	}
    }

  return NO_ERROR;

error:

  return er_errid ();
}

/*
 * set_jvm_options
 *   return: option count
 *
 * Note:
 */

static int
set_jvm_options (JavaVMOption ** vm_options, char *classpath,
		 char *option_string)
{
  int i, cnt = 0;
  const char *delim = " ";
  JavaVMOption *op;
  char *option[MAX_OPTIONS];
  char *token;

  token = strtok (option_string, delim);

  while (token != NULL && cnt < MAX_OPTIONS)
    {
      option[cnt++] = token;

      token = strtok (NULL, delim);
    }
  cnt++;

  *vm_options = (JavaVMOption *) malloc (cnt * sizeof (JavaVMOption));
  op = *vm_options;

  for (i = 0; i < cnt - 1; i++)
    {
      op[i].optionString = option[i];
      op[i].extraInfo = NULL;
    }
  op[cnt - 1].optionString = classpath;
  op[cnt - 1].extraInfo = NULL;

  return cnt;
}

/*
 * la_plugin_get_lsa
 *   return: error
 *
 * Note:
 */

int
la_plugin_get_lsa (LA_PLUGIN_INFO * plagin_info)
{
  jlong pageid;
  jint offset;
  jint gc_cnt;

  if (jvm == NULL)
    {
      return NO_ERROR;
    }

  assert (server_global_obj != NULL);
  assert (server_pageid_mid != NULL);
  assert (server_offset_mid != NULL);
  assert (server_gc_mid != NULL);

  pageid =
    JVM_CallStaticLongMethodNoArg (env_p, server_global_cls,
				   server_pageid_mid);
  offset =
    JVM_CallStaticIntMethodNoArg (env_p, server_global_cls,
				  server_offset_mid);
  gc_cnt =
    JVM_CallStaticIntMethodNoArg (env_p, server_global_cls, server_gc_mid);

  plagin_info->callback_lsa.pageid = (INT64) pageid;
  plagin_info->callback_lsa.offset = (INT64) offset;
  plagin_info->gc_cnt = (int) gc_cnt;

  return NO_ERROR;
}


/*
 * get_create_java_vm_func_ptr
 *   return: return java vm function pointer
 *
 * Note:
 */

static void *
get_create_java_vm_function_ptr (void)
{
  const char *java_home;
  char jvm_library_path[PATH_MAX];
  void *libVM_p;

  libVM_p = dlopen (JVM_LIB_FILE, RTLD_LAZY | RTLD_GLOBAL);
  if (libVM_p == NULL)
    {
      er_set (ER_WARNING_SEVERITY, ARG_FILE_LINE, ER_PLUGIN_JVM_LIB_NOT_FOUND,
	      1, dlerror ());

      java_home = prm_get_string_value (PRM_ID_HA_APPLY_PLUGIN_JAVA_HOME);

      if (strncmp (java_home, "", PATH_MAX - 1) != 0)
	{
	  snprintf (jvm_library_path, PATH_MAX - 1, "%s/%s/%s", java_home,
		    JVM_LIB_PATH, JVM_LIB_FILE);
	  libVM_p = dlopen (jvm_library_path, RTLD_LAZY | RTLD_GLOBAL);

	  if (libVM_p == NULL)
	    {
	      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		      ER_PLUGIN_JVM_LIB_NOT_FOUND, 1, dlerror ());
	      return NULL;
	    }
	}
      else
	{
	  er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE,
		  ER_PLUGIN_JVM_LIB_NOT_FOUND, 1, dlerror ());
	  return NULL;
	}
    }

  return dlsym (libVM_p, "JNI_CreateJavaVM");
}
