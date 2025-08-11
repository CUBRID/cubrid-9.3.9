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
 *      load_object.h: simplified object definitions
 */

#ifndef _LOAD_OBJECT_H_
#define _LOAD_OBJECT_H_

#ident "$Id$"

#include "dbtype.h"
#include "class_object.h"

/*
 * DESC_OBJ
 *    This is a simplified description of an object that is used in cases
 *    where we do not want or need to build complete memory representations
 *    of an instance.  This was developed primarily to support the
 *    loader/import/export utility but could be used for other things as
 *    well.
 */
typedef struct desc_obj
{
  MOP classop;
  SM_CLASS *class_;
  int updated_flag;
  int count;
  SM_ATTRIBUTE **atts;
  DB_VALUE *values;
} DESC_OBJ;


extern DESC_OBJ *make_desc_obj (SM_CLASS * class_);
extern int desc_obj_to_disk (DESC_OBJ * obj, RECDES * record, bool * index_flag);
extern int desc_disk_to_obj (MOP classop, SM_CLASS * class_,  RECDES * record, DESC_OBJ * obj, bool is_unloaddb);
extern void desc_free (DESC_OBJ * obj);

extern void desc_value_fprint (FILE * fp, DB_VALUE * value);
#if defined (CUBRID_DEBUG)
extern void desc_value_print (DB_VALUE * value);
#endif
extern int lo_migrate_out (LOID * lo, const char *pathname);
extern int lo_migrate_in (LOID * lo, const char *pathname);
extern int er_filter_fileset (FILE * ef);
extern int er_filter_errid (bool ignore_warning);

#endif /* _LOAD_OBJECT_H_ */
