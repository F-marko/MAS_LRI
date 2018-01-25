package application;

import java.awt.Desktop;
import java.io.File;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.application.Application;
import javafx.embed.swing.SwingFXUtils;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.Menu;
import javafx.scene.control.MenuBar;
import javafx.scene.control.MenuItem;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.StackPane;
import javafx.scene.layout.VBox;
import javafx.stage.FileChooser;
import javafx.stage.Stage;
import javax.imageio.ImageIO;

/** * * @web http://java-buddy.blogspot.com */
public class Test extends Application {
	@Override
	public void start(Stage primaryStage) {
		final FileChooser fileChooser = new FileChooser();
		final Button openButton = new Button("Open Image");
		openButton.setOnAction(new EventHandler<ActionEvent>() {
			@Override
			public void handle(final ActionEvent e) {
				setExtFilters(fileChooser);
				File file = fileChooser.showOpenDialog(primaryStage);
				if (file != null) {
					openNewImageWindow(file);
				}
			}
		});
		StackPane root = new StackPane();
		root.getChildren().add(openButton);
		Scene scene = new Scene(root, 400, 150);
		primaryStage.setTitle("java-buddy.blogspot.com");
		primaryStage.setScene(scene);
		primaryStage.show();
	}

	public static void main(String[] args) {
		launch(args);
	}

	private void setExtFilters(FileChooser chooser) {
		chooser.getExtensionFilters().addAll(new FileChooser.ExtensionFilter("All Images", "*.*"),
				new FileChooser.ExtensionFilter("PNG", "*.png"));
	}

	private void openNewImageWindow(File file) {
		Stage secondStage = new Stage();
		MenuBar menuBar = new MenuBar();
		Menu menuFile = new Menu("File");
		MenuItem menuItem_Save = new MenuItem("Save Image");
		menuFile.getItems().addAll(menuItem_Save);
		menuBar.getMenus().addAll(menuFile);
		
		
		
		Label name = new Label(file.getAbsolutePath());
		Image image = new Image(file.toURI().toString());
		ImageView imageView = new ImageView();
		menuItem_Save.setOnAction(new EventHandler<ActionEvent>() {
			@Override
			public void handle(ActionEvent event) {
				FileChooser fileChooser = new FileChooser();
				fileChooser.setTitle("Save Image");
				File file = fileChooser.showSaveDialog(secondStage);
				if (file != null) {
					try {
						ImageIO.write(SwingFXUtils.fromFXImage(imageView.getImage(), null), "png", file);
					} catch (IOException ex) {
						Logger.getLogger(Test.class.getName()).log(Level.SEVERE, null, ex);
					}
				}
			}
		});
		final VBox vbox = new VBox();
		vbox.setAlignment(Pos.CENTER);
		vbox.setSpacing(10);
		vbox.setPadding(new Insets(0, 10, 0, 10));
		vbox.getChildren().addAll(name, imageView);
		imageView.setFitHeight(400);
		imageView.setPreserveRatio(true);
		imageView.setImage(image);
		imageView.setSmooth(true);
		imageView.setCache(true);
		Scene scene = new Scene(new VBox(), 400, 350);
		((VBox) scene.getRoot()).getChildren().addAll(menuBar, vbox);
		secondStage.setTitle(file.getName());
		secondStage.setScene(scene);
		secondStage.show();
	}
}