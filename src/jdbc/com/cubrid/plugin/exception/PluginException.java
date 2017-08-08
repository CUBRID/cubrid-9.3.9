package com.cubrid.plugin.exception;

public class PluginException extends Exception {

	private static final long serialVersionUID = -710547582763641943L;

	public PluginException() {
		super();
	}

	public PluginException(String message, Throwable cause) {
		super(message, cause);
	}

	public PluginException(String message) {
		super(message);
	}

	public PluginException(Throwable cause) {
		super(cause);
	}
}
