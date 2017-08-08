package com.cubrid.plugin;

import java.util.Properties;

import com.cubrid.plugin.exception.*;
import com.cubrid.plugin.log.*;

public interface LogCallback {
	public void init(Properties pros) throws LogException;

	public Log insertCallback(InsertLog log) throws LogException;

	public Log updateCallback(UpdateLog log) throws LogException;

	public Log deleteCallback(DeleteLog log) throws LogException;

	public Log ddlCallback(DdlLog log) throws LogException;

	public void finish() throws LogException;
}
