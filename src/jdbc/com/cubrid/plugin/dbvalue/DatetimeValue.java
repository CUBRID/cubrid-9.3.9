package com.cubrid.plugin.dbvalue;

import java.sql.Date;
import java.sql.Time;
import java.sql.Timestamp;
import java.util.Calendar;

import com.cubrid.plugin.exception.TypeMismatchException;

public class DatetimeValue extends DBValue {
	private Timestamp timestamp;

	public DatetimeValue(int year, int mon, int day, int hour, int min,
			int sec, int msec) {
		super();

		Calendar c = Calendar.getInstance();
		c.set(year, mon - 1, day, hour, min, sec);
		c.set(Calendar.MILLISECOND, msec);

		timestamp = new Timestamp(c.getTimeInMillis());

		this.dbType = DBValue.DB_DATE;
	}

	public DatetimeValue(int year, int mon, int day, int hour, int min,
			int sec, int msec, int dbType) {
		super();

		Calendar c = Calendar.getInstance();
		c.set(year, mon - 1, day, hour, min, sec);
		c.set(Calendar.MILLISECOND, msec);

		timestamp = new Timestamp(c.getTimeInMillis());

		this.dbType = dbType;
	}

	public DatetimeValue(Timestamp timestamp) {
		this.timestamp = timestamp;

		this.dbType = DBValue.DB_DATE;
	}

	public Date getDate() throws TypeMismatchException {
		return new Date(timestamp.getTime());
	}

	public Time getTime() throws TypeMismatchException {
		return new Time(timestamp.getTime());
	}

	public Timestamp getTimestamp() throws TypeMismatchException {
		return timestamp;
	}

	public Timestamp getDatetime() throws TypeMismatchException {
		return timestamp;
	}

	public Object getObject() throws TypeMismatchException {
		return timestamp;
	}

	public String getString() throws TypeMismatchException {
		return timestamp.toString();
	}
}
