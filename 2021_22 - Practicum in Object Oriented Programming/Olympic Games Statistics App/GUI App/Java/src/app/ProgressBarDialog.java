package app;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;


public class ProgressBarDialog extends JDialog {

	// ==========================================
	// Attributes:


	// height and width of the window and the progress bar
	private final int height;
	private final int width;

	public JProgressBar progress_bar = new JProgressBar(0, 100);


	// ==========================================
	// Constructor:


	public ProgressBarDialog( final Window parent,
							  final int width,
							  final int height, 
							  String title )
	{
		super (
			parent,
			title,
			Dialog.ModalityType.DOCUMENT_MODAL
		);

		this.height = height;
		this.width = width;

		this.setBounds(
			parent.getLocationOnScreen().x + parent.getWidth() / 2 - width / 2,
			parent.getLocationOnScreen().y + parent.getHeight() / 2 - height / 2,
			width,
			height
		);

		// remove the menu bar of the dialog window
		this.setUndecorated(true);

		this.getRootPane().registerKeyboardAction(
			(ae) -> { App.exit(parent); },
			KeyStroke.getKeyStroke(KeyEvent.VK_Q, KeyEvent.CTRL_DOWN_MASK, true),
			JComponent.WHEN_IN_FOCUSED_WINDOW
		);
		this.populate();
	}


	// ==========================================
	// Constructor Helpers:


	private void populate() {
		// Progress Bar
		this.progress_bar.setValue(0);
		this.progress_bar.setPreferredSize(
			new Dimension(this.width, this.height)
		);
		// add percentages text
		this.progress_bar.setStringPainted(true);
		this.progress_bar.setFont(new Font("Monospace", Font.BOLD, 23));
		this.progress_bar.setForeground(Color.green);	
		this.progress_bar.setBackground(Color.black);
		this.add(this.progress_bar);
	}

}
