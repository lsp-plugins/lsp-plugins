/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 12 июл. 2017 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef UI_CTL_CTLMETER_H_
#define UI_CTL_CTLMETER_H_

namespace lsp
{
    namespace ctl
    {
        class CtlMeter: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                enum flags_t
                {
                    MF_MIN      = 1 << 0,
                    MF_MAX      = 1 << 1,
                    MF_LOG      = 1 << 2,
                    MF_LOG_SET  = 1 << 3,
                    MF_BALANCE  = 1 << 4,
                    MF_REV      = 1 << 5,
                    MF_ACT0     = 1 << 6,
                    MF_ACT1     = 1 << 7
                };

                enum type_t
                {
                    MT_PEAK,
                    MT_VU,
                    MT_RMS_PEAK
                };

            protected:
                CtlPort        *pPort[2];
                size_t          nFlags;
                size_t          nType;
                float           fMin;
                float           fMax;
                float           fBalance;
                float           fValue[2];
                float           fRms[2];
                float           fReport[2];
                char           *pActivityID[2];
                bool            bStereo;

                CtlPadding      sPadding;
                CtlColor        sColor[2];
                CtlExpression   sActivity[2];
                LSPTimer        sTimer;

            protected:
                void            drop_data();
                float           calc_value(const port_t *p, float value);
                static status_t update_meter(timestamp_t ts, void *arg);
                void            format_meter(float value, char *buf, size_t n) const;
                void            update_peaks(timestamp_t ts);
                void            set_meter_text(const port_t *p, LSPMeter *mtr, size_t id, float value);

                status_t        slot_show(void *ptr, void *data);
                status_t        slot_hide(void *ptr, void *data);

            public:
                explicit CtlMeter(CtlRegistry *src, LSPMeter *widget);
                virtual ~CtlMeter();

                virtual void destroy();

            public:
                virtual void init();

                virtual void set(widget_attribute_t att, const char *value);

                virtual void end();

                virtual void notify(CtlPort *port);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLMETER_H_ */
