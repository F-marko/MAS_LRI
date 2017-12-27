package test;

import java.awt.Color;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.image.BufferedImage;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.net.Socket;

import javax.imageio.ImageIO;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JTextField;

public class MasCameraMain {
	/**
	 * Create the GUI and show it. For thread safety, this method should be invoked
	 * from the event-dispatching thread.
	 */
	static BufferedImage image;
	static JLabel imagePanel = null;
	static JTextField instructionText;
	static Socket inputSocket = null;
	static BufferedWriter outputStream = null;
	static BufferedReader inputStream = null;

	private static void createAndShowGUI() {
		// Create and set up the window.
		JFrame frame = new JFrame("Multimedijske arhitekture i sustavi - Testiranje kamere");
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setMinimumSize(new Dimension(680, 480));
		// Set up the content pane.
		Container contentPane = frame.getContentPane();
		GridBagLayout outerLayout = new GridBagLayout();
		contentPane.setLayout(outerLayout);

		// Create and add the components.
		imagePanel = new JLabel();
		imagePanel.setPreferredSize(new Dimension(640, 360));
		imagePanel.setBackground(Color.LIGHT_GRAY);
		imagePanel.setOpaque(true);

		JButton getImageButton = new JButton("Fetch image");
		getImageButton.setFocusPainted(false);
		getImageButton.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent event) {
				/*
				 * Ovdje bi trebalo promijenit adresu socketa,i to radi s ovim echo serverom u
				 * sklopu ovog paketa. Mozda ne bude radilo s nasim serverom, postoji opcija
				 * promijenit ovaj ImageIO u read bytes ili nesto tako, pa cemo promijenit ako
				 * bude potrebno.
				 * 
				 */
				try {
					Socket inputSocket = new Socket("localhost", 25000);
					image = ImageIO.read(inputSocket.getInputStream());
					imagePanel.setIcon(new ImageIcon(image));
					inputSocket.close();
				} catch (IOException e) {
					imagePanel.setIcon(new ImageIcon("res/test.png"));
					// e.printStackTrace();
				}
			}
		});
		instructionText = new JTextField("Press button to fetch image");
		instructionText.setFont(instructionText.getFont().deriveFont(20f));
		instructionText.setForeground(Color.BLACK);
		instructionText.setHorizontalAlignment(JTextField.CENTER);
		instructionText.setEditable(false);
		instructionText.setBorder(null);

		GridBagConstraints bagConstraints = new GridBagConstraints();
		bagConstraints.fill = GridBagConstraints.HORIZONTAL;
		bagConstraints.insets = new Insets(5, 0, 5, 0);
		bagConstraints.gridx = 0;
		bagConstraints.gridy = 0;
		contentPane.add(instructionText, bagConstraints);
		bagConstraints.gridy = 1;
		contentPane.add(imagePanel, bagConstraints);
		bagConstraints.gridy = 2;
		bagConstraints.insets = new Insets(5, 0, 5, 0);
		contentPane.add(getImageButton, bagConstraints);

		// Display the window.
		frame.setLocationRelativeTo(null);
		frame.pack();
		frame.setVisible(true);
	}

	public static void main(String[] args) {
		// Schedule a job for the event-dispatching thread:
		// creating and showing this application's GUI.
		javax.swing.SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				createAndShowGUI();
			}
		});
	}
}