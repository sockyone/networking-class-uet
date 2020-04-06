package com.company.FTP_SERVER;

import com.company.FTP_SERVER.ConnectionHandler;
import com.company.FTP_SERVER.FileTransferServer;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

//main thread
//main server
//control server
public class FTPServer {
    private ServerSocket serverSocket;
    private int fileTransferPort;
    private int controlPort;

    public static String sharedFolder = "SharedFolder/";

    public FTPServer(int controlPort, int fileTransferPort) throws Exception {
        this.serverSocket = new ServerSocket(controlPort);
        this.fileTransferPort = fileTransferPort;
        this.controlPort = controlPort;
    }

    public void run() {
        //init transfer file thread
        System.out.println("FTP server start listening on port " + this.controlPort);

        FileTransferServer fileTransferServer = new FileTransferServer(fileTransferPort);
        new Thread(fileTransferServer).start();

        System.out.println("FTP file transfer port: " + this.fileTransferPort);

        while (true) {
            Socket socket = null;
            try {
                socket = this.serverSocket.accept();
            } catch (IOException e) {
                System.out.println("Can not accept connection");
                continue;
            }
            System.out.println("Accept connection from: " + socket.getInetAddress().toString() + ":" + socket.getPort());
            new Thread(new ConnectionHandler(socket)).start();
        }
    }
}