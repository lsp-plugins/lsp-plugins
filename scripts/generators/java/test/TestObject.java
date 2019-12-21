package test;

import java.io.Serializable;

public class TestObject implements Serializable {

	private static final long serialVersionUID = 1L;
	
	public byte xbyte;
	public short xshort;
	public int xint;
	public long xlong;
	public char xchar;
	public double xdouble;
	public float xfloat;
	public boolean xbool;
	
	public int [] iarray;
	public Object iref;
	public Object inull;
	public TestEnum xenum;
}
