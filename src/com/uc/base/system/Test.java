package com.uc.base.system;

public class Test {

	public static native int hello(int a, int b);

	public static native long max(int a, long b);

	static {
		// 这里直接用UCLibLoader代替System即可.
		UCLibLoader.loadLibrary("mmath");
	}
}
