package gui;

import java.awt.*;
import java.awt.event.MouseEvent;
import java.awt.event.MouseAdapter;


public abstract class Graph extends Canvas {


	// ==========================================
	// Attributes:


    protected float scale = 1;
    protected int exp_width;
    protected int exp_height;


    // ==========================================
    // Constructor and Helpers:


    protected Graph(final float scale, final boolean resizable_on_hover) {
        super();
        this.setScale(scale);
        if (resizable_on_hover)
            this.addMouseListener(new Graph.MouseListener());
    }


    @Override
    public void paint(Graphics g) {
        super.paint(g);
    }


    // ==========================================
    // Event Listeners:


    protected class MouseListener extends MouseAdapter {

        @Override
        public void mouseEntered(MouseEvent e) {
            Graph.this.zoomIn();
        }

        @Override
        public void mouseExited(MouseEvent e) {
            Graph.this.zoomOut();
        }

    };


    // ==========================================
    // Getters:


    public float getScale() {
        return this.scale;
    }

    public int getExpectedWidth() {
    	return this.exp_width;
    }

    public int getExpectedHeight() {
    	return this.exp_height;
    }

    // ==========================================
    // Setters:


    public void setScale(final float scale) {
        this.scale = scale;
    }


    // ==========================================
    // Protected Methods:


    protected void zoomIn() {
        this.setScale(1.42f * this.getScale());
        this.repaint();
    }

    protected void zoomOut() {
        this.setScale(0.70422535211267605633802816901408f * this.getScale());
        this.repaint();
    }

}
