/*
 * Gtk2Window.cpp
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#include <ui/gtk2/ui.h>

namespace lsp
{
    Gtk2Grid::Gtk2Grid(plugin_ui *ui): Gtk2Container(ui, W_GRID)
    {
        nRows       = 1;
        nCols       = 1;
        nCurrRow    = 0;
        nCurrCol    = 0;
        pWidget     = gtk_table_new(nRows, nCols, FALSE);
        vBitmap     = NULL;
        nVSpacing   = 0;
        nHSpacing   = 0;
    }

    Gtk2Grid::~Gtk2Grid()
    {
        bitmap_delete();
    }

    void Gtk2Grid::set(widget_attribute_t att, const char *value)
    {
        switch (att)
        {
            case A_ROWS:
                PARSE_INT(value, nRows = size_t(__));
                gtk_table_resize(GTK_TABLE(pWidget), nRows, nCols);
                break;
            case A_COLS:
                PARSE_INT(value, nCols = size_t(__));
                gtk_table_resize(GTK_TABLE(pWidget), nRows, nCols);
                break;
            case A_VSPACING:
                PARSE_INT(value, nVSpacing = size_t(__));
                break;
            case A_HSPACING:
                PARSE_INT(value, nHSpacing = size_t(__));
                break;

            default:
                Gtk2Container::set(att, value);
                break;
        }
    }

    void Gtk2Grid::begin()
    {
        nCurrRow        = 0;
        nCurrCol        = 0;

        bitmap_new();
    }

    void Gtk2Grid::bitmap_new()
    {
        size_t bytes    = (nRows * nCols + (sizeof(char) * 8) - 1) / (sizeof(char) * 8);
        vBitmap         = new char[bytes];
        for (size_t i=0; i<bytes; ++i)
            vBitmap[i] = 0;
    }

    void Gtk2Grid::bitmap_delete()
    {
        if (vBitmap != NULL)
        {
            delete[] vBitmap;
            vBitmap    = NULL;
        }
    }

    void Gtk2Grid::bitmap_set(size_t x, size_t y)
    {
        if ((x >= nCols) || (y >= nRows))
            return;
        size_t index = y * nCols + x;
        vBitmap[index / (sizeof(char) * 8)] |= (1 << (index % (sizeof(char) * 8)));
    }

    bool Gtk2Grid::bitmap_get(size_t x, size_t y)
    {
        if ((x >= nCols) || (y >= nRows))
            return false;
        size_t index = y * nCols + x;

        return vBitmap[index / (sizeof(char) * 8)] & (1 << (index % (sizeof(char) * 8)));
    }

    void Gtk2Grid::end()
    {
        bitmap_delete();

        gtk_table_set_row_spacings(GTK_TABLE(pWidget), nVSpacing);
        gtk_table_set_col_spacings(GTK_TABLE(pWidget), nHSpacing);

        Gtk2Container::end();
    }

    void Gtk2Grid::add(IWidget *widget)
    {
        Gtk2Widget *g_widget = static_cast<Gtk2Widget *>(widget);

        size_t rowspan = 1, colspan = 1;
        if (widget->getClass() == W_CELL)
        {
            Gtk2Cell *g_cell = static_cast<Gtk2Cell *>(widget);
            rowspan     = g_cell->getRowspan();
            colspan     = g_cell->getColspan();
        }

        GtkAttachOptions opts = (g_widget->fill()) ? GTK_FILL : GTK_EXPAND;

        gtk_table_attach(
            GTK_TABLE(pWidget), g_widget->widget(),
            nCurrCol, nCurrCol + colspan, nCurrRow, nCurrRow + rowspan,
            opts, opts, nHSpacing, nVSpacing);

        // Fill bitmap
        for (size_t y=0; y < rowspan; ++y)
            for (size_t x=0; x < colspan; ++x)
                bitmap_set(nCurrCol + x, nCurrRow + y);

        // Horizontally place elements
        while (bitmap_get(nCurrCol, nCurrRow))
        {
            if ((++nCurrCol) >= nCols)
            {
                nCurrCol    = 0;
                nCurrRow    ++;
            }
        }
    }

} /* namespace lsp */
