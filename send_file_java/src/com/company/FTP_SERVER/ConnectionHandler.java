package com.company.FTP_SERVER;

import com.company.FTP_UTILS.FTP;
import com.company.FTP_UTILS.FTPPacket;
import com.company.FTP_UTILS.FTPService;
import com.company.FTP_UTILS.StreamStorage;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.util.ArrayList;

public class ConnectionHandler implements Runnable {

    Socket socket;

    public ConnectionHandler(Socket socket) {
        this.socket = socket;
    }

    @Override
    public void run() {
        while (true) {
            FTPPacket packet = FTP.receivePacket(socket);
            if (packet == null) {
                //SOMETHING WRONG
                try {
                    this.socket.close();
                    break;
                } catch (IOException e) {
                    System.err.println(e.getMessage());
                    break;
                }
            } else {
                boolean isContinue = false;
                // everything ok
                switch (packet.getCommand()) {
                    case FTP.LIST:
                        isContinue = handler_LIST();
                        break;
                    case FTP.GET:
                        isContinue = handler_GET(packet);
                        break;
                    case FTP.PUT:
                        isContinue = handler_PUT(packet);
                        break;
                    case FTP.REMOVE:
                        isContinue = handler_REMOVE(packet);
                        break;
                    case FTP.LOGOUT:
                        isContinue = false;
                        System.out.println("Client disconnected: " + this.socket.getInetAddress() + ":" + this.socket.getPort());
                        break;
                    default:
                        isContinue = handler_UNKNOWN();
                        break;
                }
                if (!isContinue) break;
            }
        }

        //clean socket connection at last
        if (!this.socket.isClosed()) {
            try {
                this.socket.close();
            } catch (IOException e) {
                System.err.println(e.getMessage());
            }
        }
    }

    private boolean handler_LIST() {
        ArrayList<String> listFileNames = ContentProvider.getInstance().getListFile();
        String listFileAsString = String.join("    ", listFileNames);

        return FTP.sendPacket(this.socket, new FTPPacket(FTP.ACCEPT, listFileAsString));
    }

    private boolean handler_PUT(FTPPacket packet) {
        OutputStream stream = ContentProvider.getInstance().getOutputStream(packet.getContent());
        if (stream == null) {
            return FTP.sendPacket(this.socket, new FTPPacket(FTP.REJECT));
        }
        int hashKey = FTPService.getInstance().putIntoWaitingList(new StreamStorage(packet.getContent(), stream));
        return FTP.sendPacket(this.socket, new FTPPacket(FTP.ACCEPT, Integer.toString(hashKey)));
    }

    private boolean handler_GET(FTPPacket packet) {
        InputStream stream = ContentProvider.getInstance().getInputStream(packet.getContent());
        //I decided to revert code, avoid annoying of my IDE for duplicate code
        if (stream != null) {
            int hashKey = FTPService.getInstance().putIntoWaitingList(new StreamStorage(packet.getContent(), stream));
            return FTP.sendPacket(this.socket, new FTPPacket(FTP.ACCEPT, Integer.toString(hashKey)));
        }
        return FTP.sendPacket(this.socket, new FTPPacket(FTP.REJECT));
    }

    private boolean handler_REMOVE(FTPPacket packet) {
        if (ContentProvider.getInstance().removeFile(packet.getContent())) {
            return FTP.sendPacket(this.socket, new FTPPacket(FTP.ACCEPT));
        } else return FTP.sendPacket(this.socket, new FTPPacket(FTP.REJECT));
    }

    private boolean handler_UNKNOWN() {
        return FTP.sendPacket(this.socket, new FTPPacket(FTP.UNKNOWN));
    }

}
