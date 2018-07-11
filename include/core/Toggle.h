/*
 * Toggle.h
 *
 *  Created on: 08 апр. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_TOGGLE_H_
#define CORE_TOGGLE_H_

namespace lsp
{
    /** Simple toggle class
     *
     */
    class Toggle
    {
        private:
            enum state_t
            {
                TRG_OFF,
                TRG_PENDING,
                TRG_ON
            };

            float           fValue;
            state_t         nState;

        public:
            inline Toggle()
            {
                fValue          = 0.0f;
                nState          = TRG_OFF;
            }

            inline ~Toggle()
            {
                fValue          = 0.0f;
                nState          = TRG_OFF;
            }

        public:
            /** Initialize toggle
             *
             */
            inline void init()
            {
                fValue          = 0.0f;
                nState          = TRG_OFF;
            }

            /** Submit toggle state
             *
             * @param value toggle value [0..1]
             */
            inline void submit(float value)
            {
                if (value >= 0.5f)
                {
                    if (nState == TRG_OFF)
                        nState      = TRG_PENDING;
                }
                else
                {
                    if (nState == TRG_ON)
                        nState      = TRG_OFF;
                }
                fValue      = value;
            }

            /** Check that there is pending request for the toggle
             *
             * @return true if there is pending request for the toggle
             */
            inline bool pending() const
            {
                return nState == TRG_PENDING;
            }

            /** Commit the pending request of the toggle
             * @param off disable pending state only if toggle is in OFF state
             */
            inline void commit(bool off = false)
            {
                if (nState != TRG_PENDING)
                    return;
                if (off)
                {
                    if (fValue < 0.5f)
                        nState      = TRG_OFF;
                }
                else
                    nState      = (fValue >= 0.5f) ? TRG_ON : TRG_OFF;
            }
    };

} /* namespace lsp */

#endif /* TOGGLE_H_ */
