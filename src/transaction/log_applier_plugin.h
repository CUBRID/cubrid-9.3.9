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
 * log_applier_plugin.h - Applylogdb Plug-in Interface
 *
 * Note:
 */

#ifndef _LOG_APPLIER_PLUGIN_H_
#define _LOG_APPLIER_PLUGIN_H_

#ident "$Id$"

extern int la_plugin_start (LA_PLUGIN_INFO *, char *);
extern int la_plugin_stop (void);

extern int la_plugin_insert (DB_OTMPL *, LOG_LSA *);
extern int la_plugin_update (DB_OTMPL *, DB_OTMPL *, DB_VALUE *, LOG_LSA *);
extern int la_plugin_update_with_apply_log (DB_OBJECT *, DB_OTMPL *,
					    DB_VALUE *, LOG_LSA *);
extern int la_plugin_delete (char *, MOBJ, DB_VALUE *, LOG_LSA *);
extern int la_plugin_ddl (char *, char *);

extern int la_plugin_get_lsa (LA_PLUGIN_INFO *);


#endif /* _LOG_APPLIER_PLUGIN_H_ */
