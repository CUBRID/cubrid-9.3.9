package com.cubrid.plugin.log;

public abstract class Log {

	public static final int INSERT = 1;
	public static final int UPDATE = 2;
	public static final int DELETE = 3;
	public static final int DDL = 4;

	protected int type;
	protected String tableName;

	protected long pageId;
	protected int offset;

	public Log() {
		type = 0;
		tableName = "";
		pageId = -1;
		offset = -1;
	}

	public int getType() {
		return type;
	}

	public String getTableName() {
		return tableName;
	}

	public long getPageId() {
		return pageId;
	}

	public int getOffset() {
		return offset;
	}
}
