package com.company.FTP_UTILS;


import java.io.*;
import java.net.Socket;

//final config
public final class FTP {
    //online command
    public static final int UNKNOWN = 0;
    public static final int GET = 1;
    public static final int PUT = 2;
    public static final int LIST = 3;
    public static final int REMOVE = 4;
    public static final int REQUEST_DOWNLOAD = 5;
    public static final int REQUEST_UPLOAD = 6;
    public static final int ACCEPT = 7;
    public static final int REJECT = 8;
    public static final int LOGOUT = 9;

    public static final int LS = 11;
    public static final int RM = 12;

    private static final int FTP_BUFFER_SIZE = 8192;


    public static boolean sendPacket(Socket socket, FTPPacket packet) {
        try {
            DataOutputStream output = new DataOutputStream(socket.getOutputStream());

            //send command first
            output.writeInt(packet.getCommand());

            //count length of content
            String contentAsString = packet.getContent();
            byte[] contentAsBytes = contentAsString.getBytes();

            int lengthOfContent = contentAsBytes.length;

            //send length of content
            output.writeInt(lengthOfContent);

            //send content
            output.write(contentAsBytes);
        } catch (IOException e) {
            System.err.println(e.getMessage());
            return false;
        }
        return true;
    }

    public static FTPPacket receivePacket(Socket socket) {
        try {
            DataInputStream input = new DataInputStream(socket.getInputStream());

            //receive command first
            int commandOfPacket = input.readInt();

            //receive length of content
            int lengthOfContent = input.readInt();

            //receive content
            byte[] bytes = new byte[lengthOfContent];

            //read
            input.readFully(bytes);

            String packetContent = new String(bytes);

            return new FTPPacket(commandOfPacket, packetContent);

        } catch (IOException e) {
            System.err.println(e.getMessage());
            return null;
        }
    }

    public static long sendFile(Socket socket, InputStream fileStream) {
        long written = 0;
        byte[] bytes = new byte[FTP_BUFFER_SIZE];
        try {
            BufferedOutputStream output = new BufferedOutputStream(socket.getOutputStream());
            int count;
            while ((count = fileStream.read(bytes)) > 0) {
                output.write(bytes, 0, count);
                written += count;
                output.flush();
            }
        } catch (IOException e) {
            System.err.println(e.getMessage());
            return written;
        }
        try {
            socket.close();
        }catch (IOException e) {
            System.err.println(e.getMessage());
        }
        return written;
    }

    public static long receiveFile(Socket socket, OutputStream fileStream) {
        long read = 0;
        byte[] bytes = new byte[FTP_BUFFER_SIZE];
        try {
            BufferedInputStream input = new BufferedInputStream(socket.getInputStream());
            int count;
            while ((count = input.read(bytes)) > 0) {
                fileStream.write(bytes, 0, count);
                read += count;
            }
        } catch (IOException e) {
            System.err.println(e.getMessage());
            return read;
        }
        try {
            socket.close();
        }catch (IOException e) {
            System.err.println(e.getMessage());
        }
        return read;
    }
}
