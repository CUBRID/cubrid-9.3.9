package com.cubrid.plugin;

import java.util.concurrent.BlockingQueue;

import com.cubrid.plugin.exception.LogException;
import com.cubrid.plugin.exception.LogFatalException;
import com.cubrid.plugin.exception.LogWarningException;
import com.cubrid.plugin.exception.PluginException;
import com.cubrid.plugin.log.Log;

public class PluginThread extends Thread {

	private LogCallbackClass[] callbackClass;
	private BlockingQueue<Log> queue;

	public PluginThread(LogCallbackClass[] callbackClass,
			BlockingQueue<Log> queue) {
		this.callbackClass = callbackClass;
		this.queue = queue;
	}

	public void run() {
		while (true) {
			try {
				Log log = queue.take();
				Log finish = null;

				switch (log.getType()) {
				case Log.INSERT:
				case Log.UPDATE:
				case Log.DELETE:
				case Log.DDL:
					for (int i = 0; i < callbackClass.length; i++) {
						finish = callbackClass[i]
								.callMethod(log.getType(), log);
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
			} catch (LogException e) {
				PluginServer.log(e);
				if (e instanceof LogFatalException) {
					LogFatalException e1 = (LogFatalException) e;
					e1.exceptionHandler();
				} else if (e instanceof LogWarningException) {
					LogWarningException e1 = (LogWarningException) e;
					e1.exceptionHandler();
				}
			} catch (InterruptedException e) {
				PluginServer.log(e);
			}
		}
	}
}
