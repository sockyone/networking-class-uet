package com.company;


import com.company.FTP_SERVER.FTPServer;

public class Server {

    private static int CONTROL_THREAD_PORT = 3000;
    private static int FILE_TRANSFER_THREAD_PORT = 3001;

    public static void main(String[] args) {
        FTPServer ftpServer = null;
        try {
            ftpServer = new FTPServer(CONTROL_THREAD_PORT, FILE_TRANSFER_THREAD_PORT);
        } catch (Exception e) {
            System.err.println(e.getMessage());
            return;
        }
        ftpServer.run();
    }
}
