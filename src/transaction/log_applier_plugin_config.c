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
 * log_applier_plugin_config.c - Applylogdb Plug-in Interface configuration
 */

#ident "$Id$"

#include "config.h"
#include "error_manager.h"
#include "system_parameter.h"
#include "environment_variable.h"
#include "storage_common.h"
#include "ini_parser.h"
#include "log_applier_plugin_config.h"

#define TABLE_NAME_MAX		256

static const char SECTION_NAME[] = "plugin";
static const char DELIM[] = ":";

static int set_message_type (LA_PLUGIN_INFO *, char *);


/*
 * la_plugin_config_read - read and parse configurations
 *   return: 0 or -1 if fail
 *   plugin_info(in/out):
 */
int
la_plugin_config_read (LA_PLUGIN_INFO * plugin_info)
{
  int i;
  char conf_file[PATH_MAX];
  char table_names[LINE_MAX];
  INI_TABLE *ini;
  const char *ini_string;
  char *token;
  char *save_ptr;
  int lineno = 0;

  envvar_confdir_file (conf_file, PATH_MAX, "cubrid_apply_plugin.conf");

  ini = ini_parser_load (conf_file);
  if (ini == NULL)
    {
      er_log_debug (ARG_FILE_LINE, "cannot open conf file %s\n", conf_file);
      return -1;
    }

  if (ini->nsec != 1)
    {
      er_log_debug (ARG_FILE_LINE, "cannot find section\n");
      goto conf_error;
    }

  /* get [plugin] section vars */
  if (!ini_findsec (ini, SECTION_NAME))
    {
      er_log_debug (ARG_FILE_LINE,
		    "cannot find [%s] section in conf file %s\n",
		    SECTION_NAME, conf_file);
      goto conf_error;
    }

  plugin_info->message_type = 0;
  ini_string = ini_getstr (ini, SECTION_NAME, "MESSAGE_TYPE", "", &lineno);

  token = strtok_r ((char *) ini_string, DELIM, &save_ptr);
  while (token)
    {
      set_message_type (plugin_info, token);

      token = strtok_r (NULL, DELIM, &save_ptr);
    }

  plugin_info->table_cnt = 0;
  ini_string = ini_getstr (ini, SECTION_NAME, "TABLE", "", &lineno);

  strncpy (table_names, ini_string, LINE_MAX - 1);
  token = strtok_r (table_names, DELIM, &save_ptr);
  while (token)
    {
      plugin_info->table_cnt++;
      token = strtok_r (NULL, DELIM, &save_ptr);
    }

  plugin_info->table_name = malloc (plugin_info->table_cnt * sizeof (char *));
  for (i = 0; i < plugin_info->table_cnt; i++)
    {
      plugin_info->table_name[i] = malloc (TABLE_NAME_MAX * sizeof (char));
    }

  i = 0;
  strncpy (table_names, ini_string, LINE_MAX - 1);
  token = strtok_r (table_names, DELIM, &save_ptr);
  while (token)
    {
      strncpy (plugin_info->table_name[i++], token, TABLE_NAME_MAX - 1);
      token = strtok_r (NULL, DELIM, &save_ptr);
    }

  ini_string = ini_getstr (ini, SECTION_NAME, "CLASS_PATH", "", &lineno);
  strncpy (plugin_info->class_path, ini_string, LINE_MAX - 1);

  ini_string = ini_getstr (ini, SECTION_NAME, "JAVA_CLASS", "", &lineno);
  strncpy (plugin_info->java_class, ini_string, LINE_MAX - 1);

  ini_string = ini_getstr (ini, SECTION_NAME, "USER_STRING", "", &lineno);
  strncpy (plugin_info->user_string, ini_string, LINE_MAX - 1);

  ini_parser_free (ini);
  ini = NULL;

  return 0;

conf_error:
  if (ini)
    {
      ini_parser_free (ini);
    }

  return -1;
}

static int
set_message_type (LA_PLUGIN_INFO * plugin_info, char *str)
{
  if (strcasecmp (str, "insert") == 0)
    {
      LA_PLUGIN_SET_BIT (LA_PLUGIN_INSERT, plugin_info->message_type);
    }
  else if (strcasecmp (str, "update") == 0)
    {
      LA_PLUGIN_SET_BIT (LA_PLUGIN_UPDATE, plugin_info->message_type);
    }
  else if (strcasecmp (str, "delete") == 0)
    {
      LA_PLUGIN_SET_BIT (LA_PLUGIN_DELETE, plugin_info->message_type);
    }
  else if (strcasecmp (str, "ddl") == 0)
    {
      LA_PLUGIN_SET_BIT (LA_PLUGIN_DDL, plugin_info->message_type);
    }
  else
    {
      return -1;
    }

  return 0;
}
