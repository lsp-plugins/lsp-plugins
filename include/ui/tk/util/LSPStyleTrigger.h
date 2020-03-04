/*
 * LSPStyleTrigger.h
 *
 *  Created on: 4 мар. 2020 г.
 *      Author: sadko
 */

#ifndef UI_TK_UTIL_LSPSTYLETRIGGER_H_
#define UI_TK_UTIL_LSPSTYLETRIGGER_H_

namespace lsp
{
    namespace tk
    {
        enum style_trigger_action_t
        {
            STYLE_TRG_REDRAW,
            STYLE_TRG_RESIZE
        };
        
        class LSPWidget;

        class LSPStyleTrigger: public IStyleListener
        {
            private:
                LSPStyleTrigger & operator = (const LSPStyleTrigger &);

            protected:
                typedef struct binding_t
                {
                    ui_atom_t               id;
                    style_trigger_action_t  action;
                } binding_t;

            protected:
                LSPWidget              *pWidget;
                cstorage<binding_t>     vBindings;

            public:
                explicit LSPStyleTrigger(LSPWidget *widget);
                virtual ~LSPStyleTrigger();

            public:
                virtual void notify(ui_atom_t property);

            public:
                /**
                 * Bind trigger to the specified atom and data type
                 * @param id style property identifier
                 * @param type property type
                 * @param action trigger action
                 * @return status of operation
                 */
                status_t            bind(ui_atom_t id, ui_property_type_t type, style_trigger_action_t action);

                /**
                 * Bind trigger to the specified atom and data type
                 * @param id style property name
                 * @param type property type
                 * @param action trigger action
                 * @return status of operation
                 */
                status_t            bind(const char *name, ui_property_type_t type, style_trigger_action_t action);

                /**
                 * Bind trigger to the specified atom and data type
                 * @param id style property name
                 * @param type property type
                 * @param action trigger action
                 * @return status of operation
                 */
                status_t            bind(const LSPString *name, ui_property_type_t type, style_trigger_action_t action);

                inline status_t     bind_int(ui_atom_t id, style_trigger_action_t action)               { return bind(id, PT_INT, action); };
                inline status_t     bind_float(ui_atom_t id, style_trigger_action_t action)             { return bind(id, PT_FLOAT, action); };
                inline status_t     bind_bool(ui_atom_t id, style_trigger_action_t action)              { return bind(id, PT_BOOL, action); };
                inline status_t     bind_string(ui_atom_t id, style_trigger_action_t action)            { return bind(id, PT_STRING, action); };

                inline status_t     bind_int(const char *name, style_trigger_action_t action)           { return bind(name, PT_INT, action); };
                inline status_t     bind_float(const char *name, style_trigger_action_t action)         { return bind(name, PT_FLOAT, action); };
                inline status_t     bind_bool(const char *name, style_trigger_action_t action)          { return bind(name, PT_BOOL, action); };
                inline status_t     bind_string(const char *name, style_trigger_action_t action)        { return bind(name, PT_STRING, action); };

                inline status_t     bind_int(const LSPString *name, style_trigger_action_t action)      { return bind(name, PT_INT, action); };
                inline status_t     bind_float(const LSPString *name, style_trigger_action_t action)    { return bind(name, PT_FLOAT, action); };
                inline status_t     bind_bool(const LSPString *name, style_trigger_action_t action)     { return bind(name, PT_BOOL, action); };
                inline status_t     bind_string(const LSPString *name, style_trigger_action_t action)   { return bind(name, PT_STRING, action); };

                /**
                 * Check that trigger is already bound with the specified action
                 * @param id property identifier
                 * @param listener listener
                 * @return true if listener is bound
                 */
                bool                is_bound(ui_atom_t id, style_trigger_action_t action) const;

                /**
                 * Check that trigger is already bound with the specified action
                 * @param id property name
                 * @param listener listener
                 * @return true if listener is bound
                 */
                bool                is_bound(const char *name, style_trigger_action_t action) const;

                /**
                 * Check that trigger is already bound with the specified action
                 * @param id property name
                 * @param listener listener
                 * @return true if listener is bound
                 */
                bool                is_bound(const LSPString *name, style_trigger_action_t action) const;

                /**
                 * Unbind trigger
                 * @param id style property identifier
                 * @param listener property listener
                 * @return status of operation
                 */
                status_t            unbind(ui_atom_t id, style_trigger_action_t action);

                /**
                 * Unbind trigger
                 * @param name name of the style property
                 * @param listener property listener
                 * @return status of operation
                 */
                status_t            unbind(const char *name, style_trigger_action_t action);

                /**
                 * Unbind trigger
                 * @param name name of the style property
                 * @param listener property listener
                 * @return status of operation
                 */
                status_t            unbind(const LSPString *name, style_trigger_action_t action);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_UTIL_LSPSTYLETRIGGER_H_ */
