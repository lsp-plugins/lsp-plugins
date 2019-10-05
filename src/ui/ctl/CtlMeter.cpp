/*
 * CtlMeter.cpp
 *
 *  Created on: 12 июл. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>
#include <stdio.h>

#define METER_ATT       0.1f
#define METER_REL       0.25f

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlMeter::metadata = { "CtlMeter", &CtlWidget::metadata };

        CtlMeter::CtlMeter(CtlRegistry *src, LSPMeter *widget): CtlWidget(src, widget)
        {
            pClass          = &metadata;
            pPort[0]        = NULL;
            pPort[1]        = NULL;
            nFlags          = 0;
            nType           = MT_PEAK;
            fMin            = 0.0f;
            fMax            = 0.0f;
            fBalance        = 0.0f;
            fValue[0]       = 0.0f;
            fValue[1]       = 0.0f;
            fRms[0]         = 0.0f;
            fRms[1]         = 0.0f;
            fReport[0]      = 0.0f;
            fReport[1]      = 0.0f;
            pActivityID[0]  = NULL;
            pActivityID[1]  = NULL;
            bStereo         = false;
        }

        CtlMeter::~CtlMeter()
        {
        }

        void CtlMeter::destroy()
        {
            drop_data();
            CtlWidget::destroy();
        }

        void CtlMeter::drop_data()
        {
            for (size_t i=0; i<2; ++i)
            {
                if (pActivityID[i] != NULL)
                {
                    free(pActivityID[i]);
                    pActivityID[i] = NULL;
                }
            }
        }

        float CtlMeter::calc_value(const port_t *p, float value)
        {
            bool xlog = (nFlags & MF_LOG_SET) && (nFlags & MF_LOG);

            if ((!xlog) && (p != NULL))
                xlog = is_decibel_unit(p->unit) || (p->flags & F_LOG);

            if ((xlog) && (value < GAIN_AMP_M_120_DB))
                value   = GAIN_AMP_M_120_DB;

            float mul = (p->unit == U_GAIN_AMP) ? 20.0f/M_LN10 :
                        (p->unit == U_GAIN_POW) ? 10.0f/M_LN10 :
                        1.0f;

            return (xlog) ? mul * logf(fabs(value)) : value;
        }

        void CtlMeter::init()
        {
            CtlWidget::init();

            sActivity[0].init(pRegistry, this);
            sActivity[1].init(pRegistry, this);

            LSPMeter *mtr = widget_cast<LSPMeter>(pWidget);
            if (mtr == NULL)
                return;

            sPadding.init(mtr->padding());
            mtr->set_channels(2);

            sTimer.bind(mtr->display());
            sTimer.set_handler(update_meter, this);

            sColor[0].init_basic(NULL, NULL, NULL, A_COLOR);
            sColor[1].init_basic(NULL, NULL, NULL, A_COLOR2);
        }

        void CtlMeter::set(widget_attribute_t att, const char *value)
        {
            LSPMeter *mtr = (pWidget != NULL) ? static_cast<LSPMeter *>(pWidget) : NULL;

            switch (att)
            {
                case A_ID:
                    BIND_PORT(pRegistry, pPort[0], value);
                    break;

                case A_ID2:
                    BIND_PORT(pRegistry, pPort[1], value);
                    break;

                case A_ACTIVITY_ID:
                    if (pActivityID[0] != NULL)
                        free(pActivityID[0]);
                    pActivityID[0] = strdup(value);
                    break;

                case A_ACTIVITY2_ID:
                    if (pActivityID[1] != NULL)
                        free(pActivityID[1]);
                    pActivityID[1] = strdup(value);
                    break;

                case A_MIN:
                    PARSE_FLOAT(value, fMin = __; nFlags |= MF_MIN);
                    break;

                case A_MAX:
                    PARSE_FLOAT(value, fMax = __; nFlags |= MF_MAX);
                    break;

                case A_BALANCE:
                    PARSE_FLOAT(value,
                        fBalance = __;
                        fValue[0] = __;
                        fValue[1] = __;
                        fReport[0] = __;
                        fReport[1] = __;
                        nFlags |= MF_BALANCE
                    );
                    break;

                case A_ANGLE:
                    if (mtr != NULL)
                        PARSE_INT(value, mtr->set_angle(__));
                    break;

                case A_WIDTH:
                    if (mtr != NULL)
                        PARSE_INT(value, mtr->set_mtr_width(__));
                    break;

                case A_HEIGHT:
                    if (mtr != NULL)
                        PARSE_INT(value, mtr->set_mtr_height(__));
                    break;

                case A_BORDER:
                    PARSE_INT(value, mtr->set_border(__));
                    break;

                case A_STEREO:
                    if (mtr != NULL)
                        PARSE_BOOL(value, bStereo = __);
                    break;

                case A_TEXT:
                    if (mtr != NULL)
                        PARSE_BOOL(value, mtr->set_mtr_text_used(__));
                    break;

                case A_LOGARITHMIC:
                    PARSE_BOOL(value,
                        if (__)
                            nFlags |= MF_LOG | MF_LOG_SET;
                        else
                            nFlags = (nFlags & ~MF_LOG) | MF_LOG_SET;
                    );
                    break;

                case A_TYPE:
                    if (!strcasecmp(value, "vu"))
                        nType   = MT_VU;
                    else if (!strcasecmp(value, "peak"))
                        nType   = MT_PEAK;
                    else if (!strcasecmp(value, "rms_peak"))
                        nType   = MT_RMS_PEAK;
                    break;

                case A_REVERSIVE:
                    PARSE_BOOL(value,
                        if (__)
                            nFlags |= MF_REV;
                        else
                            nFlags &= ~MF_REV;
                    );
                    break;

                case A_ACTIVITY:
                    BIND_EXPR(sActivity[0], value);
                    nFlags |= MF_ACT0;
                    break;

                case A_ACTIVITY2:
                    BIND_EXPR(sActivity[1], value);
                    nFlags |= MF_ACT1;
                    break;

                default:
                {
                    bool set = sPadding.set(att, value);
                    set |= sColor[0].set(att, value);
                    set |= sColor[1].set(att, value);

                    if (!set)
                        CtlWidget::set(att, value);
                    break;
                }
            }
        }

        void CtlMeter::end()
        {
            // Configure activity
            for (size_t i=0; i<2; ++i)
            {
                if ((pActivityID[i] == NULL) || (nFlags & (MF_ACT0 << i)))
                    continue;

                char *str = NULL;
                int n = asprintf(&str, ":%s >= 0.5", pActivityID[i]);
                if ((n >= 0) && (str != NULL))
                {
                    sActivity[i].parse(str);
                    free(str);
                }
            }

            // Configure meter widget
            LSPMeter *mtr = (pWidget != NULL) ? static_cast<LSPMeter *>(pWidget) : NULL;
            if (mtr == NULL)
                return;

            size_t channels = (bStereo) ? 2 : 1;
            mtr->set_channels(channels);

            for (size_t i=0; i<channels; ++i)
            {
                const port_t *p = (pPort[i] != NULL) ? pPort[i]->metadata() : NULL;

                float min, max, balance;

                // Calculate minimum
                if ((p != NULL) && (nFlags & MF_MIN))
                    min = calc_value(p, fMin);
                else if ((p != NULL) && (p->flags & F_LOWER))
                    min = calc_value(p, p->min);
                else
                    min = 0.0f;
                mtr->set_mtr_min(i, min);

                // Calculate maximum
                if ((p != NULL) && (nFlags & MF_MAX))
                    max = calc_value(p, fMax);
                else if ((p != NULL) && (p->flags & F_UPPER))
                    max = calc_value(p, p->max);
                else
                    max = 1.0f;
                mtr->set_mtr_max(i, max);

                // Set balance
                if (nFlags & MF_BALANCE)
                {
                    balance = calc_value(p, fBalance);
                    mtr->set_mtr_balance(i, balance);
                    mtr->set_mtr_balance_used(i, true);
                }

                // Update configuration according to meter type
                if ((nType == MT_VU) || ((nType == MT_RMS_PEAK)))
                {
                    mtr->set_mtr_rz_value(i, 0.0f);
                    mtr->set_mtr_rz_used(i, true);

                    mtr->set_mtr_yz_value(i, -6.0f);
                    mtr->set_mtr_yz_used(i, true);

                    mtr->set_mtr_dz0_value(i, -24.0f);
                    mtr->set_mtr_dz0_amount(i, 0.2f);
                    mtr->set_mtr_dz0_used(i, true);

                    mtr->set_mtr_dz1_value(i, -48.0f);
                    mtr->set_mtr_dz1_amount(i, 0.4f);
                    mtr->set_mtr_dz1_used(i, true);

                    mtr->set_mtr_dz2_value(i, -96.0f);
                    mtr->set_mtr_dz2_amount(i, 0.6f);
                    mtr->set_mtr_dz2_used(i, true);
                }

                if (nType == MT_RMS_PEAK)
                    mtr->set_mtr_peak_used(i, true);

                // Update activity
                if (sActivity[i].valid())
                {
                    float value = sActivity[i].evaluate();
                    if (mtr != NULL)
                        mtr->set_mtr_active(i, value >= 0.5f);
                }

                sColor[i].bind(pRegistry, mtr, mtr->mtr_color(i));
            }

            // Launch the timer
            if (mtr->visible())
                sTimer.launch(-1, 50); // Schedule at 20 hz rate
        }

        void CtlMeter::notify(CtlPort *port)
        {
            CtlWidget::notify(port);

            LSPMeter *mtr = (pWidget != NULL) ? static_cast<LSPMeter *>(pWidget) : NULL;
            if (mtr == NULL)
                return;

            for (size_t i=0; i<2; ++i)
            {
                if (port == pPort[i])
                    fReport[i]      = port->get_value();

                if (sActivity[i].valid())
                {
                    float value = sActivity[i].evaluate();
                    if (mtr != NULL)
                        mtr->set_mtr_active(i, value >= 0.5f);
                }
            }
        }

        void CtlMeter::update_peaks(timestamp_t ts)
        {
            LSPMeter *mtr = (pWidget != NULL) ? static_cast<LSPMeter *>(pWidget) : NULL;
            if (mtr == NULL)
                return;

            size_t channels = mtr->channels();

            // Re-calculate values and update meter
            for (size_t i=0; i<channels; ++i)
            {
                float v         = fReport[i];
                float av        = fabs(v);

                // Peak value
                if (nFlags & MF_BALANCE)
                {
                    if (v > fBalance)
                    {
                        if (fValue[i] <= v)
                            fValue[i]       = v;
                        else
                            fValue[i]      += METER_REL * (v - fValue[i]);
                    }
                    else
                    {
                        if (fValue[i] > v)
                            fValue[i]       = v;
                        else
                            fValue[i]      += METER_REL * (v - fValue[i]);
                    }
                }
                else
                {
                    if (fValue[i] < v)
                        fValue[i]      = v;
                    else
                        fValue[i]     += METER_REL * (v - fValue[i]);
                }

                fRms[i]       += (av > fRms[i]) ? METER_ATT * (av - fRms[i]) :  METER_REL * (av - fRms[i]);

                // Limit RMS value
                if (fRms[i] < 0.0f)
                    fRms[i]        = 0.0f;

                // Update meter
                const port_t *p = (pPort[i] != NULL) ? pPort[i]->metadata() : NULL;
                if (p != NULL)
                {
                    if (nType == MT_RMS_PEAK)
                    {
                        mtr->set_mtr_peak(i, calc_value(p, fValue[i]));
                        set_meter_text(p, mtr, i, fRms[i]);
                    }
                    else
                        set_meter_text(p, mtr, i, fValue[i]);
                }
            }
        }

        void CtlMeter::set_meter_text(const port_t *p, LSPMeter *mtr, size_t id, float value)
        {
            mtr->set_mtr_value(id, calc_value(p, value));

            float avalue = fabs(value);

            // Update the value
            if ((p != NULL) && (is_decibel_unit(p->unit)))
            {
                if (avalue >= GAIN_AMP_MAX)
                {
                    mtr->set_mtr_text(id, "+inf");
                    return;
                }
                else if (avalue < GAIN_AMP_MIN)
                {
                    mtr->set_mtr_text(id, "-inf");
                    return;
                }

                value       = logf(fabs(value)) * ((p->unit == U_GAIN_POW) ? 10.0f : 20.0f) / M_LN10;
                avalue      = fabs(value);
            }

            // Now we are able to format values
            char buf[40];

            if (isnan(avalue))
                strcpy(buf, "nan");
            else if (avalue < 10.0f)
                snprintf(buf, sizeof(buf), "%.2f", value);
            else if (avalue < 100.0f)
                snprintf(buf, sizeof(buf), "%.1f", value);
            else
                snprintf(buf, sizeof(buf), "%ld", long(value));
            buf[sizeof(buf)-1] = '\0';

            // Update text of the meter
            mtr->set_mtr_text(id, buf);
        }

        status_t CtlMeter::update_meter(timestamp_t ts, void *arg)
        {
            if (arg == NULL)
                return STATUS_OK;
            CtlMeter *_this = static_cast<CtlMeter *>(arg);
            _this->update_peaks(ts);
            return STATUS_OK;
        }

        status_t CtlMeter::slot_show(void *ptr, void *data)
        {
            if (ptr == NULL)
                return STATUS_BAD_ARGUMENTS;

            CtlMeter *_this  = static_cast<CtlMeter *>(ptr);
            return _this->sTimer.resume();
        }

        status_t CtlMeter::slot_hide(void *ptr, void *data)
        {
            if (ptr == NULL)
                return STATUS_BAD_ARGUMENTS;

            CtlMeter *_this  = static_cast<CtlMeter *>(ptr);
            return _this->sTimer.cancel();
        }

    } /* namespace ctl */
} /* namespace lsp */
