package com.example;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;

public class MainActivity extends Activity
{
    //SendEmailActivity email = new SendEmailActivity();
    /**
     * Called when the activity is first created.
     */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
    }

    @Override
    public void onBackPressed()
    {
       setContentView(R.layout.main);
    }

    public void onClick_BtnTakePhoto(View v)
    {
        Intent intent = new Intent(this, TakePhotoActivity.class);
        startActivity(intent);
    }

    public void onClick_BtnSendEmail(View v)
    {
        Intent intent = new Intent(this, SendEmailActivity.class);
        startActivity(intent);

    }


}
