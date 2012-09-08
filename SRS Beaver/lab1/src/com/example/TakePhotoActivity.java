package com.example;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.widget.ImageView;

/**
 * Created with IntelliJ IDEA.
 * User: Lloyd
 * Date: 8/31/12
 * Time: 11:31 PM
 * To change this template use File | Settings | File Templates.
 */
public class TakePhotoActivity extends Activity
{
    private static final int CAMERA_PIC_REQUEST = 1337;

    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.camera);
        Intent cameraIntent = new Intent(android.provider.MediaStore.ACTION_IMAGE_CAPTURE);
        startActivityForResult(cameraIntent, CAMERA_PIC_REQUEST);
    }

    protected void onActivityResult(int requestCode, int resultCode, Intent data)
    {
        if (requestCode == CAMERA_PIC_REQUEST)
        {
            Bitmap thumbnail = (Bitmap) data.getExtras().get("data");
            ImageView image = (ImageView) findViewById(R.id.DisplayedImage);
            image.setImageBitmap(thumbnail);
        }
    }
}