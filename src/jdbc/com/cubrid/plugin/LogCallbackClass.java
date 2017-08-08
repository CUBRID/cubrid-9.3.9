package com.cubrid.plugin;

import java.util.Properties;

import com.cubrid.plugin.exception.*;
import com.cubrid.plugin.log.*;

public class LogCallbackClass {
	private String className;
	private Class<?> classObject = null;
	private LogCallback classInstance = null;

	public LogCallbackClass(String className, String rootPath,
			String classPath, String version, String localDBName,
			String userString) throws ClassNotFoundException,
			InstantiationException, IllegalAccessException, LogException {
		this.className = className;
		classObject = getClass(className);
		classInstance = (LogCallback) classObject.newInstance();

		Properties pros = new Properties();
		pros.setProperty("CLASS_NAME", className);
		pros.setProperty("ROOT_PATH", rootPath);
		pros.setProperty("CLASS_PATH", classPath);
		pros.setProperty("CUBRID_VERSION", version);
		pros.setProperty("LOCAL_DB_NAME", localDBName);
		pros.setProperty("USER_STRING", userString);

		classInstance.init(pros);
	}

	private Class<?> getClass(String name) throws ClassNotFoundException {
		ClassLoader cl = PluginClassLoader.getInstance();
		return cl.loadClass(name);
	}

	public String getClassName() {
		return className;
	}

	public Log callMethod(int methodId, Log msg) throws PluginException,
			LogException {
		switch (methodId) {
		case Log.INSERT:
			return classInstance.insertCallback((InsertLog) msg);
		case Log.UPDATE:
			return classInstance.updateCallback((UpdateLog) msg);
		case Log.DELETE:
			return classInstance.deleteCallback((DeleteLog) msg);
		case Log.DDL:
			return classInstance.ddlCallback((DdlLog) msg);
		default:
			throw new PluginException("Invaild method Id : " + methodId);
		}
	}

	public void finish() throws LogException {
		classInstance.finish();
	}
}
