package application;

import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.Socket;
import java.nio.IntBuffer;
import java.util.Arrays;

import javax.imageio.ImageIO;

import javafx.application.Application;
import javafx.embed.swing.SwingFXUtils;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Menu;
import javafx.scene.control.MenuBar;
import javafx.scene.control.MenuItem;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.image.PixelReader;
import javafx.scene.image.PixelWriter;
import javafx.scene.image.WritableImage;
import javafx.scene.image.WritablePixelFormat;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.scene.text.Text;
import javafx.stage.FileChooser;
import javafx.stage.Stage;

public class Main extends Application {

	static BufferedWriter outputStream = null;
	static BufferedReader inputStream = null;
	private Socket connectionToZynq;
	private DataOutputStream outputToZynq;
	private InputStream inputFromZynq;
	private FileOutputStream outputToFile;
	private File imageOutputFile;
	static long totalBytes = 0;
	private Image fetchedImage;

	public static void main(String[] args) {
		Application.launch(args);
	}

	@Override
	public void start(Stage stage) {
		VBox rootVBox = new VBox(0);
		rootVBox.setPrefWidth(1200);

		MenuBar menuBar = new MenuBar();
		Menu menuFile = new Menu("File");
		MenuItem menuItemSave = new MenuItem("Save image as...");
		MenuItem menuItemExit = new MenuItem("Exit application");

		Text titleText = new Text("Multimedijske arhitekture i sustavi - Testiranje kamere");
		titleText.setId("title");

		Text outputText = new Text();
		outputText.setId("outputText");

		ImageView imageView = new ImageView();
		imageView.setFitHeight(480);
		imageView.setFitWidth(640);
		imageView.maxHeight(480);
		imageView.maxWidth(640);
		imageView.minWidth(640);
		imageView.minHeight(480);

		imageView.setPreserveRatio(true);

		Button buttonFetchImage = new Button("Dohvati sliku");
		Button buttonSaveImage = new Button("Spremi sliku");
		Button buttonRotateImage = new Button("Rotiraj");
		Button buttonShowOriginal = new Button("Prikaži original");
		Button buttonShowGrayscale = new Button("C/B");
		Button buttonShowRedShade = new Button("R komp.slike");
		Button buttonShowBlueShade = new Button("G komp.slike");
		Button buttonShowGreenShade = new Button("B komp.slike");
		Button buttonShowBlurredImage = new Button("Blur");
		Button buttonShowMosaic = new Button("Mosaic");

		VBox bottomFetchAndTextPart = new VBox(10);
		bottomFetchAndTextPart.setPadding(new Insets(0, 20, 0, 20));
		bottomFetchAndTextPart.setPrefWidth(640);
		bottomFetchAndTextPart.setAlignment(Pos.CENTER);

		VBox buttonsLeftPart = new VBox(10);
		buttonsLeftPart.setPadding(new Insets(0, 10, 0, 10));
		buttonsLeftPart.setPrefWidth(200);
		buttonsLeftPart.setAlignment(Pos.CENTER);

		VBox buttonsRightPart = new VBox(10);
		buttonsRightPart.setPadding(new Insets(0, 10, 0, 10));
		buttonsRightPart.setPrefWidth(200);
		buttonsRightPart.setAlignment(Pos.CENTER);

		buttonRotateImage.setMinWidth(buttonsLeftPart.getPrefWidth());
		buttonRotateImage.getStyleClass().add("buttons");
		buttonRotateImage.setOnAction(new EventHandler<ActionEvent>() {
			@Override
			public void handle(ActionEvent e) {
				imageView.setRotate(imageView.getRotate() + 90);
			}
		});
		buttonShowGrayscale.getStyleClass().add("buttons");
		buttonShowGrayscale.setMinWidth(buttonsLeftPart.getPrefWidth());
		buttonShowGrayscale.setOnAction(new EventHandler<ActionEvent>() {
			@Override
			public void handle(ActionEvent event) {
				imageView.setImage(getGrayscaleImage(fetchedImage));

			}
		});
		buttonShowBlurredImage.getStyleClass().add("buttons");
		buttonShowBlurredImage.setMinWidth(buttonsLeftPart.getPrefWidth());
		buttonShowBlurredImage.setOnAction(new EventHandler<ActionEvent>() {
			@Override
			public void handle(ActionEvent event) {
				imageView.setImage(getBlurredImage(fetchedImage));

			}
		});
		buttonShowMosaic.getStyleClass().add("buttons");
		buttonShowMosaic.setMinWidth(buttonsLeftPart.getPrefWidth());
		buttonShowMosaic.setOnAction(new EventHandler<ActionEvent>() {
			@Override
			public void handle(ActionEvent event) {
				imageView.setImage(getMosaic(fetchedImage));

			}
		});
		buttonShowOriginal.getStyleClass().add("buttons");
		buttonShowOriginal.setMinWidth(buttonsLeftPart.getPrefWidth());
		buttonShowOriginal.setOnAction(new EventHandler<ActionEvent>() {
			@Override
			public void handle(ActionEvent event) {
				imageView.setImage(fetchedImage);

			}
		});
		buttonShowRedShade.getStyleClass().add("buttons");
		buttonShowRedShade.setMinWidth(buttonsLeftPart.getPrefWidth());
		buttonShowRedShade.setOnAction(new EventHandler<ActionEvent>() {
			@Override
			public void handle(ActionEvent event) {
				imageView.setImage(getRedShadeImage(fetchedImage));

			}
		});
		buttonShowGreenShade.getStyleClass().add("buttons");
		buttonShowGreenShade.setMinWidth(buttonsLeftPart.getPrefWidth());
		buttonShowGreenShade.setOnAction(new EventHandler<ActionEvent>() {
			@Override
			public void handle(ActionEvent event) {
				imageView.setImage(getGreenShadeImage(fetchedImage));

			}
		});
		buttonShowBlueShade.getStyleClass().add("buttons");
		buttonShowBlueShade.setMinWidth(buttonsLeftPart.getPrefWidth());
		buttonShowBlueShade.setOnAction(new EventHandler<ActionEvent>() {
			@Override
			public void handle(ActionEvent event) {
				imageView.setImage(getBlueShadeImage(fetchedImage));
			}
		});

		buttonSaveImage.getStyleClass().add("buttons");
		buttonSaveImage.setMinWidth(bottomFetchAndTextPart.getPrefWidth());
		buttonSaveImage.setOnAction(new EventHandler<ActionEvent>() {
			@Override
			public void handle(ActionEvent event) {
				FileChooser fileChooser = new FileChooser();
				fileChooser.getExtensionFilters().addAll(new FileChooser.ExtensionFilter("Image file (JPG)", "*.jpg"),
						new FileChooser.ExtensionFilter("All files", "*.*"));
				fileChooser.setTitle("Save Image");
				fileChooser.setInitialFileName("exported_image.jpg");
				File file = fileChooser.showSaveDialog(stage);

				if (file != null) {
					BufferedImage image = SwingFXUtils.fromFXImage(imageView.getImage(), null);

					BufferedImage imageRGB = new BufferedImage(640, 480, BufferedImage.OPAQUE);

					Graphics2D graphics = imageRGB.createGraphics();
					graphics.drawImage(image, 0, 0, null);
					if (file.getName().endsWith(".jpg")) {
						try {
							ImageIO.write(imageRGB, "jpg", file);
						} catch (IOException e) {
							e.printStackTrace();
						}
					} else {
						file = new File(file.getAbsolutePath() + ".jpg");
						try {
							ImageIO.write(imageRGB, "jpg", file);
						} catch (IOException e) {
							e.printStackTrace();
						}
					}
					graphics.dispose();
				}

			}
		});

		buttonFetchImage.getStyleClass().add("buttons");
		buttonFetchImage.setMinWidth(bottomFetchAndTextPart.getPrefWidth());
		buttonFetchImage.setOnAction(new EventHandler<ActionEvent>() {
			@Override
			public void handle(ActionEvent event) {
				try {
					connectionToZynq = new Socket("192.168.1.10", 7);
					outputToZynq = new DataOutputStream(connectionToZynq.getOutputStream());
					outputToZynq.writeByte('g');
					outputToZynq.flush();
					System.out.println("Poslao zahtjev za slikom...\n");
					imageOutputFile = new File("original_image.jpg");
					outputToFile = new FileOutputStream(imageOutputFile);
					byte[] buffer = new byte[8 * 1024];
					int bytesRead;
					inputFromZynq = connectionToZynq.getInputStream();
					while ((bytesRead = inputFromZynq.read(buffer)) != -1) {
						totalBytes += bytesRead;
						for (int i = 0; i < bytesRead; i++) {
							buffer[i] = (byte) (buffer[i] ^ 0xCA);
						}
						outputToFile.write(buffer, 0, bytesRead);
					}

					System.out.println("Dohvaćena slika sa PYNQ-a");
					System.out.println("Veličina primljene slike:" + totalBytes / 1024 + "kB");
					outputText.setText(
							"Dohvaćena slika sa PYNQ-a\nVeličina primljene slike:" + totalBytes / 1024 + "KB\n");
					fetchedImage = new Image(imageOutputFile.toURI().toString());
					imageView.setImage(fetchedImage);
					inputFromZynq.close();
					outputToFile.close();
					outputToZynq.close();
					connectionToZynq.close();
					buttonsLeftPart.getChildren().addAll(buttonShowOriginal, buttonShowRedShade, buttonShowBlueShade,
							buttonShowGreenShade);
					buttonsRightPart.getChildren().addAll(buttonRotateImage, buttonShowGrayscale,
							buttonShowBlurredImage, buttonShowMosaic);
					menuFile.getItems().removeAll(menuItemExit);
					menuFile.getItems().addAll(menuItemSave, menuItemExit);
					bottomFetchAndTextPart.getChildren().removeAll(buttonFetchImage, buttonSaveImage, outputText);
					bottomFetchAndTextPart.getChildren().addAll(buttonFetchImage, buttonSaveImage, outputText);
				} catch (IOException e) {
					imageView.setImage(new Image(new File("res/test.jpg").toURI().toString()));
					e.printStackTrace();
					bottomFetchAndTextPart.getChildren().removeAll(buttonFetchImage, buttonSaveImage, outputText);
					bottomFetchAndTextPart.getChildren().addAll(buttonFetchImage, outputText);
					outputText.setText("Dogodila se pogreška prilikom čitanja s PYNQ-a.");
					try {
						inputFromZynq.close();
						outputToFile.close();
						outputToZynq.close();
						connectionToZynq.close();
					} catch (IOException e1) {
						e1.printStackTrace();
					}
				}

			}
		});

		bottomFetchAndTextPart.getChildren().addAll(buttonFetchImage);

		BorderPane.setAlignment(titleText, Pos.TOP_CENTER);
		BorderPane.setAlignment(buttonsLeftPart, Pos.CENTER_LEFT);
		BorderPane.setAlignment(bottomFetchAndTextPart, Pos.BOTTOM_CENTER);
		BorderPane.setAlignment(buttonsRightPart, Pos.CENTER_RIGHT);

		BorderPane borderPaneMain = new BorderPane(imageView, titleText, buttonsRightPart, bottomFetchAndTextPart,
				buttonsLeftPart);

		borderPaneMain.setId("root");
		borderPaneMain.setPrefSize(1200, 800);

		menuFile.getItems().addAll(menuItemExit);
		menuBar.getMenus().addAll(menuFile);

		menuItemExit.setOnAction(new EventHandler<ActionEvent>() {
			@Override
			public void handle(ActionEvent event) {
				stage.close();
			}
		});
		menuItemSave.setOnAction(new EventHandler<ActionEvent>() {
			@Override
			public void handle(ActionEvent event) {
				FileChooser fileChooser = new FileChooser();
				fileChooser.getExtensionFilters().addAll(new FileChooser.ExtensionFilter("Image file (JPG)", "*.jpg"),
						new FileChooser.ExtensionFilter("All files", "*.*"));
				fileChooser.setTitle("Save Image");
				fileChooser.setInitialFileName("edited_image.jpg");
				File file = fileChooser.showSaveDialog(stage);

				if (file != null) {
					BufferedImage image = SwingFXUtils.fromFXImage(imageView.getImage(), null);

					BufferedImage imageRGB = new BufferedImage(640, 480, BufferedImage.OPAQUE);

					Graphics2D graphics = imageRGB.createGraphics();
					graphics.drawImage(image, 0, 0, null);
					if (file.getName().endsWith(".jpg")) {
						try {
							ImageIO.write(imageRGB, "jpg", file);
						} catch (IOException e) {
							e.printStackTrace();
						}
					} else {
						file = new File(file.getAbsolutePath() + ".jpg");
						try {
							ImageIO.write(imageRGB, "jpg", file);
						} catch (IOException e) {
							e.printStackTrace();
						}
					}
					graphics.dispose();
				}

			}
		});

		rootVBox.getChildren().addAll(menuBar, borderPaneMain);
		Scene scene = new Scene(rootVBox);
		scene.getStylesheets().add(Main.class.getResource("application.css").toExternalForm());
		stage.setScene(scene);
		stage.setMinHeight(800);
		stage.setMinWidth(1200);
		stage.setTitle("Multimedijske arhitekture i sustavi - Testiranje kamere");
		stage.show();
	}

	private WritableImage getGrayscaleImage(Image src) {
		int width = (int) src.getWidth();
		int height = (int) src.getHeight();
		WritableImage dest = new WritableImage(width, height);
		PixelReader reader = src.getPixelReader();
		PixelWriter writer = dest.getPixelWriter();
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				int pixel = reader.getArgb(x, y);
				int alpha = ((pixel >> 24) & 0xff);
				int red = ((pixel >> 16) & 0xff);
				int green = ((pixel >> 8) & 0xff);
				int blue = (pixel & 0xff);
				int avg = (int) (red + green + blue) / 3;

				pixel = (alpha << 24) | (avg << 16) | (avg << 8) | avg;
				writer.setArgb(x, y, pixel);
			}
		}
		return dest;
	}

	private WritableImage getRedShadeImage(Image src) {
		int width = (int) src.getWidth();
		int height = (int) src.getHeight();
		WritableImage colorShadeImage = new WritableImage(width, height);
		PixelReader pixelReader = src.getPixelReader();
		PixelWriter pixelWriter = colorShadeImage.getPixelWriter();
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				Color pixel = pixelReader.getColor(x, y);
				Color newPixel = new Color(pixel.getRed(), 0, 0, pixel.getOpacity());
				pixelWriter.setColor(x, y, newPixel);
			}
		}
		return colorShadeImage;
	}

	private WritableImage getBlueShadeImage(Image src) {
		int width = (int) src.getWidth();
		int height = (int) src.getHeight();
		WritableImage colorShadeImage = new WritableImage(width, height);
		PixelReader pixelReader = src.getPixelReader();
		PixelWriter pixelWriter = colorShadeImage.getPixelWriter();
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				Color pixel = pixelReader.getColor(x, y);
				Color newPixel = new Color(0, pixel.getBlue(), 0, pixel.getOpacity());
				pixelWriter.setColor(x, y, newPixel);
			}
		}
		return colorShadeImage;
	}

	private WritableImage getGreenShadeImage(Image src) {
		int width = (int) src.getWidth();
		int height = (int) src.getHeight();
		WritableImage colorShadeImage = new WritableImage(width, height);
		PixelReader pixelReader = src.getPixelReader();
		PixelWriter pixelWriter = colorShadeImage.getPixelWriter();
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				Color pixel = pixelReader.getColor(x, y);
				Color newPixel = new Color(0, 0, pixel.getGreen(), pixel.getOpacity());
				pixelWriter.setColor(x, y, newPixel);
			}
		}
		return colorShadeImage;
	}

	private WritableImage getBlurredImage(Image src) {
		int width = (int) src.getWidth();
		int height = (int) src.getHeight();
		WritableImage colorShadeImage = new WritableImage(width, height);
		PixelReader reader = src.getPixelReader();
		PixelWriter writer = colorShadeImage.getPixelWriter();
		WritablePixelFormat<IntBuffer> format = WritablePixelFormat.getIntArgbInstance();

		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				int centerX = x - 5;
				int centerY = y - 5;
				int kernelWidth = 5 * 2 + 1;
				int kernelHeight = 5 * 2 + 1;

				if (centerX < 0) {
					centerX = 0;
					kernelWidth = x + 5;
				} else if (x + 5 >= width) {
					kernelWidth = width - centerX;
				}

				if (centerY < 0) {
					centerY = 0;
					kernelHeight = y + 5;
				} else if (y + 5 >= height) {
					kernelHeight = height - centerY;
				}

				int[] buffer = new int[kernelWidth * kernelHeight];
				reader.getPixels(centerX, centerY, kernelWidth, kernelHeight, format, buffer, 0, kernelWidth);

				int alpha = 0;
				int red = 0;
				int green = 0;
				int blue = 0;

				for (int color : buffer) {
					alpha += (color >>> 24);
					red += (color >>> 16 & 0xFF);
					green += (color >>> 8 & 0xFF);
					blue += (color & 0xFF);
				}
				alpha = alpha / kernelWidth / kernelHeight;
				red = red / kernelWidth / kernelHeight;
				green = green / kernelWidth / kernelHeight;
				blue = blue / kernelWidth / kernelHeight;

				int blurColor = (alpha << 24) + (red << 16) + (green << 8) + blue;
				writer.setArgb(x, y, blurColor);
			}
		}
		return colorShadeImage;
	}

	private WritableImage getMosaic(Image src) {
		int width = (int) src.getWidth();
		int height = (int) src.getHeight();
		WritableImage colorShadeImage = new WritableImage(width, height);
		PixelReader reader = src.getPixelReader();
		PixelWriter writer = colorShadeImage.getPixelWriter();
		WritablePixelFormat<IntBuffer> format = WritablePixelFormat.getIntArgbInstance();

		for (int x = 5; x < width - 5 * 2; x += 5 * 2 + 1) {
			for (int y = 5; y < height - 5 * 2; y += 5 * 2 + 1) {
				int kernelWidth = 5 * 2 + 1;
				int kernelHeight = 5 * 2 + 1;

				int[] buffer = new int[kernelWidth * kernelHeight];
				reader.getPixels(x, y, kernelWidth, kernelHeight, format, buffer, 0, kernelWidth);

				int alpha = 0;
				int red = 0;
				int green = 0;
				int blue = 0;

				for (int color : buffer) {
					alpha += (color >>> 24);
					red += (color >>> 16 & 0xFF);
					green += (color >>> 8 & 0xFF);
					blue += (color & 0xFF);
				}
				alpha = alpha / kernelWidth / kernelHeight;
				red = red / kernelWidth / kernelHeight;
				green = green / kernelWidth / kernelHeight;
				blue = blue / kernelWidth / kernelHeight;

				int blurColor = (alpha << 24) + (red << 16) + (green << 8) + blue;
				Arrays.fill(buffer, blurColor);
				writer.setPixels(x, y, kernelWidth, kernelHeight, format, buffer, 0, kernelWidth);
			}
		}
		return colorShadeImage;
	}

}