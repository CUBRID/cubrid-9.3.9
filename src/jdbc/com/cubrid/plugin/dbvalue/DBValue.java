package com.cubrid.plugin.dbvalue;

import java.math.BigDecimal;
import java.sql.Date;
import java.sql.Time;
import java.sql.Timestamp;

import com.cubrid.plugin.exception.TypeMismatchException;

public abstract class DBValue {

	public static final int DB_NULL = 0;
	public static final int DB_INTEGER = 1;
	public static final int DB_FLOAT = 2;
	public static final int DB_DOUBLE = 3;
	public static final int DB_STRING = 4;
	public static final int DB_OBJECT = 5;
	public static final int DB_SET = 6;
	public static final int DB_MULTISET = 7;
	public static final int DB_SEQUENCE = 8;
	public static final int DB_ELO = 9;
	public static final int DB_TIME = 10;
	public static final int DB_TIMESTAMP = 11;
	public static final int DB_DATE = 12;
	public static final int DB_MONETARY = 13;
	public static final int DB_SHORT = 18;
	public static final int DB_NUMERIC = 22;
	public static final int DB_BIT = 23;
	public static final int DB_VARBIT = 24;
	public static final int DB_CHAR = 25;
	public static final int DB_NCHAR = 26;
	public static final int DB_VARNCHAR = 27;
	public static final int DB_BIGINT = 31;
	public static final int DB_DATETIME = 32;
	public static final int DB_BLOB = 33;
	public static final int DB_CLOB = 34;
	public static final int DB_ENUMERATION = 35;
	public static final int DB_LIST = DBValue.DB_SEQUENCE;
	public static final int DB_SMALLINT = DBValue.DB_SHORT;
	public static final int DB_VARCHAR = DBValue.DB_STRING;
	public static final int DB_UTIME = DBValue.DB_TIMESTAMP;

	protected int dbType;

	public DBValue() {
		dbType = DBValue.DB_NULL;
	}

	public int getDbType() {
		return dbType;
	}

	public byte getByte() throws TypeMismatchException {
		throw new TypeMismatchException();
	}

	public short getShort() throws TypeMismatchException {
		throw new TypeMismatchException();
	}

	public int getInt() throws TypeMismatchException {
		throw new TypeMismatchException();
	}

	public long getLong() throws TypeMismatchException {
		throw new TypeMismatchException();
	}

	public float getFloat() throws TypeMismatchException {
		throw new TypeMismatchException();
	}

	public double getDouble() throws TypeMismatchException {
		throw new TypeMismatchException();
	}

	public Byte getByteObject() throws TypeMismatchException {
		throw new TypeMismatchException();
	}

	public Short getShortObject() throws TypeMismatchException {
		throw new TypeMismatchException();
	}

	public Integer getIntegerObject() throws TypeMismatchException {
		throw new TypeMismatchException();
	}

	public Long getLongObject() throws TypeMismatchException {
		throw new TypeMismatchException();
	}

	public Float getFloatObject() throws TypeMismatchException {
		throw new TypeMismatchException();
	}

	public Double getDoubleObject() throws TypeMismatchException {
		throw new TypeMismatchException();
	}

	public Object getObject() throws TypeMismatchException {
		throw new TypeMismatchException();
	}

	public Date getDate() throws TypeMismatchException {
		throw new TypeMismatchException();
	}

	public Time getTime() throws TypeMismatchException {
		throw new TypeMismatchException();
	}

	public Timestamp getTimestamp() throws TypeMismatchException {
		throw new TypeMismatchException();
	}

	public Timestamp getDatetime() throws TypeMismatchException {
		throw new TypeMismatchException();
	}

	public BigDecimal getBigDecimal() throws TypeMismatchException {
		throw new TypeMismatchException();
	}

	public String getString() throws TypeMismatchException {
		throw new TypeMismatchException();
	}
}
