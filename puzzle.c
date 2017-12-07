#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef GDK_KEY_n
#define GDK_KEY_n GDK_n
#define GDK_KEY_o GDK_o
#define GDK_KEY_s GDK_s
#define GDK_KEY_x GDK_x
#define GDK_KEY_a GDK_a
#define GDK_KEY_v GDK_v
#endif

#define MAX_BUTTONS 15
#define GRID_SIZE 16
int MOVE_Y[2] = {0,1};
int MOVE_X[2] = {1,0};

GtkWidget *buttons[15];
int moves = 0;
int emptyButton[2] = {3,3};

GtkWidget *puzzleGrid, *statusbar;
guint statusbar_context_id;

/* window callback prototypes */
void destroy_signal(GtkWidget *widget, gpointer data);
gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data);

/* file menu callbacks prototypes */
void newGame(GtkWidget *widget, gpointer data);
void quit_menu_item(GtkWidget *widget, gpointer data);

/* help menu callback prototypes */
void about_menu_item(GtkWidget *widget, gpointer data);

void fill_grid_with_buttons(GtkWidget *grid);
void startSignalCapturing();
void doTurn(GtkWidget* button, gpointer user_data);
bool winStatus();
bool solvable(int array[]);

int
main(int argc, char *argv[])
{
	GtkWidget *window, *vbox, *menubar, *scrollw, *text_view;
	GtkWidget *file, *help;
	GtkWidget *filemenu, *helpmenu;
	GtkWidget *b_new, *b_quit;
	GtkWidget *b_about;
	GtkAccelGroup *accel_group;
  GtkTextBuffer *buffer;

	/* initialize GTK+ libraries */
	gtk_init(&argc, &argv);

	/* create main window */
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "15th Puzzle");
	gtk_window_set_default_size(GTK_WINDOW(window), 400, 400);
#if GTK_MAJOR_VERSION < 3
	gtk_window_set_default_icon_name(GTK_STOCK_EDIT);
#endif

	g_signal_connect(G_OBJECT(window), "destroy",
		G_CALLBACK(destroy_signal), NULL);
	g_signal_connect(G_OBJECT(window), "delete_event",
		G_CALLBACK(delete_event), NULL);

	/* create main container */
#if GTK_MAJOR_VERSION >= 3
	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
#else
	vbox = gtk_vbox_new(FALSE, 0);
#endif
	gtk_container_add(GTK_CONTAINER(window), vbox);

	/* create main parts - menu, scrolled window and status bar */
	menubar = gtk_menu_bar_new();

	/* content */
	scrollw = gtk_scrolled_window_new(NULL, NULL);
	text_view = gtk_text_view_new();
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
	/* fill buffer ... */
	gtk_container_add(GTK_CONTAINER(scrollw), text_view);

	/* status bar */
	statusbar = gtk_statusbar_new();

	/* setup and initialize statusbar */
	statusbar_context_id =
		gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar),
			"15th Puzzle");
	gtk_statusbar_push(GTK_STATUSBAR(statusbar), statusbar_context_id,
			g_strdup_printf("Moves: %d", moves));

	/* start accelerator group */
	accel_group = gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

	/* File pulldown menu */
	file = gtk_menu_item_new_with_mnemonic("_Game");
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file);
	filemenu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), filemenu);

	/* New button in File menu */
#if GTK_MAJOR_VERSION >= 3
	b_new = gtk_menu_item_new_with_mnemonic("_New");
#else
	b_new = gtk_image_menu_item_new_from_stock(GTK_STOCK_NEW, accel_group);
#endif
	gtk_widget_add_accelerator(b_new, "activate", accel_group,
		GDK_KEY_n, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), b_new);
	g_signal_connect(G_OBJECT(b_new), "activate", G_CALLBACK(newGame), NULL);

#if GTK_MAJOR_VERSION >= 3
	b_quit = gtk_menu_item_new_with_mnemonic("_Quit");
#else
	b_quit = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, accel_group);
#endif
	gtk_widget_add_accelerator(b_quit, "activate", accel_group,
		GDK_KEY_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), b_quit);
	g_signal_connect(G_OBJECT(b_quit), "activate", G_CALLBACK(quit_menu_item), NULL);

	/* Help pulldown menu */
	help = gtk_menu_item_new_with_mnemonic("_Help");
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), help);
	helpmenu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(help), helpmenu);

#if GTK_MAJOR_VERSION >= 3
	b_about = gtk_menu_item_new_with_mnemonic("_About");
#else
	b_about = gtk_image_menu_item_new_from_stock(GTK_STOCK_ABOUT, accel_group);
#endif
	gtk_widget_add_accelerator(b_about, "activate", accel_group,
		GDK_KEY_a, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_menu_shell_append(GTK_MENU_SHELL(helpmenu), b_about);
	g_signal_connect(G_OBJECT(b_about), "activate", G_CALLBACK(about_menu_item), NULL);


  puzzleGrid = gtk_grid_new ();
  fill_grid_with_buttons(puzzleGrid);

  gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);
  /* insert label into main window */
	gtk_box_pack_start(GTK_BOX(vbox), puzzleGrid, TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), statusbar, FALSE, FALSE, 0);


  startSignalCapturing();

	/* show the window */
	gtk_widget_show_all(window);

	/* enter GTK+ main loop */
	gtk_main();
	return 0;
}



// ######################################
void fill_grid_with_buttons(GtkWidget *grid)
{
  for (int i = 0; i < MAX_BUTTONS; i++) {
      buttons[i] = gtk_button_new_with_label(g_strdup_printf("%d", i+1));
      gtk_widget_set_size_request(buttons[i], 80,80);

      printf("%d#%d:%d\n",i, i%4, i/4);

      gtk_grid_attach(GTK_GRID (grid), buttons[i], i%4, i/4, 1, 1);
  }
}


void shuffleGrid()
{
  srand ( time(NULL) );

  int array[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  for (int i = 0; i < GRID_SIZE; i++)
  {
    size_t j = i + rand() / (RAND_MAX / (GRID_SIZE - i) + 1);
    int t = array[j];
    array[j] = array[i];
    array[i] = t;
    printf("%d\n", i);
    if(i == 15)
      break;
    g_object_ref (buttons[i]);
    gtk_container_remove(GTK_CONTAINER(puzzleGrid), buttons[i]);
  }

  for (int i = 0; i < GRID_SIZE; i++) {

    printf("%d\n", i);
    printf("%d#%d:%d\n",array[i], array[i]%4, array[i]/4);

      // buttons[i] = gtk_button_new_with_label(g_strdup_printf("%d", i+1));
      // gtk_widget_set_size_request(buttons[i], 80,80);
      //
      // printf("%d#%d:%d\n",i, i%4, i/4);
      //
    if(array[i] == 15){
      emptyButton[0] = i%4;
      emptyButton[1] = i/4;
      continue;
    }
    gtk_grid_attach(GTK_GRID (puzzleGrid), buttons[array[i]], i%4, i/4, 1, 1);
    // gtk_grid_attach(GTK_GRID (puzzleGrid), buttons[i], i%4, i/4, 1, 1);
    // gtk_grid_attach(GTK_GRID (puzzleGrid), buttons[i], i%4, i/4, 1, 1);
  }


  if(solvable(array))
    printf("Resitelne\n");
  else
    printf("Neresitelne\n");
  // gtk_grid_attach(GTK_GRID (puzzleGrid), buttons[1], 0, 0, 1, 1);
}

bool solvable(int array[])
{
  int inv_count = 0;
  for (int i = 0; i < MAX_BUTTONS; i++)
  {
      for (int j = i + 1; j < GRID_SIZE; j++)
      {
          // count pairs(i, j) such that i appears
          // before j, but i > j.
          if (array[j] >= 1 && array[i] >= 1 && array[i] > array[j])
              inv_count++;
      }
  }
  if((emptyButton[1]%2 == 0) && (inv_count%2 != 0))
    return true;
  else if(inv_count%2 == 0)
    return true;
  else
    return false;
}

void startSignalCapturing()
{
  for(int i = 0; i < MAX_BUTTONS; i++)
    g_signal_connect(buttons[i], "clicked", G_CALLBACK(doTurn), NULL);
}

bool compareArray(int a[], int b[], int c[])
{
  if(a[0] == b[0] && a[1] == b[1])
    return true;
  if(a[0] == c[0] && a[1] == c[1])
    return true;
  return false;
}

void doTurn(GtkWidget* button, gpointer user_data)
{
  int x, y;
  gtk_container_child_get(GTK_CONTAINER(puzzleGrid),
                           button,
                           "left-attach", &x,
                           "top-attach", &y, NULL);

  printf("x:%d - y:%d\n",x,y );
  printf("eX:%d - eY:%d\n",emptyButton[0],emptyButton[1] );
  printf("R:%d - R:%d\n",abs(x - emptyButton[0]),abs(y - emptyButton[1]) );

  int move[2] = {abs(x - emptyButton[0]), abs(y - emptyButton[1])};

  if(compareArray(move,MOVE_X,MOVE_Y))
  {
    gtk_container_remove(GTK_CONTAINER(puzzleGrid), button);
    gtk_grid_attach(GTK_GRID (puzzleGrid), button, emptyButton[0], emptyButton[1], 1, 1);
    moves++;
    emptyButton[0] = x;
    emptyButton[1] = y;
    gtk_statusbar_push(GTK_STATUSBAR(statusbar), statusbar_context_id,
  			g_strdup_printf("Moves: %d", moves));
  }

  if(winStatus())
    printf("Win!\n");

  printf("Cliked on: %d:%d\n",x,y);
}


bool winStatus()
{
  if(emptyButton[0] == 3 && emptyButton[1] == 3)
  {
    int x, y;
    for(int i = 0; i < MAX_BUTTONS; i++)
    {
      gtk_container_child_get(GTK_CONTAINER(puzzleGrid),
                               buttons[i],
                               "left-attach", &x,
                               "top-attach", &y, NULL);
      if(i%4 == x && i/4 == y)
        return true;
    }
    return true;
  }
  return false;
}

// ######################################


/* window destroyed */
void
destroy_signal(GtkWidget *widget, gpointer data)
{
	gtk_main_quit();
}

/* request for window close from WM */
gboolean
delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	return FALSE;   	/* send destroy signal */
}

/* edit a new file */
void newGame(GtkWidget *widget, gpointer data)
{
	g_print("New button\n");
  shuffleGrid();
}

/* quit editor */
void quit_menu_item(GtkWidget *widget, gpointer data)
{
	g_print("Quit button\n");
	gtk_main_quit();
}

/* about application */
void about_menu_item(GtkWidget *widget, gpointer data)
{
	static const gchar * const authors[] = {
		"Petr Lampa <lampa@fit.vutbr.cz>",
    "Jakub Stejskal <xstejs24[at]]stud.fit.vutbr.cz>",
		NULL
	};
	gtk_show_about_dialog (GTK_WINDOW(gtk_widget_get_toplevel(widget)),
		"authors", authors,
		"comments", "Gtk+ 15th Puzzle",
		"copyright", "Copyright \xc2\xa9 2008 Petr Lampa, Jakub Stejskal",
		"version", "1.0",
		"website", "http://www.fit.vutbr.cz",
		"program-name", "Gtk+ 15th Puzzle",
#if GTK_MAJOR_VERSION < 3
		"logo-icon-name", GTK_STOCK_EDIT,
#endif
		NULL);
}
