package com.cubrid.plugin;

import java.io.File;
import java.util.logging.FileHandler;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.logging.SimpleFormatter;

import com.cubrid.plugin.exception.*;
import com.cubrid.plugin.log.Log;

public class PluginServer {
	private static final int FATAL_EXCEPTION = -1151;
	private static final int WARNING_EXCEPTION = -1152;
	private static final long ONE_MB = 1024 * 1024;
	private static final String LOG_DIR = "log";

	private static String rootPath;
	private static String classPath;
	private static String version;
	private static String localDBName;
	private static String userString;
	private static String[] classNames;
	private static LogCallbackClass[] callbackClass;
	private static Logger logger = Logger.getLogger("plugin");

	public static long callbackPageId;
	public static int callbackOffset;
	public static int gcCount;

	public PluginServer(String classNames, String rootPath, String classPath,
			String version, String localDBName, String userString,
			long callbackPageId, int callbackOffset) {
		PluginServer.rootPath = rootPath;
		PluginServer.classPath = classPath;
		PluginServer.version = version;
		PluginServer.localDBName = localDBName;
		PluginServer.userString = userString;
		PluginServer.classNames = classNames.split(":");
		PluginServer.callbackPageId = callbackPageId;
		PluginServer.callbackOffset = callbackOffset;
		PluginServer.gcCount = 0;

		System.setSecurityManager(new PluginSecurityManager());
		System.setProperty("cubrid.server.version", version);
	}

	public int init() {
		try {
			loadCallback();
		} catch (Exception e) {
			PluginServer.log(e);
			return FATAL_EXCEPTION;
		}

		System.err.println("++ apply plugin start: success");
		return 0;
	}

	private void loadCallback() throws Exception {
		PluginServer.callbackClass = new LogCallbackClass[PluginServer.classNames.length];

		for (int i = 0; i < PluginServer.classNames.length; i++) {
			PluginServer.callbackClass[i] = new LogCallbackClass(
					PluginServer.classNames[i], PluginServer.rootPath,
					PluginServer.classPath, PluginServer.version,
					PluginServer.localDBName, PluginServer.userString);
		}
	}

	public int call(Log log) {
		try {
			Log finish = null;

			switch (log.getType()) {
			case Log.INSERT:
			case Log.UPDATE:
			case Log.DELETE:
			case Log.DDL:
				for (int i = 0; i < PluginServer.callbackClass.length; i++) {
					finish = PluginServer.callbackClass[i].callMethod(
							log.getType(), log);
				}
				break;

			default:
				throw new PluginException("Invaild Message Type Id : "
						+ log.getType());
			}

			if (finish != null) {
				PluginServer.callbackPageId = finish.getPageId();
				PluginServer.callbackOffset = finish.getOffset();
			}
		} catch (PluginException e) {
			PluginServer.log(e);
			return FATAL_EXCEPTION;
		} catch (LogException e) {
			PluginServer.log(e);
			if (e instanceof LogFatalException) {
				LogFatalException e1 = (LogFatalException) e;
				e1.exceptionHandler();
				return FATAL_EXCEPTION;
			} else if (e instanceof LogWarningException) {
				LogWarningException e1 = (LogWarningException) e;
				e1.exceptionHandler();
				return WARNING_EXCEPTION;
			}
		}

		if (Runtime.getRuntime().freeMemory() < 64 * ONE_MB) {
			System.gc();
			PluginServer.gcCount++;
		}

		return 0;
	}

	public void finish() {
		try {
			for (int i = 0; i < PluginServer.callbackClass.length; i++) {
				PluginServer.callbackClass[i].finish();
			}
		} catch (LogException e) {
			PluginServer.log(e);
		}
	}

	public static String getPluginPath() {
		return PluginServer.classPath;
	}

	public static String getVersion() {
		return PluginServer.version;
	}

	public static long getCallbackPageId() {
		return PluginServer.callbackPageId;
	}

	public static int getCallbackOffset() {
		return PluginServer.callbackOffset;
	}

	public static int getGCCont() {
		return PluginServer.gcCount;
	}

	public static void start(String classNames, String rootPath,
			String pluginPath, String version, String localDBName,
			String userString, long callbackPageId, int callbackOffset) {
		try {
			new PluginServer(classNames, rootPath, pluginPath, version,
					localDBName, userString, callbackPageId, callbackOffset);
		} catch (Exception e) {
			PluginServer.log(e);
		}
	}

	public static void log(Throwable ex) {
		FileHandler logHandler = null;
		String tableName = "";
		String logType = "";
		int type = 0;

		try {
			logHandler = new FileHandler(rootPath + File.separatorChar
					+ LOG_DIR + File.separatorChar + "apply_plugin.log",
					100 * 1024 * 1024, 2, true);

			logHandler.setFormatter(new SimpleFormatter());
			logger.addHandler(logHandler);

			if (ex instanceof LogFatalException) {
				LogFatalException fe = (LogFatalException) ex;
				tableName = fe.getLogTableName();
				type = fe.getLogType();
			} else if (ex instanceof LogWarningException) {
				LogWarningException we = (LogWarningException) ex;
				tableName = we.getLogTableName();
				type = we.getLogType();
			}

			if (type > 0) {
				switch (type) {
				case Log.INSERT:
					logType = "INSERT";
					break;
				case Log.UPDATE:
					logType = "UPDATE";
					break;
				case Log.DELETE:
					logType = "DELETE";
					break;
				case Log.DDL:
					logType = "DDL";
					break;
				}
			}

			String msg = ex.getMessage();
			if (msg != null) {
				logger.log(Level.SEVERE, msg + " | Table Name : " + tableName
						+ " | Log Type : " + logType, ex);
			} else {
				logger.log(Level.SEVERE, "No Message" + " | Table Name : "
						+ tableName + " | Log Type : " + logType, ex);
			}

			Throwable ex1 = ex.getCause();
			if (ex1 != null) {
				String msg1 = ex1.getMessage();
				if (msg1 != null) {
					logger.log(Level.SEVERE, msg1, ex1);
				} else {
					logger.log(Level.SEVERE, "", ex1);
				}
			}

		} catch (Throwable e) {
		} finally {
			if (logHandler != null) {
				try {
					logHandler.close();
					logger.removeHandler(logHandler);
				} catch (Throwable e) {
				}
			}
		}
	}
}
