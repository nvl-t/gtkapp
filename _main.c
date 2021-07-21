#include <gtk/gtk.h>
#include <vte/vte.h>
#include <glib.h>

static void
on_window_title_changed (VteTerminal *terminal, GtkWindow *window)
{
  char *title;

  title = g_strdup_printf ("%s - Terminal",
                           vte_terminal_get_window_title (terminal));
  gtk_window_set_title (window, title);
  g_free (title);
}

static void
on_eof (VteTerminal *terminal, gpointer user_data)
{
  gtk_main_quit ();
}

int
main (int argc, char **argv)
{
  GtkWidget *window, *terminal, *scrolled_win;
  GError *err = NULL;
  char *cmd = NULL;
  char **cmd_argv = NULL;
  int cmd_argc;
  GOptionContext *context = NULL;

  const GOptionEntry options[] = {
    {
          "command", 'e', 0,
          G_OPTION_ARG_STRING, &cmd,
        "Execute a command in the terminal", NULL},
    {NULL}
  };

  context =
      g_option_context_new
      ("Open matchbox terminal and parser the command line args");
  g_option_context_add_main_entries (context, options, NULL);
  g_option_context_add_group (context, gtk_get_option_group (TRUE));
  g_option_context_parse (context, &argc, &argv, &err);
  g_option_context_free (context);

  if (err != NULL) {
    g_printerr ("Failed to parse command line arguments: %s\n", err->message);
    g_error_free (err);
    return -1;
  }

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (window, "destroy", gtk_main_quit, NULL);
  gtk_window_set_title (GTK_WINDOW (window), "Terminal");
  
  scrolled_win = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(scrolled_win),
                                  GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
  gtk_container_add (GTK_CONTAINER (window), scrolled_win);

  terminal = g_object_connect
    (vte_terminal_new (),
     /* Child dying */
     "signal::child-exited", on_eof, NULL,
     "signal::eof", on_eof, NULL,
     /* Child is trying to control the terminal */
     "signal::window-title-changed", on_window_title_changed, window,
     NULL);
  gtk_container_add (GTK_CONTAINER (scrolled_win), terminal);

  if (cmd == NULL) {
    cmd = vte_get_user_shell ();
    if (!cmd)
      cmd = g_strdup (g_getenv ("SHELL"));
    if (!cmd)
      cmd = g_strdup ("/bin/sh");
  }

  if (!g_shell_parse_argv(cmd, &cmd_argc, &cmd_argv, &err)) {
    g_printerr ("Failed to parse shell command line '%s'\n", cmd);
  } else if (!vte_terminal_spawn_sync (VTE_TERMINAL (terminal),
                                       VTE_PTY_DEFAULT,
                                       NULL,
                                       cmd_argv,
                                       NULL,
                                       G_SPAWN_SEARCH_PATH,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL,
                                       &err)) {
    g_printerr ("Failed to spawn shell: %s\n", err->message);
    g_error_free (err);
  }


  gtk_window_set_default_size (GTK_WINDOW (window), 640, 480);

  gtk_widget_show_all (window);

  gtk_main ();

  g_free (cmd);
  return 0;
}
