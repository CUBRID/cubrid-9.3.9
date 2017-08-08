package com.cubrid.plugin.log;

public class DdlLog extends Log {

	private String ddl;

	public DdlLog(String tableName, String ddl) {
		this.type = Log.DDL;
		this.tableName = tableName;
		this.ddl = ddl;
	}

	public String getDdl() {
		return ddl;
	}
}
