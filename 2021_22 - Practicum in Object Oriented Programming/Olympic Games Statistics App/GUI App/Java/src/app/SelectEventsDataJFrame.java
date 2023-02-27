package app;

import java.awt.*;

import javax.swing.*;

import java.io.File;


public class SelectEventsDataJFrame extends JDialog
{

	// ==========================================
	// Attributes:


	private File events_data_file;
	private int year_filter;  // == -1 for no year filter


	// Panel containing all text fields
	private JPanel main_panel = new JPanel();

	private JPanel file_path_panel = new JPanel();
	private JTextField file_path_txt_field = new JTextField();
	private JButton browse_btn = new JButton("Browse");

	private JPanel year_filter_panel = new JPanel();
	private JTextField year_filter_txt_field = new JTextField();

	private JButton load_file_btn = new JButton("Load File");


	// ==========================================
	// Constructor:


    public SelectEventsDataJFrame(JFrame parent,
    							  String title,
    							  java.awt.Dialog.ModalityType modality_type)
    {
    	super(parent, title, modality_type);
        this.addWindowListener(App.exitWindow);

		int height_margin = 40;
		int width_margin = 50;
		this.setLayout(new BorderLayout(width_margin, height_margin));

		this.populate();
    }


	// ==========================================
	// Constructor Helpers:


	private void populate() {
		final int left_margin = 100;
		final int row_height = 25;

		// Main Content Panel
		this.main_panel.setLayout(new GridLayout(3, 1, 0, 25));
		this.main_panel.setPreferredSize(new Dimension(500, 250));
		this.main_panel.setBorder(BorderFactory.createEmptyBorder(25, 0, 0, 0));

			JPanel wrapper_path_panel = new JPanel(new FlowLayout(FlowLayout.LEFT));
			wrapper_path_panel.setBorder(BorderFactory.createEmptyBorder(0, left_margin, 0, 0));

				// File Path Panel
				this.file_path_panel.setLayout(new GridLayout(2, 1));

				 	// File Path Label
					JPanel label_wrapper = new JPanel(new FlowLayout(FlowLayout.LEFT));
					 	JLabel file_path_label = new JLabel("Events Data File:");
					 		file_path_label.setPreferredSize(new Dimension(100, row_height));
					 		label_wrapper.add(file_path_label, BorderLayout.WEST);
							this.file_path_panel.add(label_wrapper, BorderLayout.CENTER);

					// Text Field for the File Path
					JPanel input_wrapper = new JPanel();
					input_wrapper.setLayout(new FlowLayout(FlowLayout.CENTER));
				 		this.file_path_txt_field.setPreferredSize(new Dimension(270, row_height));
				 		//this.file_path_panel.add(file_path_txt_field, BorderLayout.WEST);
						input_wrapper.add(this.file_path_txt_field);

					// Browse button for File Path
						this.browse_btn.setPreferredSize(new Dimension(80, row_height));

						this.browse_btn.addActionListener((ae) -> {
							File new_file = ((App) this.getParent()).userSelectFile();
							if (new_file == null) {  // selecting files failed
								return;
							}

							this.file_path_txt_field.setText(new_file.getAbsolutePath());
						});

						input_wrapper.add(browse_btn);
						this.file_path_panel.add(input_wrapper, BorderLayout.CENTER);

					wrapper_path_panel.add(this.file_path_panel);

			 	this.main_panel.add(wrapper_path_panel);


				// Year Filter Panel
				this.year_filter_panel.setLayout(new FlowLayout(FlowLayout.LEFT));
				this.year_filter_panel.setBorder(BorderFactory.createEmptyBorder(0, left_margin, 0, 0));

					JLabel year_label = new JLabel("Filter by single year:");
						year_label.setPreferredSize(new Dimension(120, row_height));
						this.year_filter_panel.add(year_label, BorderLayout.WEST);

					// Text Field for Year Filter Panel
					this.year_filter_txt_field.setPreferredSize(new Dimension(80, row_height));
					this.year_filter_panel.add(this.year_filter_txt_field,
											   BorderLayout.EAST);

			 	this.main_panel.add(this.year_filter_panel);


			// Load File Button:
			JPanel btn_wrapper = new JPanel();
				load_file_btn.addActionListener((ae) -> {
					SelectEventsDataJFrame.this.blockFormFields();

					// values from the form are already set, now just load in the data
					try {
						if (!SelectEventsDataJFrame.this.validateAndSetFormFields())
							throw new exceptions.IncorrectlyFulfilledForm();

						((App) SelectEventsDataJFrame.this.getParent())
						.loadInEventsData(SelectEventsDataJFrame.this.events_data_file,
										  SelectEventsDataJFrame.this.year_filter);

						// successfully loaded in the data, this window
						// is no longer necessary, just display success message
						JOptionPane.showMessageDialog
							(SelectEventsDataJFrame.this.getParent(),
							 "Successfully loaded in the events data!",
							 SelectEventsDataJFrame.this.getTitle(),
							 JOptionPane.PLAIN_MESSAGE);

						SelectEventsDataJFrame.this.dispose();
					} catch (exceptions.IncorrectlyFulfilledForm err) {
						// input error messages were already displayed to user
						// so just quit this function
						return;
					} catch (Throwable err) {  // error from loadInEventsData
						// System.out.println("Failed to load in the events data.");
						JOptionPane.showMessageDialog
							(SelectEventsDataJFrame.this.getParent(),
							 "Failed to load in events data, please try again.",
							 SelectEventsDataJFrame.this.getTitle(),
							 JOptionPane.ERROR_MESSAGE);

						return;
					} finally {
						SelectEventsDataJFrame.this.unblockFormFields();
					}
				});

				btn_wrapper.add(this.load_file_btn);
				this.main_panel.add(btn_wrapper, BorderLayout.CENTER);


		this.add(this.main_panel, BorderLayout.CENTER);
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
		final String file_path_locate_err_msg = "Cannot find given file!";
		final String year_format_explain_msg
			= "To filter by year enter a positive number in the appropriate field.\nTo NOT appply this filter just leave whitespace in it.";

		// Check validity of the data in the form
		Boolean is_valid = true;
		File file = null;
		int year = -1;

		try {  // check events data file
			// if not .txt file raise error
			if (!this.file_path_txt_field.getText().endsWith(".txt"))
				throw new exceptions.WrongFileFormat();

			file = new File(this.file_path_txt_field.getText());

			if (file == null || !file.exists())
				throw new Exception("file does not exist");
		} catch (exceptions.WrongFileFormat err) {
			is_valid = false;
			JOptionPane.showMessageDialog(
				this, "Given file must be a text file (.txt)!",
				"Wrong file path input",
				JOptionPane.ERROR_MESSAGE
			);
		} catch (Exception err) {
			is_valid = false;
			JOptionPane.showMessageDialog(this, file_path_locate_err_msg,
										  "Wrong file path input",
										  JOptionPane.ERROR_MESSAGE);
		}

		try {  // check year
			if (this.year_filter_txt_field.getText().trim().equals("")) {
				// valid option - do not apply any filtering by year
				year = -1;
			} else {
				year = Integer.parseInt(this.year_filter_txt_field.getText());

				// invalid integer value
				if (year <= 0) throw new NumberFormatException();
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
			this.events_data_file = file;
			this.year_filter = year;
		}

		return is_valid;
	}

	private void blockFormFields() {
		// mark all form's fields as blocked
		this.load_file_btn.setEnabled(false);
		this.browse_btn.setEnabled(false);
		this.file_path_txt_field.setEditable(false);
		this.year_filter_txt_field.setEditable(false);
	}

	private void unblockFormFields() {
		// unblock all form's fields
		this.load_file_btn.setEnabled(true);
		this.browse_btn.setEnabled(true);
		this.file_path_txt_field.setEditable(true);
		this.year_filter_txt_field.setEditable(true);
	}
	
}
