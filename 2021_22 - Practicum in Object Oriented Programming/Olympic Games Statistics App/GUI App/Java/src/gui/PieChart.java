package gui;


import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Random;

import utils.Pair;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Color;


public class PieChart extends Graph {
    
	// ==========================================
	// Attributes:


    // data to display:
    LinkedHashMap<String, Pair<Double, Color>> percentages;

    private int label_offset_x = 12;  // in px
    private int label_offset_y = 12;  // in px

    private int radius;
    private int center_x;
    private int center_y;
    

    // ==========================================
    // Constructor and Helpers:


    public PieChart(final Map<String, Number> labeled_data,
    				final int radius,
    				final boolean zoom_on_hover)
    {
    	super(1, zoom_on_hover);  // size in px
    	this.radius = radius;

    	final int margin = 100;
    	this.exp_height = 2 * (radius + margin);
    	this.exp_width = 2 * (radius + margin);

        this.setPreferredSize(new Dimension(this.exp_width, this.exp_height));
    	this.percentages = PieChart.getCategoriesPercentages(labeled_data);
    }


    // ==========================================
    // Public GUI Methods:


    @Override
    public void paint(Graphics g) {
        // super.paint(g);

    	Color color_to_restore = g.getColor();

    	this.center_x = this.getLocation().x + this.getWidth() / 2;
        this.center_y = this.getLocation().y + this.getHeight() / 2;

        // Construct the pieces of pie for each category
        int cur_angle = 0;
        for (Map.Entry<String, Pair<Double, Color>> kv : percentages.entrySet()) {
            //final double limit_angle = cur_angle + kv.getValue().first * (2. * Math.PI);
        	final int angle_of_slice = (int) Math.round(kv.getValue().first * 360.);

            // Draw the label
            String label = kv.getKey();

            final int label_width = g.getFontMetrics().stringWidth(label);
            final int label_height = g.getFontMetrics().getHeight();
            final int label_box_diag_half = (int) Math.sqrt(label_width * label_width
            											  + label_height * label_height) / 2;

            final double middle_angle = -1 * Math.toRadians(cur_angle + angle_of_slice / 2.);

            final int label_center_x
            	= center_x
        		+ (int) ((this.radius + label_width / 1.5 + this.label_offset_x) * Math.cos(middle_angle));

            final int label_center_y
            	= center_y 
            	+ (int) ((this.radius + 2.25 * label_height + this.label_offset_y) * Math.sin(middle_angle));

            // Draw the slice:

            g.setColor(kv.getValue().second);
            g.fillArc(center_x - this.radius, center_y - this.radius,
            		  2 * this.radius, 2 * this.radius,
            		  cur_angle, angle_of_slice);
            cur_angle += angle_of_slice;

            g.setColor(Color.black);
            g.drawString(
                label,
                label_center_x - label_width / 2,  // x
                label_center_y - label_height / 2  // y
    		);

        }

        g.setColor(color_to_restore);
    }


    // ==========================================
    // Private Helper Methods:


    private static LinkedHashMap<String, Pair<Double, Color>> getCategoriesPercentages
        (final Map<String, Number> labeled_data)
    {
        // get the total amount
        double total_sum = 0.0;

        for (final Number val : labeled_data.values()) {
        	total_sum += val.doubleValue();
        }

        // calc and store percentage for each kv pair
        Random psrng = new Random();  // for random colors
        LinkedHashMap<String, Pair<Double, Color>> percentages = new LinkedHashMap<>();

        for (final Map.Entry<String, Number> kv : labeled_data.entrySet()) {
            final double percentage = kv.getValue().doubleValue() / total_sum;

            System.out.println(kv.getKey() + ": " + 100. * percentage + "%");
            
            if (100. * percentage < 2.5) {  // too small to get chunk of its own
            	Pair<Double, Color> other_group = percentages.getOrDefault
        			("Other", new Pair<Double, Color>
        				(0., new Color(psrng.nextFloat(), psrng.nextFloat(), psrng.nextFloat()))
					);

            	Double new_other_percentage = other_group.first + percentage;
            	other_group.first = new_other_percentage;
                percentages.put("Other", other_group);
            } else {  // large enough to display on its own
                percentages.put(
                    kv.getKey(),
                    new Pair<Double, Color> (percentage, new Color(psrng.nextFloat(), psrng.nextFloat(), psrng.nextFloat()))
			    );
            }
        }

        return percentages;
    }

}
