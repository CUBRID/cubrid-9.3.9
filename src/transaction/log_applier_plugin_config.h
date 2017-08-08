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
 * log_applier_plugin_config.h - Applylogdb Plug-in Interface configuration
 *
 * Note:
 */

#ifndef _LOG_APPLIER_PLUGIN_CONFIG_H_
#define _LOG_APPLIER_PLUGIN_CONFIG_H_

#ident "$Id$"

#define LA_PLUGIN_EMPTY				0x0000	/* empty flag */
#define LA_PLUGIN_INSERT			0x0001	/* send a insert message to plugin */
#define LA_PLUGIN_UPDATE			0x0002	/* send a update message to plugin */
#define LA_PLUGIN_DELETE			0x0004	/* send a delete message to plugin */
#define LA_PLUGIN_DDL				0x0008	/* send a schema message to plugin */

#define LA_PLUGIN_IS_INSERT(x)			(x & LA_PLUGIN_INSERT)
#define LA_PLUGIN_IS_UPDATE(x)			(x & LA_PLUGIN_UPDATE)
#define LA_PLUGIN_IS_DELETE(x)			(x & LA_PLUGIN_DELETE)
#define LA_PLUGIN_IS_DDL(x)			(x & LA_PLUGIN_DDL)

#define LA_PLUGIN_CLEAR_BIT(this, here)		(here &= ~this)
#define LA_PLUGIN_SET_BIT(this, here)		(here |= this)

/*
 * Log applier plugin state
 */
#define LA_PLUGIN_STATE_MASTER			0x0001	/* plugin master */
#define LA_DB_SERVER_STATE_MASTER		0x0010	/* db server master */

#define LA_PLUGIN_STATE_IS_MASTER(x)		(x & LA_PLUGIN_STATE_MASTER)
#define LA_DB_SERVER_STATE_IS_MASTER(x)		(x & LA_DB_SERVER_STATE_MASTER)

/* Log applier plugin info */
typedef struct la_plugin_info LA_PLUGIN_INFO;
struct la_plugin_info
{
  int message_type;
  int table_cnt;
  char **table_name;
  char host_name[MAXHOSTNAMELEN];
  char class_path[LINE_MAX];
  char java_class[LINE_MAX];
  char user_string[LINE_MAX];
  LOG_LSA sent_lsa;		/* sent to jvm */
  LOG_LSA callback_lsa;		/* call the user defined function */
  INT64 insert_cnt;
  INT64 update_cnt;
  INT64 delete_cnt;
  INT64 ddl_cnt;
  INT64 fatal_cnt;
  INT64 warning_cnt;
  time_t last_log_record_time;
  int gc_cnt;
  int status;
};

extern int la_plugin_config_read (LA_PLUGIN_INFO * plugin_info);

#endif /* _LOG_APPLIER_PLUGIN_CONFIG_H_ */
