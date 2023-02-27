package app;

//import java.awt.event.KeyEvent;

import java.io.File;
import java.lang.Thread.UncaughtExceptionHandler;

import java.util.HashMap;
import java.util.ArrayList;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

import gui.GraphFrame;
import gui.XY_GraphFrame;
import gui.PieChart;
import gui.XY_Graph;

import utils.Pair;

import data_loaders.DataBase;
import filter.Filter;


public class App extends JFrame {


	// ==========================================
	// Attributes:


	// Menu Bar GUI
	private JMenu open_file_menu = new JMenu("Open");
	private JDialog select_events_data_dialog;

	// Filters GUI
	private JPanel filters_panel = new JPanel();

	private JTextField year_filter_txt_field = new JTextField();
	private Integer year_filter_value;

	private JTextField sport_filter_txt_field = new JTextField();
	private String sport_filter_value;

	private JTextField country_filter_txt_field = new JTextField();
	private String country_filter_value;

	private static final String[] event_types_options = { "No Filter" , "Individual", "Team" };
	private JComboBox<String> event_type_filter_combo_box = new JComboBox<String>(event_types_options);
	private String event_type_filter_value;

	private static final String[] medals_options = { "No Filter", "Gold" , "Silver", "Bronze", "No Medal" };
	private JComboBox<String> medals_filter_combo_box = new JComboBox<String>(medals_options);
	private String medal_filter_value;

	private JButton apply_filters_btn = new JButton("Apply");


	// Results GUI
	private JLabel num_athletes_txt = new JLabel("Number of athletes who participated:");
	private JLabel num_disciplines_txt = new JLabel("Number of disciplines on olympic games:");
	private JLabel avg_height_txt = new JLabel("Average height of athletes:");
	private JLabel avg_weight_txt = new JLabel("Average weight of athletes:");

	private JButton display_pie_chart_btn = new JButton("Open Pie Chart");
	private JButton display_games_graph_btn = new JButton("Open Games Graph");

	private GraphFrame pie_chart_window;
	private HashMap<String, Number> pie_chart_data = new HashMap<>();

	private XY_GraphFrame xy_graph_window;

	// Progress bar
	private Throwable thread_uncaught_error;


	public static WindowListener exitWindow = new WindowAdapter() {
		@Override
		public void windowClosing(WindowEvent e) {
			e.getWindow().dispose();
		}
	};

	public static WindowListener exitWindowWithConfirmation = new WindowAdapter() {
		@Override
		public void windowClosing(WindowEvent e) {
			int response = JOptionPane.showConfirmDialog
				(e.getWindow(), "Are you sure you want to exit the app?",
				 "Exit app", JOptionPane.YES_NO_OPTION);
			
			if (response == JOptionPane.YES_OPTION) {
				e.getWindow().dispose();
			}
		}
	};


	// ==========================================
	// Constructor:


    public App() {
		this.init();
    }

	private void init() {
		this.setTitle("Olympic Games Statistics App");
		// this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		this.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
		this.addWindowListener(new WindowAdapter() {
			@Override
			public void windowClosing(WindowEvent e) {
				if (e.getWindow() == App.this) App.exit(App.this);
			}
		});

		int height_margin = 40;
		int width_margin = 50;
		this.setLayout(new BorderLayout(width_margin, height_margin));
		// this.setLayout(new GridLayout(4, 1, width_margin, height_margin));
		this.setResizable(false);
		this.setSize(1080, 720);

		this.populate();

		// unblock the forms when successfully loading in the events data
		this.setBlockUserActions(false);
		// do not block the option of loading in the events data
		this.open_file_menu.setEnabled(true);

		//this.pack();
		this.setVisible(true);
	}

	/**
	 * Resets the app entirely, as if it were just opened.
	 */
	private void reset() {
		this.resetDataBased();
		this.resetData();
	}

	/**
	 * Resets everything that depends on data loaded in from files,
	 * but not that data itself.
	 * For example clears out all search results and its graphs.
	 */
	private void resetDataBased() {
		this.clearResults();
	}

	/**
	 * Resets the database.
	 */
	private void resetData() {
		DataBase.getInstance().clearInstance();
	}

	/**
	 * Clears all values in results GUI.
	 */
	private void clearResults() {
		this.num_athletes_txt.setText("Number of athletes who participated:");
		this.num_disciplines_txt.setText("Number of disciplines on olympic games:");
		this.avg_height_txt.setText("Average height of athletes:");
		this.avg_weight_txt.setText("Average weight of athletes:");

		if (this.pie_chart_window != null) this.pie_chart_window.dispose();
		if (this.xy_graph_window != null) this.xy_graph_window.dispose();

		this.revalidate();
	}


	// ==============================================
	// Public Data Getters for plotting:


	public XY_Graph getXY_Graph_Num_Disciplines(Window parent, final int year_start, final int year_end)
	{
		ArrayList<Pair<Integer, Integer>> summer_games_data = new ArrayList<>();
		ArrayList<Pair<Integer, Integer>> winter_games_data = new ArrayList<>();

		final ProgressBarDialog progress_bar_dialog = new ProgressBarDialog(
			parent != null ? parent : this,  // parent
			150,   // width
			35,    // height
			"Getting XY_Graph Num Disciplines Data"
		);

		Thread thread = new Thread(new Runnable() {
			@Override
			public void run() {
				try {

					Filter filter = new Filter(
						DataBase.getInstance(),
						progress_bar_dialog.progress_bar
					);

					filter.getSummerWinterTotalNumDisciplines(summer_games_data, winter_games_data,
															  year_start, year_end);

				} catch (Throwable err) {  // rethrow
					System.out.println("Error during getting XY Num Disc ocurred: " + err);
					throw err;
				} finally {
					progress_bar_dialog.dispose();
				}
			}
		});

		thread.setUncaughtExceptionHandler(new UncaughtExceptionHandler() {
			@Override
			public void uncaughtException(Thread thread_src, Throwable error) {
				JOptionPane.showMessageDialog
				(App.this,
				 "Failed to get XY Graph's Data",
				 App.this.getTitle(),
				 JOptionPane.ERROR_MESSAGE);
			}
		});

		System.out.println("Suppost to get XY Num Disciplines now.");

		// Start loading in events data
		thread.start();

		// make progress bar visible and start the thread
		// that loads in events data
		progress_bar_dialog.setVisible(true);

		System.out.println("Done Suppost to get XY Num Disciplines now.");

		return new XY_Graph(summer_games_data, winter_games_data, 10, 20, 700, 500, false);
	}

	public XY_Graph getXY_Graph_AvgHeight(Window parent, final int year_start, final int year_end)
	{
		ArrayList<Pair<Integer, Integer>> summer_games_data = new ArrayList<>();
		ArrayList<Pair<Integer, Integer>> winter_games_data = new ArrayList<>();

		final ProgressBarDialog progress_bar_dialog = new ProgressBarDialog(
			parent != null ? parent : this,  // parent
			150,   // width
			35,    // height
			"Getting XY_Graph Avg Height Data"
		);

		Thread thread = new Thread(new Runnable() {
			@Override
			public void run() {
				try {

					Filter filter = new Filter(
						DataBase.getInstance(),
						progress_bar_dialog.progress_bar
					);

					filter.getSummerWinterAvgHeight(summer_games_data, winter_games_data,
													year_start, year_end);

				} catch (Throwable err) {  // rethrow
					System.out.println("Error during getting XY Avg Height ocurred: " + err);
					throw err;
				} finally {
					progress_bar_dialog.dispose();
				}
			}
		});

		thread.setUncaughtExceptionHandler(new UncaughtExceptionHandler() {
			@Override
			public void uncaughtException(Thread thread_src, Throwable error) {
				JOptionPane.showMessageDialog
				(App.this,
				 "Failed to get XY Graph's Data",
				 App.this.getTitle(),
				 JOptionPane.ERROR_MESSAGE);
			}
		});

		System.out.println("Suppost to get XY Avg Height now.");

		// Start loading in events data
		thread.start();

		// make progress bar visible and start the thread
		// that loads in events data
		progress_bar_dialog.setVisible(true);

		System.out.println("Done Suppost to get XY Avg Height now.");

		return new XY_Graph(summer_games_data, winter_games_data, 10, 20, 700, 500, false);
	}

	public XY_Graph getXY_Graph_AvgWeight(Window parent, final int year_start, final int year_end)
	{
		ArrayList<Pair<Integer, Integer>> summer_games_data = new ArrayList<>();
		ArrayList<Pair<Integer, Integer>> winter_games_data = new ArrayList<>();

		final ProgressBarDialog progress_bar_dialog = new ProgressBarDialog(
			parent != null ? parent : this,  // parent
			150,   // width
			35,    // height
			"Getting XY_Graph Avg Weight Data"
		);

		Thread thread = new Thread(new Runnable() {
			@Override
			public void run() {
				try {

					Filter filter = new Filter(
						DataBase.getInstance(),
						progress_bar_dialog.progress_bar
					);

					filter.getSummerWinterAvgWeight(summer_games_data, winter_games_data,
												   year_start, year_end);

				} catch (Throwable err) {  // rethrow
					System.out.println("Error during getting XY Avg Weight ocurred: " + err);
					throw err;
				} finally {
					progress_bar_dialog.dispose();
				}
			}
		});

		thread.setUncaughtExceptionHandler(new UncaughtExceptionHandler() {
			@Override
			public void uncaughtException(Thread thread_src, Throwable error) {
				JOptionPane.showMessageDialog
				(App.this,
				 "Failed to get XY Graph's Data",
				 App.this.getTitle(),
				 JOptionPane.ERROR_MESSAGE);
			}
		});

		System.out.println("Suppost to get XY Avg Weight now.");

		// Start loading in events data
		thread.start();

		// make progress bar visible and start the thread
		// that loads in events data
		progress_bar_dialog.setVisible(true);

		System.out.println("Done Suppost to get XY Avg Weight now.");

		return new XY_Graph(summer_games_data, winter_games_data, 10, 20, 700, 500, false);
	}

	private HashMap<String, Number> getPieChartData() {
		return this.pie_chart_data;
	}


	// ==============================================
	// GUI:


	private void populate() {

		// Menu Bar
		this.setMenuBar();

		// Main Panel:
			final int top_margin = 40;
			final int grid_width_margin = 10;
			final int filters_grid_row_height = 25;
			final int filters_grid_width = (3 + 2) * grid_width_margin + 200 * 4;
			final int left_margin = (this.getWidth() - filters_grid_width) / 2;

			// Filters Panel:
				this.filters_panel.setLayout(new BorderLayout());
				this.filters_panel.setBorder(
					BorderFactory.createEmptyBorder(top_margin, 0, 0, 0)
				);

				JPanel wrapper1 = new JPanel(new FlowLayout(FlowLayout.LEFT));
					wrapper1.setBorder(BorderFactory.createEmptyBorder(0, left_margin, 0, 0));
					JLabel filters_label = new JLabel("Filters");
						filters_label.setFont(new Font("Monospace", Font.ITALIC, 30));
						filters_label.setPreferredSize(new Dimension(100, 30));
						wrapper1.add(filters_label);
						this.filters_panel.add(wrapper1, BorderLayout.NORTH);

				// add row with title of the text field and text filed in next row
				JPanel wrapper2 = new JPanel();
					wrapper2.setPreferredSize(new Dimension(filters_grid_width, filters_grid_row_height * 2 + 50));
					wrapper2.setBorder(BorderFactory.createEmptyBorder(20, 0, 0, 0));

					JPanel filters_grid = new JPanel(new GridLayout(2, 6, grid_width_margin, 3));

						// Text Fields and their labels Label
						JLabel year_filter_label = new JLabel("Year");
						this.year_filter_txt_field.setPreferredSize(new Dimension(25, filters_grid_row_height));

						JLabel sport_filter_label = new JLabel("Sport");
						this.sport_filter_txt_field.setPreferredSize(new Dimension(155, filters_grid_row_height));

						JLabel country_filter_label = new JLabel("Country");
						this.country_filter_txt_field.setPreferredSize(new Dimension(75, filters_grid_row_height));

						JLabel event_type_filter_label = new JLabel("Event Type");
						this.event_type_filter_combo_box.setPreferredSize(new Dimension(50, filters_grid_row_height));

						JLabel medal_filter_label = new JLabel("Medal");
						this.medals_filter_combo_box.setPreferredSize(new Dimension(40, filters_grid_row_height));

						// Add all filter labels and text fields:
						filters_grid.add(year_filter_label);
						filters_grid.add(sport_filter_label);
						filters_grid.add(country_filter_label);
						filters_grid.add(event_type_filter_label);
						filters_grid.add(medal_filter_label);

						filters_grid.add(this.year_filter_txt_field);
						filters_grid.add(this.sport_filter_txt_field);
						filters_grid.add(this.country_filter_txt_field);
						filters_grid.add(this.event_type_filter_combo_box);
						filters_grid.add(this.medals_filter_combo_box);


					wrapper2.add(filters_grid);
					this.filters_panel.add(wrapper2, BorderLayout.CENTER);

				JPanel wrapper3 = new JPanel();
					// wrapper3.setBorder(BorderFactory.createEmptyBorder(20, 0, 0, 0));
					this.apply_filters_btn.setPreferredSize(new Dimension(100, 30));
					this.apply_filters_btn.addActionListener((ae) -> {
						App.this.setBlockUserActions(false);

						try {
							if (!App.this.validateAndSetFormFields())
								throw new exceptions.IncorrectlyFulfilledForm();

							App.this.applyFilters();
						} catch (exceptions.IncorrectlyFulfilledForm err) {
							// input error messages were already displayed to user
							// so just quit this function
							return;
						}  catch (Throwable err) {  // failed to apply the filters, notify the user
							JOptionPane.showMessageDialog
								(App.this, "Failed to apply the given filters",
								 App.this.getTitle(),
								 JOptionPane.ERROR_MESSAGE);
						} finally {
							// enable the graph buttons as well, since they are off
							// until the very first apply filters button click
							App.this.setBlockUserActions(true);
						}
					});

					wrapper3.add(this.apply_filters_btn);
					this.filters_panel.add(wrapper3, BorderLayout.SOUTH);

				this.add(filters_panel, BorderLayout.NORTH);
			
			// Results Panel:
				JPanel results_panel = new JPanel(new BorderLayout());
				results_panel.setBorder(BorderFactory.createEmptyBorder(0, left_margin, 0, 0));

					JPanel wrapper4 = new JPanel(new FlowLayout(FlowLayout.LEFT));
						JLabel filtered_results_label = new JLabel("Filtered Results");
							filtered_results_label.setFont(new Font("Monospace", Font.ITALIC, 30));
							filtered_results_label.setPreferredSize(new Dimension(250, 30));
							wrapper4.add(filtered_results_label);
						results_panel.add(wrapper4, BorderLayout.NORTH);

					JPanel wrapper_res_panel = new JPanel(new BorderLayout());
					wrapper_res_panel.setBorder(BorderFactory.createEmptyBorder(20, 20, 0, 0));

						JPanel text_results = new JPanel(new GridLayout(4, 1));
							this.num_athletes_txt.setFont(new Font("Monospace", Font.ITALIC, 20));
							this.num_disciplines_txt.setFont(new Font("Monospace", Font.ITALIC, 20));
							this.avg_height_txt.setFont(new Font("Monospace", Font.ITALIC, 20));
							this.avg_weight_txt.setFont(new Font("Monospace", Font.ITALIC, 20));

							text_results.add(this.num_athletes_txt);
							text_results.add(this.num_disciplines_txt);
							text_results.add(this.avg_height_txt);
							text_results.add(this.avg_weight_txt);

							wrapper_res_panel.add(text_results, BorderLayout.NORTH);
						results_panel.add(wrapper_res_panel, BorderLayout.CENTER);

					// Add all results related GUI
					this.add(results_panel, BorderLayout.CENTER);

				// Graphs Buttons:
					JPanel graph_buttons = new JPanel(new BorderLayout());
						graph_buttons.setBorder(BorderFactory.createEmptyBorder(0, left_margin, 150, 450));

						JPanel wrapper_graph_butttons = new JPanel(new GridLayout(1, 2, 30, 10));
						wrapper_graph_butttons.setPreferredSize(new Dimension(160, 30));
						// Pie Chart Button:
								JPanel wrapper5 = new JPanel(new BorderLayout());
									this.display_pie_chart_btn.setPreferredSize(new Dimension(80, 30));
									this.display_pie_chart_btn.addActionListener((ae) -> {
										HashMap<String, Number> data = App.this.getPieChartData();
										App.this.pie_chart_window = new GraphFrame
											(App.this, "Pie Chart", new PieChart(data, 120, false));
										App.this.pie_chart_window.setVisible(true);
									});
									wrapper5.add(this.display_pie_chart_btn, BorderLayout.CENTER);
									wrapper_graph_butttons.add(wrapper5);

							// Games Graph Button:
								JPanel wrapper6 = new JPanel(new BorderLayout());
									this.display_games_graph_btn.setPreferredSize(new Dimension(80, 30));
									this.display_games_graph_btn.addActionListener((ae) -> {
										this.xy_graph_window = new XY_GraphFrame (
											App.this,
											"XY Graph",
											this.getXY_Graph_Num_Disciplines(App.this, -1, -1)
										);
										this.xy_graph_window.setVisible(true);
									});
									wrapper6.add(this.display_games_graph_btn, BorderLayout.CENTER);
									wrapper_graph_butttons.add(wrapper6);

							graph_buttons.add(wrapper_graph_butttons, BorderLayout.CENTER);

						this.add(graph_buttons, BorderLayout.SOUTH);
	}

	private void loadInEventsDataFile()
	{
		this.select_events_data_dialog = new SelectEventsDataJFrame
								  (this, "", Dialog.ModalityType.DOCUMENT_MODAL);

		int width = 600;
		int height = 400;
		this.select_events_data_dialog.setBounds(
			this.getLocationOnScreen().x + this.getWidth() / 2 - width / 2,
			this.getLocationOnScreen().y + this.getHeight() / 2 - height / 2,
			width,
			height
		);

		this.select_events_data_dialog.setTitle("Load in Events Data");
		this.select_events_data_dialog.setVisible(true);
	}

	private void loadInAthletesDataFile() {
		final File file = this.userSelectFile();
		if (file == null) return;  // failed to select file
		if (!file.getAbsolutePath().toString().endsWith(".txt")) {
			// user selected file which is not if .txt extension
			JOptionPane.showMessageDialog(
				this,
				"Given file must be a text file (.txt)!",
				"Wrong file path input",
				JOptionPane.ERROR_MESSAGE
			);
			return;
		}

		final ProgressBarDialog progress_bar_dialog = new ProgressBarDialog(
			this,  // parent
			150,   // width
			35,    // height
			"Load in Athletes Data Progress"
		);

		Thread thread = new Thread(new Runnable() {
			@Override
			public void run() {
				try {
					// clear the old database, the main window and everything
					App.this.resetDataBased();

					try {  // load events into the database
						App.loadInAthletesData(progress_bar_dialog.progress_bar, file);
					} catch (java.io.FileNotFoundException err) {
						// ignore, correct file should have been passed
					}

					App.this.revalidate();
				} catch (Throwable err) {  // rethrow
					throw err;
				} finally {
					progress_bar_dialog.dispose();
				}
			}
		});

		thread.setUncaughtExceptionHandler(new UncaughtExceptionHandler() {
			@Override
			public void uncaughtException(Thread thread_src, Throwable error) {
				JOptionPane.showMessageDialog
				(App.this,
				 "Failed to load in athletes data, please try again.",
				 App.this.getTitle(),
				 JOptionPane.ERROR_MESSAGE);
			}
		});

		System.out.println("Suppost to load in Athletes file now.");

		// Start loading in events data
		thread.start();

		// make progress bar visible and start the thread
		// that loads in events data
		progress_bar_dialog.setVisible(true);

		System.out.println("Done Suppost to load in Athletes file now.");
	}


	// ==============================================
	// GUI Helpers:


	private void setMenuBar() {
		JMenuBar menu_bar = new JMenuBar();  // window's menu bar

			// Setup File Menu:
			JMenu file_menu = new JMenu("File");

				file_menu.setMnemonic(KeyEvent.VK_F);  // Alt+F shortcut

				// Open File Option for loading in the data from a file:

					// Open -> Events
					JMenuItem open_events = new JMenuItem("Events Data");

					// on click open file explorer to navigate to file,
					// then prompt user for optional filtering when loading in
					// and load in the data and reset the rest of the app
					open_events.addActionListener((ae) -> {
						loadInEventsDataFile();
					});

					this.open_file_menu.add(open_events);
					// Open -> Events done


					// Open -> Athletes
					JMenuItem open_athletes = new JMenuItem("Athletes Data");

					// on click open file explorer to navigate to file,
					// then prompt user for optional filtering when loading in
					// and load in the data and reset the rest of the app
					open_athletes.addActionListener((ae) -> {
						this.loadInAthletesDataFile();
					});

					this.open_file_menu.add(open_athletes);
					// Open -> Athletes done

				file_menu.add(this.open_file_menu);  // add Open option to File
				// Open File done


				// Setup Quit Option
				JMenuItem exit = new JMenuItem("Exit");

					exit.addActionListener( (ae) -> {
						App.exit(App.this);
					});

					file_menu.add(exit);  // add Quit option to File
					// Quit Option done

				menu_bar.add(file_menu);  // add File to the Menu Bar
				// File Menu done

			this.setJMenuBar(menu_bar);  // add Menu Bar to this Frame
			// Menu Bar done
	}

	/**
	 * Shows File Chooser to user and returns the file they selected.
	 * In case of error shows error dialog and returns null.
	 * 
	 * @return File user selected, in case of error it's null.
	 */
	File userSelectFile() {
		File file = null;

		try {
			JFileChooser file_explorer = new JFileChooser();
			file_explorer.setCurrentDirectory(new File(".."));

			// exit the option if the error occurred while selecting the file
			// and display error message
			if (JFileChooser.APPROVE_OPTION != file_explorer.showOpenDialog(null)) {
				JOptionPane.showMessageDialog(
					this,
					"Failed to select file",
					this.getTitle() + ": Select File",
					JOptionPane.ERROR_MESSAGE
				);
				throw new Exception("User failed to select file");
			}

			file = new File(file_explorer.getSelectedFile()
										 .getAbsolutePath());
		} catch (Exception err) {
			// ignore and just return null in finally block
		} finally {
			return file;
		}
	}

	private void setBlockUserActions(Boolean do_not_block) {
		this.open_file_menu.setEnabled(do_not_block);

		// mark all filters form's fields as blocked
		this.apply_filters_btn.setEnabled(do_not_block);
		this.year_filter_txt_field.setEditable(do_not_block);
		this.sport_filter_txt_field.setEditable(do_not_block);
		this.country_filter_txt_field.setEditable(do_not_block);
		this.event_type_filter_combo_box.setEnabled(do_not_block);
		this.medals_filter_combo_box.setEnabled(do_not_block);

		// mark all graphs buttons as blocked
		this.display_pie_chart_btn.setEnabled(do_not_block);
		this.display_games_graph_btn.setEnabled(do_not_block);

		this.revalidate();
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
		int year = -1;

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
			this.year_filter_value = year;
			this.sport_filter_value = this.sport_filter_txt_field.getText().trim();
			this.country_filter_value = this.country_filter_txt_field.getText().trim();
			
			// Set event type combo box value
			if (this.event_type_filter_combo_box.getSelectedItem().toString()
				.equals(App.event_types_options[0]))
			{  // Do not apply this filter
				this.event_type_filter_value = "";
			} else if (this.event_type_filter_combo_box.getSelectedItem().toString()
				.equals(App.event_types_options[1]))
			{  // Individual
				this.event_type_filter_value = App.event_types_options[1];
			} else if (this.event_type_filter_combo_box.getSelectedItem().toString()
					  .equals(App.event_types_options[2]))
			{  // Team
				this.event_type_filter_value = App.event_types_options[2];
			}

			// Set medal combo box value
			if (this.medals_filter_combo_box.getSelectedItem().toString()
				.equals(App.medals_options[0]))
			{  // Do not apply this filter
				this.medal_filter_value = "";
			} else {
				this.medal_filter_value = this.medals_filter_combo_box.getSelectedItem().toString();
			}

			// System.out.println("Filter search on year: " + this.year_filter_value);
			// System.out.println("Filter search on sport_filter_value: " + this.sport_filter_value);
			// System.out.println("Filter search on country_filter_value: " + this.country_filter_value);
			// System.out.println("Filter search on event_type_filter_value: " + this.event_type_filter_value);
			// System.out.println("Filter search on medal: " + this.medal_filter_value);
		}

		return is_valid;
	}


	// ==============================================
	// App Functionality:


	/**
	 * Calls exit(0), asks for confirmation over confirm dialog.
	 * 
	 * @param parent_window - Window under which the confirm
	 * 						  dialog box will appear
	 */
	static void exit(Window parent_window) {
		int response = JOptionPane.showConfirmDialog
						(parent_window, "Are you sure you want to exit the app?",
						 "Exit app", JOptionPane.YES_NO_OPTION);

		if (response == JOptionPane.YES_OPTION) {
			System.exit(0);
		}
	}

	/**
	 * Loads in the events data from given file and applies
	 * given year filter if it is a positive integer.
	 * Resets the entire app afterwards.
	 */
	void loadInEventsData(final File in_file, final int year_filter) throws Throwable
	{
		final ProgressBarDialog progress_bar_dialog = new ProgressBarDialog(
			this.select_events_data_dialog,  // parent
			150,   // width
			35,    // height
			"Load in Events Data Progress"
		);

		Thread thread = new Thread(new Runnable() {
			@Override
			public void run() {
				try {
					// clear the old database, the main window and everything
					App.this.reset();
					// load events into the database
					try {
						App.this.loadInEventsData(progress_bar_dialog.progress_bar, in_file, year_filter);
					} catch (java.io.FileNotFoundException err) {
						// ignore, correct file should have been passed
					}

					App.this.revalidate();
				} catch (Throwable err) {  // rethrow
					throw err;
				} finally {
					progress_bar_dialog.dispose();
				}
			}
		});

		thread.setUncaughtExceptionHandler(new UncaughtExceptionHandler() {
			@Override
			public void uncaughtException(Thread thread_src, Throwable error) {
				App.this.thread_uncaught_error = error;
			}
		});

		System.out.println("Suppost to load in Events file now.");

		// Start loading in events data
		thread.start();

		// make progress bar visible and start the thread
		// that loads in events data
		progress_bar_dialog.setVisible(true);

		System.out.println("Done Suppost to load in Events file now.");

		// rethrow exception if there was any and clear out the flag
		if (this.thread_uncaught_error != null) {
			Throwable to_rethrow = this.thread_uncaught_error;
			this.thread_uncaught_error = null;
			throw to_rethrow;
		}
	}

	private void loadInEventsData(JProgressBar progress_bar,
										 File in_file, int year_filter)
    	throws java.io.FileNotFoundException
	{
		System.out.println("Begin to load in Events file now.");

		DataBase db = DataBase.getInstance();
		int skipped_lines = db.parseCompetitors(in_file, year_filter, progress_bar, true);

		if (skipped_lines != 0)
			System.out.println("Skipped lines while loading in events data: " + skipped_lines);

		// Successfully loaded in events data so unlock the
		// forms functionality
		this.setBlockUserActions(true);

		// mark all graphs buttons as blocked, they cannot
		// be used before the filtering is applied for first time
		this.display_pie_chart_btn.setEnabled(false);
		//this.display_games_graph_btn.setEnabled(false);

		System.out.println("Finish to load in Events file now.");
	}

	private static void loadInAthletesData(JProgressBar progress_bar, File in_file)
    	throws java.io.FileNotFoundException
	{
		System.out.println("Begin to load in Athletes file now.");

		DataBase db = DataBase.getInstance();
		int skipped_lines = db.parseAthletes(in_file, progress_bar, true);

		if (skipped_lines != 0)
			System.out.println("Skipped lines while loading in athletes data: " + skipped_lines);


		System.out.println("Finish to load in Athletes file now.");
	}

	private void applyFilters()
	{
		final ProgressBarDialog progress_bar_dialog = new ProgressBarDialog(
			this,  // parent
			150,   // width
			35,    // height
			"Applying Filters Progress"
		);

		Thread thread = new Thread(new Runnable() {
			@Override
			public void run() {
				try {

					Filter filter = new Filter(
						DataBase.getInstance(),
						progress_bar_dialog.progress_bar
					);

					int num_athletes[] = { 0 };
					int num_disciplines[] = { 0 };
					float avg_height[] = { 0.f };
					float avg_weight[] = { 0.f };
					App.this.pie_chart_data = new HashMap<>();

					// set the pie chart data and get results data:
					filter.PerformFilter(
						num_athletes, num_disciplines, avg_height, avg_weight, App.this.pie_chart_data,
						App.this.year_filter_value,
						App.this.sport_filter_value,
						App.this.country_filter_value,
						App.this.event_type_filter_value,
						App.this.medal_filter_value
					);

					// display result in text:

					App.this.num_athletes_txt.setText("Number of athletes who participated: " + num_athletes[0]);
					App.this.num_disciplines_txt.setText("Number of disciplines on olympic games: " + num_disciplines[0]);
					App.this.avg_height_txt.setText("Average height of athletes: " + String.format("%.2f", avg_height[0]) + " cm");
					App.this.avg_weight_txt.setText("Average weight of athletes: " + String.format("%.2f", avg_weight[0]) + " kg");

					// rerender the text and everything
					App.this.revalidate();

				} catch (Throwable err) {  // rethrow
					System.out.println("Error during filtering ocurred: " + err);
					err.printStackTrace();
					throw err;
				} finally {
					progress_bar_dialog.dispose();
				}
			}
		});

		thread.setUncaughtExceptionHandler(new UncaughtExceptionHandler() {
			@Override
			public void uncaughtException(Thread thread_src, Throwable error) {
				JOptionPane.showMessageDialog
				(App.this,
				 "Failed to apply given filters",
				 App.this.getTitle(),
				 JOptionPane.ERROR_MESSAGE);
			}
		});

		System.out.println("Suppost to apply filters now.");

		// Start loading in events data
		thread.start();

		// make progress bar visible and start the thread
		// that loads in events data
		progress_bar_dialog.setVisible(true);

		System.out.println("Done Suppost to apply filters now.");
	}

}
