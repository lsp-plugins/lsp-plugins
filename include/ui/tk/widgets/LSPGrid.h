/*
 * LSPGrid.h
 *
 *  Created on: 20 июн. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_LSPGRID_H_
#define UI_TK_LSPGRID_H_

namespace lsp
{
    namespace tk
    {
        
        class LSPGrid: public LSPWidgetContainer
        {
            public:
                static const w_class_t    metadata;

            protected:
                typedef struct cell_t
                {
                    size_request_t      r;
                    realize_t           a;
                    realize_t           s;
                    padding_t           p;
                    LSPWidget          *pWidget;

                    ssize_t             nRows;      // Number of rows taken by cell
                    ssize_t             nCols;      // Number of columns taken by cell
                } cell_t;

                typedef struct header_t
                {
                    ssize_t             nMinSize;
                    ssize_t             nSize;
                    ssize_t             nSpacing;
                    ssize_t             nOffset;
                    bool                bExpand;
                } header_t;

            protected:
                size_t      nHSpacing;
                size_t      nVSpacing;
                size_t      nCurrRow;
                size_t      nCurrCol;
                bool        bVertical;

                cstorage<cell_t>    vCells;
                cstorage<header_t>  vRows;
                cstorage<header_t>  vCols;

            protected:
                virtual LSPWidget  *find_widget(ssize_t x, ssize_t y);

                void                destroy_cells();
                static inline bool  hidden_widget(const cell_t *w);
                static void         split_size(header_t *h, size_t items, size_t size);
                static void         distribute_size(cstorage<header_t> &vh, size_t idx, size_t items, size_t rq_size);
                static size_t       estimate_size(cstorage<header_t> &vh, size_t idx, size_t items, size_t *spacing);
                static void         assign_coords(header_t *h, size_t items, size_t start);
                cell_t             *alloc_cell();
                status_t            tag_cell(cell_t *c, bool main);

            public:
                explicit LSPGrid(LSPDisplay *dpy, bool horizontal = true);
                virtual ~LSPGrid();

                virtual status_t    init();
                virtual void        destroy();

            //---------------------------------------------------------------------------------
            // Properties
            public:
                /** Get number of columns
                 *
                 * @return number of columns
                 */
                inline size_t               columns() const { return vCols.size(); };

                /** Get number of rows
                 *
                 * @return number of rows
                 */
                inline size_t               rows() const    { return vRows.size(); };

                /** Get vertical spacing between cells
                 *
                 * @return vertical spacing between cells
                 */
                inline size_t               vertical_spacing() const { return nVSpacing; };

                /** Get vertical spacing between cells
                 *
                 * @return vertical spacing between cells
                 */
                inline size_t               vspacing() const { return nVSpacing; };

                /** Get horizontal spacing between cells
                 *
                 * @return horizontal spacing between cells
                 */
                inline size_t               horizontal_spacing() const { return nHSpacing; };

                /** Get horizontal spacing between cells
                 *
                 * @return horizontal spacing between cells
                 */
                inline size_t               hspacing() const { return nHSpacing; };

                /** Check that grid has vertical orientation
                 *
                 * @return true if grid has vertical orientation
                 */
                inline bool                 vertical_orientation() const { return bVertical; };

                /** Check that grid has vertical orientation
                 *
                 * @return true if grid has vertical orientation
                 */
                inline bool                 vertical() const { return bVertical; };

                /** Check that grid has horizontal orientation
                 *
                 * @return true if grid has horizontal orientation
                 */
                inline bool                 horizontal_orientation() const { return !bVertical; };

                /** Check that grid has horizontal orientation
                 *
                 * @return true if grid has horizontal orientation
                 */
                inline bool                 horizontal() const { return !bVertical; };

                /** Get grid orientation
                 *
                 * @return grid orientation
                 */
                inline orientation_t        orientation() const { return (bVertical) ? O_VERTICAL : O_HORIZONTAL; };

            //---------------------------------------------------------------------------------
            // Manipulation
            public:
                /** Set number of rows in the table
                 *
                 * @param rows number of rows in the table
                 * @return status of operation
                 */
                status_t                    set_rows(size_t rows);

                /** Set number of columns in the table
                 *
                 * @param rows number of columns in the table
                 * @return status of operation
                 */
                status_t                    set_columns(size_t columns);

                /** Set table orientation
                 *
                 * @param value table orientation
                 * @return status of operation
                 */
                status_t                    set_orientation(orientation_t value);

                /** Set table vertical orientation
                 *
                 * @param value vertical orientation flag
                 * @return status of operation
                 */
                inline status_t             set_vertical(bool value = true) { return set_orientation((value) ? O_VERTICAL : O_HORIZONTAL); }

                /** Set table horizontal orientation
                 *
                 * @param value horizontal orientation flag
                 * @return status of operation
                 */
                inline status_t             set_horizontal(bool value = true) { return set_orientation((value) ? O_HORIZONTAL : O_VERTICAL); }

                /** Set grid's vertical spacing
                 *
                 * @param value grid's vertical spacing
                 * @return status of operation
                 */
                status_t                    set_hspacing(size_t value);

                /** Set grid's horizontal spacing
                 *
                 * @param value grid's horizontal spacing
                 * @return status of operation
                 */
                status_t                    set_vspacing(size_t value);

                /** Set spacing for the grid
                 *
                 * @param hor horizontal spacing
                 * @param vert vertical spacing
                 * @return status of operation
                 */
                status_t                    set_spacing(size_t hor, size_t vert);

                /** Render widget
                 *
                 * @param s surface to render
                 * @param force force flag
                 */
                virtual void render(ISurface *s, bool force);

                /** Add widget to the grid
                 *
                 * @param widget widget to add
                 * @return status of operation
                 */
                virtual status_t add(LSPWidget *widget);

                /** Add widget to the grid
                 *
                 * @param widget widget to add
                 * @param rowspan number of rows
                 * @param colspan number of columns
                 * @return status of operation
                 */
                virtual status_t add(LSPWidget *widget, size_t rowspan, size_t colspan);

                /** Remove widget from grid
                 *
                 * @param widget widget to remove
                 * @return status of operation
                 */
                virtual status_t remove(LSPWidget *widget);

                /** Realize the widget
                 *
                 * @param r realization structure
                 */
                virtual void realize(const realize_t *r);

                /** Size request of the widget
                 *
                 * @param r size request
                 */
                virtual void size_request(size_request_t *r);

        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPGRID_H_ */
