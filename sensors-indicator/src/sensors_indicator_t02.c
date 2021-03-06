/**************************************************************************************** 
   sensors_indicator_t02.c:
   - testing the daemon

   compile with : 
   ./configure 
   make

****************************************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string>

#include <gtk/gtk.h>
#include <libappindicator/app-indicator.h>

typedef struct app_struct {
    GtkWidget* window_;
    AppIndicator* indicator_;
}app_struct;

static const gchar *ui_info =
    "<ui>"
    "  <popup name='IndicatorPopup'>"
    "    <menuitem action='Core1' />"
    "    <menuitem action='Core2' />"
    "    <menuitem action='Quit'  />"
    "  </popup>"
    "</ui>";

void nothing() {};

gboolean refresh_temperature(app_struct* app)
{
    GtkWidget *indicator_menu;
    GtkActionGroup *action_group;
    GtkUIManager *uim;
    GError *error = NULL;

    FILE* pipe_sensors =
        popen("sensors | awk '/^Core/ {print $3} ' | sed s/+// | sed s/°C// ", "r");
    if(!pipe_sensors)
        return -1;
    
    std::string c1_entry="Core 1:    ";
    std::string c2_entry="Core 2:    ";

    int core_temp[2], iCore=0;
    char buffer[10];
    while(fgets(buffer, sizeof(buffer), pipe_sensors) != NULL && iCore<2)
    {
        core_temp[iCore] = atoi(buffer);
        iCore++;
    }
    pclose(pipe_sensors);

    c1_entry += std::to_string(core_temp[0]) + "°";
    c2_entry += std::to_string(core_temp[1]) + "°";

    
    GtkActionEntry entries[] = {
        { "SensorsMenu", NULL, "_Sensors" },
        { "Core1", "", c1_entry.c_str(), "", "", G_CALLBACK(nothing) },
        { "Core2", "", c2_entry.c_str(), "", "", G_CALLBACK(nothing) },
        { "Quit", "application-exit", "_Quit", "<control>Q",
          "Exit the application", G_CALLBACK(gtk_main_quit) },
    };
    static guint n_entries = G_N_ELEMENTS(entries);

    /* Menus */
    action_group = gtk_action_group_new("AppActions");
    gtk_action_group_add_actions(action_group, entries, n_entries, app->window_);
    uim = gtk_ui_manager_new();
    gtk_ui_manager_insert_action_group(uim, action_group, 0);
    /* gtk_action_set_sensitive(gtk_action_group_get_action(action_group, */
    /*                                                      "Core1"), 0); */
    if(!gtk_ui_manager_add_ui_from_string(uim, ui_info, -1, &error))
    {
        g_message("Failed to build menus: %s\n", error->message);
        g_error_free(error);
        error = NULL;
    }
    
    /* Indicator */
    indicator_menu = gtk_ui_manager_get_widget(uim, "/ui/IndicatorPopup");

    std::string icon_name = "/usr/share/pixmaps/sensors_cold.svg";
    if(core_temp[0] > 50 || core_temp[1] > 50)
        icon_name = "/usr/share/pixmaps/sensors_warm.svg";
    else if(core_temp[0] > 60 || core_temp[1] > 60)
        icon_name = "/usr/share/pixmaps/sensors_hot.svg";

    app_indicator_set_icon(app->indicator_, icon_name.c_str());

    app_indicator_set_menu(app->indicator_, GTK_MENU(indicator_menu));
    /* set status */
    app_indicator_set_status(app->indicator_, APP_INDICATOR_STATUS_ACTIVE);

    return TRUE;
}

int main(int argc, char **argv)
{
    /* DAEMON SETUP */
    /* Our process ID and Session ID */
    pid_t pid, sid;

    /* Fork off the parent process */
    pid = fork();
    if(pid < 0)
        exit(EXIT_FAILURE);

    /* If we got a good PID, then
       we can exit the parent process. */
    if(pid > 0)
        exit(EXIT_SUCCESS);

    /* Change the file mode mask */
    umask(0);

    /* Open any logs here */

    /* Create a new SID for the child process */
    sid = setsid();
    if(sid < 0)
        exit(EXIT_FAILURE);

    /* Change the current working directory */
    if((chdir("/")) < 0)
        exit(EXIT_FAILURE);

    /* Close out the standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    /**********************************************************************************/
    /* PROGRAM */
    /* define variables gtk*/
    GtkWidget* window;
    AppIndicator* indicator = app_indicator_new("sensors-client",
                                                 "/usr/share/pixmaps/sensors_cold.svg",
                                                 APP_INDICATOR_CATEGORY_APPLICATION_STATUS);

    /* 10 sec default refresh time */
    int refresh_time=10;
    if(argc > 1)
        refresh_time=atoi(argv[1]);

    /* gtk init with no args */
    gtk_init(0, NULL);

    /* main window */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Sensors Indicator");
    g_signal_connect(G_OBJECT(window),
                      "destroy",
                      G_CALLBACK(gtk_main_quit),
                      NULL);

    /* fill gtk_indicator struct */
    app_struct gtk_indicator = {window, indicator};

    g_timeout_add_seconds(refresh_time,
                          (GSourceFunc)refresh_temperature,
                          (gpointer)&gtk_indicator);
    gtk_main();

    return 0;

}
