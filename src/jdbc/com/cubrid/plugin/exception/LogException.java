package com.cubrid.plugin.exception;

public abstract class LogException extends Exception {

	private static final long serialVersionUID = 2483506986918003438L;

	public LogException() {
		super();
	}

	public LogException(String message, Throwable cause) {
		super(message, cause);
	}

	public LogException(String message) {
		super(message);
	}

	public LogException(Throwable cause) {
		super(cause);
	}
}
