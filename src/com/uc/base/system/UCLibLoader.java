package com.uc.base.system;

import java.util.LinkedList;
import java.util.List;

import android.os.Build;

/**
 * 
 * @author Andrewlu 2016.03.24. 用于API 23加载系统so库使用. 通过定制加载过程避免系统弹窗安全提醒.
 * 
 */
public class UCLibLoader {
	private final static String TAG = "UCLibLoader";
	private final static List<String> LOADEDLIBS_LIST = new LinkedList<String>();

	public static synchronized void loadLibrary(String libName) {
		if (LOADEDLIBS_LIST.contains(libName))
			return;
		LOADEDLIBS_LIST.add(libName);
		if (Build.VERSION.SDK_INT >= 23) {
			nativeLoadLibrary(libName);
		} else {
			System.loadLibrary(libName);
		}
	}

	private native static void nativeLoadLibrary(String libName);

	static {
		if (Build.VERSION.SDK_INT >= 23) {
			// 通过bootstrap加载系统非公开API库.
			System.loadLibrary("bootstrap");
		}
	}
}
