/*
 * LSPIndexes.cpp
 *
 *  Created on: 9 авг. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        void LSPItemSelection::on_remove(ssize_t value)
        {
        }

        void LSPItemSelection::on_add(ssize_t value)
        {
        }

        void LSPItemSelection::on_fill()
        {
        }

        bool LSPItemSelection::validate(ssize_t value)
        {
            return true;
        }

        void LSPItemSelection::request_fill(ssize_t *first, ssize_t *last)
        {
            *first  = 0;
            *last   = -1;
        }

        void LSPItemSelection::on_clear()
        {
        }

        LSPItemSelection::LSPItemSelection()
        {
            bMultiple   = false;
        }

        LSPItemSelection::~LSPItemSelection()
        {
            vIndexes.flush();
        }

        bool LSPItemSelection::contains(ssize_t value) const
        {
            cstorage<ssize_t> *s = const_cast<cstorage<ssize_t> *>(&vIndexes);

            ssize_t first   = 0, last = s->size();
            ssize_t *ptr    = s->get_array();

            while (first < last)
            {
                ssize_t center = (first + last) >> 1;
                ssize_t v = ptr[center];

                if (v < value)
                    first = ++center;
                else if (v > value)
                    last  = --center;
                else
                    return true;
            }

            return false;
        }

        status_t LSPItemSelection::put(ssize_t value)
        {
            if (!bMultiple)
                return set_value(value);

            if (!validate(value))
                return STATUS_BAD_ARGUMENTS;

            ssize_t first   = 0, last = vIndexes.size();
            ssize_t *ptr    = vIndexes.get_array();

            while (first < last)
            {
                ssize_t center = (first + last) >> 1;
                ssize_t v = ptr[center];

                if (v < value)
                    first = ++center;
                else if (v > value)
                    last  = --center;
                else
                    break;
            }

            if (!vIndexes.insert(first, &value))
                return STATUS_NO_MEM;

            on_add(value);
            return STATUS_OK;
        }

        status_t LSPItemSelection::remove(ssize_t value)
        {
            ssize_t first   = 0, last = vIndexes.size();
            ssize_t *ptr    = vIndexes.get_array();

            while (first < last)
            {
                ssize_t center = (first + last) >> 1;
                ssize_t v = ptr[center];

                if (v < value)
                    first = ++center;
                else if (v > value)
                    last  = --center;
                else
                {
                    if (!vIndexes.remove(center))
                        return STATUS_NO_MEM;
                    on_remove(value);
                }
            }

            return STATUS_OK;
        }

        ssize_t LSPItemSelection::get(size_t index) const
        {
            cstorage<ssize_t> *s = const_cast<cstorage<ssize_t> *>(&vIndexes);
            ssize_t *ptr = s->get(index);
            return (ptr != NULL) ? *ptr : -1;
        }

        void LSPItemSelection::clear()
        {
            vIndexes.flush();
            on_clear();
        }

        void LSPItemSelection::fill()
        {
            ssize_t first = 0, last = -1;
            request_fill(&first, &last);
            vIndexes.clear();

            while (first <= last)
            {
                vIndexes.add(&first);
                first++;
            }

            on_fill();
        }

        ssize_t LSPItemSelection::value() const
        {
            return get(0);
        }

        status_t LSPItemSelection::set_value(ssize_t value)
        {
            // Check if it is command to clear all settings
            if (value < 0)
            {
                clear();
                return STATUS_OK;
            }

            // Validate value
            if (!validate(value))
                return STATUS_BAD_ARGUMENTS;

            // Prevent from re-setting value
            size_t n        = vIndexes.size();
            if (n == 1)
            {
                ssize_t *idx = vIndexes.get(0);
                if (*idx == value)
                    return STATUS_OK;
            }

            // Create new set of values
            cstorage<ssize_t> items;
            if (!items.add(value))
                return STATUS_NO_MEM;

            // Swap collections
            vIndexes.swap(&items);

            // Clear previous values
            ssize_t *ptr    = items.get_array();
            for (size_t i=0; i<n; ++i)
            {
                if (*ptr != value)
                    on_remove(*ptr);
                ptr++;
            }
            on_add(value);

            items.flush();

            return STATUS_OK;
        }

        status_t LSPItemSelection::toggle_value(ssize_t value)
        {
            if (bMultiple)
            {
                if (value < 0)
                    return STATUS_OK;
                if (!validate(value))
                    return STATUS_BAD_ARGUMENTS;

                // Try to remove
                ssize_t first   = 0, last = vIndexes.size();
                ssize_t *ptr    = vIndexes.get_array();

                while (first < last)
                {
                    ssize_t center = (first + last) >> 1;
                    ssize_t v = ptr[center];

                    if (v < value)
                        first = ++center;
                    else if (v > value)
                        last  = --center;
                    else
                    {
                        if (!vIndexes.remove(center))
                            return STATUS_NO_MEM;
                        on_remove(value);
                        return STATUS_OK;
                    }
                }

                // Did not remove, add
                if (!vIndexes.insert(first, &value))
                    return STATUS_NO_MEM;

                on_add(value);
                return STATUS_OK;
            }
            else
            {
                ssize_t *idx = vIndexes.get(0);
                if ((idx == NULL) || (*idx != value))// Item is not present
                    set_value(value);
                else
                    set_value(-1);
            }

            return STATUS_OK;
        }

        status_t LSPItemSelection::insert_item(ssize_t value)
        {
            ssize_t first   = 0, last = vIndexes.size();
            ssize_t *ptr    = vIndexes.get_array();
            ssize_t n       = vIndexes.size();

            while (first < last)
            {
                ssize_t center  = (first + last) >> 1;
                ssize_t v       = ptr[center];

                if (v < value)
                    first = ++center;
                else if (v > value)
                    last  = --center;
                else
                {
                    first = center;
                    break;
                }
            }

            // Increment by 1 all numbers in list
            while (first < n)
                ptr[first++]++;

            return STATUS_OK;
        }

        status_t LSPItemSelection::remove_item(ssize_t value)
        {
            ssize_t first   = 0, last = vIndexes.size();
            ssize_t *ptr    = vIndexes.get_array();
            ssize_t n       = vIndexes.size();

            while (first < last)
            {
                ssize_t center  = (first + last) >> 1;
                ssize_t v       = ptr[center];

                if (v < value)
                    first = ++center;
                else if (v > value)
                    last  = --center;
                else
                {
                    vIndexes.remove(center);
                    first   = center;
                    ptr     = vIndexes.get_array();
                    n       --;
                    break;
                }
            }

            // Decrement by 1 all numbers in list
            while (first < n)
                ptr[first++]--;

            return STATUS_OK;
        }

        status_t LSPItemSelection::swap_items(ssize_t index1, ssize_t index2)
        {
            // Validate values
            if (!validate(index1))
                return STATUS_BAD_ARGUMENTS;
            if (!validate(index2))
                return STATUS_BAD_ARGUMENTS;

            // Find locations
            ssize_t loc1 = -1, loc2 = -1;
            ssize_t *ptr    = vIndexes.get_array();

            // Find location of the first item
            ssize_t first   = 0, last = vIndexes.size();
            while (first < last)
            {
                ssize_t center = (first + last) >> 1;
                ssize_t v = ptr[center];

                if (v < index1)
                    first = ++center;
                else if (v > index1)
                    last  = --center;
                else
                {
                    loc1     = center;
                    break;
                }
            }

            // Find location of the second item
            first   = 0, last = vIndexes.size();
            while (first < last)
            {
                ssize_t center = (first + last) >> 1;
                ssize_t v = ptr[center];

                if (v < index2)
                    first = ++center;
                else if (v > index2)
                    last  = --center;
                else
                {
                    loc2     = center;
                    break;
                }
            }

            // Check if both values are of the same state
            if (((loc1 < 0) && (loc2 < 0)) || ((loc1 >= 0) && (loc2 >= 0)))
                return STATUS_OK;

            // Add item with other identifier
            ssize_t value   = (loc1 >= 0) ? loc2 : loc1;
            if (loc1 < 0)
                loc1            = loc2;

            first   = 0, last = vIndexes.size();
            ptr     = vIndexes.get_array();

            while (first < last)
            {
                ssize_t center = (first + last) >> 1;
                ssize_t v = ptr[center];

                if (v < value)
                    first = ++center;
                else if (v > value)
                    last  = --center;
                else
                    break;
            }

            if (!vIndexes.insert(first, &value))
                return STATUS_NO_MEM;

            // Now we need to remove previous item
            if (first <= loc1)
                loc1++;
            vIndexes.remove(loc1);

            return STATUS_OK;
        }

        status_t LSPItemSelection::set_multiple(bool multiple)
        {
            if (multiple == bMultiple)
                return STATUS_OK;
            bMultiple = multiple;
            if ((bMultiple) || (vIndexes.size() < 2))
                return STATUS_OK;

            ssize_t *ptr = vIndexes.get(0);
            return set_value(*ptr);
        }
    } /* namespace tk */
} /* namespace lsp */
