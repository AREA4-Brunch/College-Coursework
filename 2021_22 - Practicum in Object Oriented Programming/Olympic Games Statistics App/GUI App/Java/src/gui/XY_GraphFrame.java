package gui;


import java.awt.Dimension;
import java.awt.BorderLayout;
import java.awt.*;

import javax.swing.*;

import app.App;

public class XY_GraphFrame extends JFrame {


	// ==========================================
	// Attributes:


    private JPanel graph_wrapper = new JPanel(new BorderLayout());
    private XY_Graph graph;

    private JTextField year_start_txt_field = new JTextField();
    private JTextField year_end_txt_field = new JTextField();
    
    private static String options[] = {
        "Number of Disciplines",
        "Average height of athletes",
        "Average weight of athletes"
    };

    private JComboBox<String> selector_data = new JComboBox<>(options);


    private JButton apply_filters_btn = new JButton("Apply");


    // Form fields values:
    private int year_start;
    private int year_end;

    private App app;


    // ==========================================
    // Constructor and Helpers:


    public XY_GraphFrame(App parent, String title, final XY_Graph graph) {
        super(title);
        this.app = parent;
        this.graph = graph;

        this.addWindowListener(App.exitWindow);
        this.setResizable(false);
        this.setLayout(new BorderLayout());

        this.setBounds(parent.getLocationOnScreen().x,
        			   parent.getLocationOnScreen().y,
        			   this.graph.getGraphWidth(),
                       this.graph.getGraphHeight());

        this.populate();
        
        this.pack();
        // this.setVisible(true);
    }

    private void populate() {
        final int width = this.graph.getGraphWidth();
        final int height = this.graph.getGraphHeight();
        final int margin = 100;

        // JFrame:

            // Filtering:
            JPanel filtering_panel = new JPanel(new GridLayout(3, 1, 0, 20));
                filtering_panel.setBorder(BorderFactory.createEmptyBorder(45, 0, 0, 0));

                JPanel year_filters = new JPanel(new FlowLayout(FlowLayout.CENTER, 20, 0));
            		JLabel from_year_label = new JLabel("From year");
            		JLabel to_year_label = new JLabel("to year");

	                this.year_start_txt_field.setPreferredSize(new Dimension(60, 25));
	                this.year_end_txt_field.setPreferredSize(new Dimension(60, 25));

	                year_filters.add(from_year_label);
                    year_filters.add(this.year_start_txt_field);
            		year_filters.add(to_year_label);
                    year_filters.add(this.year_end_txt_field);

                    filtering_panel.add(year_filters);

                JPanel data_select = new JPanel(new BorderLayout());
                        selector_data.setPreferredSize(new Dimension(100, 35));
                        data_select.add(selector_data, BorderLayout.NORTH);
                    filtering_panel.add(data_select);

                JPanel filter_btn_wrapper = new JPanel(new FlowLayout(FlowLayout.CENTER, 20, 0));
                    this.apply_filters_btn.setPreferredSize(new Dimension(80, 30));
                    this.apply_filters_btn.addActionListener((ae) -> {
                        XY_GraphFrame.this.blockUserActions(false);

                        // values from the form are already set, now just load in the data
                        try {
                            if (!XY_GraphFrame.this.validateAndSetFormFields())
                                throw new exceptions.IncorrectlyFulfilledForm();
    
                            XY_Graph new_graph = null;
                            if (XY_GraphFrame.this.selector_data.getSelectedItem().toString()
                                .equals(XY_GraphFrame.options[0]))
                            {  // "Number of Disciplines"
                                new_graph = ((App) XY_GraphFrame.this.app)
                                             .getXY_Graph_Num_Disciplines(XY_GraphFrame.this,
                                            		 					  XY_GraphFrame.this.year_start,
                                                                          XY_GraphFrame.this.year_end);

                            } else if (XY_GraphFrame.this.selector_data.getSelectedItem()
                                        .equals(XY_GraphFrame.options[1]))
                            {  // "Average height of athletes"
                                new_graph = ((App) XY_GraphFrame.this.app)
                                             .getXY_Graph_AvgHeight(XY_GraphFrame.this,
                                            		 				XY_GraphFrame.this.year_start,
                                                                    XY_GraphFrame.this.year_end);
                            } else if (XY_GraphFrame.this.selector_data.getSelectedItem()
                                        .equals(XY_GraphFrame.options[2]))
                            {  // "Average weight of athletes"
                                new_graph = ((App) XY_GraphFrame.this.app)
                                             .getXY_Graph_AvgWeight(XY_GraphFrame.this,
                                            		 				XY_GraphFrame.this.year_start,
                                                                    XY_GraphFrame.this.year_end);
                            }

                            if (new_graph != null) {  // set new graph
                                XY_GraphFrame.this.graph_wrapper.remove(XY_GraphFrame.this.graph);
                                XY_GraphFrame.this.graph = new_graph;
                                XY_GraphFrame.this.graph_wrapper.add(XY_GraphFrame.this.graph);
                                XY_GraphFrame.this.graph.enforceSize(width, height);
                                // XY_GraphFrame.this.graph.setPreferredSize(new Dimension(width - margin, height - margin));
                                XY_GraphFrame.this.graph_wrapper.revalidate();
                            }
                        } catch (exceptions.IncorrectlyFulfilledForm err) {
                            // input error messages were already displayed to user
                            // so just quit this function
                            return;
                        } catch (Throwable err) {  // error from loadInEventsData
                            System.out.println(err);
                            // System.out.println("Failed to load in the events data.");
                            JOptionPane.showMessageDialog
                                (XY_GraphFrame.this.getParent(),
                                 "Failed to filter the data.",
                                 XY_GraphFrame.this.getTitle(),
                                 JOptionPane.ERROR_MESSAGE);

                            return;
                        } finally {
                            XY_GraphFrame.this.blockUserActions(true);
                        }
                    });
                    filter_btn_wrapper.add(this.apply_filters_btn);
                    filtering_panel.add(filter_btn_wrapper);

                this.add(filtering_panel, BorderLayout.NORTH);

            // Adding Graph:
                this.graph.setPreferredSize(new Dimension(width + margin, height + margin));
                this.graph_wrapper.add(this.graph, BorderLayout.CENTER);
                this.add(this.graph_wrapper, BorderLayout.CENTER);
    }


	// ==============================================
	// GUI Helpers:


	private void blockUserActions(Boolean do_not_block) {
		// mark all filters form's fields as blocked
		this.apply_filters_btn.setEnabled(do_not_block);
		this.year_start_txt_field.setEditable(do_not_block);
		this.year_end_txt_field.setEditable(do_not_block);
        this.selector_data.setEnabled(do_not_block);
	}


	/**
	 * Checks for validity of all form fields and
	 * in case something is invalid displays message
	 * to user that explains the wrong input.
	 * If everything is valid sets all parameters in this object.
	 * 
	 * @return Whether all fields in the form are valid.
	 */
	private Boolean validateAndSetFormFields() {
		final String year_format_explain_msg
			= "To filter by year enter a positive number in the appropriate field.\nTo NOT appply this filter just leave whitespace in it.";

		// Check validity of the data in the form
		Boolean is_valid = true;
		int year_start = -1;
		int year_end = -1;

		try {  // check year start
			if (this.year_start_txt_field.getText().trim().equals("")) {
				// valid option - do not apply any filtering by year
				year_start = -1;
			} else {
				year_start = Integer.parseInt(this.year_start_txt_field.getText());

				// invalid integer value
				if (year_start <= 0) throw new NumberFormatException();
			}
		} catch (NumberFormatException err) {  // non integer value
			is_valid = false;
			JOptionPane.showMessageDialog(
				this, year_format_explain_msg,
				"Wrong filter by year input",
				JOptionPane.ERROR_MESSAGE
			);
		}

		try {  // check year end
			if (this.year_end_txt_field.getText().trim().equals("")) {
				// valid option - do not apply any filtering by year
				year_end = -1;
			} else {
				year_end = Integer.parseInt(this.year_end_txt_field.getText());

				// invalid integer value
				if (year_end <= 0) throw new NumberFormatException();
			}
		} catch (NumberFormatException err) {  // non integer value
			is_valid = false;
			JOptionPane.showMessageDialog(
				this, year_format_explain_msg,
				"Wrong filter by year input",
				JOptionPane.ERROR_MESSAGE
			);
		}

		if (is_valid) {  // set all fields
			this.year_start = year_start;
            this.year_end = year_end;
		}

		return is_valid;
	}

}
