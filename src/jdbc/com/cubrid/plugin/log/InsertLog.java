package com.cubrid.plugin.log;

import java.util.HashMap;
import java.util.Set;

public class InsertLog extends Log {

	private HashMap<Object, Object> values;

	public InsertLog(String tableName, HashMap<Object, Object> values,
			long pageId, int offset) {
		this.type = Log.INSERT;
		this.tableName = tableName;
		this.values = values;
		this.pageId = pageId;
		this.offset = offset;
	}

	public HashMap<Object, Object> getValues() {
		return values;
	}

	public int valueSize() {
		return values.size();
	}

	public String getQueryWithHostVariables() {
		String query = "INSERT INTO " + getTableName() + " (";
		Set<Object> keySet = values.keySet();
		Object[] key = (Object[]) keySet.toArray();

		for (int i = 0; i < valueSize(); i++) {
			String keyString = (String) key[i];

			query += keyString;
			if (i != valueSize() - 1) {
				query += ", ";
			}
		}
		query += ") VALUES (";

		for (int i = 0; i < valueSize(); i++) {
			query += "?";
			if (i != valueSize() - 1) {
				query += ", ";
			}
		}
		query += ");";

		return query;
	}
}
