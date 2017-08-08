package com.cubrid.plugin.log;

import java.util.HashMap;
import java.util.Set;

public class UpdateLog extends Log {

	private HashMap<Object, Object> oldValues;
	private HashMap<Object, Object> values;
	private HashMap<Object, Object> pk;

	public UpdateLog(String tableName, HashMap<Object, Object> oldValues,
			HashMap<Object, Object> values, HashMap<Object, Object> pk,
			long pageId, int offset) {
		this.type = Log.UPDATE;
		this.tableName = tableName;
		this.oldValues = oldValues;
		this.values = values;
		this.pk = pk;
		this.pageId = pageId;
		this.offset = offset;
	}

	public HashMap<Object, Object> getOldValues() {
		return oldValues;
	}

	public int oldValueSize() {
		return oldValues.size();
	}

	public HashMap<Object, Object> getValues() {
		return values;
	}

	public int valueSize() {
		return values.size();
	}

	public HashMap<Object, Object> getPk() {
		return pk;
	}

	public int pkSize() {
		return pk.size();
	}

	public String getQueryWithHostVariables() {
		String query = "UPDATE " + getTableName() + " SET ";
		Set<Object> keySet = values.keySet();
		Object[] key = (Object[]) keySet.toArray();

		for (int i = 0; i < valueSize(); i++) {
			String keyString = (String) key[i];

			query += keyString + " = " + "?";
			if (i != valueSize() - 1) {
				query += ", ";
			}
		}
		query += " WHERE ";

		keySet = pk.keySet();
		key = (Object[]) keySet.toArray();

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
