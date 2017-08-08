package com.cubrid.plugin.dbvalue;

import com.cubrid.plugin.exception.TypeMismatchException;

public class BooleanValue extends DBValue {
	private byte value = 0;

	public BooleanValue(boolean b, int dbType) {
		super();
		if (b) {
			value = 1;
		}

		this.dbType = dbType;
	}

	public Short getShortObject() throws TypeMismatchException {
		return new Short(value);
	}

	public Integer getIntegerObject() throws TypeMismatchException {
		return new Integer(value);
	}

	public Float getFloatObject() throws TypeMismatchException {
		return new Float(value);
	}

	public Double getDoubleObject() throws TypeMismatchException {
		return new Double(value);
	}

	public String getString() throws TypeMismatchException {
		return "" + value;
	}
}
