package com.example;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;

/**
 * Created with IntelliJ IDEA.
 * User: Lloyd
 * Date: 8/31/12
 * Time: 11:30 PM
 * To change this template use File | Settings | File Templates.
 */
public class SendEmailActivity extends Activity
{

    private Button send;
    private EditText address, subject, emailtext;

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.sendemail);
        send = (Button) findViewById(R.id.btnSend);
        address = (EditText) findViewById(R.id.email);
        subject = (EditText) findViewById(R.id.subject);
        emailtext = (EditText) findViewById(R.id.body);

        send.setOnClickListener(new OnClickListener()
        {
            @Override
            public void onClick(View view)
            {
                final Intent emailIntent = new Intent(android.content.Intent.ACTION_SEND);
                emailIntent.setType("plain/text");
                emailIntent.putExtra(android.content.Intent.EXTRA_EMAIL, new String[]{address.getText().toString()});
                emailIntent.putExtra(android.content.Intent.EXTRA_SUBJECT, subject.getText());
                emailIntent.putExtra(android.content.Intent.EXTRA_TEXT, emailtext.getText());
                SendEmailActivity.this.startActivity(Intent.createChooser(emailIntent, "Send mail..."));
                setContentView(R.layout.main);
            }
        });
    }
}