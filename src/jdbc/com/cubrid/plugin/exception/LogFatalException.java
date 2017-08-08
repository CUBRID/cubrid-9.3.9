package com.cubrid.plugin.exception;

import com.cubrid.plugin.PluginServer;
import com.cubrid.plugin.log.Log;

public class LogFatalException extends LogException {

	private static final long serialVersionUID = -3154695709201671632L;
	private Log log;

	public LogFatalException(Log log) {
		super();
		this.log = log;
	}

	public LogFatalException(String message, Throwable cause, Log log) {
		super(message, cause);
		this.log = log;
	}

	public LogFatalException(String message, Log log) {
		super(message);
		this.log = log;
	}

	public LogFatalException(Throwable cause, Log log) {
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
