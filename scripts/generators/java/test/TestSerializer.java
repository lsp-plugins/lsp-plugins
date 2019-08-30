package test;

import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectOutputStream;

public class TestSerializer {
	public static void main(String[] args) throws FileNotFoundException, IOException
	{
		ObjectOutputStream os = new ObjectOutputStream(new FileOutputStream("test-java.bin"));
		os.writeInt(0x11332244);
		os.writeByte(0xce);
		os.writeUTF("writeUTF string");
		os.writeObject("writeObject string");
		
		TestObject o, arr[] = new TestObject[2];
		String shared = "Shared string";
		
		o = new TestObject();
		o.xbyte	= 0x5a;
		o.xshort = 0x55aa;
		o.xint = 0x10203040;
		o.xlong = 0x1122334455667788L;
		o.xchar = 'X';
		o.xdouble = 440.0;
		o.xfloat = 48000.0f;
		o.xbool = false;
		o.iarray = new int[] { 1, 2, 3, 4, 5, 6, 7, 8 };
		o.iref = "Referenced string";
		o.inull = null;
		o.xenum = TestEnum.ONE;
		arr[0] = o;
		
		o = new TestObject();
		o.xbyte	= (byte)0xa5;
		o.xshort = (short)0xaa55;
		o.xint = 0x20304050;
		o.xlong = 0x5566778811223344L;
		o.xchar = 'Y';
		o.xdouble = 880.0;
		o.xfloat = 41000.0f;
		o.xbool = true;
		o.iarray = new int[] { 5, 4, 3, 2 };
		o.iref = new Object[] { "String 1", new Long(10), new Character('a'), "String 2", TestEnum.FIVE, shared };
		o.inull = TestEnum.FOUR;
		o.xenum = TestEnum.TWO;
		arr[1] = o;
		
		os.writeObject(arr);
		os.writeObject(o);
		
		o = new TestObject();
		o.xbyte	= 0x44;
		o.xshort = 0x44cc;
		o.xint = 0xcafebabe;
		o.xlong = 0x1020304050607080L;
		o.xchar = 'Z';
		o.xdouble = 125.0;
		o.xfloat = 192000.0f;
		o.xbool = false;
		o.iarray = new int[] { };
		o.iref = shared;
		o.inull = null;
		o.xenum = TestEnum.ONE;
		
		os.writeObject(o);
		
		os.writeObject(TestEnum.ONE);
		os.writeObject(TestEnum.TWO);
		os.writeObject(TestEnum.THREE);
		
		os.reset();
		
		os.writeObject(o);
		os.writeObject(TestEnum.ONE);
		os.writeObject(TestEnum.TWO);
		os.writeObject(TestEnum.THREE);
		
		os.flush();
		os.close();
	}
}
