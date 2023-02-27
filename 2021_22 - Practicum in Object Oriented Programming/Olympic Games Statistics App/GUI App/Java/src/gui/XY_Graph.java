package gui;


import java.util.ArrayList;

import utils.Pair;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Color;
import java.awt.Polygon;
// import java.awt.FontMetrics;


public class XY_Graph extends Graph {
    
	// ==========================================
	// Attributes:


    private float scale_x;
    private float scale_y;

    private int width_;
    private int height_;

    private int x_axis_start;
    private int x_axis_end;
    private int x_axis_step;

    private int y_axis_start;
    private int y_axis_end;
    private int y_axis_step;

    
    private int margin = 5;

    private ArrayList<Pair<Integer, Integer>> points_red;
    private ArrayList<Pair<Integer, Integer>> points_blue;


    // ==========================================
    // Constructor and Helpers:


    public XY_Graph(ArrayList<Pair<Integer, Integer>> points_red,
                    ArrayList<Pair<Integer, Integer>> points_blue,
                    final int x_axis_step,
                    final int y_axis_step,
                    final int enforce_width,  // negative not to apply
                    final int enforce_height,  // negative not to apply
                    boolean zoom_on_hover)
    {
    	super(1f, zoom_on_hover);  // scale is 1
        this.scale_x = 1f;
        this.scale_y = 1f;

        this.x_axis_step = x_axis_step;
        this.y_axis_step = y_axis_step;

        this.points_red = points_red;
        this.points_blue = points_blue;
        this.init();

        this.enforceSize(enforce_width, enforce_height);

        // recalculate the width and height
        this.width_ = (int) (this.scale_x * (this.x_axis_end - this.x_axis_start + 1 + this.margin));
        this.height_ = (int) (this.scale_y * (this.y_axis_end - this.y_axis_start + 1 + this.margin));

        this.setPreferredSize(new Dimension(this.width_, this.height_));
    }

    private void init() {
        // find minimum x-axis value in both arrays
        this.x_axis_start = Integer.MAX_VALUE;
        this.y_axis_start = Integer.MAX_VALUE;

        this.x_axis_end = Integer.MIN_VALUE;
        this.y_axis_end = Integer.MIN_VALUE;

        for (Pair<Integer, Integer> point : points_red) {
            this.x_axis_start = Math.min(this.x_axis_start, point.first);
            this.y_axis_start = Math.min(this.y_axis_start, point.second);
            this.x_axis_end = Math.max(this.x_axis_end, point.first);
            this.y_axis_end = Math.max(this.y_axis_end, point.second);
        }

        for (Pair<Integer, Integer> point : points_blue) {
            this.x_axis_start = Math.min(this.x_axis_start, point.first);
            this.y_axis_start = Math.min(this.y_axis_start, point.second);
            this.x_axis_end = Math.max(this.x_axis_end, point.first);
            this.y_axis_end = Math.max(this.y_axis_end, point.second);
        }

        int round_down = this.x_axis_start % this.x_axis_step;
        this.x_axis_start = this.x_axis_start - round_down;
        if (round_down == 0) this.x_axis_start -= this.x_axis_step;

        round_down = this.y_axis_start % this.y_axis_step;
        this.y_axis_start = this.y_axis_start - round_down;
        if (round_down == 0) this.y_axis_start -= this.y_axis_step;

        // enforce only positive quadrant
        if (this.x_axis_start < 0) this.x_axis_start = 0;
        if (this.y_axis_start < 0) this.y_axis_start = 0;

        this.x_axis_end = this.x_axis_end
                        - this.x_axis_end % this.x_axis_step
                        + this.x_axis_step;

        this.y_axis_end = this.y_axis_end
                        - this.y_axis_end % this.y_axis_step
                        + this.y_axis_step;
    }


    // ==========================================
    // Public Getters:


    public int getGraphWidth() {
        return this.width_;
    }

    public int getGraphHeight() {
        return this.height_;
    }


    // ==========================================
    // Public Setters:


    public void enforceSize(int enforce_width, int enforce_height) {
        // width and height with scale 1
        final int width = (int) (1.f * (this.x_axis_end - this.x_axis_start + 1 + this.margin));
        final int height = (int) (1.f * (this.y_axis_end - this.y_axis_start + 1 + this.margin));

        if (enforce_width > 0)
            this.scale_x = enforce_width / width;

        if (enforce_height > 0)
            this.scale_y = enforce_height / height;

        this.scale = (float) (this.scale_x + this.scale_y) / 3.f;

        // recalculate the width and height
        this.width_ = (int) (this.scale_x * (this.x_axis_end - this.x_axis_start + 1 + this.margin));
        this.height_ = (int) (this.scale_y * (this.y_axis_end - this.y_axis_start + 1 + this.margin));
        // this.width_ = enforce_width;
        // this.height_ = enforce_height;
    }


    // ==========================================
    // Public GUI Methods:


    @Override
    public void paint(Graphics g) {
    	Color color_to_restore = g.getColor();

        // Draw outline box
        final int width = (int) (this.scale_x * (this.x_axis_end - this.x_axis_start + 1));
        final int height = (int) (this.scale_y * (this.y_axis_end - this.y_axis_start + 1));

        //final int width = this.width_;
        //final int height = this.height_;
        final int origin_x = this.getLocation().x + this.getWidth() / 2 - width / 2;
        final int origin_y = this.getLocation().y + this.getHeight() / 2 + height / 2;

        Polygon outline = new Polygon();
        outline.addPoint(origin_x, origin_y - height - (int) ( this.margin));
        outline.addPoint(origin_x + width + (int) ( this.margin),
                         origin_y - height - (int) ( this.margin));
        outline.addPoint(origin_x + width + (int) ( this.margin), origin_y);
        outline.addPoint(origin_x, origin_y);
        g.drawPolygon(outline);

        // Draw Labels/Markers on x-axis:
        final int markers_offset = 6;
        final int mark_length = 4;
        g.setColor(Color.black);
        for (int value = this.x_axis_start;
             value <= this.x_axis_end;
             value += this.x_axis_step)
        {
            final int cur_x = origin_x + (int) (this.scale_x * (value - this.x_axis_start));

            String label = "" + value;
            final int label_width = g.getFontMetrics().stringWidth(label);
            final int label_height = g.getFontMetrics().getHeight();

            g.drawLine(cur_x, origin_y, cur_x, origin_y + mark_length);

            g.drawString(label,
                         cur_x - label_width / 2,
                         origin_y + label_height + markers_offset);
        }

        // Draw Labels/Markers on y-axis:
        g.setColor(Color.black);
        for (int value = this.y_axis_start;
             value <= this.y_axis_end;
             value += this.y_axis_step)
        {
            final int cur_y = origin_y - (int) (this.scale_y * (value - this.y_axis_start));

            String label = "" + value;
            final int label_width = g.getFontMetrics().stringWidth(label);
            final int label_height = g.getFontMetrics().getHeight();

            g.drawLine(origin_x, cur_y, origin_x - mark_length, cur_y);

            g.drawString(label,
                         origin_x - label_width - markers_offset,
                         cur_y + label_height / 3);
        }

        // Draw the red points:
        final int point_thickness = Math.min(21, (int) (this.scale * 4));
        g.setColor(Color.red);
        for (Pair<Integer, Integer> point : points_red) {
            Integer cur_x = origin_x + (int) (this.scale_x * (point.first - this.x_axis_start));
            Integer cur_y = origin_y - (int) (this.scale_y * (point.second - this.y_axis_start));
            g.fillOval(cur_x - point_thickness / 2, cur_y - point_thickness / 2,
                       point_thickness, point_thickness);
        }

        // Draw the blue points:
        g.setColor(Color.blue);
        for (Pair<Integer, Integer> point : points_blue) {
            Integer cur_x = origin_x + (int) (this.scale_x * (point.first - this.x_axis_start));
            Integer cur_y = origin_y - (int) (this.scale_y * (point.second - this.y_axis_start));
            g.fillOval(cur_x - point_thickness / 2, cur_y - point_thickness / 2,
                       point_thickness, point_thickness);
        }

        g.setColor(color_to_restore);
    }

}
