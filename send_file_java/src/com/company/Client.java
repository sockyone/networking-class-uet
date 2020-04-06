package com.company;

import com.company.FTP_CLIENT.FTPClient;

import java.util.Scanner;


public class Client {

    private static final int SERVER_CONTROL_PORT = 3000;
    private static final int SERVER_FILE_TRANSFER_PORT = 3001;

    public static void main(String[] args) {
        FTPClient client = null;
        System.out.print("Insert FTP host address: ");
        String serverAddress = new Scanner(System.in).nextLine();
        try {
            client = new FTPClient(serverAddress, SERVER_CONTROL_PORT, SERVER_FILE_TRANSFER_PORT);
        } catch (Exception e) {
            System.err.println(e.getMessage());
            return;
        }
        client.run();
    }
}
