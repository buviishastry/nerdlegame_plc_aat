#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define ROWS 6
#define COLS 8

GtkWidget *cells[ROWS][COLS];
GtkWidget *status_label;
int current_row = 0;
int current_col = 0;

char answer[COLS + 1];

const char *equations[] = {
    "12+19=31",
    "48/06=08",
    "9+10=19",
    "56-27=29",
    "34+61=95",
    "13*05=65”
};

#define EQ_COUNT (sizeof(equations) / sizeof(equations[0]))

void generate_random_equation() {
    int index = rand() % EQ_COUNT;
    strcpy(answer, equations[index]);
}

void set_cell_color(GtkWidget *entry, const char *color) {
    GtkCssProvider *provider = gtk_css_provider_new();
    char css[300];

    snprintf(css, sizeof(css),
        "entry { background-color: %s; color: black; font-size: 20px; font-weight: bold; }",
        color);

    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    gtk_style_context_add_provider(
        gtk_widget_get_style_context(entry),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER);
}

void check_guess(GtkButton *button, gpointer data) {
    char guess[COLS + 1];

    for (int i = 0; i < COLS; i++) {
        const char *t = gtk_entry_get_text(GTK_ENTRY(cells[current_row][i]));
        guess[i] = t[0];
    }
    guess[COLS] = '\0';

    for (int i = 0; i < COLS; i++) {
        if (guess[i] == answer[i]) {
            set_cell_color(cells[current_row][i], "#6aaa64");
        } else if (strchr(answer, guess[i])) {
            set_cell_color(cells[current_row][i], "#c9b458");
        } else {
            set_cell_color(cells[current_row][i], "#787c7e");
        }
        gtk_editable_set_editable(GTK_EDITABLE(cells[current_row][i]), FALSE);
    }

    if (strcmp(guess, answer) == 0) {
        char msg[150];
        snprintf(msg, sizeof(msg),
            "You guessed it in %d attempts.\nThank you for playing Nerdle.",
            current_row + 1);

        gtk_label_set_text(GTK_LABEL(status_label), msg);
        return;
    }

    current_row++;
    current_col = 0;

    if (current_row == ROWS) {
        char msg[200];
        snprintf(msg, sizeof(msg),
            "You couldn't guess it in 6 attempts.\nThe equation was: %s",
            answer);

        gtk_label_set_text(GTK_LABEL(status_label), msg);
    }
}

gboolean on_key(GtkWidget *widget, GdkEventKey *event, gpointer data) {
    if (current_row >= ROWS) return FALSE;

    if ((event->keyval >= '0' && event->keyval <= '9') ||
        event->keyval == '+' || event->keyval == '-' ||
        event->keyval == '*' || event->keyval == '/' ||
        event->keyval == '=') {

        if (current_col < COLS) {
            char s[2] = {event->keyval, '\0'};
            gtk_entry_set_text(GTK_ENTRY(cells[current_row][current_col]), s);
            current_col++;
        }
    }

    if (event->keyval == GDK_KEY_BackSpace && current_col > 0) {
        current_col--;
        gtk_entry_set_text(GTK_ENTRY(cells[current_row][current_col]), "");
    }

    return TRUE;
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    srand(time(NULL));
    generate_random_equation();

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Nerdle");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 500);
    gtk_container_set_border_width(GTK_CONTAINER(window), 15);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    GtkWidget *title = gtk_label_new("Welcome to Nerdle\nA Math-Based Wordle Game");
    gtk_widget_set_halign(title, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 5);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);

    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            cells[r][c] = gtk_entry_new();
            gtk_entry_set_max_length(GTK_ENTRY(cells[r][c]), 1);
            gtk_entry_set_width_chars(GTK_ENTRY(cells[r][c]), 2);
            gtk_entry_set_alignment(GTK_ENTRY(cells[r][c]), 0.5);
            gtk_editable_set_editable(GTK_EDITABLE(cells[r][c]), FALSE);
            gtk_grid_attach(GTK_GRID(grid), cells[r][c], c, r, 1, 1);
        }
    }

    gtk_box_pack_start(GTK_BOX(vbox), grid, FALSE, FALSE, 5);

    GtkWidget *check_btn = gtk_button_new_with_label("Check");
    gtk_box_pack_start(GTK_BOX(vbox), check_btn, FALSE, FALSE, 5);

    status_label = gtk_label_new("");
    gtk_widget_set_halign(status_label, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(vbox), status_label, FALSE, FALSE, 5);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(window, "key-press-event", G_CALLBACK(on_key), NULL);
    g_signal_connect(check_btn, "clicked", G_CALLBACK(check_guess), NULL);

    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}