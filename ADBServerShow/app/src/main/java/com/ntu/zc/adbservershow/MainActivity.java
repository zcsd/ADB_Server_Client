package com.ntu.zc.adbservershow;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;

public class MainActivity extends Activity {

    private final int SERVER_PORT = 9000;
    private TextView textView;
    private String content = "";

    boolean flag =true;
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        textView = (TextView)findViewById(R.id.tv);

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
                            Log.e("hehheh", "Someone come:");
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    Toast.makeText(MainActivity.this, "Someone Connected", 0).show();
                                }
                            });

                            BufferedReader in = new BufferedReader(new InputStreamReader(client.getInputStream(),"GB2312"));
                            String str;

                            while((str = in.readLine()) != null){
                                System.out.println(str);
                                content = str+"\n";
                                runOnUiThread(new Runnable() {
                                    @Override
                                    public void run() {
                                        Toast.makeText(MainActivity.this, content, 1).show();
                                    }
                                });
                            }

                        } catch (Exception e) {
                            e.printStackTrace();
                        } finally {
                            try {
                                client.close();
                            } catch (IOException e) {
                                // TODO Auto-generated catch block
                                e.printStackTrace();
                            }
                        }
                        try {
                            Thread.sleep(500);
                        } catch (InterruptedException e) {
                            // TODO Auto-generated catch block
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