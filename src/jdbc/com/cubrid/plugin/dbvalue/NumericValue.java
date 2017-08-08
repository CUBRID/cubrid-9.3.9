package com.cubrid.plugin.dbvalue;

import java.math.BigDecimal;

import com.cubrid.plugin.exception.TypeMismatchException;

public class NumericValue extends DBValue {
	private BigDecimal value;

	public NumericValue(String value) {
		super();
		this.value = new BigDecimal(value);

		this.dbType = DBValue.DB_NUMERIC;
	}

	public NumericValue(String value, int dbType) {
		super();
		this.value = new BigDecimal(value);

		this.dbType = dbType;
	}

	public byte getByte() throws TypeMismatchException {
		return value.byteValue();
	}

	public short getShort() throws TypeMismatchException {
		return value.shortValue();
	}

	public int getInt() throws TypeMismatchException {
		return value.intValue();
	}

	public long getLong() throws TypeMismatchException {
		return value.longValue();
	}

	public float getFloat() throws TypeMismatchException {
		return value.floatValue();
	}

	public double getDouble() throws TypeMismatchException {
		return value.doubleValue();
	}

	public Byte getByteObject() throws TypeMismatchException {
		return new Byte(value.byteValue());
	}

	public Short getShortObject() throws TypeMismatchException {
		return new Short(value.shortValue());
	}

	public Integer getIntegerObject() throws TypeMismatchException {
		return new Integer(value.intValue());
	}

	public Long getLongObject() throws TypeMismatchException {
		return new Long(value.longValue());
	}

	public Float getFloatObject() throws TypeMismatchException {
		return new Float(value.floatValue());
	}

	public Double getDoubleObject() throws TypeMismatchException {
		return new Double(value.doubleValue());
	}

	public BigDecimal getBigDecimal() throws TypeMismatchException {
		return value;
	}

	public Object getObject() throws TypeMismatchException {
		return getBigDecimal();
	}

	public String getString() throws TypeMismatchException {
		return value.toString();
	}
}
