/*
 * types.h
 *
 *  Created on: 02 февр. 2016 г.
 *      Author: sadko
 */

#ifndef CONTAINER_LV2_TYPES_H_
#define CONTAINER_LV2_TYPES_H_

namespace lsp
{
    typedef struct lv2_path_t: public path_t
    {
        enum flags_t
        {
            P_PATH          = 1 << 0,
            P_REQUEST       = 1 << 1,
            P_ACCEPTED      = 1 << 2
        };

        size_t      nState;
        char        sPath[PATH_MAX];
        char        sRequest[PATH_MAX];

        virtual void init()
        {
            sPath[0]    = '\0';
            sRequest[0] = '\0';
            nState      = 0;
        }

        virtual const char *get_path()
        {
            return sPath;
        }

        virtual void accept()
        {
            if (!(nState & P_PATH))
                return;
            nState  |= P_ACCEPTED;
        }

        virtual void commit()
        {
            if ((nState & (P_PATH | P_ACCEPTED)) != (P_PATH | P_ACCEPTED))
                return;
            if (nState & P_REQUEST)
            {
                strcpy(sPath, sRequest);
                nState  = P_PATH;
            }
            else
                nState  = 0;
        }

        virtual bool pending()
        {
            return (nState & (P_PATH | P_ACCEPTED)) == P_PATH;
        }

        virtual bool accepted()
        {
            return (nState & (P_PATH | P_ACCEPTED)) == (P_PATH | P_ACCEPTED);
        }

        inline void submit(const char *path, size_t len)
        {
            // Determine size of path
            size_t count = (len >= PATH_MAX) ? PATH_MAX - 1 : len;

            if ((nState & (P_PATH | P_ACCEPTED)) == 0)
            {
                memcpy(sPath, path, count);
                sPath[count]        = '\0';
                nState             |= P_PATH;
            }
            else
            {
                memcpy(sRequest, path, count);
                sRequest[count]     = '\0';
                nState             |= P_REQUEST;
            }
        }

    } lv2_path_t;
}

#endif /* CONTAINER_LV2_TYPES_H_ */
