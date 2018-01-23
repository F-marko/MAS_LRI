package test;

import java.awt.Rectangle;
import java.awt.Robot;
import java.awt.Toolkit;
import java.awt.image.BufferedImage;
import java.net.ServerSocket;
import java.net.Socket;

import javax.imageio.ImageIO;

public class EchoServer {
	public static void main(String[] args) throws Exception {
		BufferedImage screencapture = new Robot()
				.createScreenCapture(new Rectangle(Toolkit.getDefaultToolkit().getScreenSize()));
		try (ServerSocket serv = new ServerSocket(25000)) {
			System.out.println("waiting...");
			try (Socket socket = serv.accept()) {
				System.out.println("client connected");
				ImageIO.write(screencapture, "jpg", socket.getOutputStream());
				System.out.println("sent");
			}
		}
	}
}