package com.ntu.zc.adbservershow;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.widget.ImageView;
import android.widget.Toast;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.ServerSocket;
import java.net.Socket;

public class MainActivity extends Activity {

    private final int SERVER_PORT = 9000;
    private ImageView imageView;
    public byte imageByte[];
    public int imageSize = 640*480*3;

    boolean flag =true;
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        imageView = (ImageView)findViewById(R.id.imageView);

        new Thread() {
            public void run() {
                startServer();
            }
        }.start();
    }

    private void startServer() {
        try {
            ServerSocket serverSocket = new ServerSocket(SERVER_PORT);

            while (true) {
                final Socket client = serverSocket.accept();
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        try {
                            Log.e("hehe", "Someone come:");
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    Toast.makeText(MainActivity.this, "Someone Connected", 0).show();
                                }
                            });

                            InputStream in = client.getInputStream();

                            while(true) {
                                ByteArrayOutputStream baos = new ByteArrayOutputStream();
                                byte buffer[] = new byte[1024];
                                int remainingBytes = imageSize;

                                while (remainingBytes > 0) {
                                    int bytesRead = in.read(buffer);

                                    if (bytesRead < 0) {
                                        throw new IOException("Unexpected end of data");
                                    }
                                    baos.write(buffer, 0, bytesRead);
                                    remainingBytes -= bytesRead;
                                }

                                //in.close();
                                imageByte = baos.toByteArray();
                                //baos.close();

                                int nrOfPixels = 640*480;
                                int pixels[] = new int[nrOfPixels];
                                for (int i = 0; i < nrOfPixels; i++) {
                                    int r = imageByte[3 * i];
                                    int g = imageByte[3 * i + 1];
                                    int b = imageByte[3 * i + 2];

                                    if (r < 0) r = r + 256;
                                    if (g < 0) g = g + 256;
                                    if (b < 0) b = b + 256;
                                    pixels[i] = Color.rgb(b, g, r);
                                }
                           /*     runOnUiThread(new Runnable() {
                                    @Override
                                    public void run() {
                                        Toast.makeText(MainActivity.this, "Received!!", 1).show();
                                    }
                                });*/
                                final Bitmap bitmap = Bitmap.createBitmap(pixels, 640, 480, Bitmap.Config.ARGB_8888);
                                runOnUiThread(new Runnable() {
                                    @Override
                                    public void run() {
                                        imageView.setImageBitmap(bitmap);
                                    }
                                });


                            }
                        } catch (Exception e) {
                            e.printStackTrace();
                        } finally {
                            try {
                                client.close();
                            } catch (IOException e) {
                                e.printStackTrace();
                            }
                        }
                        try {
                            Thread.sleep(500);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                    }
                }).start();

            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    protected void onDestroy() {
        flag = false;
    };
}