package com.cubrid.plugin.dbvalue;

import java.math.BigDecimal;
import java.sql.Date;
import java.sql.Time;
import java.sql.Timestamp;

import com.cubrid.plugin.exception.TypeMismatchException;

public class StringValue extends DBValue {
	private String value;

	public StringValue(String value) {
		super();
		this.value = value;

		this.dbType = DBValue.DB_STRING;
	}

	public StringValue(String value, int dbType) {
		super();
		this.value = value;

		this.dbType = dbType;
	}

	public byte getByte() throws TypeMismatchException {
		try {
			return Byte.parseByte(value);
		} catch (NumberFormatException e) {
			throw new TypeMismatchException(e.getMessage());
		}
	}

	public short getShort() throws TypeMismatchException {
		try {
			return Short.parseShort(value);
		} catch (NumberFormatException e) {
			throw new TypeMismatchException(e.getMessage());
		}
	}

	public int getInt() throws TypeMismatchException {
		try {
			return Integer.parseInt(value);
		} catch (NumberFormatException e) {
			throw new TypeMismatchException(e.getMessage());
		}
	}

	public long getLong() throws TypeMismatchException {
		try {
			return Long.parseLong(value);
		} catch (NumberFormatException e) {
			throw new TypeMismatchException(e.getMessage());
		}
	}

	public float getFloat() throws TypeMismatchException {
		try {
			return Float.parseFloat(value);
		} catch (NumberFormatException e) {
			throw new TypeMismatchException(e.getMessage());
		}
	}

	public double getDouble() throws TypeMismatchException {
		try {
			return Double.parseDouble(value);
		} catch (NumberFormatException e) {
			throw new TypeMismatchException(e.getMessage());
		}
	}

	public Byte getByteObject() throws TypeMismatchException {
		try {
			return Byte.valueOf(value);
		} catch (NumberFormatException e) {
			throw new TypeMismatchException(e.getMessage());
		}
	}

	public Short getShortObject() throws TypeMismatchException {
		try {
			return Short.valueOf(value);
		} catch (NumberFormatException e) {
			throw new TypeMismatchException(e.getMessage());
		}
	}

	public Integer getIntegerObject() throws TypeMismatchException {
		try {
			return Integer.valueOf(value);
		} catch (NumberFormatException e) {
			throw new TypeMismatchException(e.getMessage());
		}
	}

	public Long getLongObject() throws TypeMismatchException {
		try {
			return Long.valueOf(value);
		} catch (NumberFormatException e) {
			throw new TypeMismatchException(e.getMessage());
		}
	}

	public Float getFloatObject() throws TypeMismatchException {
		try {
			return Float.valueOf(value);
		} catch (NumberFormatException e) {
			throw new TypeMismatchException(e.getMessage());
		}
	}

	public Double getDoubleObject() throws TypeMismatchException {
		try {
			return Double.valueOf(value);
		} catch (NumberFormatException e) {
			throw new TypeMismatchException(e.getMessage());
		}
	}

	public Date getDate() throws TypeMismatchException {
		try {
			return Date.valueOf(value);
		} catch (IllegalArgumentException e) {
			throw new TypeMismatchException(e.getMessage());
		}
	}

	public Time getTime() throws TypeMismatchException {
		try {
			return Time.valueOf(value);
		} catch (IllegalArgumentException e) {
			throw new TypeMismatchException(e.getMessage());
		}
	}

	public Timestamp getTimestamp() throws TypeMismatchException {
		try {
			return Timestamp.valueOf(value);
		} catch (IllegalArgumentException e) {
			throw new TypeMismatchException(e.getMessage());
		}
	}

	public Timestamp getDatetime() throws TypeMismatchException {
		try {
			return Timestamp.valueOf(value);
		} catch (IllegalArgumentException e) {
			throw new TypeMismatchException(e.getMessage());
		}
	}

	public BigDecimal getBigDecimal() throws TypeMismatchException {
		try {
			return new BigDecimal(value);
		} catch (NumberFormatException e) {
			throw new TypeMismatchException(e.getMessage());
		}
	}

	public Object getObject() throws TypeMismatchException {
		return value;
	}

	public String getString() throws TypeMismatchException {
		return value;
	}
}
