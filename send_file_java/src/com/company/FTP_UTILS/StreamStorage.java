package com.company.FTP_UTILS;

import java.io.InputStream;
import java.io.OutputStream;

public class StreamStorage {

    private InputStream inputStream = null;
    private OutputStream outputStream = null;
    private String fileName = null;

    public StreamStorage(String fileName, InputStream inputStream) {
        this.inputStream = inputStream;
        this.fileName = fileName;
    }

    public StreamStorage(String fileName, OutputStream outputStream) {
        this.outputStream = outputStream;
        this.fileName = fileName;
    }

    public OutputStream getOutputStream() {
        return this.outputStream;
    }

    public InputStream getInputStream() {
        return this.inputStream;
    }

    public String getFileName() {
        return this.fileName;
    }
}