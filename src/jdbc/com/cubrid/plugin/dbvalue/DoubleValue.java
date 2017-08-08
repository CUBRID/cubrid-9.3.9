package com.cubrid.plugin.dbvalue;

import java.math.BigDecimal;

import com.cubrid.plugin.exception.TypeMismatchException;

public class DoubleValue extends DBValue {
	private double value;

	public DoubleValue(double value) {
		super();
		this.value = value;

		this.dbType = DBValue.DB_DOUBLE;
	}

	public DoubleValue(double value, int dbType) {
		super();
		this.value = value;

		this.dbType = dbType;
	}

	public byte getByte() throws TypeMismatchException {
		return (byte) value;
	}

	public short getShort() throws TypeMismatchException {
		return (short) value;
	}

	public int getInt() throws TypeMismatchException {
		return (int) value;
	}

	public long getLong() throws TypeMismatchException {
		return (long) value;
	}

	public float getFloat() throws TypeMismatchException {
		return (float) value;
	}

	public double getDouble() throws TypeMismatchException {
		return value;
	}

	public Byte getByteObject() throws TypeMismatchException {
		return new Byte((byte) value);
	}

	public Short getShortObject() throws TypeMismatchException {
		return new Short((short) value);
	}

	public Integer getIntegerObject() throws TypeMismatchException {
		return new Integer((int) value);
	}

	public Long getLongObject() throws TypeMismatchException {
		return new Long((long) value);
	}

	public Float getFloatObject() throws TypeMismatchException {
		return new Float((float) value);
	}

	public Double getDoubleObject() throws TypeMismatchException {
		return new Double(value);
	}

	public BigDecimal getBigDecimal() throws TypeMismatchException {
		return new BigDecimal(value);
	}

	public Object getObject() throws TypeMismatchException {
		return getDoubleObject();
	}

	public String getString() throws TypeMismatchException {
		return "" + value;
	}
}
