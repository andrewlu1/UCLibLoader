package com.uc.base.system;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

public class MainActivity extends Activity {
	private final static String TAG = "MAIN_ACTIVITY";

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		int ret = Test.hello(2, 5);
		Log.i("JNI Test", "2+5=" + ret);

	}

	public void onClick(View view) {
		// loadLibrary();
	}

}
