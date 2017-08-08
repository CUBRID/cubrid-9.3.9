package com.cubrid.plugin.dbvalue;

import java.sql.Date;
import java.sql.Time;
import java.sql.Timestamp;
import java.util.Calendar;

import com.cubrid.plugin.exception.TypeMismatchException;

public class TimestampValue extends DBValue {
	private Timestamp timestamp;

	public TimestampValue(int year, int mon, int day, int hour, int min, int sec) {
		super();
		Calendar cal = Calendar.getInstance();
		cal.set(year, mon - 1, day, hour, min, sec);
		cal.set(Calendar.MILLISECOND, 0);

		this.timestamp = new Timestamp(cal.getTimeInMillis());

		this.dbType = DBValue.DB_TIMESTAMP;
	}

	public TimestampValue(int year, int mon, int day, int hour, int min,
			int sec, int dbType) {
		super();
		Calendar cal = Calendar.getInstance();
		cal.set(year, mon - 1, day, hour, min, sec);
		cal.set(Calendar.MILLISECOND, 0);

		this.timestamp = new Timestamp(cal.getTimeInMillis());

		this.dbType = dbType;
	}

	public TimestampValue(Timestamp timestamp) {
		this.timestamp = timestamp;

		this.dbType = DBValue.DB_TIMESTAMP;
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
