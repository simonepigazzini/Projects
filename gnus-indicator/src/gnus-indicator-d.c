/**************************************************************************************** 

   gnus-indicator-d_v0:
   - this deamon create a unity indicator in the unity panel for the gnus email client
   - status update must be passed as argv[1] at startup
   - status management is performed throught the 'refresh-gnus-indicator' script

   compile with : 
   gcc gnus-indicator-d.c 
       gnus-indicator-d 
       `pkg-config --cflags --libs gtk+-2.0` 
       `pkg-config --cflags --libs appindicator-0.1` 

****************************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>

#include <gtk/gtk.h>
#include <libappindicator/app-indicator.h>

/****************************************************************************************/

static GtkActionEntry entries[] = {
    { "FileMenu", NULL, "_File" },
    { "Quit",     "application-exit", "_Quit", "<control>Q",
      "Exit the application", G_CALLBACK (gtk_main_quit) },
};

/****************************************************************************************/

static guint n_entries = G_N_ELEMENTS (entries);

/****************************************************************************************/

static const gchar *ui_info =
    "<ui>"
    "  <popup name='IndicatorPopup'>"
    "    <menuitem action='Quit' />"
    "  </popup>"
    "</ui>";

/****************************************************************************************/
/* MAIN */

int main (int argc, char **argv)
{
    /**********************************************************************************/
    /* DAEMON SETUP */

    /* Our process ID and Session ID */
    pid_t pid, sid;

    /* Fork off the parent process */
    pid = fork();
    if (pid < 0) 
        exit(EXIT_FAILURE);

    /* If we got a good PID, then
       we can exit the parent process. */
    if (pid > 0) 
        exit(EXIT_SUCCESS);

    /* Change the file mode mask */
    umask(0);

    /* Open any logs here */        

    /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0) 
        exit(EXIT_FAILURE);

    /* Change the current working directory */
    if ((chdir("/")) < 0) 
        exit(EXIT_FAILURE);

    /* Close out the standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    /**********************************************************************************/
    /* PROGRAM */

    /* define variables gtk*/
    GtkWidget *window;
    GtkWidget *indicator_menu;
    GtkActionGroup *action_group;
    GtkUIManager *uim;
    GError *error = NULL;
    AppIndicator *indicator;
    /* define custum variables */
    int status = atoi(argv[1]);
    
    /* gtk init with no args */
    gtk_init (0, NULL);

    /* main window */
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "Gnus Indicator");
    g_signal_connect (G_OBJECT (window),
                      "destroy",
                      G_CALLBACK (gtk_main_quit),
                      NULL);

    /* Menus */
    action_group = gtk_action_group_new ("AppActions");
    gtk_action_group_add_actions (action_group, entries, n_entries, window);
    uim = gtk_ui_manager_new ();
    gtk_ui_manager_insert_action_group (uim, action_group, 0);
    if (!gtk_ui_manager_add_ui_from_string (uim, ui_info, -1, &error))
    {
        g_message ("Failed to build menus: %s\n", error->message);
        g_error_free (error);
        error = NULL;
    }
    
    /* Indicator */
    indicator_menu = gtk_ui_manager_get_widget (uim, "/ui/IndicatorPopup");

    indicator = app_indicator_new ("example-simple-client",
                                   "/usr/share/pixmaps/gnus_indicator_base.png",
                                   APP_INDICATOR_CATEGORY_APPLICATION_STATUS);

    app_indicator_set_attention_icon(indicator,
                                     "/usr/share/pixmaps/gnus_indicator_attention.png");
    app_indicator_set_menu (indicator, GTK_MENU (indicator_menu));
    /* set status */
    if(status == 0)
        app_indicator_set_status (indicator, APP_INDICATOR_STATUS_ACTIVE);
    else if(status == 1)
        app_indicator_set_status (indicator, APP_INDICATOR_STATUS_ATTENTION);
    else
        app_indicator_set_status (indicator, APP_INDICATOR_STATUS_PASSIVE);
    
    gtk_main();

    return 0;
}
