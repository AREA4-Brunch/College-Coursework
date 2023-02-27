package gui;


import javax.swing.JFrame;
import javax.swing.JPanel;
import java.awt.Dimension;
import java.awt.BorderLayout;
import java.awt.Window;


public class GraphFrame extends JFrame {


	// ==========================================
	// Attributes:


    private JPanel wrapper = new JPanel(new BorderLayout());
    private Graph graph;


    // ==========================================
    // Constructor and Helpers:


    public GraphFrame(Window parent, String title, final Graph graph) {
        super(title);
        this.graph = graph;

        this.addWindowListener(app.App.exitWindow);
        this.setResizable(false);
        this.setLayout(new BorderLayout());

        final int margin = 330;
        final int width = graph.getExpectedWidth() + margin;
        final int height = graph.getExpectedHeight() + margin;

        this.setBounds(parent.getLocationOnScreen().x,
        			   parent.getLocationOnScreen().y,
        			   width, height);
        // Populate
       		this.wrapper.add(this.graph, BorderLayout.CENTER);
   			this.add(this.wrapper, BorderLayout.CENTER);
   			
        this.pack();
        // this.setVisible(true);
    }

}
