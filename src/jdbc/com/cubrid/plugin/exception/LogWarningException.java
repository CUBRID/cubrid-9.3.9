package com.cubrid.plugin.exception;

import com.cubrid.plugin.PluginServer;
import com.cubrid.plugin.log.Log;

public class LogWarningException extends LogException {

	private static final long serialVersionUID = 4837821683364141007L;
	private Log log;

	public LogWarningException(Log log) {
		super();
		this.log = log;
	}

	public LogWarningException(String message, Throwable cause, Log log) {
		super(message, cause);
		this.log = log;
	}

	public LogWarningException(String message, Log log) {
		super(message);
		this.log = log;
	}

	public LogWarningException(Throwable cause, Log log) {
		super(cause);
		this.log = log;
	}

	public int getLogType() {
		int type = 0;

		if (log != null) {
			type = log.getType();
		}

		return type;
	}

	public String getLogTableName() {
		String tableName = "";

		if (log != null) {
			tableName = log.getTableName();
		}

		return tableName;
	}

	public void exceptionHandler() {
		PluginServer.log(this);
	}
}
