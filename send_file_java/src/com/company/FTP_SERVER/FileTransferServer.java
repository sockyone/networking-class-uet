package com.company.FTP_SERVER;

import com.company.FTP_UTILS.FTP;
import com.company.FTP_UTILS.FTPPacket;
import com.company.FTP_UTILS.FTPService;
import com.company.FTP_UTILS.StreamStorage;

import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;


//transfer data server
public class FileTransferServer implements Runnable {

    ServerSocket serverSocket = null;
    int port;

    public FileTransferServer(int port) {
        this.port = port;
    }

    private class HandleFileTransferConnection implements Runnable {
        Socket socket;

        public HandleFileTransferConnection(Socket socket) {
            this.socket = socket;
        }

        @Override
        public void run() {
            FTPPacket packet = FTP.receivePacket(this.socket);
            if (packet != null) {
                //main handle
                StreamStorage streamStorage = FTPService.getInstance().getFromWaitingList(Integer.parseInt(packet.getContent()));
                if (streamStorage == null) {
                    //not exists
                    FTP.sendPacket(this.socket, new FTPPacket(FTP.REJECT));
                } else {
                    if (packet.getCommand() == FTP.REQUEST_DOWNLOAD) {
                        // do download
                        InputStream fileStream = streamStorage.getInputStream();
                        if (fileStream == null) {
                            FTP.sendPacket(this.socket, new FTPPacket(FTP.REJECT));
                        } else {
                            //start sending file
                            //Long.toString(new File(streamStorage.getFileName()).length()
                            long fileLength = ContentProvider.getInstance().getFileSize(streamStorage.getFileName());
                            if (FTP.sendPacket(this.socket, new FTPPacket(FTP.ACCEPT,
                                    Long.toString(fileLength)))) {
                                FTP.sendFile(socket, fileStream);
                            }
                            try {
                                fileStream.close();
                            } catch (IOException e) {
                                System.err.println(e.getMessage());
                            }
                            ContentProvider.getInstance().stopReading(streamStorage.getFileName());
                        }
                    } else if (packet.getCommand() == FTP.REQUEST_UPLOAD) {
                        // do upload
                        OutputStream fileStream = streamStorage.getOutputStream();
                        if (fileStream == null) {
                            FTP.sendPacket(this.socket, new FTPPacket(FTP.REJECT));
                        } else {
                            if (FTP.sendPacket(this.socket, new FTPPacket(FTP.ACCEPT))) {
                                FTP.receiveFile(socket, fileStream);
                            }
                            try {
                                fileStream.close();
                            } catch (IOException e) {
                                System.err.println(e.getMessage());
                            }
                            ContentProvider.getInstance().stopWriting(streamStorage.getFileName());
                        }
                    }
                }
            }

            if (!this.socket.isClosed()) {
                try {
                    this.socket.close();
                } catch (IOException e) {
                    System.err.println(e.getMessage());
                }
            }

            System.out.println("---------------------------");
            System.out.println("Uploaded: " + ContentProvider.getInstance().getUploadedNum());
            System.out.println("Downloaded: " + ContentProvider.getInstance().getDownloadedNum());
            System.out.println("---------------------------");
        }
    }

    @Override
    public void run() {
        //try to open server connection
        try {
            this.serverSocket = new ServerSocket(this.port);
        } catch (Exception e) {
            System.err.println(e.getMessage());
            return;
        }
        while (true) {
            Socket socket = null;
            try {
                socket = this.serverSocket.accept();
            } catch (IOException e) {
                System.out.println("Can not accept connection");
                continue;
            }
            new Thread(new HandleFileTransferConnection(socket)).start();
        }
    }
}
