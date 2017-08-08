package com.cubrid.plugin.dbvalue;

import java.sql.Date;
import java.sql.Time;
import java.sql.Timestamp;
import java.util.Calendar;

import com.cubrid.plugin.exception.TypeMismatchException;

public class DateValue extends DBValue {
	private Date date;

	public DateValue(int year, int mon, int day) {
		super();
		Calendar cal = Calendar.getInstance();
		cal.set(year, mon - 1, day, 0, 0, 0);
		cal.set(Calendar.MILLISECOND, 0);

		date = new Date(cal.getTimeInMillis());

		this.dbType = DBValue.DB_DATE;
	}

	public DateValue(int year, int mon, int day, int dbType) {
		super();
		Calendar cal = Calendar.getInstance();
		cal.set(year, mon - 1, day, 0, 0, 0);
		cal.set(Calendar.MILLISECOND, 0);

		date = new Date(cal.getTimeInMillis());

		this.dbType = dbType;
	}

	public DateValue(Date date) {
		this.date = date;

		this.dbType = DBValue.DB_DATE;
	}

	public Date getDate() throws TypeMismatchException {
		return date;
	}

	public Time getTime() throws TypeMismatchException {
		return new Time(date.getTime());
	}

	public Timestamp getTimestamp() throws TypeMismatchException {
		return new Timestamp(date.getTime());
	}

	public Timestamp getDatetime() throws TypeMismatchException {
		return new Timestamp(date.getTime());
	}

	public Object getObject() throws TypeMismatchException {
		return date;
	}

	public String getString() throws TypeMismatchException {
		return date.toString();
	}
}
