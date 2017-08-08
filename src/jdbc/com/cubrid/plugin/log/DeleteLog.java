package com.cubrid.plugin.log;

import java.util.HashMap;
import java.util.Set;

public class DeleteLog extends Log {

	private HashMap<Object, Object> pk;

	public DeleteLog(String tableName, HashMap<Object, Object> pk, long pageId,
			int offset) {
		this.type = Log.DELETE;
		this.tableName = tableName;
		this.pk = pk;
		this.pageId = pageId;
		this.offset = offset;
	}

	public HashMap<Object, Object> getPk() {
		return pk;
	}

	public int pkSize() {
		return pk.size();
	}

	public String getQueryWithHostVariables() {
		String query = "DELETE FROM " + getTableName() + " WHERE ";
		Set<Object> keySet = pk.keySet();
		Object[] key = (Object[]) keySet.toArray();

		for (int i = 0; i < pkSize(); i++) {
			String keyString = (String) key[i];

			if (i > 0) {
				query += " AND ";
			}
			query += keyString + " = " + "?";
		}
		query += ";";

		return query;
	}
}
