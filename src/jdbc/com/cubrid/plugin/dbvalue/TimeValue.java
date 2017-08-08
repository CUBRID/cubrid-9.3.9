package com.cubrid.plugin.dbvalue;

import java.sql.Date;
import java.sql.Time;
import java.sql.Timestamp;
import java.util.Calendar;

import com.cubrid.plugin.exception.TypeMismatchException;

public class TimeValue extends DBValue {
	private Time time;

	public TimeValue(int hour, int min, int sec) {
		super();
		Calendar cal = Calendar.getInstance();
		cal.set(0, 0, 0, hour, min, sec);
		cal.set(Calendar.MILLISECOND, 0);

		this.time = new Time(cal.getTimeInMillis());

		this.dbType = DBValue.DB_TIME;
	}

	public TimeValue(int hour, int min, int sec, int dbType) {
		super();
		Calendar cal = Calendar.getInstance();
		cal.set(0, 0, 0, hour, min, sec);
		cal.set(Calendar.MILLISECOND, 0);

		this.time = new Time(cal.getTimeInMillis());

		this.dbType = dbType;
	}

	public TimeValue(Time time) {
		this.time = time;

		this.dbType = DBValue.DB_TIME;
	}

	public Date getDate() throws TypeMismatchException {
		return new Date(time.getTime());
	}

	public Time getTime() throws TypeMismatchException {
		return time;
	}

	public Timestamp getTimestamp() throws TypeMismatchException {
		return new Timestamp(time.getTime());
	}

	public Timestamp getDatetime() throws TypeMismatchException {
		return new Timestamp(time.getTime());
	}

	public Object getObject() throws TypeMismatchException {
		return time;
	}

	public String getString() throws TypeMismatchException {
		return time.toString();
	}
}
