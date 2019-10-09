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
                enum flags_t
                {
                    F_DEFAULT           = 1 << 0,
                    F_NTF_LISTENERS     = 1 << 1,
                    F_NTF_CHILDREN      = 1 << 2
                };

                typedef struct property_t
                {
                    ui_atom_t           id;         // Unique identifier of property
                    ssize_t             type;       // Type of property
                    size_t              refs;       // Number of references
                    size_t              changes;    // Number of changes
                    size_t              flags;      // Flags
                    LSPStyle           *owner;      // Style that is owning a property
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
                cvector<LSPStyle>       vParents;
                cvector<LSPStyle>       vChildren;
                cstorage<property_t>    vProperties;
                cstorage<listener_t>    vListeners;
                ssize_t                 nLock;
                bool                    bDelayed;

            public:
                explicit LSPStyle();
                virtual ~LSPStyle();

                status_t            init();
                void                destroy();

            protected:
                void                undef_property(property_t *property);
                void                do_destroy();
                void                delayed_notify();
                property_t         *get_property_recursive(ui_atom_t id);
                property_t         *get_parent_property(ui_atom_t id);
                property_t         *get_property(ui_atom_t id);
                status_t            set_property(ui_atom_t id, property_t *src);
                status_t            sync_property(property_t *p);
                property_t         *create_property(ui_atom_t id, const property_t *src);
                property_t         *create_property(ui_atom_t id, ui_property_type_t type);
                status_t            set_property_default(property_t *dst);
                status_t            copy_property(property_t *dst, const property_t *src);

                inline const property_t   *get_property(ui_atom_t id) const { return const_cast<LSPStyle *>(this)->get_property(id); };
                inline const property_t   *get_property_recursive(ui_atom_t id) const { return const_cast<LSPStyle *>(this)->get_property_recursive(id); };

                void                sync();
                void                notify_change(property_t *prop);
                void                notify_children(property_t *prop);
                void                notify_listeners(property_t *prop);

            public:
                /**
                 * Get number of parent styles
                 * @return number of parent styles
                 */
                inline size_t       parents() const     { return vParents.size();   }

                /**
                 * Get parent style
                 * @param idx sequential number of parent style starting with 0
                 * @return parent style or NULL if does not exist
                 */
                inline LSPStyle    *parent(size_t idx)  { return vParents.get(idx); };

                /**
                 * Set parent style
                 * @param parent parent style
                 * @param idx parent index, negative value means last
                 * @return status of operation
                 */
                status_t            add_parent(LSPStyle *parent, ssize_t idx = -1);

                /**
                 * Remove the parent style
                 * @param parent parent style to remove
                 * @return status of operation
                 */
                status_t            remove_parent(LSPStyle *parent);

                /**
                 * Check whether style has a parent
                 * @param parent parent style
                 * @param recursive flag that indicates that a recursive search should be performed
                 * @return true if style has a parent
                 */
                bool                has_parent(LSPStyle *parent, bool recursive = false);

                /**
                 * Get number of child styles
                 * @return number of child styles
                 */
                inline size_t       children() const    { return vChildren.size();   }

                /**
                 * Get child style
                 * @param idx sequential number of child style starting with 0
                 * @return child style or NULL if does not exist
                 */
                inline LSPStyle    *child(size_t idx)  { return vChildren.get(idx); };

                /**
                 * Add child style
                 * @param child child style
                 * @param idx child index, negative value means last
                 * @return status of operation
                 */
                status_t            add_child(LSPStyle *child, ssize_t idx = -1);

                /** Remove child style
                 *
                 * @param child child style to remove
                 * @return status of operation
                 */
                status_t            remove_child(LSPStyle *child);

                /**
                 * Check whether style has a child
                 * @param child child style
                 * @param recursive flag that indicates that a recursive search should be performed
                 * @return true if style has a child
                 */
                bool                has_child(LSPStyle *child, bool recursive = false);

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
                /**
                 * Start transactional update of properties.
                 * All listeners and children will be notified
                 * only after transaction becomes completed
                 */
                void                begin();

                /**
                 * End transactional update of properties.
                 * All listeners and children will be notified
                 */
                void                end();

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
