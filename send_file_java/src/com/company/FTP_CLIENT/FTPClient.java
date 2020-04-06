package com.company.FTP_CLIENT;

import com.company.FTP_UTILS.FTP;
import com.company.FTP_UTILS.FTPPacket;

import java.io.*;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Scanner;

public class FTPClient {

    private Socket socket = null;
    private Scanner scn = null;
    private int fileTransferPort = 0;
    private String serverAddress = null;

    public FTPClient(String serverAddress, int serverPort, int fileTransferPort) throws Exception {
        this.socket = new Socket(serverAddress, serverPort);
        this.scn = new Scanner(System.in);
        this.fileTransferPort = fileTransferPort;
        this.serverAddress = serverAddress;
    }

    public void run() {
        if (this.socket == null || this.scn == null) {
            System.err.println("You can not run FTP client because your initialization failed");
        }
        System.out.println("Welcome to FTP server!");
        while (true) {
            boolean isContinue = false;
            System.out.print(">>> ");
            String commandAsString = scn.nextLine();
            if (commandAsString.equals("@logout")) {
                if (!handlerCommand_LOGOUT()) {
                    System.out.println("Disconnected but not in right way");
                } else {
                    System.out.println("Disconnect to server successfully");
                }
                break;
            }
            Command command = CommandSolver.commandSolve(commandAsString);
            switch (command.getCommand()) {
                case FTP.GET:
                    isContinue = handlerCommand_GET_PUT(command);
                    break;
                case FTP.PUT:
                    isContinue = handlerCommand_GET_PUT(command);
                    break;
                case FTP.LIST:
                    isContinue = handlerCommand_LIST(command);
                    break;
                case FTP.REMOVE:
                    isContinue = handlerCommand_REMOVE(command);
                    break;
                case FTP.LS:
                    isContinue = handlerCommand_LS();
                    break;
                case FTP.RM:
                    isContinue = handlerCommand_RM(command);
                    break;
                case FTP.UNKNOWN:
                    isContinue = true;
                    System.out.println(command.getPayload());
                    break;
            }

            if (!isContinue) {
                System.err.println("Some error occurs, disconnect from server");
                break;
            }
        }
        try {
            this.socket.close();
        } catch (IOException e) {
            System.err.println(e.getMessage());
        }
    }

    private boolean handlerCommand_GET_PUT(Command command) {
        if (command.getCommand() == FTP.PUT && !new File(command.getPayload()).exists()) {
            System.out.println("File is not exist to upload");
            return true;
        }
        if (FTP.sendPacket(this.socket, CommandSolver.commandToPacket(command))) {
            FTPPacket packet = FTP.receivePacket(this.socket);
            if (packet == null) return false;
            if (packet.getCommand() == FTP.ACCEPT) {
                //do get file
                int hashKey = Integer.parseInt(packet.getContent());
                Socket socket = null;
                try {
                    socket = new Socket(this.serverAddress, this.fileTransferPort);
                } catch (IOException e) {
                    System.err.println(e.getMessage());
                    return false;
                }
                //send file
                if (command.getCommand() == FTP.GET) {
                    return this.downloadFile(socket, hashKey, command.getPayload());
                } else {
                    return this.uploadFile(socket, hashKey, command.getPayload());
                }
            } else {
                System.out.println(command.getCommand() == FTP.GET ? "Can not download file":"Can not upload file");
                return true;
            }
        }
        return false;
    }

    private boolean handlerCommand_LIST(Command command) {
        if (FTP.sendPacket(this.socket, CommandSolver.commandToPacket(command))) {
            FTPPacket packet = FTP.receivePacket(this.socket);
            if (packet != null) {
                int responseCommand = packet.getCommand();
                if (responseCommand == FTP.REJECT) {
                    System.out.println("Server rejected your command!");
                } else if (responseCommand == FTP.ACCEPT) {
                    System.out.println("File list:");
                    System.out.println(packet.getContent());
                } else {
                    System.out.println("Unknown response from server");
                }
                return true;
            }
        }
        return false;
    }

    private boolean handlerCommand_REMOVE(Command command) {
        if (FTP.sendPacket(this.socket, CommandSolver.commandToPacket(command))) {
            FTPPacket packet = FTP.receivePacket(this.socket);
            if (packet == null) return false;
            if (packet.getCommand() == FTP.ACCEPT) {
                System.out.println("Delete file on server successfully");
            } else if (packet.getCommand() == FTP.REJECT) {
                System.out.println("Delete file on server failed");
            } else {
                System.out.println("Unknown response from server");
            }
            return true;
        }
        return false;
    }

    private boolean handlerCommand_LS() {
        File rootFolder = new File(".");

        File[] listOfFiles = rootFolder.listFiles();

        ArrayList<String> fileNames = new ArrayList<>();


        for (int i = 0; i < listOfFiles.length; i++) {
            if (listOfFiles[i].isFile())
                fileNames.add(listOfFiles[i].getName());
        }

        System.out.println(String.join("    ", fileNames));

        return true;
    }

    private boolean handlerCommand_RM(Command command) {
        if (new File(command.getPayload()).delete()) {
            System.out.println("Remove file successfully in local");
        } else {
            System.out.println("Remove file failed in local");
        }
        return true;
    }

    private boolean handlerCommand_LOGOUT() {
        return FTP.sendPacket(this.socket, new FTPPacket(FTP.LOGOUT));
    }

    private boolean downloadFile(Socket socket, int hashKey, String fileName) {
        //send hashKey
        if (FTP.sendPacket(socket, new FTPPacket(FTP.REQUEST_DOWNLOAD, Integer.toString(hashKey)))) {
            FTPPacket packet = FTP.receivePacket(socket);
            if (packet == null) return false;
            if (packet.getCommand() == FTP.ACCEPT) {
                // download file
                OutputStream fileStream = null;
                try {
                    File file = new File(fileName);
                    fileStream = new FileOutputStream(file);
                } catch (IOException e) {
                    System.err.println(e.getMessage());
                    return false;
                }
                long read = FTP.receiveFile(socket, fileStream);
                System.out.println("Downloaded " + read + " bytes");
                if (read == Long.parseLong(packet.getContent())) {
                    System.out.println("Download file successfully");
                } else {
                    System.out.println("Download file failed");
                }
                try {
                    fileStream.close();
                } catch (IOException e) {
                    System.err.println(e.getMessage());
                }
                return true;
            } else {
                System.out.println("Can not download file");
            }
        }
        return false;
    }

    private boolean uploadFile(Socket socket, int hashKey, String fileName) {
        //check if exist file
        if (FTP.sendPacket(socket, new FTPPacket(FTP.REQUEST_UPLOAD, Integer.toString(hashKey)))) {
            FTPPacket packet = FTP.receivePacket(socket);
            if (packet == null) return false;
            if (packet.getCommand() == FTP.ACCEPT) {
                // download file
                InputStream fileStream = null;
                try {
                    File file = new File(fileName);
                    fileStream = new FileInputStream(file);
                } catch (IOException e) {
                    System.err.println(e.getMessage());
                    return false;
                }
                long written = FTP.sendFile(socket, fileStream);
                System.out.println("Uploaded " + written + " bytes");
                if (written == new File(fileName).length()) {
                    System.out.println("Upload file successfully");
                } else {
                    System.out.println("Upload file failed");
                }
                try {
                    fileStream.close();
                } catch (IOException e) {
                    System.err.println(e.getMessage());
                }
                return true;
            } else {
                System.out.println("Can not upload file");
            }
        }
        return false;
    }
}


