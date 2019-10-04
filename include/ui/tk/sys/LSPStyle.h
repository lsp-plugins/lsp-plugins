/*
 * LSPStyle.h
 *
 *  Created on: 1 окт. 2019 г.
 *      Author: sadko
 */

#ifndef UI_TK_SYS_LSPSTYLE_H_
#define UI_TK_SYS_LSPSTYLE_H_

namespace lsp
{
    namespace tk
    {
        class LSPColor;
        
        /**
         * Style Listener
         */
        class IStyleListener
        {
            public:
                virtual ~IStyleListener();

            public:
                /**
                 * Notify about property value change
                 * @param property property identifier
                 */
                virtual void notify(ui_atom_t property);
        };

        /**
         * Some widget style. Allows nesting
         */
        class LSPStyle
        {
            protected:
                typedef struct property_t
                {
                    ui_atom_t           id;         // Unique identifier of property
                    ssize_t             type;       // Type of property
                    size_t              refs;       // Number of references
                    size_t              changes;    // Number of changes
                    bool                dfl;        // Default value
                    union
                    {
                        ssize_t     iValue;
                        float       fValue;
                        bool        bValue;
                        char       *sValue;
                    } v;
                } property_t;

                typedef struct listener_t
                {
                    ui_atom_t           nId;        // Property identifier
                    IStyleListener     *pListener;  // Listener
                } listener_t;

            private:
                LSPStyle               *pParent;
                cvector<LSPStyle>       vChildren;
                cstorage<property_t>    vProperties;
                cstorage<listener_t>    vListeners;

            public:
                explicit LSPStyle();
                virtual ~LSPStyle();

                status_t            init();
                void                destroy();

            protected:
                void                undef_property(property_t *property);
                void                do_destroy();
                static property_t  *get_property_recursive(const LSPStyle *curr, ui_atom_t id);
                property_t         *get_property(ui_atom_t id);
                status_t            set_property(ui_atom_t id, property_t *src);
                status_t            sync_property(property_t *p);
                property_t         *create_property(ui_atom_t id, const property_t *src);
                property_t         *create_property(ui_atom_t id, ui_property_type_t type);
                status_t            set_property_default(property_t *dst);
                status_t            copy_property(property_t *dst, const property_t *src);
                inline const property_t   *get_property(ui_atom_t id) const { return const_cast<LSPStyle *>(this)->get_property(id); };

                void                sync();
                void                notify_change(const property_t *prop);
                void                notify_children(const property_t *prop);
                void                notify_listeners(const property_t *prop);

            public:
                /**
                 * Add child style
                 * @param child child style
                 * @return status of operation
                 */
                status_t            add(LSPStyle *child);

                /** Remove child style
                 *
                 * @param child child style to remove
                 * @return status of operation
                 */
                status_t            remove(LSPStyle *child);

                /**
                 * Set parent style
                 * @param parent parent style
                 * @return statys of operation
                 */
                status_t            set_parent(LSPStyle *parent);

                /**
                 * Get parent style
                 * @return parent style
                 */
                inline LSPStyle    *parent()        { return pParent; };

                /**
                 * Check whether style has a child
                 * @param child child style
                 * @param recursive flag that indicates that a recursive search should be performed
                 * @return true if style has a child
                 */
                bool                has_child(LSPStyle *child, bool recursive = false);

                /**
                 * Check whether style has a parent
                 * @param parent parent style
                 * @param recursive flag that indicates that a recursive search should be performed
                 * @return true if style has a parent
                 */
                bool                has_parent(LSPStyle *parent, bool recursive = false);

                /**
                 * Get root style
                 * @return root style
                 */
                LSPStyle           *root();

            public:
                /**
                 * Bind listener to property
                 * @param id property identifier
                 * @return status of operation
                 */
                status_t            bind(ui_atom_t id, ui_property_type_t type, IStyleListener *listener);

                inline status_t     bind_int(ui_atom_t id, IStyleListener *listener)        { return bind(id, PT_INT, listener); };
                inline status_t     bind_float(ui_atom_t id, IStyleListener *listener)      { return bind(id, PT_FLOAT, listener); };
                inline status_t     bind_bool(ui_atom_t id, IStyleListener *listener)       { return bind(id, PT_BOOL, listener); };
                inline status_t     bind_string(ui_atom_t id, IStyleListener *listener)     { return bind(id, PT_STRING, listener); };

                /**
                 * Check that listener is already bound to the property
                 * @param id property identifier
                 * @param listener listener
                 * @return true if listener is bound
                 */
                bool                is_bound(ui_atom_t id, IStyleListener *listener) const;

                /**
                 * Unbind listener from a property
                 * @param id property identifier
                 * @param listener property listener
                 * @return status of operation
                 */
                status_t            unbind(ui_atom_t id, IStyleListener *listener);

            public:
                /**
                 * Return overall number of properties
                 * @return overall number of properties
                 */
                inline size_t       properties() const  { return vProperties.size(); }

                /**
                 * Return overall number of listeners
                 * @return overall number of listeners
                 */
                inline size_t       listeners() const   { return vListeners.size(); }

            public:
                status_t            get_int(ui_atom_t id, ssize_t *dst) const;
                status_t            get_float(ui_atom_t id, float *dst) const;
                status_t            get_bool(ui_atom_t id, bool *dst) const;
                status_t            get_string(ui_atom_t id, LSPString *dst) const;
                status_t            get_string(ui_atom_t id, const char **dst) const;
                bool                exists(ui_atom_t id) const;
                bool                is_default(ui_atom_t id) const;
                ssize_t             get_type(ui_atom_t id) const;

                status_t            set_int(ui_atom_t id, ssize_t value);
                status_t            set_float(ui_atom_t id, float value);
                status_t            set_bool(ui_atom_t id, bool value);
                status_t            set_string(ui_atom_t id, const LSPString *value);
                status_t            set_string(ui_atom_t id, const char *value);
                status_t            set_default(ui_atom_t id);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_SYS_LSPSTYLE_H_ */
