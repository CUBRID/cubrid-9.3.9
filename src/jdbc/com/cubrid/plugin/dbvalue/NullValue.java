package com.cubrid.plugin.dbvalue;

import java.math.BigDecimal;
import java.sql.Date;
import java.sql.Time;
import java.sql.Timestamp;

import com.cubrid.plugin.exception.TypeMismatchException;

public class NullValue extends DBValue {
	public NullValue() {
		super();
	}

	public NullValue(int dbType) {
		super();

		this.dbType = dbType;
	}

	public Byte getByteObject() throws TypeMismatchException {
		return null;
	}

	public Short getShortObject() throws TypeMismatchException {
		return null;
	}

	public Integer getIntegerObject() throws TypeMismatchException {
		return null;
	}

	public Long getLongObject() throws TypeMismatchException {
		return null;
	}

	public Float getFloatObject() throws TypeMismatchException {
		return null;
	}

	public Double getDoubleObject() throws TypeMismatchException {
		return null;
	}

	public Date getDate() throws TypeMismatchException {
		return null;
	}

	public Time getTime() throws TypeMismatchException {
		return null;
	}

	public Timestamp getTimestamp() throws TypeMismatchException {
		return null;
	}

	public Timestamp getDatetime() throws TypeMismatchException {
		return null;
	}

	public BigDecimal getBigDecimal() throws TypeMismatchException {
		return null;
	}

	public Object getObject() throws TypeMismatchException {
		return null;
	}

	public String getString() throws TypeMismatchException {
		return null;
	}
}
